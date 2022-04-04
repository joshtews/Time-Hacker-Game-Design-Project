// \file random.cpp
// \brief Code for the random number generator CRandom.

#include <windows.h>

#include "random.h"
#include "debug.h"

CRandom::CRandom(){ 
  sowseed(); //seed random number generator
} //constructor

/// Seed the random number generator from the clock.

void CRandom::sowseed(){ 
  srand(timeGetTime()); 
} //sowseed

/// Return a random integer within a given range.
/// \param i Bottom of range.
/// \param j Top of range.
/// \return A random positive integer r such that i<=r<=j.

int CRandom::number(int i, int j){  
  return rand()%(j - i + 1) + i;
} //number
