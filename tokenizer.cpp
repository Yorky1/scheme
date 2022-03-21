#include "tokenizer.h"
#include "error.h"

#include <cctype>
#include <unordered_set>

namespace {

bool IsSymbol(char c) {
    // < = >*#0-9?!-
    static std::unordered_set<char> symbols = {'<', '=', '>', '*', '/', '#', '?', '!', '-', '+'};
    return isalnum(c) || symbols.count(c);
}

bool IsInSyntax(char c) {
    static std::unordered_set<char> other_symbols = {'.', '\'', '(', ')'};
    return other_symbols.count(c) || IsSymbol(c) || std::isspace(c);
}

}  // namespace

bool QuoteToken::operator==(const QuoteToken&) const {
    return true;
}

bool SymbolToken::operator==(const SymbolToken& other) const {
    return name == other.name;
}

bool DotToken::operator==(const DotToken&) const {
    return true;
}

bool ConstantToken::operator==(const ConstantToken& other) const {
    return value == other.value;
}

Tokenizer::Tokenizer(std::istream* in) : in_(in) {
    Next();
}

bool Tokenizer::IsEnd() {
    return is_end_;
}

void Tokenizer::Next() {
    while (!IsNowEnd() && std::isspace(Peek())) {
        Get();
    }
    if (IsNowEnd()) {
        is_end_ = true;
        return;
    }
    char c = Peek();
    if (c == '(') {
        current_token_ = BracketToken::OPEN;
        Get();
        return;
    }
    if (c == ')') {
        current_token_ = BracketToken::CLOSE;
        Get();
        return;
    }
    if (c == '\'') {
        current_token_ = QuoteToken();
        Get();
        return;
    }
    if (c == '.') {
        current_token_ = DotToken();
        Get();
        return;
    }

    if (isdigit(c)) {
        current_token_ = ConstantToken{GetNumber()};
        return;
    }

    if (c == '+' || c == '-') {
        Get();
        int64_t x = GetNumber();
        if (x == -1) {
            current_token_ = SymbolToken{c + GetString()};
        } else {
            current_token_ = ConstantToken{c == '+' ? x : -x};
        }
        return;
    }

    current_token_ = SymbolToken{GetString()};
}

Token Tokenizer::GetToken() {
    return *current_token_;
}

int64_t Tokenizer::GetNumber() {
    if (IsNowEnd() || !std::isdigit(Peek())) {
        return -1;
    }
    int64_t result = 0;
    while (!IsNowEnd() && std::isdigit(Peek())) {
        result = result * 10 + (Get() - '0');
    }
    return result;
}

std::string Tokenizer::GetString() {
    std::string result;
    while (!IsNowEnd()) {
        SyntaxAssert(IsInSyntax(Peek()));
        if (!IsSymbol(Peek())) {
            break;
        }
        result.push_back(Get());
    }
    return result;
}

bool Tokenizer::IsNowEnd() {
    return in_->peek() == std::char_traits<char>::eof();
}

char Tokenizer::Peek() {
    return in_->peek();
}

char Tokenizer::Get() {
    SyntaxAssert(IsInSyntax(Peek()));
    return in_->get();
}
