#include <cstdio>
#include <fstream>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include<bits/stdc++.h>
#include <sys/types.h>
#include <vector>
#include<magic_enum.hpp>
#ifndef TOKEN_HPP
#define TOKEN_HPP

enum tokenT{ 
 
  TOK_ARTH_OP,//+,-,*,/,%, **
  TOK_RELATION_OP,// >,<,>=,<=,!=,==
  TOK_BINARY_OP,//&,|,^,~ 
  TOK_LOGIC_OP,//,&&,||
  TOK_DOT,//yes
  TOK_COLON,//yes
  TOK_COMMA,//yes
  TOK_EQUAL,//yes
  TOK_NEG,//yes
  TOK_SEMICOLON,//yes
  TOK_AT,//yes
  TOK_OPEN_RD,//yes
  TOK_CLOSE_RD,//yes
  TOK_OPEN_CUR,//yes
  TOK_CLOSE_CUR,//yes
  TOK_OPEN_BIG,//yes
  TOK_ARROW,// \/\/
  TOK_CLOSE_BIG,
  TOK_IDENTIFIER,
  TOK_NUM_LIT,
  TOK_STRING_LIT, 
  TOK_IF,
  TOK_ELSE,
  TOK_LET,
  TOK_FOR,
  TOK_WHILE,
  TOK_CLASS,
  TOK_ENCAPS,
  TOK_TYPE_INT,
  TOK_TYPE_FLOAT,
  TOK_TYPE_DOUBLE,
  TOK_TYPE_STRING,
  TOK_RETURN,
  TOK_TRUE,
  TOK_FALSE,
  TOK_CONTINUE,
  TOK_BREAK,
  TOK_NONE,
  TOK_THIS,
  TOK_REF,
  TOK_VOID,
  TOK_MEM_ALLOC,
  TOK_PRIVATE,
  TOK_PUB,
  TOK_EDIT,
  TOK_EOF,
  TOK_QUESTION



};
struct Pos{
   int line;
   int pos;
   Pos addto(int pos=0,int line=0){
      Pos cp=*this;
      cp.line+=line;
      cp.pos+=pos;
      return cp;
   };

};
class Token{
  public:
    
    tokenT type;
    std::string value;
    Pos startPos;
    Pos endPos;
    Token(tokenT type,std::string value,Pos posstart,Pos posend):type(type),value(value ),startPos(posstart),endPos(posend){}
    Token()=default;
   friend std::ostream& operator<<(std::ostream& os, const Token& obj);
   friend std::string operator+(const std::string& str,const Token& tkn){
      std::string fin;
      fin.append(str);
      fin.append(magic_enum::enum_name<tokenT>(tkn.type));
      fin.append(":"+tkn.value);
      return fin;
   }
   friend std::string operator+(const Token& tkn,const std::string& str){
      std::string fin;
      fin.append(magic_enum::enum_name<tokenT>(tkn.type));
      fin.append(":"+tkn.value);
      fin.append(str);
      return fin;
   }

};
std::ostream& operator<<(std::ostream& os, const Token& obj){
          os<<magic_enum::enum_value<tokenT>(obj.type)<<":"<<obj.value;
        return os; }
