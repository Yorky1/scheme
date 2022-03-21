#pragma once

#include "tokenizer.h"
#include "object.h"

#include <memory>

Object* Read(Tokenizer* tokenizer, Context& context);