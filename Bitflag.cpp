#include "Bitflag.h"

void Bitflag::setBit(int toSet) {
    _bits |= toSet;
};

bool Bitflag::checkBit(int toCheck) {
    return _bits & toCheck;
};

bool Bitflag::checkAny() {
    return _bits != 0b0000;
};