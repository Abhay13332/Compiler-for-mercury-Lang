#ifndef RECDESCPARSER_H
#define RECDESCPARSER_H
#include <astNode.hpp>
 #include <stdexcept>
#include <vector>
Expr* prattparser();
 
class RecDesParser{
  public:
    RecDesParser(std::vector<Token> tokens):tokens(tokens){}
    Token getNext(){
        if(ptr==tokens.size()+1){
            return Token{TOK_EOF,"",tokens.back().endPos,tokens.back().endPos};
        }
        if(ptr>tokens.size()) throw std::runtime_error("trying reading after EOF");
        return tokens[ptr++];
    }
    
    Token peek(){
        if(ptr==tokens.size()+1){
            return Token{TOK_EOF,"",tokens.back().endPos,tokens.back().endPos};
        }
        if(ptr>tokens.size()) throw std::runtime_error("trying reading after EOF");
        return tokens[ptr];
    }
    void advance(int steps=1){
        ptr+=steps;
    }
    int getcurrentpos(){
        return ptr;
    }
    void setposto(int pos){
        ptr=pos;
    }
    bool isEOFReached(){
         if(ptr==tokens.size()+1){
            return true;
         }
         return false;
    }
    void back(int steps=1){
      ptr-=steps;
    }
   
    FunctionType* functiontypeHandle(){
           FunctionType* tp=new FunctionType();
            
           Token tk;
           if((tk=getNext()).type!=TOK_OPEN_RD) throw std::runtime_error("open bracket not found:");
           tp->startPos=tk.startPos.addto(-1);

           
           while ((tk=getNext()).type!=TOK_CLOSE_RD) {
            tp->argsType.push_back(typeHandle());
            if(peek().type==TOK_COMMA ){
                getNext();
            }else if( peek().type!=TOK_CLOSE_BIG){
                throw std::runtime_error("unexpected token:"+tk);
            }
           }
           if(getNext().type!=TOK_ARROW) throw std::runtime_error("unexpected token");
           tp->returnType=typeHandle();
           tp->endPos=tp->returnType->endPos;
           return tp;
    }
    Type* typeHandle(){
        Token tk=getNext();
        Pos stpos=tk.startPos;
        Type* tp;
        if(tk.type==TOK_TYPE_INT){
            tp=new  PrimitiveType(PT_INT,stpos,tk.endPos) ;            
        }else if(tk.type==TOK_TYPE_STRING){
            tp= new PrimitiveType(PT_STRING,stpos,tk.endPos);
        }else if(tk.type==TOK_TYPE_DOUBLE){
            tp= new PrimitiveType(PT_DOUBLE,stpos,tk.endPos);
        }else if(tk.type==TOK_TYPE_FLOAT){
            tp= new PrimitiveType(PT_FLOAT,stpos,tk.endPos);
        }else if(tk.type==TOK_IDENTIFIER){
            tp= new DeclaredType(tk.value,stpos,tk.endPos);
        }else if(tk.type==TOK_REF){
            RefType* reftp=new RefType();
            reftp->origType=typeHandle();
            reftp->startPos=stpos;
            reftp->endPos=reftp->origType->endPos;
            tp= reftp;
        }else if(tk.type==TOK_QUESTION){
            OptionalType* opType =new OptionalType();
            opType->tp=typeHandle();
            opType->startPos=tk.startPos;
            opType->endPos=opType->tp->endPos;
            tp= opType;
        }
        else if(tk.type==TOK_NEG){
            tp= functiontypeHandle();
        }else{
            throw std::runtime_error("something unexpected token found"+tk);
        }
        if((tk=getNext()).type==TOK_OPEN_BIG){
        if((tk=getNext()).type!=TOK_CLOSE_BIG) throw std::runtime_error("something unexpected token found"+tk);
           
            ArrType* arrtp=new ArrType();
            arrtp->origType=tp;
            arrtp->endPos=tk.endPos;
            arrtp->startPos=tp->startPos;
            tp=arrtp;
        }
        return tp;

    }
    DeclStmt* declarationHandle(){
        Pos stpos=getNext().startPos;
        VarDeclStmt *decl=new VarDeclStmt();
        Token tk;
        if((tk=getNext()).type!=TOK_IDENTIFIER) throw std::runtime_error(" identifier name not present");
           decl->name=tk.value;
        if((tk=getNext()).type==TOK_COLON){
           decl->type=typeHandle();
        }else{
           decl->type=nullptr;
        }
        if(tk.type==TOK_EQUAL){
           decl->value=prattparser();
           if((tk=getNext()).type==TOK_SEMICOLON){
            decl->startPos=stpos;
            decl->endPos=tk.endPos;
            
           }else{
            std::runtime_error("unexpected token"+tk);
           }
        }else if(tk.type==TOK_SEMICOLON){
          decl->value=nullptr;
        }
        return decl;
    }
    EditFuncDeclStmt* editFunctionDelc(){
        return new EditFuncDeclStmt();
    };
    AssignmentStmt* assignmentHandle(){
        AssignmentStmt* assignStmt=new AssignmentStmt();
        assignStmt->startPos=peek().startPos;
        assignStmt->target=prattparser();
        if(getNext().type!=TOK_EQUAL){
           throw std::runtime_error("error on assignment");
        }
        assignStmt->value=prattparser();
        assignStmt->endPos=validateSemiColon();
        return assignStmt;

    };
    Pos validateSemiColon(){
        Token tk;
        if((tk=getNext()).type!=TOK_SEMICOLON) throw std::runtime_error("unexpected error"+tk);
        return tk.endPos;
    }
    ForStmt* forStmthandle(){
        ForStmt* stmt=new ForStmt();
        stmt->startPos=getNext().startPos;
        Token tk;
        if((tk=getNext()).type!=TOK_OPEN_RD) throw std::runtime_error("unexpected token:"+tk);
         stmt->iterator=prattparser();
        if((tk=getNext()).type!=TOK_COMMA) throw std::runtime_error("unexpected token:"+tk);
         stmt->steps=prattparser();
        if((tk=getNext()).type!=TOK_CLOSE_RD) throw std::runtime_error("unexpected token:"+tk);
        stmt->block=blockExprHandler();
        stmt->endPos=stmt->block->endPos;
        return stmt;
        
    };
    WhileStmt* whileStmtHandle(){
        WhileStmt* whilestmt=new WhileStmt();
        whilestmt->startPos=getNext().startPos;
        Token tk;
        if((tk=getNext()).type!=TOK_OPEN_RD) throw std::runtime_error("unexpected token:"+tk);
        whilestmt->condition=prattparser();
        if((tk=getNext()).type!=TOK_CLOSE_RD) throw std::runtime_error("unexpected token:"+tk);
        whilestmt->block=blockExprHandler();
        whilestmt->endPos=whilestmt->block->endPos;
        return whilestmt;

    };
    ReturnStmt* returnStmtHandle(){
        ReturnStmt *rtStmt=new ReturnStmt();
        rtStmt->startPos=getNext().startPos;
        rtStmt->value=prattparser();
        
        rtStmt->endPos=validateSemiColon();
        return rtStmt;
    };
    ContinueStmt* continueStmtHandle(){
        ContinueStmt *continueStmt=new ContinueStmt();
        continueStmt->startPos=getNext().startPos;
        continueStmt->endPos=validateSemiColon();
        return continueStmt;
    };
    BreakStmt* breakStmtHandle(){
        BreakStmt* breakStmt=new BreakStmt();
        breakStmt->startPos=getNext().startPos;
        breakStmt->endPos=validateSemiColon();
        return breakStmt;
    };
     bool isExpr(){
        Token tk;
        int initpos=getcurrentpos();
        while((tk=getNext()).type!=TOK_CLOSE_CUR){
         if(tk.type==TOK_SEMICOLON){
            setposto(initpos);
            return false;
         }else if(tk.type==TOK_OPEN_CUR){
            isExpr();
         }
        }
     setposto(initpos);
     return true;
    }
 
