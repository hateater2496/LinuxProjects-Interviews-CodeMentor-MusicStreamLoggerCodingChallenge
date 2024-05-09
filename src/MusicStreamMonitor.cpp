/// @file MusicStreamMonitor.cpp
/// @author Mike Pryor
/// @date 4/29/2024
/// @details This project is for a coding challenge to track changes in a music streaming service.
///          This file contains the implementation of the MusicStreamMonitor class.
/// 
#include "IObserver.h"
#include "MusicStreamMonitor.h"
#include "httplib.h"
#include "Logger.h"

MusicStreamMonitor::MusicStreamMonitor(const std::string& aHttpAddrPort, const int aMillisecondsBetweenRefresh,
    const int aMillisecondsBetweenRetries, const int aNumRetries)
: m_httpAddrPort(aHttpAddrPort)
, m_millisecondsBetweenRefresh(aMillisecondsBetweenRefresh)
, m_millisecondsBetweenRetries(aMillisecondsBetweenRetries)
, m_numRetries(aNumRetries)
, m_running(false)
{
}

MusicStreamMonitor::~MusicStreamMonitor()
{
}

void MusicStreamMonitor::attach(IObserver* aPtrObserver)
{
    m_observers.push_back(aPtrObserver);
}

void MusicStreamMonitor::detach(IObserver* aPtrObserver)
{
    m_observers.erase(std::remove_if(m_observers.begin(), m_observers.end(),
        [&](const IObserver* ptrObserver)
        {
            return ptrObserver == aPtrObserver;
        })
    , m_observers.end());
}

void MusicStreamMonitor::notify()
{
    for (auto& observer : m_observers)
    {
        observer->update(this);
    }
}

void MusicStreamMonitor::monitorMusicStream()
{
    // Create an HTTP client
    httplib::Client client(m_httpAddrPort.c_str());
    int retries = 0;
    m_running = true;

    while (m_running && retries < m_numRetries)
    {
        auto res = client.Get("/music");
        if (isResultValid(res))
        {
            retries = 0;
            try
            {
                buildSongIdsFromSimplePlaylist(res);
                checkForRemovedSongs();
                for (auto& id : m_songIds)
                {
                    res = client.Get("/music/" + id);
                    if (isResultValid(res))
                    {
                        retries = 0;
                        auto song = nlohmann::ordered_json::parse(res->body);
                        processSong(song);
                    }
                    else
                    {
                        ++retries;
                    }
                }
            }
            catch (const nlohmann::json_abi_v3_11_3::detail::out_of_range& ex)
            {
                MusicStreamLogError("JSON key-value missing:\n" + std::string(ex.what()));
            }
            catch (const std::exception& ex)
            {
                MusicStreamLogError("JSON parse failed:\n" + std::string(ex.what()));
            }
            m_songIds.clear();
        }
        else
        {
            ++retries;
        }

        // Wait between refreshing state of songs
        std::this_thread::sleep_for(std::chrono::milliseconds(m_millisecondsBetweenRefresh));
    }
}

void MusicStreamMonitor::monitorMusicStreamThreaded()
{
    m_running = true;
    m_thread = std::thread([this]()
    {
        monitorMusicStream();
    });
}

void MusicStreamMonitor::stopMonitoringMusicStream()
{
    m_running = false;
}

void MusicStreamMonitor::stopMonitoringMusicStreamThreaded()
{
    stopMonitoringMusicStream();
    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

bool MusicStreamMonitor::isResultValid(const httplib::Result& aRes)const
{
    bool valid = false;
    if (aRes && (aRes->status == httplib::StatusCode::OK_200))
    {
        valid = true;
    }
    else
    {
        MusicStreamLogWarn("HTTP client connection failed with error: " + httplib::to_string(aRes.error()));
        // Wait between retries
        std::this_thread::sleep_for(std::chrono::milliseconds(m_millisecondsBetweenRetries));
    }
    return valid;
}

void MusicStreamMonitor::buildSongIdsFromSimplePlaylist(const httplib::Result& aRes)
{
    auto songs = nlohmann::ordered_json::parse(aRes->body);
    for (auto song : songs)
    {
        m_songIds.insert(song.at("id").template get<std::string>());
    }
}

void MusicStreamMonitor::processSong(const nlohmann::ordered_json& aSong)
{
    auto id = aSong.at("id").template get<std::string>();
    auto songFound = m_songs.find(id);
    if (songFound != m_songs.end())
    {
        if (aSong != songFound->second)
        {
            m_operationToProcess["op"] = MusicStreamMonitor::Operation::REPLACE;
            m_operationToProcess["prevSong"] = songFound->second;
            m_operationToProcess["curSong"] = aSong;
            notify();
            songFound->second = aSong;
        }
    }
    else
    {
        m_operationToProcess["op"] = MusicStreamMonitor::Operation::INSERT;
        m_operationToProcess["curSong"] = aSong;
        notify();
        m_songs.insert(std::pair<std::string, nlohmann::ordered_json>(id, aSong));
    }
}

void MusicStreamMonitor::checkForRemovedSongs()
{
    std::vector<std::string> idsToRemove;
    for (auto& song : m_songs)
    {
        auto songFound = m_songIds.find(song.first);
        if (songFound == m_songIds.end())
        {
            m_operationToProcess["op"] = MusicStreamMonitor::Operation::REMOVE;
            m_operationToProcess["curSong"] = song.second;
            notify();
            idsToRemove.push_back(song.first);
        }
    }
    for (auto& id : idsToRemove)
    {
        m_songs.erase(id);
    }
}

const nlohmann::ordered_json& MusicStreamMonitor::getOperationToProcess()const
{
    return m_operationToProcess;
}

void MusicStreamMonitor::clearOperationToProcess()
{
    m_operationToProcess.clear();
}
