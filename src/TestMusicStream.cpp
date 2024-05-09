/// @file MusicStreamMonitor.h
/// @author Mike Pryor
/// @date 4/29/2024
/// @details This project is for a coding challenge to track changes in a music streaming service.
///          IOT system. This file contains the unit tests for MusicStreamMonitor and MusicStreamLogger.
/// 
#include "MusicStreamMonitor.h"
#include "MusicStreamLogger.h"
#include "Logger.h"

#include "acutest.h"
#include "httplib.h"
#include "nlohmann/json.hpp"

#include <set>
#include <thread>

class MockMusicStreamMonitor : public MusicStreamMonitor
{
public:
    MockMusicStreamMonitor() : MusicStreamMonitor("http://localhost:8080")
    {
    }

    using MusicStreamMonitor::notify;
    using MusicStreamMonitor::isResultValid;
    using MusicStreamMonitor::buildSongIdsFromSimplePlaylist;
    using MusicStreamMonitor::processSong;
    using MusicStreamMonitor::checkForRemovedSongs;

    // Create getters and setters for the protected member variables
    const std::string& getHttpAddrPort()const
    {
        return m_httpAddrPort;
    }

    int getMillisecondsBetweenRefresh()const
    {
        return m_millisecondsBetweenRefresh;
    }

    int getMillisecondsBetweenRetries()const
    {
        return m_millisecondsBetweenRetries;
    }

    int getNumRetries()const
    {
        return m_numRetries;
    }

    bool getRunning()const
    {
        return m_running;
    }

    const std::vector<IObserver*>& getObservers()const
    {
        return m_observers;
    }

    const std::map<std::string, nlohmann::ordered_json>& getMusicStream()const
    {
        return m_songs;
    }

    const std::set<std::string>& getSongIds()const
    {
        return m_songIds;
    }

    const nlohmann::ordered_json& getOperationToProcess()const
    {
        return m_operationToProcess;
    }

    void setHttpAddrPort(const std::string& aHttpAddrPort)
    {
        m_httpAddrPort = aHttpAddrPort;
    }

    void setMillisecondsBetweenRefresh(const int aMillisecondsBetweenRefresh)
    {
        m_millisecondsBetweenRefresh = aMillisecondsBetweenRefresh;
    }

    void setMillisecondsBetweenRetries(const int aMillisecondsBetweenRetries)
    {
        m_millisecondsBetweenRetries = aMillisecondsBetweenRetries;
    }

    void setNumRetries(const int aNumRetries)
    {
        m_numRetries = aNumRetries;
    }

    void setRunning(const bool aRunning)
    {
        m_running = aRunning;
    }

    void setObservers(const std::vector<IObserver*>& aObservers)
    {
        m_observers = aObservers;
    }

    void setMusicStream(const std::map<std::string, nlohmann::ordered_json>& aMusicStream)
    {
        m_songs = aMusicStream;
    }

    void setSongIds(const std::set<std::string>& aSongIds)
    {
        m_songIds = aSongIds;
    }

    void setOperationToProcess(const nlohmann::ordered_json& aOperationToProcess)
    {
        m_operationToProcess = aOperationToProcess;
    }
};

// Create MockMusicStreamLogger class following the same pattern as MockMusicStreamMonitor
class MockMusicStreamLogger : public MusicStreamLogger
{
public:
    MockMusicStreamLogger(MusicStreamMonitor* aPtrMusicStreamMonitor)
    : MusicStreamLogger(aPtrMusicStreamMonitor)
    , m_updated(false)
    {
    }

    using MusicStreamLogger::logReplacementSong;
    using MusicStreamLogger::getChanges;
    using MusicStreamLogger::convertSeconds;
    using MusicStreamLogger::processSongOperation;

    // Create getters and setters for the protected member variables
    MusicStreamMonitor* getPtrMusicStreamMonitor()const
    {
        return m_ptrMusicStreamMonitor;
    }

    void setPtrMusicStreamMonitor(MusicStreamMonitor* aPtrMusicStreamMonitor)
    {
        m_ptrMusicStreamMonitor = aPtrMusicStreamMonitor;
    }

    void update(MusicStreamMonitor* aPtrMusicStreamMonitor) override
    {
        m_updated = true;
    }

    bool isUpdated()const
    {
        return m_updated;
    }

    void clearOperationToProcess()
    {
        m_updated = false;
        m_ptrMusicStreamMonitor->clearOperationToProcess();
    }
protected:
    bool m_updated;
};

