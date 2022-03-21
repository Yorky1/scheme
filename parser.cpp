#include "parser.h"
#include "context.h"
#include <variant>

namespace {

ConstantToken* GetIfConstantToken(Token* token) {
    return std::get_if<ConstantToken>(token);
}

BracketToken* GetIfBracketToken(Token* token) {
    return std::get_if<BracketToken>(token);
}

SymbolToken* GetIfSymbolToken(Token* token) {
    return std::get_if<SymbolToken>(token);
}

QuoteToken* GetIfQuoteToken(Token* token) {
    return std::get_if<QuoteToken>(token);
}

DotToken* GetIfDotToken(Token* token) {
    return std::get_if<DotToken>(token);
}

template <typename T, typename... Args>
Object* MakeShared(Context& context, Args... args) {
    auto res = std::make_shared<T>(args...);
    context.AddPtrToData(res);
    return res.get();
}

Object* ReadText(Tokenizer* tokenizer, Context& context);

Object* ReadList(Tokenizer* tokenizer, Context& context) {
    SyntaxAssert(!tokenizer->IsEnd());
    Token token = tokenizer->GetToken();
    if (auto ptr = GetIfBracketToken(&token); ptr != nullptr && *ptr == BracketToken::CLOSE) {
        return nullptr;
    }
    std::shared_ptr<Cell> cell = std::make_shared<Cell>();
    context.AddPtrToData(cell);
    cell->SetFirst(ReadText(tokenizer, context));
    SyntaxAssert(!tokenizer->IsEnd());
    token = tokenizer->GetToken();
    if (auto ptr = GetIfBracketToken(&token); ptr != nullptr && *ptr == BracketToken::CLOSE) {
        return cell.get();
    } else if (auto ptr = GetIfDotToken(&token); ptr != nullptr) {
        tokenizer->Next();
        SyntaxAssert(!tokenizer->IsEnd());
        cell->SetSecond(ReadText(tokenizer, context));
    } else {
        cell->SetSecond(ReadList(tokenizer, context));
    }
    return cell.get();
}

Object* ReadText(Tokenizer* tokenizer, Context& context) {
    SyntaxAssert(!tokenizer->IsEnd());
    Token token = tokenizer->GetToken();
    tokenizer->Next();
    if (auto ptr = GetIfBracketToken(&token); ptr != nullptr) {
        SyntaxAssert(*ptr != BracketToken::CLOSE);
        auto res = ReadList(tokenizer, context);
        SyntaxAssert(!tokenizer->IsEnd() && tokenizer->GetToken() == Token{BracketToken::CLOSE});
        tokenizer->Next();
        return res;
    }
    SyntaxAssert(!GetIfDotToken(&token));
    if (auto ptr = GetIfQuoteToken(&token); ptr != nullptr) {
        std::shared_ptr<Cell> cell = std::make_shared<Cell>();
        context.AddPtrToData(cell);
        std::shared_ptr<Symbol> qu = std::make_shared<Symbol>("quote");
        context.AddPtrToData(qu);
        cell->SetFirst(qu.get());
        SyntaxAssert(!tokenizer->IsEnd());
        auto res = ReadText(tokenizer, context);
        std::shared_ptr<Cell> cell2 = std::make_shared<Cell>();
        context.AddPtrToData(cell2);
        cell2->SetFirst(res);
        cell->SetSecond(cell2.get());
        return cell.get();
    }
    if (auto ptr = GetIfConstantToken(&token); ptr != nullptr) {
        auto res = std::make_shared<Number>(ptr->value);
        context.AddPtrToData(res);
        return res.get();
    }
    if (auto ptr = GetIfSymbolToken(&token); ptr != nullptr) {
        auto res = std::make_shared<Symbol>(ptr->name);
        context.AddPtrToData(res);
        return res.get();
    }
    SyntaxAssert(false);
    return nullptr;
}

}  // namespace

Object* Read(Tokenizer* tokenizer, Context& context) {
    auto res = ReadText(tokenizer, context);
    SyntaxAssert(tokenizer->IsEnd());
    return res;
}
