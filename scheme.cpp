#include "scheme.h"
#include "parser.h"
#include "function_registry.h"

#include <string>
#include <memory>
#include <sstream>

namespace {

Object *ParseRequest(const std::string &request, Context &context) {
    std::istringstream ss(request);
    Tokenizer tokenizer(&ss);
    auto res = Read(&tokenizer, context);
    SyntaxAssert(tokenizer.IsEnd());
    return res;
}

}  // namespace

Interpreter::Interpreter() : context_(new Context()) {
    context_->SetData(&data_);
    FunctionRegistry &registry = FunctionRegistry::Instance();
    registry.RegisterFunction<PNumberFunction>("number?");
    registry.RegisterFunction<EqualFunction>("=");
    registry.RegisterFunction<MonIncFunction>("<");
    registry.RegisterFunction<MonDecFunction>(">");
    registry.RegisterFunction<MonNonIncFunction>(">=");
    registry.RegisterFunction<MonNonDecFunction>("<=");
    registry.RegisterFunction<PlusFunction>("+");
    registry.RegisterFunction<MinusFunction>("-");
    registry.RegisterFunction<MultiplyFunction>("*");
    registry.RegisterFunction<DivisionFunction>("/");
    registry.RegisterFunction<MaxFunction>("max");
    registry.RegisterFunction<MinFunction>("min");
    registry.RegisterFunction<AbsFunction>("abs");
    registry.RegisterFunction<PPairFunction>("pair?");
    registry.RegisterFunction<PNullFunction>("null?");
    registry.RegisterFunction<PListFunction>("list?");
    registry.RegisterFunction<ConsFunction>("cons");
    registry.RegisterFunction<CarFunction>("car");
    registry.RegisterFunction<CdrFunction>("cdr");
    registry.RegisterFunction<ListFunction>("list");
    registry.RegisterFunction<ListRefFunction>("list-ref");
    registry.RegisterFunction<ListTailFunction>("list-tail");
    registry.RegisterFunction<True>("#t");
    registry.RegisterFunction<False>("#f");
    registry.RegisterFunction<PBooleanFunction>("boolean?");
    registry.RegisterFunction<NotFuntion>("not");
    registry.RegisterFunction<AndFunction>("and");
    registry.RegisterFunction<OrFunction>("or");
    registry.RegisterFunction<QuoteFunction>("quote");
    registry.RegisterFunction<IfFunction>("if");
    registry.RegisterFunction<LambdaBuilderFunction>("lambda");
    registry.RegisterFunction<DefineFunction>("define");
    registry.RegisterFunction<SetFunction>("set!");
    registry.RegisterFunction<SetCdrFunction>("set-cdr!");
    registry.RegisterFunction<SetCarFunction>("set-car!");
    registry.RegisterFunction<PSymbolFunction>("symbol?");
}

std::string Interpreter::Run(const std::string &request) {
    Object *parsed_request = ParseRequest(request, *context_);
    std::ostringstream ss;
    RuntimeAssert(parsed_request != nullptr);
    auto res = parsed_request->Eval(*context_);
    if (res) {
        res->Print(&ss);
    } else {
        ss << "()";
    }
    return ss.str();
}