class Tokenizer{
std::ifstream file;
int lineno=0;
public:
Tokenizer(std::string name):file(name){

     if (!file.is_open()) {
        std::cerr << "Error: Could not open file." << std::endl;
    }

}
~Tokenizer(){
    file.close();


}
private:
char peekValid(std::string msg="invalid string literal"){
    int ch;
    if((ch=file.peek())==EOF){
       throw std::runtime_error(msg);
    }
    return ch;
}
char getValid(std::string msg="invalid string literal"){
    int ch;
    if((ch=file.get())==EOF){
       throw std::runtime_error(msg);
    }
    return ch;
}
Pos getCurrentpos(int back=0){
  return Pos{.line=lineno,.pos=static_cast<int>(file.tellg())-back};
}
template<int steps=1>
void moveBack(){
    static_assert(steps>0,"sdfg");
    file.clear();
    file.seekg(-steps,std::ios::cur);
    if(file.fail()){
        std::runtime_error("invalid seek");
    }
    if(file.tellg()==std::streampos(-1)){
        std::runtime_error("invalid seek");
    }
    }
std::vector<Token> numericLiteral(){
   moveBack();
   Pos startPos=getCurrentpos();
   std::string tempData;
   std::vector<Token> result;
   char ch;
   while(file.get(ch)){
      if(ch>='0'&& ch<='9'){
         tempData+=ch;
      }else if(ch=='.'){
         if(peekValid()=='.'){
           getValid();
           result.emplace_back(TOK_NUM_LIT,tempData,startPos,getCurrentpos(3));
           result.emplace_back(TOK_DOT,std::string(),getCurrentpos(2),getCurrentpos(2));
           result.emplace_back(TOK_DOT,std::string(),getCurrentpos(1),getCurrentpos(1));
           tempData="";
         }else{
            tempData+='.';
         }
      }else{
         moveBack();

         result.emplace_back(TOK_NUM_LIT,tempData,startPos,getCurrentpos(1));
         break;
      }
   }
   return result;
}
Token stringLiteral(){
   auto startpos=getCurrentpos(1);
   std::string data;
   bool close=false;
   bool ismultiline=false;
   if(file.get()=='"' ){
    if(file.get()=='"'){

        ismultiline=true;
    }else{
    moveBack();
    }
   }else{
        moveBack();

   }
   char ch;
   while(file.get(ch)){
     if(ch=='\\'){
        char chnext=getValid();
        switch (chnext) {
          case 't':data+='\t';break;
          case 'n':data+='\n';break;
          case 'r':data+='\r';break;
          case 'b':data+='\b';break;
          case '0':data+='\0';break;
          case '"':data+='\"';break;
          case '\\':data+='\\';break;   
        }
     }else if(ch=='\n'&& (!ismultiline)){
        throw std::runtime_error("string not ended");   
     }else if(ch== '"'){
       if(!ismultiline){
        close=true;
        break;
       }else if(file.get()=='"' && file.get()=='"'){
        close=true;
        break;
       }else{
        throw std::runtime_error("multiline not closed properly");
       }
     }else{
        data+=ch; 
     }
   }
   if(!close){
    throw std::runtime_error("string not closed");
   }

return Token{TOK_STRING_LIT,data,startpos,getCurrentpos(1)};
}
Token identifier(){
   moveBack();
   Pos startpos=getCurrentpos();
   std::string identifier;
   char ch;
   while(file.get(ch)){
        if((ch>='0' && ch<='9' )||(ch>='a'&& ch<='z')|| (ch>='A'&& ch<='Z')|| ch=='_'){
            identifier+=ch;
        }else{
         moveBack();
         break;
        }
   }
   const static std::vector<std::string> predefs={"if","else","let","for","while","class","encaps","int","float","double","string","return","true","false","void","continue","break","none","this","ref","pub","private","mem_alloc","edit"};
   const static std::vector<tokenT> predefTs={TOK_IF,TOK_ELSE,TOK_LET,TOK_FOR,TOK_WHILE, TOK_CLASS,TOK_ENCAPS,TOK_TYPE_INT,TOK_TYPE_FLOAT,TOK_TYPE_DOUBLE,TOK_TYPE_STRING,TOK_RETURN,TOK_TRUE,TOK_FALSE,TOK_VOID,TOK_CONTINUE,TOK_BREAK,TOK_NONE,TOK_THIS,TOK_REF,TOK_PUB,TOK_PRIVATE,TOK_MEM_ALLOC,TOK_EDIT};
   for(const  auto &[predef,predefT]:std::views::zip(predefs,predefTs)){
      if(predef==identifier){
         return Token(predefT,"",startpos,getCurrentpos(1));
      }
   }
   return Token(TOK_IDENTIFIER,identifier,startpos,getCurrentpos());
}
public:
std::vector<Token> tokenize(){
    std::vector<Token> tkn;
    char ch;
    
    while(file.get(ch)){
      Pos currPos=getCurrentpos(1);
     if(ch=='"'){
      tkn.push_back(stringLiteral());
     }else if(ch=='+'||ch=='/'||ch=='^'||ch=='%'||ch=='~'){
        tkn.emplace_back((ch=='^'||ch=='~')?TOK_BINARY_OP:TOK_ARTH_OP,std::string(1,ch),currPos,currPos);
     }else if(ch=='-'){
      if(peekValid()=='>'){
       getValid();
       tkn.emplace_back(TOK_ARROW,std::string(),currPos,currPos);
      }else{
         tkn.emplace_back(TOK_ARTH_OP,"-",currPos,currPos);
      }
     }else if (ch=='>'||ch=='<') {
      if(peekValid()=='='){
         getValid();
         tkn.emplace_back(TOK_RELATION_OP,ch=='>'?">=":"<=",currPos,getCurrentpos(1));
      }else{
         tkn.emplace_back(TOK_RELATION_OP,std::string(1,ch),currPos,currPos);
      }
     
     }
     else if(ch=='*'|| ch=='&'|| ch=='|'){
         if(peekValid()==ch){
            getValid();
            tkn.emplace_back(ch=='*'?TOK_ARTH_OP:TOK_LOGIC_OP,std::string(2,ch),currPos,getCurrentpos(1));
         }else{
            tkn.emplace_back(ch=='*'?TOK_ARTH_OP:TOK_BINARY_OP,std::string(2,ch),currPos,currPos);

         }
     }
     else if(ch=='.')tkn.emplace_back(TOK_DOT,std::string(),currPos,currPos);
     else if(ch==':')tkn.emplace_back(TOK_COLON,std::string(),currPos,currPos);
     else if(ch==';')tkn.emplace_back(TOK_SEMICOLON,std::string(),currPos,currPos);
     else if(ch=='?')tkn.emplace_back(TOK_QUESTION,std::string(),currPos,currPos);

     else if(ch=='!'){
       if(peekValid()=='='){
         getValid();
         tkn.emplace_back(TOK_RELATION_OP,"!=",currPos,getCurrentpos(1));

      }else{

         tkn.emplace_back(TOK_NEG,std::string(),currPos,currPos);}
      }
     else if(ch=='='){
       if(peekValid()=='='){
         getValid();
         tkn.emplace_back(TOK_RELATION_OP,"==",currPos,getCurrentpos(1));

      }else{
        tkn.emplace_back(TOK_EQUAL,std::string(),currPos,currPos);
      }}
     else if(ch==',')tkn.emplace_back(TOK_COMMA,std::string(),currPos,currPos);
     else if(ch=='@')tkn.emplace_back(TOK_AT,std::string(),currPos,currPos);
     else if(ch=='(')tkn.emplace_back(TOK_OPEN_RD,std::string(),currPos,currPos);
     else if(ch=='{')tkn.emplace_back(TOK_OPEN_CUR,std::string(),currPos,currPos);
     else if(ch=='[')tkn.emplace_back(TOK_OPEN_BIG,std::string(),currPos,currPos);
     else if(ch==')')tkn.emplace_back(TOK_CLOSE_RD,std::string(),currPos,currPos);
     else if(ch=='}')tkn.emplace_back(TOK_CLOSE_CUR,std::string(),currPos,currPos);
     else if(ch==']')tkn.emplace_back(TOK_CLOSE_BIG,std::string(),currPos,currPos);
     else if(ch>='0'&& ch<='9'){
      std::vector<Token> result=numericLiteral();
      tkn.insert(tkn.end(),result.begin(),result.end());
     }
     else if(ch=='\n'){
      lineno++;
     }else if(ch=='\t'||ch==' '||ch=='\r'){}
     else if((ch>='a'&& ch<='z')|| (ch>='A'&& ch<='Z')|| (ch=='_')){
        tkn.push_back(identifier());
     }


    }


    return tkn;
}
};
#endif