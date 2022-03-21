#pragma once

#include <memory>
#include <string>
#include <unordered_map>

struct List;

class FunctionRegistry;

class Object;

class Number;
class Symbol;
class Cell;

class Function;

// quote
class QuoteFunction;

// number?
class PNumberFunction;

// comparison
class EqualFunction;
class MonIncFunction;
class MonDecFunction;
class MonNonIncFunction;
class MonNonDecFunction;

// arithmetics
class PlusFunction;
class MinusFunction;
class MultiplyFunction;
class DivisionFunction;

// maxmin
class MaxFunction;
class MinFunction;

// abs
class AbsFunction;

// pair?
class PPairFunction;

// null?
class PNullFunction;

// list?
class PListFunction;

// pair operations
class ConsFunction;
class CarFunction;
class CdrFunction;

// list operations
class ListFunction;
class ListRefFunction;
class ListTailFunction;

// boolean
class True;
class False;

// boolean?
class PBooleanFunction;

// boolean operations
class NotFuntion;
class AndFunction;
class OrFunction;

// advanced
class IfFunction;
class LambdaFunction;
class LambdaBuilderFunction;
class DefineFunction;
class SetFunction;
class SetCdrFunction;
class SetCarFunction;
class PSymbolFunction;

void SyntaxAssert(bool boolean);

void RuntimeAssert(bool boolean);

void NameAssert(bool boolean);

class Context;
