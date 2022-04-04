/// \file random.h
/// \brief Header for random number generator.

#pragma once

/// \brief The random number generator.

class CRandom{
  public:
    CRandom(); ///< Constructor
    int number(int i, int j); ///< Get random number in i..j.
    void sowseed(); ///< Seed the random number generator.
}; //CRandom
