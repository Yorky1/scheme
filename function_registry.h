#pragma once

#include "scheme_fwd.h"
#include "context.h"

#include <type_traits>
#include <unordered_map>

class IFunctionProducer {
public:
    virtual std::shared_ptr<Function> Produce() = 0;
    virtual ~IFunctionProducer() = default;
};

template <typename T>
class FunctionProducer : public IFunctionProducer {
    std::shared_ptr<Function> Produce() override {
        return std::make_shared<T>();
    }
};

class FunctionRegistry {
public:
    static FunctionRegistry& Instance();

    template <typename T>
    void RegisterFunction(const std::string& name) {
        static_assert(std::is_base_of_v<Function, T>);
        producers_[name] = std::make_shared<FunctionProducer<T>>();
    }

    bool HasFunction(const std::string& name);
    Function* GetFunction(const std::string& name, Context& context);

private:
    FunctionRegistry() = default;

    std::unordered_map<std::string, std::shared_ptr<IFunctionProducer>> producers_;
};
