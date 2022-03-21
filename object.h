#pragma once

#include "scheme_fwd.h"
#include "error.h"
#include "function_registry.h"
#include "context.h"

#include <memory>
#include <iostream>
#include <algorithm>
#include <vector>

template <class T>
bool Is(Object* obj) {
    return dynamic_cast<T*>(obj) != nullptr;
}

template <class T>
T* As(Object* obj) {
    RuntimeAssert(obj != nullptr);
    return dynamic_cast<T*>(obj);
}

template <class Iterator, class BinaryOp>
int64_t FoldNumber(Iterator first, Iterator last, int64_t init, BinaryOp func, Context& context) {
    while (first != last) {
        RuntimeAssert(*first != nullptr);
        auto ptr = (*first)->Eval(context);
        RuntimeAssert(Is<Number>(ptr));
        int64_t number = As<Number>(ptr)->GetValue();
        init = func(init, number);
        ++first;
    }
    return init;
}

Function* GetBooleanFunction(bool boolean, Context& context);

bool ToBool(const Object* func);

List ParseToList(Cell* obj);

Cell* ParseToCell(List& list);

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual Object* Eval(Context& context) = 0;
    virtual void Print(std::ostream* out) = 0;
    virtual ~Object() = default;
};

class Number : public Object {
public:
    Number(int64_t number) : number_(number) {
    }
    void Print(std::ostream* out) override {
        (*out) << number_;
    }
    Object* Eval(Context& context) override {
        return this;
    }
    int64_t GetValue() const {
        return number_;
    }

private:
    int64_t number_;
};

class Symbol : public Object {
public:
    Symbol(const std::string& name) : name_(name) {
    }
    void Print(std::ostream* out) override {
        (*out) << name_;
    }
    Object* Eval(Context& context) override;

    const std::string& GetName() const {
        return name_;
    }

private:
    std::string name_;
};

struct List {
    std::vector<Object*> objects;
    bool is_wrong = false;
};

class Function : public Object {
public:
    virtual Object* Eval(Context& context) override;

    virtual void Print(std::ostream* out) override;

    virtual Object* Eval(const List& list, Context& context) = 0;

    virtual ~Function() = default;
};

class Cell : public Object {
public:
    void Print(std::ostream* out) override;

    Object* Eval(Context& context) override;

    Object* GetFirst() const {
        return first_;
    }
    Object* GetSecond() const {
        return second_;
    }

    void SetFirst(Object* first) {
        first_ = first;
    }

    void SetSecond(Object* second) {
        second_ = second;
    }

private:
    Object* first_;
    Object* second_;
};

class True : public Function {
public:
    True() = default;
    Object* Eval(Context& context) override {
        return this;
    }
    void Print(std::ostream* out) override {
        (*out) << "#t";
    }
    Object* Eval(const List& list, Context& context) override;
};

class False : public Function {
public:
    False() = default;
    Object* Eval(Context& context) override {
        return this;
    }
    void Print(std::ostream* out) override {
        (*out) << "#f";
    }
    Object* Eval(const List& list, Context& context) override;
};

class QuoteFunction : public Function {
public:
    QuoteFunction() = default;
    Object* Eval(const List& list, Context& context) override;
};

class PNumberFunction : public Function {
public:
    PNumberFunction() = default;
    Object* Eval(const List& list, Context& context) override;
};

class EqualFunction : public Function {
public:
    EqualFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class MonIncFunction : public Function {
public:
    MonIncFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class MonDecFunction : public Function {
public:
    MonDecFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class MonNonIncFunction : public Function {
public:
    MonNonIncFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class MonNonDecFunction : public Function {
public:
    MonNonDecFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class PlusFunction : public Function {
public:
    PlusFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class MinusFunction : public Function {
public:
    MinusFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class MultiplyFunction : public Function {
public:
    MultiplyFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class DivisionFunction : public Function {
public:
    DivisionFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class MaxFunction : public Function {
public:
    MaxFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class MinFunction : public Function {
public:
    MinFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class AbsFunction : public Function {
public:
    AbsFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class PPairFunction : public Function {
public:
    PPairFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class PNullFunction : public Function {
public:
    PNullFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class PListFunction : public Function {
public:
    PListFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class ConsFunction : public Function {
public:
    ConsFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class CarFunction : public Function {
public:
    CarFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class CdrFunction : public Function {
public:
    CdrFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class ListFunction : public Function {
public:
    ListFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class ListRefFunction : public Function {
public:
    ListRefFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class ListTailFunction : public Function {
public:
    ListTailFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class PBooleanFunction : public Function {
public:
    PBooleanFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class NotFuntion : public Function {
public:
    NotFuntion() = default;

    Object* Eval(const List& list, Context& context) override;
};

class AndFunction : public Function {
public:
    AndFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class OrFunction : public Function {
public:
    OrFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class IfFunction : public Function {
public:
    IfFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class LambdaBuilderFunction : public Function {
public:
    LambdaBuilderFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class LambdaFunction : public Function {
public:
    LambdaFunction() = default;

    Object* Eval(const List& list, Context& context) override;

private:
    std::vector<std::string> args_;
    std::shared_ptr<Context> context_;
    std::vector<Object*> functions_;

private:
    friend class LambdaBuilderFunction;
    friend class DefineFunction;
};

class DefineFunction : public Function {
public:
    DefineFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class SetFunction : public Function {
public:
    SetFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class SetCdrFunction : public Function {
public:
    SetCdrFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class SetCarFunction : public Function {
public:
    SetCarFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};

class PSymbolFunction : public Function {
public:
    PSymbolFunction() = default;

    Object* Eval(const List& list, Context& context) override;
};
