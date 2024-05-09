/// @file main.cpp
/// @author Mike Pryor
/// @date 4/29/2024
/// @details This project is for a coding challenge to track changes in a music streaming service.
///          This file contains the main application, which creates a MusicStreamMonitor and a MusicStreamLogger.
/// 
#include "MusicStreamMonitor.h"
#include "MusicStreamLogger.h"

#include "cmdparser.hpp"
#include "httplib.h"
#include "nlohmann/json.hpp"
#include "Logger.h"

void configureParser(cli::Parser& parser)
{
    parser.set_optional<std::string>("a", "address", "http://localhost:8080", "Address and port for http client.");
    parser.set_optional<int>("r", "refresh", 500, "Milliseconds between refresh when monitoring the music stream.");
    parser.set_optional<int>("t", "time", 4500, "Time between http connection retries.");
    parser.set_optional<int>("n", "retries", 60, "Number of times to retry the http connection.");
    parser.set_optional<int>("l", "level", 1, "Log level (-1: ERROR, 0: WARN, 1: INFO, 2: DEBUG, 3: TRACE).");
}

void checkValues(int& val, const int lowerThresh, const int upperThresh, const std::string& whatFaster,
    const std::string& whatDelay)
{
    if (val < 0)
    {
        val = 0;
    }
    if (val < lowerThresh)
    {
        MusicStreamLogWarn(whatFaster + " at a faster rate can be very CPU-intensive");
    }
    if (val > upperThresh)
    {
        MusicStreamLogWarn(std::to_string(val / 1000.0) + " seconds may result in " + whatDelay
            + " with a noticeable delay.");
    }
}

void setLogLevel(const LoggerLevel level)
{
  switch(level)
  {
    case LoggerLevel::ERROR:
      Logger::setLogLevel(LoggerLevel::ERROR);
      MusicStreamLogError("Changes to the music stream will not be printed.");
      break;
    case LoggerLevel::WARN:
      Logger::setLogLevel(LoggerLevel::WARN);
      MusicStreamLogError("Changes to the music stream will not be printed.");
      break;
    case LoggerLevel::INFO:
      Logger::setLogLevel(LoggerLevel::INFO);
      break;
    case LoggerLevel::DEBUG:
      Logger::setLogLevel(LoggerLevel::DEBUG);
      break;
    case LoggerLevel::TRACE:
      Logger::setLogLevel(LoggerLevel::TRACE);
      break;
    default:
      Logger::setLogLevel(LoggerLevel::INFO);
      break;
  }
}

int main(int argc, char** argv)
{
    // Above this time (1.5 seconds), state changes may display with a noticeable delay
    int millisecondsBetweenRefreshThreshold = 1500;
    // Lower than this refresh/retry rate (15 ms) may impact other threads/processes
    int cpuClockThresh = 15;
    // Above this time (10 seconds), connection retries may seem very slow
    int millisecondsBetweenRetriesThreshold = 10000;

    // Handle arguments
    cli::Parser parser(argc, argv);
    configureParser(parser);
    parser.run_and_exit_if_error();
    auto logLevel = parser.get<int>("l");
    if (-1 <= logLevel && 3 >= logLevel)
    {
        setLogLevel(static_cast<LoggerLevel>(logLevel));
    }
    else
    {
        setLogLevel(LoggerLevel::INFO);
    }
    auto httpAddrAndPort = parser.get<std::string>("a");
    auto millisecondsBetweenRefresh = parser.get<int>("r");
    checkValues(millisecondsBetweenRefresh, cpuClockThresh, millisecondsBetweenRefreshThreshold, "Refreshing",
        "state changes displaying");
    auto millisecondsBetweenRetries = parser.get<int>("t");
    checkValues(millisecondsBetweenRetries, cpuClockThresh, millisecondsBetweenRetriesThreshold,
        "Retrying the connection ", "connection retries occurring");
    auto numRetries = parser.get<int>("n");
    if (numRetries < 0)
    {
        numRetries = 0;
    }
    
    // Start the monitor for the music stream and log changes
    MusicStreamLogInfo("Connecting to '" + httpAddrAndPort + "', " + std::to_string(millisecondsBetweenRefresh) + ", "
        + std::to_string(millisecondsBetweenRetries) + ", " + std::to_string(numRetries));
    std::unique_ptr<MusicStreamMonitor> ptrMusicStreamMonitor = std::make_unique<MusicStreamMonitor>(httpAddrAndPort,
        millisecondsBetweenRefresh, millisecondsBetweenRetries, numRetries);
    std::unique_ptr<MusicStreamLogger> ptrMusicStreamLogger = 
        std::make_unique<MusicStreamLogger>(ptrMusicStreamMonitor.get());
    ptrMusicStreamMonitor->monitorMusicStreamThreaded();
    // Wait for user input to stop monitoring
    MusicStreamLogInfo("Press <Enter> to quit");
    std::cin.get();
    ptrMusicStreamMonitor->stopMonitoringMusicStreamThreaded();

    return 0;
}
