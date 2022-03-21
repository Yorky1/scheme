#include "object.h"

namespace {

template <typename T>
T *MakeObject(Context &context) {
    std::shared_ptr<T> res = std::make_shared<T>();
    context.AddPtrToData(res);
    return res.get();
}

Number *MakeSharedNumber(int64_t number, Context &context) {
    std::shared_ptr<Number> res = std::make_shared<Number>(number);
    context.AddPtrToData(res);
    return As<Number>(res.get());
}

}  // namespace

Function *GetBooleanFunction(bool boolean, Context &context) {
    if (boolean) {
        return MakeObject<True>(context);
    } else {
        return MakeObject<False>(context);
    }
}

bool ToBool(Object *func) {
    if ((Is<Symbol>(func) && As<Symbol>(func)->GetName() == "#f") || Is<False>(func)) {
        return false;
    }
    return true;
}

List ParseToList(Cell *obj) {
    if (obj == nullptr) {
        return List();
    }
    List result;
    std::vector<Object *> &list = result.objects;
    if (obj->GetFirst()) {
        list.push_back(obj->GetFirst());
    }
    while (obj->GetSecond() && Is<Cell>(obj->GetSecond())) {
        obj = As<Cell>(obj->GetSecond());
        list.push_back(obj->GetFirst());
    }
    if (obj->GetSecond()) {
        result.is_wrong = true;
        list.push_back(obj->GetSecond());
    }
    return result;
}

Cell *ParseToCell(List &list, Context &context) {
    Cell *result = MakeObject<Cell>(context);
    std::vector<Object *> &objects = list.objects;
    if (objects.empty()) {
        return result;
    }
    auto now = result;
    for (size_t i = 0; i < objects.size(); ++i) {
        if (i + 1 == objects.size() && list.is_wrong) {
            now->SetSecond(objects[i]);
        } else {
            Cell *cell = MakeObject<Cell>(context);
            cell->SetFirst(objects[i]);
            now->SetSecond(cell);
            now = cell;
        }
    }
    return result;
}

Object *Symbol::Eval(Context &context) {
    auto &instance = FunctionRegistry::Instance();
    if (name_ == "#t") {
        return MakeObject<True>(context);
    }
    if (name_ == "#f") {
        return MakeObject<False>(context);
    }
    if (instance.HasFunction(name_)) {
        return instance.GetFunction(name_, context);
    }
    NameAssert(context.HasVariable(name_));
    return context.GetVariable(name_);
}

Object *Cell::Eval(Context &context) {
    List list = ParseToList(As<Cell>(this));
    std::vector<Object *> &args = list.objects;
    RuntimeAssert(!args.empty());
    args[0] = args.front()->Eval(context);
    RuntimeAssert(Is<Function>(args.front()));
    return As<Function>(list.objects[0])->Eval(list, context);
}

Object *Function::Eval(Context &context) {
    RuntimeAssert(false);
    return nullptr;
};

void Function::Print(std::ostream *out) {
    RuntimeAssert(false);
}

void Cell::Print(std::ostream *out) {
    (*out) << "(";
    auto list = ParseToList(As<Cell>(this));
    for (size_t i = 0; i < list.objects.size(); ++i) {
        if (!list.objects[i]) {
            continue;
        }
        if (i > 0) {
            (*out) << " ";
        }
        if (i + 1 == list.objects.size() && list.is_wrong) {
            (*out) << ". ";
        }
        list.objects[i]->Print(out);
    }
    (*out) << ")";
}

Object *True::Eval(const List &list, Context &context) {
    RuntimeAssert(false);
    return nullptr;
}

Object *False::Eval(const List &list, Context &context) {
    RuntimeAssert(false);
    return nullptr;
}

Object *QuoteFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    RuntimeAssert(args.size() <= 2);
    return args[1];
}

Object *PNumberFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    RuntimeAssert(args.size() == 2);
    return GetBooleanFunction(Is<Number>(args[1]->Eval(context)), context);
}

