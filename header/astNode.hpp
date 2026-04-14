#ifndef ASTNODE_H
#define ASTNODE_H

#include <concepts>
#include <string>
#include <sys/select.h>
#include <vector>
#include <token.hpp>
struct Node{
    virtual ~Node()=default;
    Node(Node&)=delete;
    Pos startPos;
    Pos endPos;
};
struct Expr:Node{};
struct Stmt:Node{};
struct Type :Node{};
struct Program:Node{
    std::vector<Stmt*> stmts;
};
enum primitiveType{
    PT_INT,
    PT_DOUBLE,
    PT_FLOAT,
    PT_STRING,
    PT_LONG
};
struct PrimitiveTyp:Type  {
    primitiveType type;
    bool ref;
};
struct DeclaredType:Type  {
    std::string name;
    bool ref;

};
struct FunctionType:Type{
    std::vector<Type> argsType;
    Type returnType;
    bool ref;
};
struct BlockExpr:Expr  {
    std::vector<Stmt*> stmts;
    Expr* value;
};
struct DeclStmt:Stmt  {
    std::string name;
};
struct VarDeclStmt:DeclStmt{
    Type* type;   
    Expr* value;
};
struct FuncDeclStmt:DeclStmt  {
    std::string name;
    Type* returnType;
    std::vector<std::pair<std::string,Type>> args;
    BlockExpr* stmtBlock;
};
struct EditFuncDeclStmt:FuncDeclStmt{};
struct ClassDeclStmt :DeclStmt {
    bool encaps=false;
    std::vector<DeclStmt*> publicStmts;
    std::vector<DeclStmt*> privateStmts;
};
struct AssignmentStmt:Stmt  {
    std::string identifier;
    Expr* value;
};
struct IFStmt:Stmt{
   Expr* condition;
   BlockExpr* onCondTrue;
   BlockExpr* orElse; 
};
struct ForStmt:Stmt  {
    Expr* iterator;
    int steps;
    BlockExpr* block;
};
struct WhileStmt:Stmt  {
    Expr* condition;
    BlockExpr* block;
};
struct ReturnStmt:Stmt{
    Expr* value;
};
struct ContinueStmt:Stmt{};
struct BreakStmt:Stmt{};
struct TrueExpr:Expr{};
enum binaryOperators{
  B_ADD,B_SUB,B_MUL,B_DIV,B_REM,B_POW,


  B_GT,B_LT,B_GTE,B_LTE,B_NTE,B_EQ,

  B_AND,B_OR,B_XOR,

  B_LOGOR,B_LOGAND,

};
enum unaryOperator{
 U_INC,U_DEC,
 U_NEG,
};

namespace literals{
struct IntExpr:Expr{
    int value;
};
struct StringExpr:Expr{
    std::string value;

};
struct LongExpr:Expr{
    long long value;
};
struct FloatExpr:Expr{
    float value;
};
struct DoubleExpr:Expr{
    double value;
};
};


struct BinaryExpr:Expr{
  Expr* left;
  Expr* right;
  binaryOperators op;

};
struct UnaryOp  {
    Expr* desendent;
    Expr* op;
};
struct CallExpr:Expr{
  Expr* callee;
  std::vector<Expr*> args;
};

















#endif // ASTNODE_H

