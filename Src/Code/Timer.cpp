/// \file timer.cpp
/// \brief Code for timer class CTimer.

#include "timer.h"
#include "debug.h"

CTimer::CTimer(): m_nStartTime(0){ 
  m_nPauseOffset = 0;
  m_bPaused = false;
} //constructor

/// Start the timer from zero.

void CTimer::start(){
  m_nStartTime = timeGetTime();
} //start

/// Get the time.
/// \return The time in milliseconds.

int CTimer::time(bool truetime){ 
  if(m_bPaused && !truetime)
    return m_nPauseStart - m_nStartTime;
  else
    return timeGetTime() - m_nStartTime - m_nPauseOffset;
} //time

int CTimer::getTrueTime()
{
  return timeGetTime();
}

void CTimer::pause()
{
  m_nPauseStart = timeGetTime();
  m_bPaused = true;
}

void CTimer::resume()
{
  m_bPaused = false;
  m_nPauseOffset += timeGetTime() - m_nPauseStart;
}

/// The elapsed function is a useful function for measuring repeating time 
/// intervals. Given the start and duration times, this function returns TRUE 
/// if the interval is over, and has the side-effect of resetting the start
/// time when that happens, thus setting things up for the next interval.
/// \param start Start of time interval
/// \param interval Duration of time interval
/// \return TRUE if time interval is over

bool CTimer::elapsed(int &start, int interval, bool truetime){
  int curtime = time(truetime); //current time
  
  if (curtime < start)
    start = curtime;

  if(curtime >= start + interval){ //if interval is over
    start = curtime; //reset the start 
    return true; //succeed
  } //if

  else return false; //otherwise, fail
} //elapsed