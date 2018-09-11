#ifndef LOG_MANAGER_CPP
#define LOG_MANAGER_CPP

#include <iostream>
#include <fstream>
#include <string>
#include "LogHelper.cpp"
using namespace std;

class LogManager {
public:
	//Logging without NodeID
	static void AddLog(std::string message) {
		std::string fileName = "./CppLogs.txt";
		fstream myfile;
		myfile.open(fileName, fstream::app);
		if (myfile.is_open()) {
			myfile << message << endl;
			myfile.close();
		}
	}
	static void AddLog(std::string message, int number) {
		std::string fileName = "./CppLogs.txt";
		fstream myfile;
		myfile.open(fileName, fstream::app);
		if (myfile.is_open()) {
			myfile << message << ":" << number << endl;
			myfile.close();
		}
	}
	static void AddLog(std::string message, uint32_t number) {
		std::string fileName = "./CppLogs.txt";
		fstream myfile;
		myfile.open(fileName, fstream::app);
		if (myfile.is_open()) {
			myfile << message << ":" << number << endl;
			myfile.close();
		}
	}
	static void AddLog(std::string message, bool number) {
		std::string fileName = "./CppLogs.txt";
		fstream myfile;
		myfile.open(fileName, fstream::app);
		if (myfile.is_open()) {
			myfile << message << ":" << number << endl;
			myfile.close();
		}
	}
	static void AddLog(std::string message, std::string value) {
		std::string fileName = "./CppLogs.txt";
		fstream myfile;
		myfile.open(fileName, fstream::app);
		if (myfile.is_open()) {
			myfile << message << ":" << value << endl;
			myfile.close();
		}
	}
	static void AddLog(std::string message, std::vector<std::string> value) {
		std::string fileName = "./CppLogs.txt";
		fstream myfile;
		myfile.open(fileName, fstream::app);
		if (myfile.is_open()) {
			std::string temp = "";
			for(unsigned int i = 0; i < (unsigned)value.size(); i++)
				temp += value[i];

			myfile << message << ":" << temp << endl;
			myfile.close();
		}
	}

	//Logging with NodeID
	static void AddLogWithNodeId(std::string message) {
		std::string fileName = "./CppLogs.txt";
		fstream myfile;
		myfile.open(fileName, fstream::app);
		if (myfile.is_open()) {
			myfile << "NodeId=" << LogHelper::GetNodeId() << ": " << message<< endl;
			myfile.close();
		}
	}
	static void AddLogWithNodeId(std::string message, int number) {
		std::string fileName = "./CppLogs.txt";
		fstream myfile;
		myfile.open(fileName, fstream::app);
		if (myfile.is_open()) {
			myfile << "NodeId=" << LogHelper::GetNodeId() << ": " << message << ":" << number << endl;
			myfile.close();
		}
	}
	static void AddLogWithNodeId(std::string message, uint32_t number) {
		std::string fileName = "./CppLogs.txt";
		fstream myfile;
		myfile.open(fileName, fstream::app);
		if (myfile.is_open()) {
			myfile << "NodeId=" << LogHelper::GetNodeId() << ": " << message << ":" << number << endl;
			myfile.close();
		}
	}
	static void AddLogWithNodeId(std::string message, uint64_t number) {
		std::string fileName = "./CppLogs.txt";
		fstream myfile;
		myfile.open(fileName, fstream::app);
		if (myfile.is_open()) {
			myfile << "NodeId=" << LogHelper::GetNodeId() << ": " << message << ":" << number << endl;
			myfile.close();
		}
	}
	static void AddLogWithNodeId(std::string message, uint8_t* number) {
		std::string fileName = "./CppLogs.txt";
		fstream myfile;
		myfile.open(fileName, fstream::app);
		if (myfile.is_open()) {
			myfile << "NodeId=" << LogHelper::GetNodeId() << ": " << message << ":" << number << endl;
			myfile.close();
		}
	}
	static void AddLogWithNodeId(std::string message, bool number) {
		std::string fileName = "./CppLogs.txt";
		fstream myfile;
		myfile.open(fileName, fstream::app);
		if (myfile.is_open()) {
			myfile << "NodeId=" << LogHelper::GetNodeId() << ": " << message << ":" << number << endl;
			myfile.close();
		}
	}
	static void AddLogWithNodeId(std::string message, std::string value) {
		std::string fileName = "./CppLogs.txt";
		fstream myfile;
		myfile.open(fileName, fstream::app);
		if (myfile.is_open()) {
			myfile << "NodeId=" << LogHelper::GetNodeId() << ": " << message << ":" << value << endl;
			myfile.close();
		}
	}
	static void AddLogWithNodeId(std::string message, std::vector<std::string> value) {
		std::string fileName = "./CppLogs.txt";
		fstream myfile;
		myfile.open(fileName, fstream::app);
		if (myfile.is_open()) {
			std::string temp = "";
			for(unsigned int i = 0; i < (unsigned)value.size(); i++)
				temp += value[i];

			myfile << "NodeId=" << LogHelper::GetNodeId() << ": " << message << ":" << temp << endl;
			myfile.close();
		}
	}
	/*Sample code to log with current time
	#include <time.h>
	#include <chrono>
	static void AddLog(std::string message) {
			std::string fileName = "./CppLogs.txt";
			fstream myfile;
			myfile.open(fileName, fstream::app);
			if (myfile.is_open()) {
				auto time = std::chrono::system_clock::now();
				std::time_t currentTime = std::chrono::system_clock::to_time_t(time);
				myfile << message << " | " << std::ctime(&currentTime);
				myfile<<"NodeId="<<LogHelper::GetNodeId()<<": "<<message<<endl;
				myfile.close();
			}
		} */
};
#endif
