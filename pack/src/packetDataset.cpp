#include "packetDataset.h"
#include "file-utils.h"
#include <future>
#include "common-utils.h"
#include "csv.h"
#include "timer.h"

namespace groundnut{

void PacketDataset::AutoLoad(const std::string& datasetRawFolder, const std::string& pktDatasetFilePath)
{
    if(std::filesystem::exists(pktDatasetFilePath))
    {
        LoadBin(pktDatasetFilePath);
    }
    else
    {
        {
            PROFILE_SCOPE("LoadPcap", "NoParam");
            LoadPcap(datasetRawFolder);
        }
      
        Serialize(pktDatasetFilePath);  
    }
}


void PacketDataset::LoadPcap(const std::filesystem::path& inputFolder)
{
    
    std::vector<std::filesystem::path> paths = groundnut::GetFilesInSuffix(inputFolder, {".pcap", ".pcapng"});
    
    rawPacketVec.resize(paths.size(), nullptr);
    
    {
        // future for each file kpacket storage task
        storeFutures.clear();
        std::vector<std::promise<void>> storePromises(paths.size());
        
        // future for garbage collection
        std::promise<void> gcPromise;
        auto gcFuture = gcPromise.get_future();

        // create and start a gc thread 
        std::thread gcThread([&] {
            GarbageCollection(paths.size());
            gcPromise.set_value();
        });
        

        for (int i = 0; i < paths.size(); i++)
        {
            pcpp::IFileReaderDevice* reader = pcpp::IFileReaderDevice::getReader(paths[i].string());
            OpenReader(reader);

            rawPacketVec[i] = new pcpp::RawPacketVector();        
            auto& rawVec = rawPacketVec[i];  

            if(reader->getNextPackets(*rawVec, -1))
            {
                stat.totalPktNum += rawVec->size();
                storeFutures.push_back(storePromises[i].get_future());

                std::vector<std::future<void>> tasks;
                size_t chunkSize = rawVec->size()/(10 - 1); 
                if(chunkSize == 0){chunkSize = 1;}

                auto splittedVec = groundnut::SplitVector(rawVec->m_Vector, chunkSize);//// changed pcpp code and let m_vector accessable.
              
                for (auto& smallrawVec : splittedVec)
                {
                    tasks.emplace_back(std::async(std::launch::async,
                        [this, i, smallrawVec] {
                            StoreKPackets(i, smallrawVec);
                        }));
                }

                // 等待 store small tasks完成  could improve
                for (auto& task : tasks) {
                    task.wait();
                }

                // tell gc could recycle
                storePromises[i].set_value();
                gcNotice.notify_all();
            }
            else
            {
                storeFutures.emplace_back(std::async(std::launch::deferred, [] { }));
            }
            
            CloseReader(reader);
        }

        // 等待垃圾回收线程 
        gcThread.join();    
        //gcFuture.wait();
    }
}

void PacketDataset::StoreKPackets(int index, std::vector<pcpp::RawPacket*> rawVec)
{
    //PROFILE_SCOPE("StoreKPackets ");
	
	bool srcSucc, dstSucc;
	pcpp::IPv4Layer* layerV4 = nullptr;
	pcpp::EthLayer* layerEth = nullptr;
	std::string srcAddr, dstAddr;
	pcpp::Packet parsedPacket;

	for (pcpp::RawPacket* pRawPacket : rawVec)
	{	
		parsedPacket = pcpp::Packet(pRawPacket);

		if ( !parsedPacket.isPacketOfType(pcpp::Ethernet) )
		{
			continue;
		}

        layerEth = parsedPacket.getLayerOfType<pcpp::EthLayer>();

        srcAddr = layerEth->getSourceMac().toString();
        dstAddr = layerEth->getDestMac().toString();
		
		srcSucc = AddPacket(srcAddr, true, pRawPacket);
		dstSucc = AddPacket(dstAddr, false, pRawPacket);

		if (srcSucc || dstSucc)
		{
			stat.usefulPktNum++;
		}
	}

}

void PacketDataset::GarbageCollection(int fileNum)
{
    // 异步清理每个文件的资源
    for (size_t i = 0; i < fileNum; i++) 
    {       
        // wait storeFutures has started
        std::unique_lock<std::mutex> lock(gcMutex);
        gcNotice.wait(lock, [=]{return storeFutures.size() >= i + 1;});

        // wait storeFuture task finished
        storeFutures[i].wait();
 
        rawPacketVec[i]->clear();
        delete rawPacketVec[i];
        rawPacketVec[i] = nullptr;           
    }
}

int PacketDataset::safeGetDevId(const std::string& addr)
{
    std::shared_lock<std::shared_mutex> lock(addressMapMutex);  // 共享读锁
    auto it = devicesMap.find(addr);
    if (it != devicesMap.end()) {
        return it->second.GetDevId();
    }
    return -1;
}

bool PacketDataset::AddPacket(const std::string& addr, bool isSource, pcpp::RawPacket* pRawPacket)
{   
        //filter only add target device packet
        int id = safeGetDevId(addr);
        if (id < 0) { return false; }

        uint16_t devId = id;
        int len = pRawPacket->getFrameLength();
        short signedLen = len > 3000 ? 3000 : len;
        pcpp::Packet packet(pRawPacket);
        uint32_t hash = pcpp::hash5Tuple(&packet);
        timespec timestamp = pRawPacket->getPacketTimeStamp();

        if (!isSource)
        {
            signedLen = -signedLen;
            stat.devStat[devId].totalRecv.fetch_add(1, std::memory_order_relaxed);
        }
        else
        {
            stat.devStat[devId].totalSend.fetch_add(1, std::memory_order_relaxed);
        }

        AddPacket(devId, signedLen, hash, timestamp);
    
        return true;
}
 
void PacketDataset::AddPacket(short deviceId, short signedLen, uint32_t hash5tuple, timespec timestamp)
{
    std::unique_lock<std::mutex> lock(pktDatasetMutex);
    dataset.emplace(deviceId, signedLen, hash5tuple, timestamp);
}

void PacketDataset::OpenReader(pcpp::IFileReaderDevice* reader)
{
    if (reader == NULL)
    {
        throw "pcap format error";
    }

    if (!reader->open())
    {
        throw "pcap cannot open.";
    }
}

void PacketDataset::CloseReader(pcpp::IFileReaderDevice* reader)
{
	if (reader)
	{
		reader->close();
		reader = nullptr;
	}
}

void PacketDataset::UpdateTargetDevices(std::filesystem::path mapping_mac_path)
{
    io::CSVReader<2> reader(mapping_mac_path);  
    reader.read_header(io::ignore_extra_column, "Device", "MAC");
        
    std::string name, mac;

    KDevice::ClearDevIdCounter();
    while (reader.read_row(name, mac)) {
        AddTragetDevice(mac, name);
    }
}

}