    BlockExpr* directblockHandler(){
       BlockExpr* block=new BlockExpr();
       block->startPos=peek().startPos;
       block->value=prattparser();
       block->endPos=block->value->endPos;
       return block;

    }
    IFExpr* ifexprHandle(){
        IFExpr* expr=new IFExpr();
        expr->startPos=getNext().startPos;
        Token tk;
        if((tk=getNext()).type!=TOK_OPEN_RD) throw  std::runtime_error(" condition is not present");
        expr->condition=prattparser();
        if((tk=getNext()).type!=TOK_CLOSE_RD) throw std::runtime_error(" close braces is not present");
        if(peek().type==TOK_OPEN_CUR){
            expr->onCondTrue=blockExprHandler();
        }else{
            expr->onCondTrue=directblockHandler();
        }
        if((tk=getNext()).type==TOK_ELSE){
            if(peek().type==TOK_IF){
                expr->orElse=ifexprHandle();
            }else if(peek().type==TOK_OPEN_CUR){
                expr->orElse=blockExprHandler();
            }else{
                expr->orElse=directblockHandler();
            }
            expr->endPos=expr->orElse->endPos;
        }else{
            back();
            expr->orElse=nullptr;
            expr->endPos=expr->onCondTrue->endPos;
            
        }
        return expr;
        
    };
    
