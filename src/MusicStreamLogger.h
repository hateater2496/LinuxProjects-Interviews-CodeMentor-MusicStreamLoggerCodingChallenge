/// @file MusicStreamLogger.h
/// @author Mike Pryor
/// @date 4/29/2024
/// @details This project is for a coding challenge to track changes in a music streaming service.
///          This file contains the declaration of the MusicStreamLogger class.
/// 
#pragma once
#include "IObserver.h"
#include "nlohmann/json.hpp"

/// @class MusicStreamLogger
/// @brief The MusicStreamLogger class is responsible for logging the state changes of songs.
/// 
/// MusicStreamLogger is an observer class that receives updates from a MusicStreamMonitor.
/// It provides methods to process change operations in song state, scale brightness of songs,
/// and log the change operations.
class MusicStreamLogger: public IObserver
{
public:
    /// @brief Constructs a MusicStreamLogger object and attaches this observer to a MusicStreamMonitor.
    /// 
    /// @param aPtrMusicStreamMonitor A pointer to a MusicStreamMonitor MusicStreamLogger is observing.
    MusicStreamLogger(MusicStreamMonitor* aPtrMusicStreamMonitor);

    /// @brief Virtual destructor to complete the v-table and detach this observer from the monitor.
    virtual ~MusicStreamLogger();

    /// @brief Updates the MusicStreamLogger with the latest state of the MusicStreamMonitor.
    /// 
    /// @param aPtrMusicStreamMonitor A pointer to a MusicStreamMonitor object.
    virtual void update(MusicStreamMonitor* aPtrMusicStreamMonitor) override;

protected:
    /// @brief Processes a song operation.
    /// 
    /// This method processes a song state operation specified by a JSON object.
    /// 
    /// @param aOperationToProcess The JSON object representing the song state operation to process.
    virtual void processSongOperation(const nlohmann::ordered_json& aOperationToProcess) const;

    /// @brief Logs the replacement of a song.
    /// 
    /// This method logs the replacement of a song by comparing the previous song state with the current song state.
    /// 
    /// @param aPrevSong The JSON object representing the previous song state.
    /// @param aCurSong The JSON object representing the current song state.
    virtual std::string logReplacementSong(const nlohmann::ordered_json& aPrevSong, const nlohmann::ordered_json& aCurSong) const;

    /// @brief Gets the changes in the state of a song.
    /// 
    /// This method cleans up the JSON object representing the difference between the previous and current song state.
    /// 
    /// @param aId The ID of the song.
    /// @param aSongDiff The JSON object representing the difference between the previous and current song state.
    /// @return The JSON object representing the changes in the song state.
    virtual std::vector<nlohmann::ordered_json> getChanges(const std::string& aId, const nlohmann::ordered_json& aSongDiff) const;

    /// @brief Converts seconds to minutes and seconds with mm:ss format.
    /// 
    /// This method converts the seconds specified by a JSON object.
    /// 
    /// @param aSong The JSON object representing the song.
    virtual void convertSeconds(nlohmann::ordered_json& aSong, const std::string& key) const;

    /// @remark A pointer to the MusicStreamMonitor object.
    MusicStreamMonitor* m_ptrMusicStreamMonitor;
};
