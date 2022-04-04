/// \file timer.h
/// \brief Interface for the timer class CTimer.

#pragma once

#include <windows.h> //needed for BOOL

/// The \brief The timer. 
///
/// The timer allows you to manage game events by duration, rather than
/// on a frame-by-frame basis. This simple version of the timer is based on
/// the Windows API function timeGetTime, which is notoriously inaccurate
/// but perfectly adequate for a simple game demo.

class CTimer{
  private:
    int m_nStartTime; ///< Time that timer was started.
    int m_nPauseOffset;
    

  public:
    CTimer(); ///< Constructor.
    void start(); ///< Start the timer.
    int time(bool truetime = false); ///< Return the time in ms.
    void pause(); //used for when you want to stop time
    void resume(); //unpause
    bool elapsed(int &start, int interval, bool truetime = false); ///< Has interval ms elapsed since start?
    int getTrueTime();
    bool m_bPaused;
    int m_nPauseStart;
}; //CTimer

