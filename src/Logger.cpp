/// @file Logger.cpp
/// @author Mike Pryor
/// @date 4/29/2024
/// @details This project is for a coding challenge to track changes in a music streaming service.
///          This file contains the definitions of the Logger class utility methods.
/// 
#include <iostream>
#include "Logger.h"

LoggerLevel Logger::m_logLevel = LoggerLevel::ERROR;
const std::map<LoggerLevel, std::string> Logger::m_loggerLevelToStringMap = {
  {LoggerLevel::ERROR, "ERROR"},
  {LoggerLevel::WARN, "WARN"},
  {LoggerLevel::INFO, "INFO"},
  {LoggerLevel::DEBUG, "DEBUG"},
  {LoggerLevel::TRACE, "TRACE"}
};

void Logger::log(const LoggerLevel anLogLevel, const std::string &aMsg, const int aLine,
      const std::string& aFilename, const std::string aFunction)
{
  if (anLogLevel <= m_logLevel)
  {
    using namespace std::string_literals;
    std::string logMsg = getLogLevelAsString(anLogLevel) + " "s;
    if (aLine > 0 && !aFilename.empty() && !aFunction.empty())
    {
        logMsg += aFilename + ":"s + std::to_string(aLine) + "::"s + aFunction + ":::"s;
    }
    logMsg += aMsg;
    std::cout << logMsg << std::endl;
  }
}

void Logger::setLogLevel(LoggerLevel aLevel)
{
  m_logLevel = aLevel;
}

std::string Logger::getLogLevelAsString(const LoggerLevel anLogLevel)
{
  std::string logLevelStr = "UNKNOWN";
  auto logLevelToStringIter = m_loggerLevelToStringMap.find(anLogLevel);
  if (logLevelToStringIter != m_loggerLevelToStringMap.end())
  {
    logLevelStr = logLevelToStringIter->second;
  }
  return logLevelStr;
}

LoggerLevel Logger::getLogLevel()
{
  return m_logLevel;
}
