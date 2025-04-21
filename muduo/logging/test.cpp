#include "AsyncLogging.h"
#include "Logging.h"
#include <functional>
#include <memory>
#include <thread>
#include <vector>
#include <chrono>
#include <iostream>

std::unique_ptr<AsyncLogging> g_asyncLog;

void asyncOutput(const char* msg, int len)
{
    g_asyncLog->append(msg, len);
}

void logFunc()
{
    for (int i = 0; i <= 5; i++)
    {
        std::thread([i]() {
            for (int j = 1; j <= 10; ++j)
            {
                LOG_INFO << "Thread " << i << " - log" << j;
            }
        }).detach();
    }
}

int main() {
    g_asyncLog = std::make_unique<AsyncLogging>("stdout", 1024 * 1024);
    g_asyncLog->start();

    Logger::setOutput(asyncOutput);

    logFunc();

    std::this_thread::sleep_for((std::chrono::seconds(5)));

    g_asyncLog->stop();

    std::cout << "Logging test finished." << std::endl;

    return 0;
}