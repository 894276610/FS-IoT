#ifndef __KTIMER_H__
#define __KTIMER_H__

#include <chrono>
#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <filesystem>
#include <thread>
#include <mutex>

struct ProfileResult
{
	std::string Name;
	std::string Params;
	long long Start, End;
	std::thread::id ThreadID;
};

struct InstrumentationSession
{
	std::string Name;
};

class Instrumentor
{
private:
	InstrumentationSession* m_CurrentSession;
	std::ofstream m_OutputStream;
	int m_ProfileCount = 0;
	std::mutex mtx;
	std::mutex profileCountMtx;

public:
	Instrumentor()
		: m_CurrentSession(nullptr), m_ProfileCount(0)
	{}
	
	std::string currentTimeFormatted() {
		auto now = std::chrono::system_clock::now();
		auto in_time_t = std::chrono::system_clock::to_time_t(now);
		
		std::tm tm_buf;
		localtime_r(&in_time_t, &tm_buf); // POSIX版本，线程安全
		
		std::stringstream ss;
		ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
		return ss.str();
	}

	void BeginCsvSession(const std::string& name, const std::string& filepath)
	{
		if (m_ProfileCount != 0)
		{
			std::cout << "something wrong m_profile count is not 0";
		}

		m_OutputStream.open(filepath, std::ios::out | std::ios::app);
		m_CurrentSession = new InstrumentationSession{ name };
	}

	void EndCsvSession()
	{
		m_OutputStream.close();
		delete m_CurrentSession;
		m_CurrentSession = nullptr;			
		m_ProfileCount = 0;
	}

	// void BeginSession(const std::string& name, const std::string& filepath)
	// {
	// 	if (m_ProfileCount != 0)
	// 	{
	// 		std::cout << "something wrong m_profile count is not 0";
	// 	}
	// 	m_OutputStream.open(filepath);
	// 	WriteHeader();
	// 	m_CurrentSession = new InstrumentationSession{ name };
	// }

	// void EndSession()
	// {
	// 	WriteFooter();
	// 	m_OutputStream.close();
	// 	delete m_CurrentSession;
	// 	m_CurrentSession = nullptr;			
	// 	m_ProfileCount = 0;
	// }

	void WriteCsvProfile(const ProfileResult& result)
	{
		std::unique_lock<std::mutex> lock(mtx);
		if(m_OutputStream.tellp() == 0) {
            m_OutputStream << "Process,Params,Time,TimeStamp" << std::endl;
        }

		// Timestamp use the same format as time.strftime("%Y-%m-%d %H:%M:%S")
		m_OutputStream << result.Name << "," << result.Params << "," << (result.End - result.Start)/1e3f << "," << currentTimeFormatted() << std::endl;
	}
	void WriteProfile(const ProfileResult& result)
	{
		std::unique_lock<std::mutex> lock(mtx);
	
		if (m_ProfileCount++ > 0)
		{
			m_OutputStream << ",";
		}
		
		std::string name = result.Name;
		std::replace(name.begin(), name.end(), '"', '\'');

		m_OutputStream << "{";
		m_OutputStream << "\"cat\":\"function\",";
		m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
		m_OutputStream << "\"name\":\"" << name << "\",";
		m_OutputStream << "\"ph\":\"X\",";
		m_OutputStream << "\"pid\":0,";
		m_OutputStream << "\"tid\":" << result.ThreadID << ",";
		m_OutputStream << "\"ts\":" << result.Start;
		m_OutputStream << "}";

		m_OutputStream.flush();
	}

	void WriteHeader()
	{
		m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
		m_OutputStream.flush();
	}

	void WriteFooter()
	{
		m_OutputStream << "]}";
		m_OutputStream.flush();
	}

	static Instrumentor& Get()
	{
		static Instrumentor* instance = new Instrumentor();
		return *instance;
	}
};

class InstrumentationTimer
{
public:
	InstrumentationTimer(const std::string name, const std::string param)
		:m_ProcessName(name), m_ParamDescription(param), m_Stopped(false)
	{
		m_StartTimePoint = std::chrono::high_resolution_clock::now();
	}

	~InstrumentationTimer()
	{
		Stop();
	}

	void Stop()
	{
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		auto start = std::chrono::time_point_cast<std::chrono::milliseconds>(m_StartTimePoint).time_since_epoch().count();
		auto stop = std::chrono::time_point_cast<std::chrono::milliseconds>(endTimepoint).time_since_epoch().count();
		
		std::thread::id threadID = std::this_thread::get_id();
		Instrumentor::Get().WriteCsvProfile({m_ProcessName, m_ParamDescription, start, stop, threadID});
		// Instrumentor::Get().WriteProfile({ m_Name, start, stop, threadID});
	}

private:
	std::string m_ProcessName;
	std::string m_ParamDescription;
	std::chrono::time_point< std::chrono::high_resolution_clock> m_StartTimePoint;
	bool m_Stopped;
};

#define PROFILING 1
#if PROFILING
#define PROFILE_SCOPE(name, param) InstrumentationTimer time##__LINE__((name), (param))
#define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCSIG__)
#else
#define PROFILE_SCOPE(name)
#endif

#endif