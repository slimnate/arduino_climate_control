#include "Bitflag.h"

// Set the bit(s) specified by 'toSet'
void Bitflag::setBit(const u_int toSet) {
    _bits |= toSet;
};

// Return true if 'toCheck' is set
bool Bitflag::checkBit(const u_int toCheck) {
    return _bits & toCheck;
};

// Return true if any bits are set
bool Bitflag::checkAny() {
    return _bits != 0;
};