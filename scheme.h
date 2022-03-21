#pragma once

#include "scheme_fwd.h"
#include "context.h"
#include <string>

class Interpreter {
public:
    Interpreter();
    std::string Run(const std::string& request);

private:
    std::vector<std::shared_ptr<Object>> data_;
    std::shared_ptr<Context> context_;
};