// Use the acutest framework to test the MockMusicStreamMonitor constructor
void test_MockMusicStreamMonitor_constructor()
{
    MockMusicStreamMonitor mockMusicStreamMonitor;
    TEST_CHECK(mockMusicStreamMonitor.getHttpAddrPort() == "http://localhost:8080");
    TEST_CHECK(mockMusicStreamMonitor.getMillisecondsBetweenRefresh() == 500);
    TEST_CHECK(mockMusicStreamMonitor.getMillisecondsBetweenRetries() == 4500);
    TEST_CHECK(mockMusicStreamMonitor.getNumRetries() == 60);
    TEST_CHECK(!mockMusicStreamMonitor.getRunning());
    TEST_CHECK(mockMusicStreamMonitor.getObservers().empty());
    TEST_CHECK(mockMusicStreamMonitor.getMusicStream().empty());
    TEST_CHECK(mockMusicStreamMonitor.getSongIds().empty());
    TEST_CHECK(mockMusicStreamMonitor.getOperationToProcess().empty());
}

// Use the acutest framework to test the MockMusicStreamMonitor attach and detach methods
void test_MockMusicStreamMonitor_attach_detach()
{
    MockMusicStreamMonitor mockMusicStreamMonitor;
    {
        MusicStreamLogger observer(&mockMusicStreamMonitor);
        MusicStreamLogger observer2(&mockMusicStreamMonitor);
        TEST_CHECK(mockMusicStreamMonitor.getObservers().size() == 2);
    }
    TEST_CHECK(mockMusicStreamMonitor.getObservers().empty());
}

// Use the acutest framework to test the MockMusicStreamMonitor notify method
void test_MockMusicStreamMonitor_notify()
{
    MockMusicStreamMonitor mockMusicStreamMonitor;
    MockMusicStreamLogger observer(&mockMusicStreamMonitor);
    MockMusicStreamLogger observer2(&mockMusicStreamMonitor);
    mockMusicStreamMonitor.notify();
    TEST_CHECK(observer.isUpdated());
    TEST_CHECK(observer2.isUpdated());
    observer.clearOperationToProcess();
    observer2.clearOperationToProcess();
    TEST_CHECK(!observer.isUpdated());
    TEST_CHECK(!observer2.isUpdated());
}

// Use the acutest framework to test the MockMusicStreamMonitor isResultValid method
void test_MockMusicStreamMonitor_isResultValid()
{
    MockMusicStreamMonitor mockMusicStreamMonitor;
    mockMusicStreamMonitor.setMillisecondsBetweenRetries(15);
    httplib::Result res;
    TEST_CHECK(!mockMusicStreamMonitor.isResultValid(res));
    std::unique_ptr<httplib::Response> response = std::make_unique<httplib::Response>();
    response->status = httplib::StatusCode::OK_200;
    httplib::Result res2(std::move(response), httplib::Error::Success);
    TEST_CHECK(mockMusicStreamMonitor.isResultValid(res2));
}

// Use the acutest framework to test the MockMusicStreamMonitor buildSongIdsFromSimplePlaylist method
void test_MockMusicStreamMonitor_buildSongIdsFromSimplePlaylist()
{
    MockMusicStreamMonitor mockMusicStreamMonitor;
    std::unique_ptr<httplib::Response> response = std::make_unique<httplib::Response>();
    response->body = "[{\"id\":\"1\"},{\"id\":\"2\"},{\"id\":\"3\"}]";
    httplib::Result res(std::move(response), httplib::Error::Success);
    mockMusicStreamMonitor.buildSongIdsFromSimplePlaylist(res);
    TEST_CHECK(mockMusicStreamMonitor.getSongIds().size() == 3);
    TEST_CHECK(mockMusicStreamMonitor.getSongIds().find("1") != mockMusicStreamMonitor.getSongIds().end());
    TEST_CHECK(mockMusicStreamMonitor.getSongIds().find("2") != mockMusicStreamMonitor.getSongIds().end());
    TEST_CHECK(mockMusicStreamMonitor.getSongIds().find("3") != mockMusicStreamMonitor.getSongIds().end());
}

