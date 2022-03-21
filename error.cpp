#include "error.h"

void SyntaxAssert(bool boolean) {
    if (!boolean) {
        throw SyntaxError("");
    }
}

void RuntimeAssert(bool boolean) {
    if (!boolean) {
        throw RuntimeError("");
    }
}

void NameAssert(bool boolean) {
    if (!boolean) {
        throw NameError("");
    }
}