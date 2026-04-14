#include<token.hpp>
int main(){
  Tokenizer tokenizer("../pratt.test");
  for(const auto& tkn:tokenizer.tokenize()){
   if(tkn.type==TOK_IDENTIFIER ||tkn.type==TOK_NUM_LIT || tkn.type==TOK_STRING_LIT){
   std::cout << magic_enum::enum_name(tkn.type)<<"("<<tkn.value<<") ";

   }else
   std::cout << magic_enum::enum_name(tkn.type)<<" ";
  }


};