// Use the acutest framework to test the MockMusicStreamMonitor processSong method
void test_MockMusicStreamMonitor_processSong()
{
    MockMusicStreamMonitor mockMusicStreamMonitor;
    std::unique_ptr<httplib::Response> response = std::make_unique<httplib::Response>();
    // add ID through buildSongIdsFromSimplePlaylist
    response->body = "[{\"id\":\"1\",\"name\":\"Song 1\"}]";
    httplib::Result res(std::move(response), httplib::Error::Success);
    mockMusicStreamMonitor.buildSongIdsFromSimplePlaylist(res);
    response = std::make_unique<httplib::Response>();
    response->body = "{\"id\":\"1\",\"name\":\"Song 1\",\"artist\":\"Artist 1\",\"album\":\"Album 1\",\"track_number\":9,\"time_passed\":255,\"time_remaining\":32}";
    mockMusicStreamMonitor.processSong(nlohmann::ordered_json::parse(response->body));
    TEST_CHECK(mockMusicStreamMonitor.getMusicStream().size() == 1);
    TEST_CHECK(mockMusicStreamMonitor.getMusicStream().find("1") != mockMusicStreamMonitor.getMusicStream().end());
    TEST_CHECK(mockMusicStreamMonitor.getOperationToProcess().size() == 2);
    TEST_CHECK(mockMusicStreamMonitor.getOperationToProcess().at("op").template get<MusicStreamMonitor::Operation>() == MusicStreamMonitor::Operation::INSERT);
    TEST_CHECK(mockMusicStreamMonitor.getOperationToProcess().at("curSong").at("id").template get<std::string>() == "1");
}

// Use the acutest framework to test the MockMusicStreamMonitor checkForRemovedSongs method
void test_MockMusicStreamMonitor_checkForRemovedSongs()
{
    MockMusicStreamMonitor mockMusicStreamMonitor;
    std::unique_ptr<httplib::Response> response = std::make_unique<httplib::Response>();
    response->body = "[{\"id\":\"1\",\"name\":\"Song 1\"},{\"id\":\"2\",\"name\":\"Song 2\"},{\"id\":\"3\",\"name\":\"Song 3\"}]";
    httplib::Result res(std::move(response), httplib::Error::Success);
    mockMusicStreamMonitor.buildSongIdsFromSimplePlaylist(res);
    response = std::make_unique<httplib::Response>();
    response->body = "{\"id\":\"1\",\"name\":\"Song 1\",\"artist\":\"Artist 1\",\"album\":\"Album 1\",\"track_number\":9,\"time_passed\":255,\"time_remaining\":32}";
    mockMusicStreamMonitor.processSong(nlohmann::ordered_json::parse(response->body));
    response->body = "{\"id\":\"2\",\"name\":\"Song 2\",\"artist\":\"Artist 2\",\"album\":\"Album 2\",\"track_number\":4,\"time_passed\":284,\"time_remaining\":0}";
    mockMusicStreamMonitor.processSong(nlohmann::ordered_json::parse(response->body));
    response->body = "{\"id\":\"3\",\"name\":\"Song 3\",\"artist\":\"Artist 3\",\"album\":\"Album 3\",\"track_number\":6,\"time_passed\":128,\"time_remaining\":154}";
    mockMusicStreamMonitor.processSong(nlohmann::ordered_json::parse(response->body));
    response->body = "[{\"id\":\"2\",\"name\":\"Song 2\"},{\"id\":\"3\",\"name\":\"Song 3\"}]";
    httplib::Result res2(std::move(response), httplib::Error::Success);
    // Clear song IDs to track removed IDs in the new simple playlist
    std::set<std::string> songIds;
    mockMusicStreamMonitor.setSongIds(songIds);
    mockMusicStreamMonitor.buildSongIdsFromSimplePlaylist(res2);
    mockMusicStreamMonitor.checkForRemovedSongs();
    TEST_CHECK(mockMusicStreamMonitor.getMusicStream().size() == 2);
    TEST_CHECK(mockMusicStreamMonitor.getMusicStream().find("1") == mockMusicStreamMonitor.getMusicStream().end());
    TEST_CHECK(mockMusicStreamMonitor.getOperationToProcess().size() == 2);
    TEST_CHECK(mockMusicStreamMonitor.getOperationToProcess().at("op").template get<MusicStreamMonitor::Operation>() == MusicStreamMonitor::Operation::REMOVE);
    TEST_CHECK(mockMusicStreamMonitor.getOperationToProcess().at("curSong").at("id").template get<std::string>() == "1");
}

