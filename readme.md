# FS-IoT: Fast Few Shot IoT Devices Identification

## 项目简介
FSIoT 提供了从原始的PCAP文件的端到端的物联网设备识别解决方案。

## Step1 获取数据集

我们采用了UNSW数据集和BehavIoT数据集进行实验评估。
其中，UNSW数据集可以直接下载，BehavIoT数据集需要通过邮件联系作者获取（感谢胡天睿）。

- UNSW201620数据集：https://iotanalytics.unsw.edu.au/iottraces.html
- BehavIoT数据集：https://moniotrlab.khoury.northeastern.edu/behaviot-dataset-imc23/

## Step2 下载预先构建的 docker 镜像
docker 镜像正在制作中... 
预计5月底完成镜像制作

### 本领域的其他开源代码

本项目已集成ByteIoT/Shahid/Ahmed的方法，并稍作修改以保持实验设置一致性，进行了对照试验。
以下是本领域相关的开源代码(感谢段晨鑫, Dilawer Ahmed的开源代码)

- ByteIoT/Shahid: https://github.com/ZebornDuan/IoTClassifier
- Ahmed: https://github.com/dilawer11/iot-device-fingerprinting

### 对照实验相关论文

+ Chenxin D., Hao G., Guanglei S., Jiahai Y. and Zhiliang W. "ByteIoT: a Practical IoT Device Identification System based on Packet Length Distribution", in IEEE Transactions on Network and Service Management, 2021. doi: 10.1109/TNSM.2021.3130312.
+ Shahid, Mustafizur R., et al. "IoT devices recognition through network traffic analysis." 2018 IEEE international conference on big data (big data). IEEE, 2018.
+ D. Ahmed, A. Das, and F. Zaffar, “Analyzing the feasibility and generalizability of fingerprinting internet of things devices,” Proceedings
on Privacy Enhancing Technologies, vol. 2022, pp. 578 – 600, 2022.

### 手动在 Ubuntu 上安装依赖环境