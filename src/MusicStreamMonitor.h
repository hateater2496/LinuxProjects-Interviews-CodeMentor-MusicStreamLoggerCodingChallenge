/// @file MusicStreamMonitor.h
/// @author Mike Pryor
/// @date 4/29/2024
/// @details This project is for a coding challenge to track changes in a music streaming service.
///          This file contains the declaration of the MusicStreamMonitor class.
/// 
#pragma once
#include "nlohmann/json.hpp"

#include <set>
#include <thread>

/// @remark Forward declarations
class IObserver;
namespace httplib
{
    class Result;
}

///
/// @class MusicStreamMonitor
/// @brief The MusicStreamMonitor class is responsible for monitoring the music stream and notifying observers of any
///        changes.
/// 
/// The MusicStreamMonitor class provides methods to attach and detach observers, as well as methods to start and stop
/// monitoring songs. It also provides methods to process songs, check for removed songs, and retrieve the operation
/// to process.
///
class MusicStreamMonitor
{
public:
    ///
    /// @brief Constructs a MusicStreamMonitor object with the specified HTTP address and port.
    /// @param aHttpAddrPort The HTTP address and port to monitor.
    /// @param aMillisecondsBetweenRefresh The number of milliseconds between each refresh (default: 500).
    /// @param aMillisecondsBetweenRetries The number of milliseconds between each retry (default: 4500).
    /// @param aNumRetries The maximum number of retries (default: 60).
    ///
    MusicStreamMonitor(const std::string& aHttpAddrPort, const int aMillisecondsBetweenRefresh = 500,
        const int aMillisecondsBetweenRetries = 4500, const int aNumRetries = 60);

    ///
    /// @brief Virtual destructor for the MusicStreamMonitor object to complete the v-table.
    ///
    virtual ~MusicStreamMonitor();

    ///
    /// @brief Enumeration representing the possible song state operations.
    ///
    enum class Operation {INSERT, REPLACE, REMOVE};

    ///
    /// @brief Attaches an observer to the MusicStreamMonitor.
    /// @param aPtrObserver A pointer to the observer to attach.
    ///
    virtual void attach(IObserver* aPtrObserver);

    ///
    /// @brief Detaches an observer from the MusicStreamMonitor.
    /// @param aPtrObserver A pointer to the observer to detach.
    ///
    virtual void detach(IObserver* aPtrObserver);

    ///
    /// @brief Starts monitoring the songs.
    ///
    virtual void monitorMusicStream();

    ///
    /// @brief Starts monitoring the songs in a separate thread.
    ///
    virtual void monitorMusicStreamThreaded();

    ///
    /// @brief Stops monitoring the songs.
    ///
    virtual void stopMonitoringMusicStream();

    ///
    /// @brief Stops monitoring the songs in a separate thread.
    ///
    virtual void stopMonitoringMusicStreamThreaded();

    ///
    /// @brief Gets the song state operation to process.
    /// @return The song state operation to process.
    ///
    virtual const nlohmann::ordered_json& getOperationToProcess() const;

    ///
    /// @brief Clears the song state operation to process.
    ///
    virtual void clearOperationToProcess();

protected:
    ///
    /// @brief Default constructor for the MusicStreamMonitor object for defived classes.
    ///
    MusicStreamMonitor() = default;

    ///
    /// @brief Notifies all attached observers.
    ///
    virtual void notify();

    ///
    /// @brief Checks if the HTTP result is valid.
    /// @param aRes The HTTP result to check.
    /// @return True if the HTTP result is valid, false otherwise.
    ///
    virtual bool isResultValid(const httplib::Result& aRes) const;

    ///
    /// @brief Builds the song IDs from the simple representation of songs' ID and name.
    /// @param aRes The result containing the simple representation of songs' ID and name.
    ///
    virtual void buildSongIdsFromSimplePlaylist(const httplib::Result& aRes);

    ///
    /// @brief Processes a song to determine whether it is being inserted, replaced, or removed.
    /// @param aSong The song to process.
    ///
    virtual void processSong(const nlohmann::ordered_json& aSong);

    ///
    /// @brief Checks for removed songs by determining if the current set of songs is a subset of the previous.
    ///
    virtual void checkForRemovedSongs();

    std::string m_httpAddrPort;
    int m_millisecondsBetweenRefresh;
    int m_millisecondsBetweenRetries;
    int m_numRetries;
    bool m_running;
    std::thread m_thread;
    std::vector<IObserver*> m_observers;
    std::map<std::string, nlohmann::ordered_json> m_songs;
    std::set<std::string> m_songIds;
    nlohmann::ordered_json m_operationToProcess;
};
