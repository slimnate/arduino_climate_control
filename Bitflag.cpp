#include "Bitflag.h"

// Set the bit(s) specified by 'toSet'
void Bitflag::setBit(int toSet) {
    _bits |= toSet;
};

// Return true if 'toCheck' is set
bool Bitflag::checkBit(int toCheck) {
    return _bits & toCheck;
};

// Return true if any bits are set
bool Bitflag::checkAny() {
    return _bits != 0;
};