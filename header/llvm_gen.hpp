#ifndef LLVM_GEN_H
#define LLVM_GEN_H

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <astNode.hpp>
class ASTVisitor {
public:
   virtual llvm::Value* visit(BlockExpr* expr)=0;
   virtual llvm::Value* visit(IFExpr* expr)=0;
   virtual llvm::Value* visit(VarDeclStmt* expr)=0;
   virtual llvm::Value* visit(FuncDeclStmt* expr)=0;
   virtual llvm::Value* visit(ClassDeclStmt* expr)=0;
   virtual llvm::Value* visit(AssignmentStmt* expr)=0;
   virtual llvm::Value* visit(ForStmt* expr)=0;
   virtual llvm::Value* visit(WhileStmt* expr)=0;
   virtual llvm::Value* visit(ReturnStmt* expr)=0;
   virtual llvm::Value* visit(ContinueStmt* expr)=0;
   virtual llvm::Value* visit(BreakStmt* expr)=0;
   virtual llvm::Value* visit(TrueExpr* expr)=0;
   virtual llvm::Value* visit(FalseExpr* expr)=0;
   virtual llvm::Value* visit(literals::DoubleExpr* expr)=0;
   virtual llvm::Value* visit(literals::FloatExpr* expr)=0;
   virtual llvm::Value* visit(literals::IntExpr* expr)=0;
   virtual llvm::Value* visit(literals::LongExpr* expr)=0;
   virtual llvm::Value* visit(literals::StringExpr* expr)=0;

   virtual llvm::Value* visit(BinaryExpr* expr)=0;
   virtual llvm::Value* visit(UnaryExpr* expr)=0;
   virtual llvm::Value* visit(CallExpr* expr)=0;
   virtual llvm::Value* visit(ObjectMemberExpr* expr)=0;
   virtual llvm::Value* visit(ObjectIdxExpr* expr)=0;
   virtual llvm::Value* visit(IdentifierExpr* expr)=0;
   virtual llvm::Value* visit(ThisExpr* expr)=0;
   virtual llvm::Value* visit(SuperExpr* expr)=0;
   virtual llvm::Value* visit(LambdaExpr* expr)=0;
  

};
class IRGEN:public ASTVisitor{
      llvm::LLVMContext context;
      llvm::IRBuilder<> builder;
      
};


#endif // LLVM_GEN_H