// Use the acutest framework to test the MusicStreamMonitor monitorMusicStream and stopMonitoringMusicStream methods
void test_MusicStreamMonitor_monitorMusicStream_stopMonitoringMusicStream()
{
    MockMusicStreamMonitor mockMusicStreamMonitor;
    mockMusicStreamMonitor.setHttpAddrPort("http://localhost:8080");
    mockMusicStreamMonitor.setMillisecondsBetweenRefresh(15);
    mockMusicStreamMonitor.setMillisecondsBetweenRetries(15);
    mockMusicStreamMonitor.setNumRetries(1000);
    mockMusicStreamMonitor.monitorMusicStreamThreaded();
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
    mockMusicStreamMonitor.stopMonitoringMusicStreamThreaded();
    TEST_CHECK(!mockMusicStreamMonitor.getRunning());
}

// Use the acutest framework to test the MusicStreamLogger constructor
void test_MusicStreamLogger_constructor()
{
    MockMusicStreamMonitor mockMusicStreamMonitor;
    MusicStreamLogger musicStreamLogger(&mockMusicStreamMonitor);
    TEST_CHECK(!mockMusicStreamMonitor.getObservers().empty());
}

// Use the acutest framework to test the MusicStreamLogger update method
void test_MusicStreamLogger_update()
{
    MockMusicStreamMonitor mockMusicStreamMonitor;
    MusicStreamLogger musicStreamLogger(&mockMusicStreamMonitor);
    mockMusicStreamMonitor.processSong(nlohmann::ordered_json::parse("{\"id\":\"1\",\"name\":\"Song 1\",\"artist\":\"Artist 1\",\"album\":\"Album 1\",\"track_number\":9,\"time_passed\":255,\"time_remaining\":32}"));
    TEST_CHECK(!mockMusicStreamMonitor.getOperationToProcess().empty());
    TEST_CHECK(mockMusicStreamMonitor.getOperationToProcess().at("op").template get<MusicStreamMonitor::Operation>() == MusicStreamMonitor::Operation::INSERT);
    TEST_CHECK(mockMusicStreamMonitor.getOperationToProcess().at("curSong").at("id").template get<std::string>() == "1");
}

// Use the acutest framework to test the MusicStreamLogger processSongOperation method
void test_MusicStreamLogger_processSongOperation()
{
    MockMusicStreamMonitor mockMusicStreamMonitor;
    MockMusicStreamLogger mockMusicStreamLogger(&mockMusicStreamMonitor);
    nlohmann::ordered_json operationToProcess;
    operationToProcess["op"] = MusicStreamMonitor::Operation::INSERT;
    operationToProcess["curSong"] = nlohmann::ordered_json::parse("{\"id\":\"1\",\"name\":\"Song 1\",\"artist\":\"Artist 1\",\"album\":\"Album 1\",\"track_number\":9,\"time_passed\":255,\"time_remaining\":32}");
    mockMusicStreamLogger.processSongOperation(operationToProcess);
    operationToProcess["op"] = MusicStreamMonitor::Operation::REPLACE;
    operationToProcess["prevSong"] = nlohmann::ordered_json::parse("{\"id\":\"1\",\"name\":\"Song 1\",\"artist\":\"Artist 1\",\"album\":\"Album 1\",\"track_number\":9,\"time_passed\":255,\"time_remaining\":32}");
    operationToProcess["curSong"] = nlohmann::ordered_json::parse("{\"id\":\"1\",\"name\":\"Song 1\",\"artist\":\"Artist 1\",\"album\":\"Album 1\",\"track_number\":9,\"time_passed\":275,\"time_remaining\":12}");
    mockMusicStreamLogger.processSongOperation(operationToProcess);
    operationToProcess["op"] = MusicStreamMonitor::Operation::REMOVE;
    operationToProcess["curSong"] = nlohmann::ordered_json::parse("{\"id\":\"1\",\"name\":\"Song 1\",\"artist\":\"Artist 1\",\"album\":\"Album 1\",\"track_number\":9,\"time_passed\":275,\"time_remaining\":12}");
    mockMusicStreamLogger.processSongOperation(operationToProcess);
}

