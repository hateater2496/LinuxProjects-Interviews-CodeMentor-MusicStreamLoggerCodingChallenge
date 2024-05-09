/// @file Logger.h
/// @author Mike Pryor
/// @date 4/29/2024
/// @details This project is for a coding challenge to track changes in a music streaming service.
///          This file contains the declaration of the Logger class.
/// 
#pragma once
#include <map>
#include <string>

enum class LoggerLevel { ERROR = -1, WARN = 0, INFO = 1, DEBUG = 2, TRACE = 3 };

/// @class Logger
/// @brief A class for logging messages with different log levels.
class Logger
{
public:
  /// @brief Logs a message with the specified log level.
  /// @param anLogLevel The log level of the message.
  /// @param aMsg The message to be logged.
  /// @param aLine The line number where the log statement is called (default: "").
  /// @param aFilename The name of the file where the log statement is called (default: "").
  /// @param aFunction The name of the function where the log statement is called (default: "").
  static void log(LoggerLevel anLogLevel, const std::string &aMsg, const int aLine = -1,
      const std::string& aFilename = "", const std::string aFunction = "");

  /// @brief Converts a log level to its corresponding string representation.
  /// @param aLevel The log level to be converted.
  /// @return The string representation of the log level.
  static std::string getLogLevelAsString(const LoggerLevel aLevel);

  /// @brief Sets the log level.
  /// @param aLevel The log level to be set.
  static void setLogLevel(const LoggerLevel aLevel);

  /// @brief Gets the current log level.
  /// @return The current log level.
  static LoggerLevel getLogLevel();

private:
  static LoggerLevel m_logLevel; ///< The current log level.
  static const std::map<LoggerLevel, std::string> m_loggerLevelToStringMap; ///< A map of log levels to their string representations.
};

#define MusicStreamLogError(x) Logger::log(LoggerLevel::ERROR, x, __LINE__, __FILE__, __FUNCTION__)
#define MusicStreamLogWarn(x) Logger::log(LoggerLevel::WARN, x, __LINE__, __FILE__, __FUNCTION__)
/// @remark Log info messages in a prettier way by removing the line, file, and function information
#define MusicStreamLogInfo(x) Logger::log(LoggerLevel::INFO, x)
#define MusicStreamLogDebug(x) Logger::log(LoggerLevel::DEBUG, x, __LINE__, __FILE__, __FUNCTION__)
#define MusicStreamLogTrace(x) Logger::log(LoggerLevel::TRACE, x, __LINE__, __FILE__, __FUNCTION__)