Object *EqualFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    bool res = true;
    bool was = false;
    FoldNumber(
        args.begin() + 1, args.end(), 0,
        [&res, &was](int64_t a, int64_t b) {
            if (!was) {
                was = true;
            } else if (a != b) {
                res = false;
            }
            return b;
        },
        context);
    return GetBooleanFunction(res, context);
}

Object *MonIncFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    bool res = true;
    bool was = false;
    FoldNumber(
        args.begin() + 1, args.end(), 0,
        [&res, &was](int64_t a, int64_t b) {
            if (!was) {
                was = true;
            } else if (a >= b) {
                res = false;
            }
            return b;
        },
        context);
    return GetBooleanFunction(res, context);
}

Object *MonDecFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    bool res = true;
    bool was = false;
    FoldNumber(
        args.begin() + 1, args.end(), 0,
        [&res, &was](int64_t a, int64_t b) {
            if (!was) {
                was = true;
            } else if (a <= b) {
                res = false;
            }
            return b;
        },
        context);
    return GetBooleanFunction(res, context);
}

Object *MonNonIncFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    bool res = true;
    bool was = false;
    FoldNumber(
        args.begin() + 1, args.end(), 0,
        [&res, &was](int64_t a, int64_t b) {
            if (!was) {
                was = true;
            } else if (a < b) {
                res = false;
            }
            return b;
        },
        context);
    return GetBooleanFunction(res, context);
}

Object *MonNonDecFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    bool res = true;
    bool was = false;
    FoldNumber(
        args.begin() + 1, args.end(), 0,
        [&res, &was](int64_t a, int64_t b) {
            if (!was) {
                was = true;
            } else if (a > b) {
                res = false;
            }
            return b;
        },
        context);
    return GetBooleanFunction(res, context);
}

Object *PlusFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    int64_t res = FoldNumber(
        args.begin() + 1, args.end(), 0, [](int64_t a, int64_t b) { return a + b; }, context);
    return MakeSharedNumber(res, context);
}

Object *MinusFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    RuntimeAssert(args.size() >= 2);
    auto init = args[1]->Eval(context);
    RuntimeAssert(Is<Number>(init));
    int64_t res = FoldNumber(
        args.begin() + 2, args.end(), As<Number>(init)->GetValue(),
        [](int64_t a, int64_t b) { return a - b; }, context);
    return MakeSharedNumber(res, context);
}

Object *MultiplyFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    int64_t res = FoldNumber(
        args.begin() + 1, args.end(), 1, [](int64_t a, int64_t b) { return a * b; }, context);
    return MakeSharedNumber(res, context);
}

Object *DivisionFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    RuntimeAssert(args.size() >= 2);
    auto init = args[1]->Eval(context);
    RuntimeAssert(Is<Number>(init));
    int64_t res = FoldNumber(
        args.begin() + 2, args.end(), As<Number>(init)->GetValue(),
        [](int64_t a, int64_t b) { return a / b; }, context);
    return MakeSharedNumber(res, context);
}

Object *MaxFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    RuntimeAssert(args.size() >= 2);
    auto init = args[1]->Eval(context);
    RuntimeAssert(Is<Number>(init));
    int64_t res = FoldNumber(
        args.begin() + 2, args.end(), As<Number>(init)->GetValue(),
        [](int64_t a, int64_t b) { return std::max(a, b); }, context);
    return MakeSharedNumber(res, context);
}

Object *MinFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    RuntimeAssert(args.size() >= 2);
    auto init = args[1]->Eval(context);
    RuntimeAssert(Is<Number>(init));
    int64_t res = FoldNumber(
        args.begin() + 2, args.end(), As<Number>(init)->GetValue(),
        [](int64_t a, int64_t b) { return std::min(a, b); }, context);
    return MakeSharedNumber(res, context);
}

Object *AbsFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    RuntimeAssert(args.size() == 2);
    auto init = args[1]->Eval(context);
    RuntimeAssert(Is<Number>(init));
    return MakeSharedNumber(std::abs(As<Number>(init)->GetValue()), context);
}