// Use the acutest framework to test the MusicStreamLogger logReplacementSong method
void test_MusicStreamLogger_logReplacementSong()
{
    MockMusicStreamMonitor mockMusicStreamMonitor;
    MockMusicStreamLogger mockMusicStreamLogger(&mockMusicStreamMonitor);
    nlohmann::ordered_json prevSong = nlohmann::ordered_json::parse("{\"id\":\"1\",\"name\":\"Song 1\",\"artist\":\"Artist 1\",\"album\":\"Album 1\",\"track_number\":9,\"time_passed\":255,\"time_remaining\":32}");
    nlohmann::ordered_json curSong = nlohmann::ordered_json::parse("{\"id\":\"1\",\"name\":\"Song 1\",\"artist\":\"Artist 1\",\"album\":\"Album 1\",\"track_number\":9,\"time_passed\":275,\"time_remaining\":12}");
    std::string logStr = mockMusicStreamLogger.logReplacementSong(prevSong, curSong);
    TEST_CHECK(!logStr.empty());
    TEST_CHECK(logStr.find("Song state changed") != std::string::npos);
    TEST_CHECK(logStr == "Song state changed: \n{\n    \"id\": \"1\",\n    \"time_passed\": \"04:35\"\n}Song state changed: \n{\n    \"id\": \"1\",\n    \"time_remaining\": \"00:12\"\n}");
}

// Use the acutest framework to test the MusicStreamLogger getChanges method
void test_MusicStreamLogger_getChanges()
{
    MockMusicStreamMonitor mockMusicStreamMonitor;
    MockMusicStreamLogger mockMusicStreamLogger(&mockMusicStreamMonitor);
    nlohmann::ordered_json songDiff = nlohmann::ordered_json::parse("[{\"op\":\"replace\",\"path\":\"/time_passed\",\"value\":275}]");
    std::vector<nlohmann::ordered_json> changes = mockMusicStreamLogger.getChanges("1", songDiff);
    TEST_CHECK(changes.size() == 1);
    TEST_CHECK(changes[0].at("id").template get<std::string>() == "1");
    TEST_CHECK(changes[0].at("time_passed").template get<int>() == 275);
}

// Use the acutest framework to test the MusicStreamLogger convertSeconds method
void test_MusicStreamLogger_convertSeconds()
{
    MockMusicStreamMonitor mockMusicStreamMonitor;
    MockMusicStreamLogger mockMusicStreamLogger(&mockMusicStreamMonitor);
    nlohmann::ordered_json song = nlohmann::ordered_json::parse("{\"id\":\"1\",\"name\":\"Song 1\",\"artist\":\"Artist 1\",\"album\":\"Album 1\",\"track_number\":9,\"time_passed\":144,\"time_remaining\":100}");
    mockMusicStreamLogger.convertSeconds(song, "time_passed");
    mockMusicStreamLogger.convertSeconds(song, "time_remaining");
    TEST_CHECK(song.at("time_remaining").template get<std::string>() == "01:40");
    TEST_CHECK(song.at("time_passed").template get<std::string>() == "02:24");
}

// Create TEST_LIST to run all the tests
TEST_LIST =
{
    { "test_MockMusicStreamMonitor_constructor", test_MockMusicStreamMonitor_constructor },
    { "test_MockMusicStreamMonitor_attach_detach", test_MockMusicStreamMonitor_attach_detach },
    { "test_MockMusicStreamMonitor_notify", test_MockMusicStreamMonitor_notify },
    { "test_MockMusicStreamMonitor_isResultValid", test_MockMusicStreamMonitor_isResultValid },
    { "test_MockMusicStreamMonitor_buildSongIdsFromSimplePlaylist", test_MockMusicStreamMonitor_buildSongIdsFromSimplePlaylist },
    { "test_MockMusicStreamMonitor_processSong", test_MockMusicStreamMonitor_processSong },
    { "test_MockMusicStreamMonitor_checkForRemovedSongs", test_MockMusicStreamMonitor_checkForRemovedSongs },
    { "test_MusicStreamMonitor_monitorMusicStream_stopMonitoringMusicStream", test_MusicStreamMonitor_monitorMusicStream_stopMonitoringMusicStream },
    { "test_MusicStreamLogger_constructor", test_MusicStreamLogger_constructor },
    { "test_MusicStreamLogger_update", test_MusicStreamLogger_update },
    { "test_MusicStreamLogger_processSongOperation", test_MusicStreamLogger_processSongOperation },
    { "test_MusicStreamLogger_logReplacementSong", test_MusicStreamLogger_logReplacementSong },
    { "test_MusicStreamLogger_getChanges", test_MusicStreamLogger_getChanges },
    { "test_MusicStreamLogger_convertSeconds", test_MusicStreamLogger_convertSeconds },
    { nullptr, nullptr }
};
