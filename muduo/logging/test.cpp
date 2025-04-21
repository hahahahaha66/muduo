#include "AsyncLogging.h"
#include "Logging.h"
#include <functional>
#include <thread>
#include <vector>
#include <chrono>
#include <iostream>

void logFunc(int id, int count)
{
    for (int i = 0; i <=count; i++) 
    {
        LOG_INFO << "Thread " << id << " log line" << i;
        std::this_thread::sleep_for((std::chrono::milliseconds(1)));
    }
}

int main() {
    AsyncLogging* asyncLog = new AsyncLogging("Log/test_log", 1024 * 1024 * 10);
    Logger::setOutput(std::bind(&AsyncLogging::append, asyncLog, std::placeholders::_1, std::placeholders::_2));

    const int threadNum = 4;
    const int logPerThread = 5000;
    std::vector<std::thread> threads;
    for (int i = 0; i < threadNum; i++)
    {
        threads.emplace_back(logFunc, i + 1, logPerThread);
    }

    for (auto& t : threads)
    {
        t.join();
    }

    std::cout << "All logs submitted, waiting for flush..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));

    asyncLog->stop();
    delete asyncLog;

    std::cout << "Log test finished. check output files in ./log/" << std::endl;
    return 0;
}