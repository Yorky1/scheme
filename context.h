#pragma once

#include "scheme_fwd.h"

#include <unordered_map>
#include <vector>

class Context : public std::enable_shared_from_this<Context> {
public:
    Context() = default;

    Context(std::shared_ptr<Context>& other) : up_(other), data_(other->data_) {
        variables_.clear();
    }

    void SetData(std::vector<std::shared_ptr<Object>>* data) {
        data_ = data;
    }

    void AddPtrToData(std::shared_ptr<Object> ptr) {
        data_->push_back(ptr);
    }

    bool HasVariable(const std::string& name) const;

    Object* GetVariable(const std::string& name);

    void AddVariable(const std::string& name, Object* value) {
        variables_[name] = value;
    }

    void SetUp(std::shared_ptr<Context> context) {
        up_ = context;
    }

    std::vector<std::shared_ptr<Object>>* GetData() {
        return data_;
    }

private:
    bool HasLocalVariable(const std::string& name) const {
        return variables_.count(name);
    }

    Object* GetLocalVariable(const std::string& name) {
        return variables_.at(name);
    }

private:
    std::unordered_map<std::string, Object*> variables_;
    std::shared_ptr<Context> up_ = nullptr;
    std::vector<std::shared_ptr<Object>>* data_ = nullptr;
};