    ExprStmt* exprStmtHandle(Expr* expr){
        auto exprStmt=new ExprStmt();
        exprStmt->expr=expr;
        exprStmt->startPos=expr->startPos;
        exprStmt->endPos=expr->endPos;
        return exprStmt;
    }
   
    BlockExpr* blockExprHandler(){
         BlockExpr* block=new BlockExpr();
         block->startPos=getNext().startPos;
         block->value=nullptr;
         Token tk;
         while((tk=getNext()).type!=TOK_CLOSE_CUR){
            if((tk=getNext()).type==TOK_LET){
                back();
                block->stmts.push_back(declarationHandle());
            }else if (tk.type==TOK_IF) {
               back();
               block->stmts.push_back(exprStmtHandle(ifexprHandle()));
               
            }else if(tk.type==TOK_FOR){
                back();
                block->stmts.push_back(forStmthandle());

            }else if (tk.type==TOK_WHILE) {
               back();
               block->stmts.push_back(whileStmtHandle());
            }else if(tk.type==TOK_RETURN){
                back();
               block->stmts.push_back(returnStmtHandle());
                       
            }else if(tk.type==TOK_BREAK){
                back();
               block->stmts.push_back(breakStmtHandle());

            }else if(tk.type==TOK_CONTINUE){
               back();
               block->stmts.push_back(continueStmtHandle());

            }
            else if( tk.type==TOK_NUM_LIT|| tk.type==TOK_STRING_LIT){
                back();
                block->stmts.push_back(exprStmtHandle(prattparser()));
            }else if(tk.type==TOK_IDENTIFIER){
                back();
                if(isExpr()){
                    block->value=prattparser();
                }else{
                    block->stmts.push_back(assignmentHandle());
                }  
            }
         }
         block->endPos=tk.endPos;
         return block;

         
    }