Object *PPairFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    RuntimeAssert(args.size() == 2);
    auto obj = args[1]->Eval(context);
    if (!Is<Cell>(obj)) {
        return GetBooleanFunction(false, context);
    }
    auto check_list = ParseToList(As<Cell>(obj));
    return GetBooleanFunction(check_list.objects.size() == 2, context);
}

Object *PNullFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    RuntimeAssert(args.size() == 2);
    auto obj = args[1]->Eval(context);
    return GetBooleanFunction(obj == nullptr, context);
}

Object *PListFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    RuntimeAssert(args.size() == 2);
    auto obj = args[1]->Eval(context);
    if (obj == nullptr) {
        return GetBooleanFunction(true, context);
    }
    if (!Is<Cell>(obj)) {
        return GetBooleanFunction(false, context);
    }
    auto check_list = ParseToList(As<Cell>(obj));
    return GetBooleanFunction(!check_list.is_wrong, context);
}

Object *ConsFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    RuntimeAssert(args.size() == 3);
    Cell *result = MakeObject<Cell>(context);
    result->SetFirst(args[1]->Eval(context));
    result->SetSecond(args[2]->Eval(context));
    return result;
}

Object *CarFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    RuntimeAssert(args.size() == 2);
    auto obj = args[1]->Eval(context);
    RuntimeAssert(Is<Cell>(obj));
    return As<Cell>(obj)->GetFirst();
}

Object *CdrFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    RuntimeAssert(args.size() == 2);
    auto obj = args[1]->Eval(context);
    RuntimeAssert(Is<Cell>(obj));
    return As<Cell>(obj)->GetSecond();
}

Object *ListFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    List result;
    result.objects.reserve(args.size() - 1);
    for (size_t i = 1; i < args.size(); ++i) {
        result.objects.push_back(args[i]->Eval(context));
    }
    return ParseToCell(result, context);
}

Object *ListRefFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    RuntimeAssert(args.size() == 3);
    auto lst = args[1]->Eval(context);
    auto pos = args[2]->Eval(context);
    RuntimeAssert(Is<Cell>(lst) && Is<Number>(pos));
    List argList = ParseToList(As<Cell>(lst));
    size_t intPos = As<Number>(pos)->GetValue();
    RuntimeAssert(intPos < argList.objects.size() - argList.is_wrong);
    return argList.objects[intPos];
}

Object *ListTailFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    RuntimeAssert(args.size() == 3);
    auto lst = args[1]->Eval(context);
    auto pos = args[2]->Eval(context);
    RuntimeAssert(Is<Cell>(lst) && Is<Number>(pos));
    List argList = ParseToList(As<Cell>(lst));
    size_t intPos = As<Number>(pos)->GetValue();
    RuntimeAssert(intPos <= argList.objects.size() - argList.is_wrong);
    List result;
    result.objects.reserve(argList.objects.size() - intPos);
    for (size_t i = intPos; i < argList.objects.size(); ++i) {
        result.objects.push_back(argList.objects[i]);
    }
    result.is_wrong = argList.is_wrong;
    return ParseToCell(result, context);
}

Object *PBooleanFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    RuntimeAssert(args.size() == 2);
    auto obj = args[1]->Eval(context);
    return GetBooleanFunction(Is<True>(obj) || Is<False>(obj), context);
}

Object *NotFuntion::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    RuntimeAssert(args.size() == 2);
    return GetBooleanFunction(!ToBool(args[1]->Eval(context)), context);
}

Object *AndFunction::Eval(const List &list, Context &context) {
    for (size_t i = 1; i < list.objects.size(); ++i) {
        auto obj = list.objects[i]->Eval(context);
        if (!ToBool(obj) || i + 1 == list.objects.size()) {
            return obj;
        }
    }
    return GetBooleanFunction(true, context);
}

Object *OrFunction::Eval(const List &list, Context &context) {
    for (size_t i = 1; i < list.objects.size(); ++i) {
        auto obj = list.objects[i]->Eval(context);
        if (ToBool(obj) || i + 1 == list.objects.size()) {
            return obj;
        }
    }
    return GetBooleanFunction(false, context);
}

