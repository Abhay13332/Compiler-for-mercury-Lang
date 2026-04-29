#include<token.hpp>
#include<prattParser.hpp>
int main(){
  Tokenizer tokenizer("/home/abhay/project/compiler/tests/pratt/t3");
    PrattParser parser(tokenizer.tokenize());
  Expr* root = parser.parseExpression();
  return 0;

};