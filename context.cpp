#include "context.h"

bool Context::HasVariable(const std::string& name) const {
    if (HasLocalVariable(name)) {
        return true;
    }
    std::shared_ptr<Context> now = up_;
    while (now) {
        if (now->HasLocalVariable(name)) {
            return true;
        }
        now = now->up_;
    }
    return false;
}

Object* Context::GetVariable(const std::string& name) {
    RuntimeAssert(HasVariable(name));
    if (HasLocalVariable(name)) {
        return GetLocalVariable(name);
    }
    std::shared_ptr<Context> now = up_;
    while (now) {
        if (now->HasLocalVariable(name)) {
            return now->GetLocalVariable(name);
        }
        now = now->up_;
    }
    return nullptr;
}