Object *IfFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    SyntaxAssert(1 < args.size() && args.size() < 5);
    if (ToBool(args[1]->Eval(context))) {
        return args[2]->Eval(context);
    } else if (args.size() > 3) {
        return args[3]->Eval(context);
    }
    return nullptr;
}

Object *LambdaBuilderFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    SyntaxAssert(args.size() >= 3);
    LambdaFunction *lambda = MakeObject<LambdaFunction>(context);
    auto lambda_args = args[1] == nullptr ? List() : ParseToList(As<Cell>(args[1]));
    lambda->context_ = std::make_shared<Context>();
    lambda->context_->SetUp(context.shared_from_this());
    lambda->context_->SetData(context.GetData());

    //    lambda->context_.SetUp(context);
    lambda->args_.reserve(lambda_args.objects.size());
    for (auto ptr : lambda_args.objects) {
        RuntimeAssert(Is<Symbol>(ptr));
        lambda->args_.push_back(As<Symbol>(ptr)->GetName());
    }
    lambda->functions_.reserve(args.size() - 2);
    for (size_t i = 2; i < args.size(); ++i) {
        lambda->functions_.push_back(args[i]);
    }
    return lambda;
}

Object *LambdaFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    SyntaxAssert(args.size() == args_.size() + 1);
    for (size_t i = 1; i < args.size(); ++i) {
        context_->AddVariable(args_[i - 1], args[i]->Eval(*context_));
    }
    Object *res;
    for (auto &f : functions_) {
        res = f->Eval(*context_);
    }
    return res;
}

Object *DefineFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    SyntaxAssert(args.size() == 3);
    if (Is<Cell>(args[1])) {
        auto func = ParseToList(As<Cell>(args[1]));
        RuntimeAssert(Is<Symbol>(func.objects[0]));
        List func_args;
        func_args.objects.reserve(func.objects.size() - 1);
        for (size_t i = 1; i < func.objects.size(); ++i) {
            func_args.objects.push_back(func.objects[i]);
        }
        auto func_args_cell = ParseToCell(func_args, context);
        List to_lambda;
        to_lambda.objects = {nullptr, func_args_cell, args[2]};
        auto lambda = LambdaBuilderFunction().Eval(to_lambda, context);
        context.AddVariable(As<Symbol>(func.objects[0])->GetName(), lambda);
        As<LambdaFunction>(lambda)->context_ = std::make_shared<Context>(context);
        return nullptr;
    }
    RuntimeAssert(Is<Symbol>(args[1]) && args[2] != nullptr);
    auto to_add = args[2]->Eval(context);
    context.AddVariable(As<Symbol>(args[1])->GetName(), to_add);
    return nullptr;
}

Object *SetFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    SyntaxAssert(args.size() == 3 && Is<Symbol>(args[1]));
    NameAssert(context.HasVariable(As<Symbol>(args[1])->GetName()));
    context.AddVariable(As<Symbol>(args[1])->GetName(), args[2]->Eval(context));
    return nullptr;
}

Object *SetCdrFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    SyntaxAssert(args.size() == 3 && Is<Symbol>(args[1]) && args[2] != nullptr);
    Cell *res = MakeObject<Cell>(context);
    res->SetSecond(args[2]->Eval(context));
    context.AddVariable(As<Symbol>(args[1])->GetName(), res);
    return nullptr;
}

Object *SetCarFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    SyntaxAssert(args.size() == 3 && Is<Symbol>(args[1]) && args[2] != nullptr);
    Cell *res = MakeObject<Cell>(context);
    res->SetFirst(args[2]->Eval(context));
    context.AddVariable(As<Symbol>(args[1])->GetName(), res);
    return nullptr;
}

Object *PSymbolFunction::Eval(const List &list, Context &context) {
    const std::vector<Object *> &args = list.objects;
    SyntaxAssert(args.size() == 2);
    return GetBooleanFunction(Is<Symbol>(args[1]->Eval(context)), context);
}
