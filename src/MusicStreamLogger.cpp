/// @file MusicStreamLogger.cpp
/// @author Mike Pryor
/// @date 4/29/2024
/// @details This project is for a coding challenge to track changes in a music streaming service.
///          This file contains the implementation of the MusicStreamLogger class.
/// 
#include "MusicStreamLogger.h"
#include "MusicStreamMonitor.h"
#include "Logger.h"

MusicStreamLogger::MusicStreamLogger(MusicStreamMonitor* aPtrMusicStreamMonitor)
: m_ptrMusicStreamMonitor(aPtrMusicStreamMonitor)
{
    m_ptrMusicStreamMonitor->attach(this);
}

MusicStreamLogger::~MusicStreamLogger()
{
    m_ptrMusicStreamMonitor->detach(this);
}

void MusicStreamLogger::update(MusicStreamMonitor* aPtrMusicStreamMonitor)
{
    if (aPtrMusicStreamMonitor == m_ptrMusicStreamMonitor)
    {
        try
        {
            processSongOperation(m_ptrMusicStreamMonitor->getOperationToProcess());
        }
        catch (const nlohmann::json_abi_v3_11_3::detail::out_of_range& ex)
        {
            MusicStreamLogError("JSON key-value missing:\n" + std::string(ex.what()));
        }
    }
}

void MusicStreamLogger::processSongOperation(const nlohmann::ordered_json& aOperationToProcess)const
{
    auto operation = aOperationToProcess.at("op").template get<MusicStreamMonitor::Operation>();
    switch(operation)
    {
        case MusicStreamMonitor::Operation::INSERT:
            if (aOperationToProcess.size() > 1)
            {
                auto curSong = aOperationToProcess.at("curSong");
                convertSeconds(curSong, "time_passed");
                convertSeconds(curSong, "time_remaining");
                MusicStreamLogInfo("Song added:\n" + curSong.dump(4));
            }
            else
            {
                MusicStreamLogError("Could not process operation due to missing song state: " + aOperationToProcess.size());
            }
            break;
        case MusicStreamMonitor::Operation::REPLACE:
            if (aOperationToProcess.size() > 2)
            {
                logReplacementSong(aOperationToProcess.at("prevSong"), aOperationToProcess.at("curSong"));
            }
            else
            {
                MusicStreamLogError("Could not process operation due to missing song state: " + aOperationToProcess.size());
            }
            break;
        case MusicStreamMonitor::Operation::REMOVE:
            if (aOperationToProcess.size() > 1)
            {
                std::string name = aOperationToProcess.at("curSong").at("name").template get<std::string>();
                std::string id = aOperationToProcess.at("curSong").at("id").template get<std::string>();
                MusicStreamLogInfo("Song removed: \n" + name + "(" + id + ") has been removed");
            }
            else
            {
                MusicStreamLogError("Could not process operation due to missing song state: " + aOperationToProcess.size());
            }
            break;
    }
}

std::string MusicStreamLogger::logReplacementSong(const nlohmann::ordered_json& aPrevSong,
    const nlohmann::ordered_json& aCurSong)const
{
    std::string logStr = "";
    nlohmann::ordered_json songDiff = nlohmann::ordered_json::diff(aPrevSong, aCurSong);
    std::string id = aPrevSong.at("id").template get<std::string>();
    auto changes = getChanges(id, songDiff);
    // Handle each state change as a separate event per the exercise instructions
    for (auto& change : changes)
    {
        convertSeconds(change, "time_passed");
        convertSeconds(change, "time_remaining");
        std::string newLogStr = "Song state changed: \n" + change.dump(4);
        MusicStreamLogInfo(newLogStr);
        logStr += std::move(newLogStr);
    }
    return logStr;
}

std::vector<nlohmann::ordered_json> MusicStreamLogger::getChanges(const std::string& aId, const nlohmann::ordered_json& aSongDiff)const
{
    std::vector<nlohmann::ordered_json> changes;
    for (auto& change : aSongDiff)
    {
        nlohmann::ordered_json curChange;
        curChange["id"] = aId;
        std::string key = change.at("path").template get<std::string>();
        // Remove preceding '/' from path to get original key
        key = key.substr(1, key.size() - 1);
        curChange[key] = change.at("value");
        changes.push_back(curChange);
    }
    return changes;
}

void MusicStreamLogger::convertSeconds(nlohmann::ordered_json& aSong, const std::string& key)const
{
    if (aSong.find(key) != aSong.end())
    {
        auto seconds = aSong.at(key).template get<int>();
        MusicStreamLogDebug("Changed time: " + std::to_string(seconds));
        auto minutes = seconds / 60;
        using namespace std::string_literals;
        auto time = (minutes < 10 ? "0"s : ""s) + std::to_string(minutes) + ":"
            + ((seconds % 60) < 10 ? "0" : "") + std::to_string(seconds % 60);
        aSong[key] = time;
    }
}
