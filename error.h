#pragma once

#include <stdexcept>

struct SyntaxError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct RuntimeError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct NameError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

void SyntaxAssert(bool boolean);

void RuntimeAssert(bool boolean);

void NameAssert(bool boolean);
