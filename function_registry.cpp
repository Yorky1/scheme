#include "function_registry.h"
#include "object.h"

// static
FunctionRegistry& FunctionRegistry::Instance() {
    static FunctionRegistry singleton;
    return singleton;
}

Function* FunctionRegistry::GetFunction(const std::string& name, Context& context) {
    SyntaxAssert(producers_.count(name));
    auto res = producers_[name]->Produce();
    context.AddPtrToData(std::dynamic_pointer_cast<Object>(res));
    return res.get();
}

bool FunctionRegistry::HasFunction(const std::string& name) {
    return producers_.count(name);
}
