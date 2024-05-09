/// @file IObserver.h
/// @author Mike Pryor
/// @date 4/29/2024
/// @details This project is for a coding challenge to track changes in a music streaming service.
///          IObserver is an interface class that is used to update observers with changes in the state of
///          the songs list.
/// 
#pragma once
/// @remark Forward declaration
class MusicStreamMonitor;
/// @brief The interface for observers of the MusicStreamMonitor class.
class IObserver
{
public:
    /// @brief Virtual destructor to manage the v-table.
    virtual ~IObserver(){}
    /// @brief Update method called by the MusicStreamMonitor.
    /// @param monitor A pointer to the MusicStreamMonitor object.
    virtual void update(MusicStreamMonitor* monitor) = 0;
protected:
    /// @brief Default constructor for derived classes.
    IObserver() = default;
};
