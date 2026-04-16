#ifndef ASTNODE_H
#define ASTNODE_H

#include <string>
#include <sys/select.h>
#include <vector>
#include <token.hpp>
struct Node{
    virtual ~Node()=default;
    Node(Node&)=default;
    Node(Node&&)=default;
    Node()=default;

    Pos startPos;
    Pos endPos;
    Node(Pos startpos,Pos endPos ):startPos(startpos),endPos(endPos){};
};
struct Expr:Node{};
struct Stmt:Node{};
struct Type :public Node{
    Type()=default;
    Type(Pos startpos,Pos endPos):Node(startpos,endPos){}
};
struct AutoType:Type{};
struct ProgramNode:Node{
    std::vector<Stmt*> stmts;
};
enum primitives{
    PT_INT,
    PT_DOUBLE,
    PT_FLOAT,
    PT_STRING,
    PT_LONG
};
struct PrimitiveType:Type  {
    primitives type;
  
    PrimitiveType(primitives type ,Pos startpos=Pos(),Pos endpos=Pos()):type(type),Type(startpos,endpos){}
};

  
struct ExprStmt:Stmt  {
    Expr* expr;
};
struct RefType:Type  {
    Type* origType;
    
};

struct DeclaredType:Type  {
    std::string name;
    DeclaredType(std::string name="",Pos stpos=Pos(),Pos endpos=Pos()):name(name),Type(stpos,endpos){}

};
 

struct ArrType:Type{//[]
      Type* origType;
};
struct OptionalType:Type{
    Type* tp;
};
struct FunctionType:Type{
    std::vector<Type*> argsType;
    Type* returnType;
};


struct BlockExpr:Expr  {
    std::vector<Stmt*> stmts;
    Expr* value;
};
struct IFExpr:Expr{
   Expr* condition;
   BlockExpr* onCondTrue;
   Expr* orElse; 
};

struct DeclStmt:Stmt{
    std::string name;
};
struct VarDeclStmt:DeclStmt{
    Type* type;   
    Expr* value;
};
struct FuncDeclStmt:DeclStmt  {
    Type* returnType;
    std::vector<std::pair<std::string,Type*>> args;
    BlockExpr* block;
};
struct EditFuncDeclStmt:FuncDeclStmt{};
struct ClassDeclStmt :DeclStmt {
    bool encaps=false;
    std::vector<DeclStmt*> publicStmts;
    std::vector<DeclStmt*> privateStmts;
};
struct AssignmentStmt:Stmt  {
    Expr* target;
  
    Expr* value;
};

struct ForStmt:Stmt  {
    Expr* iterator;
    Expr* steps;
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
struct FalseExpr:Expr{};

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
 struct UnaryExpr:Expr {
    Expr* desendent;
    Expr* op;
};

struct CallExpr:Expr{//call(...args)
  Expr* callee;
  std::vector<Expr*> args;
};
struct ObjectMemberExpr:Expr  {///obj.menbername
    Expr* object;
    std::string memberName;
};
struct ObjectIdxExpr:Expr{//obj[idx]
    Expr* object;
    Expr* idx;
 };
struct IdentifierExpr:Expr  {
    std::string name;
};
struct ThisExpr:Expr  {};
struct SuperExpr:Expr{};
struct LambdaExpr:Expr{
    Type* returnType;
    std::vector<std::pair<std::string,Type>> args;
    BlockExpr* stmtBlock;
};















#endif // ASTNODE_H

