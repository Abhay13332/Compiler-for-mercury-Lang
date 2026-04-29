#ifndef LLVM_GEN_H
#define LLVM_GEN_H

#include <llvm/ADT/APInt.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <astNode.hpp>
class ASTVisitor {
public:
   virtual llvm::Value* visit(BlockExpr* node)=0;
   virtual llvm::Value* visit(IFExpr* node)=0;
   virtual llvm::Value* visit(VarDeclStmt* node)=0;
   virtual llvm::Value* visit(FuncDeclStmt* node)=0;
   virtual llvm::Value* visit(ClassDeclStmt* node)=0;
   virtual llvm::Value* visit(AssignmentStmt* node)=0;
   virtual llvm::Value* visit(ForStmt* node)=0;
   virtual llvm::Value* visit(WhileStmt* node)=0;
   virtual llvm::Value* visit(ReturnStmt* node)=0;
   virtual llvm::Value* visit(ContinueStmt* node)=0;
   virtual llvm::Value* visit(BreakStmt* node)=0;
   virtual llvm::Value* visit(TrueExpr* node)=0;
   virtual llvm::Value* visit(FalseExpr* node)=0;
   virtual llvm::Value* visit(literals::DoubleExpr* node)=0;
   virtual llvm::Value* visit(literals::FloatExpr* node)=0;
   virtual llvm::Value* visit(literals::IntExpr* node)=0;
   virtual llvm::Value* visit(literals::LongExpr* node)=0;
   virtual llvm::Value* visit(literals::StringExpr* node)=0;

   virtual llvm::Value* visit(BinaryExpr* node)=0;
   virtual llvm::Value* visit(UnaryExpr* node)=0;
   virtual llvm::Value* visit(CallExpr* node)=0;
   virtual llvm::Value* visit(ObjectMemberExpr* node)=0;
   virtual llvm::Value* visit(ObjectIdxExpr* node)=0;
   virtual llvm::Value* visit(IdentifierExpr* node)=0;
   virtual llvm::Value* visit(ThisExpr* node)=0;
   virtual llvm::Value* visit(SuperExpr* node)=0;
   virtual llvm::Value* visit(LambdaExpr* node)=0;
  

};
class IRGEN:public ASTVisitor{
      llvm::LLVMContext context;
      llvm::IRBuilder<> builder;
    std::unique_ptr<llvm::Module> module;
    
    // Simple Symbol Table: maps variable names to their memory location (alloca)
    std::map<std::string, llvm::Value*> namedValues;

public:
    IRGEN() : builder(context) {
        module = std::make_unique<llvm::Module>("my_compiler", context);
    }
    llvm::Value* visit(literals::IntExpr *node){
        return llvm::ConstantInt::get(context,llvm::APInt(32,node->value,true));
    }
};


#endif // LLVM_GEN_H