    DeclStmt* functionDeclaration(){
        FuncDeclStmt* fdecl =new FuncDeclStmt();
        Pos stpos=getNext().startPos;
        fdecl->startPos=stpos;
        Token tk=getNext();
        if(tk.type==TOK_EDIT) return editFunctionDelc();
        if(tk.type!=TOK_IDENTIFIER) throw std::runtime_error("unexpected token(Idnetifier expected):"+tk);
        fdecl->name=tk.value;
        if((tk=getNext()).type!=TOK_OPEN_RD)throw std::runtime_error("unexpected token(open Brace expected):"+tk);
        while((tk=getNext()).type!=TOK_CLOSE_RD){
            if(tk.type!=TOK_IDENTIFIER) throw std::runtime_error("unexpected token(Idnetifier expected):"+tk);
            auto name=tk.value;
            if((tk=getNext()).type!=TOK_COLON) throw std::runtime_error("unexpected token(COLON expected):"+tk);
            auto *type=typeHandle();
            fdecl->args.emplace_back(name,type);
            if((tk=getNext()).type==TOK_CLOSE_BIG){
                 break;
            }else if(tk.type!=TOK_COMMA){
               throw std::runtime_error("unexpected TOKEN(COMMA or CLOSE BRACES EXPECTED):"+tk);
            }            

        }
        if((tk=getNext()).type==TOK_ARROW){
            fdecl->returnType=typeHandle();
        }else{
             fdecl->returnType=nullptr;
        }
        if((tk=getNext()).type==TOK_OPEN_CUR) {
         back();
         fdecl->block=blockExprHandler();
         fdecl->endPos=fdecl->block->endPos;
         return fdecl;
        }else {
          back();
          fdecl->block=directblockHandler();
          if((tk=getNext()).type!=TOK_SEMICOLON)throw std::runtime_error("unexpected TOKEN(semiolon  expected):"+tk);
          fdecl->endPos=tk.endPos;
          return fdecl;
        }



    }
    DeclStmt* classhandle(){
        ClassDeclStmt* stmt=new ClassDeclStmt();
        Token tk;
        stmt->startPos=peek().startPos;
        if((tk=getNext()).type==TOK_ENCAPS) {
           if((tk=getNext()).type!=TOK_CLASS) throw std::runtime_error("unexpected token");
           stmt->encaps=true;
        }else if((tk).type!=TOK_CLASS) throw std::runtime_error("unexpected token");
        
        if((tk=getNext()).type!=TOK_IDENTIFIER) throw std::runtime_error("unexptec token");
        stmt->name=tk.value;
        if((tk=getNext()).type!=TOK_OPEN_CUR) throw std::runtime_error("unexptec token");
        bool vispub=false;
        while((tk=getNext()).type!=TOK_CLOSE_CUR){
            if(peek().type==TOK_LET){
               if(vispub)stmt->publicStmts.push_back(declarationHandle());
               else stmt->privateStmts.push_back(declarationHandle());
            }else if(peek().type==TOK_NEG){
                  if(vispub)stmt->publicStmts.push_back(functionDeclaration());
               else stmt->privateStmts.push_back(functionDeclaration());
            }else if(peek().type==TOK_ENCAPS || peek().type==TOK_CLASS){
                 if(vispub)stmt->publicStmts.push_back(classhandle());
                 else stmt->privateStmts.push_back(functionDeclaration());
            }else if(peek().type==TOK_PRIVATE){
               advance();
               if((tk=getNext()).type!=TOK_COLON) throw std::runtime_error("unexpected error");
               vispub=false;
            }else if(peek().type==TOK_PUB){
               advance();
               if((tk=getNext()).type!=TOK_COLON) throw std::runtime_error("unexpected error");
               vispub=true;
            }else{
                throw std::runtime_error("something unexpected");
            } 
        }
        stmt->endPos=tk.endPos;
        return stmt;
    }
    ProgramNode parse(){
        ProgramNode program;
        Token tk;
        while((tk=getNext()).type!=TOK_EOF){
            if(tk.type==TOK_LET){
                back();
                program.stmts.push_back(declarationHandle());
            }else if(tk.type==TOK_NEG){
                back();
                program.stmts.push_back(functionDeclaration());
            }else if(tk.type==TOK_ENCAPS || tk.type==TOK_CLASS){
                back();
                program.stmts.push_back(classhandle());
            }else{

                throw std::runtime_error("not known Global token:"+tk);
            }
        }
        program.startPos=Pos{0,0};
        program.endPos=Pos(program.stmts.back()->endPos);
        return (program);  

    }

    private:
    int ptr=0;
    std::vector<Token>  tokens;

};

#endif // RECDESCPARSER_H