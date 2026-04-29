#ifndef PRATT_PARSER_H
#define PRATT_PARSER_H

#include "magic_enum.hpp"
#include "token.hpp"
#include "astNode.hpp"

#include <string>
#include <sys/types.h>
#include<vector>
#include<unordered_map>
#include<functional>
#include<stdexcept>

Expr* lambdaExprpr();
Expr* ifElsePr();

class PrattParser {
    using NudFunc = std::function<Expr*(Token)>;
    using LedFunc = std::function<Expr*(Token,Expr*)>;

    std::vector<Token> tokens;
    int pos = 0;

    std::unordered_map<tokenT,NudFunc> nud; //parse token at start of expression
    std::unordered_map<tokenT,LedFunc> led; //parse token with left operand
    std::unordered_map<tokenT,int> bp; //binding power or operator precedence

    public:

        PrattParser(const std::vector<Token>& t):tokens(t){
            registerRules();

        }

        Token& peek() {
            if (pos >= tokens.size()) return tokens.back(); // or EOF token
            return tokens[pos];
        }

        Token advance(){
            return tokens[pos++];
        }

        bool match(tokenT t){
            if(peek().type == t){
                advance();
                return true;
            }
            return false;
        }

        Expr* parseExpression(int rbp=0){
 
            Token t = advance(); // take token from the string in advance
            auto nudIt = nud.find(t.type);
            // std::cout<<nud.contains(t.type)<<std::endl;
            // if(nudIt == nud.end()) throw std::runtime_error("No prefix rule for this token: " + std::to_string((int)t.type));
            
           
            if(nudIt == nud.end()) {
                std::cout << "ERROR TOKEN TYPE: " << magic_enum::enum_name<tokenT>(t.type) << " VALUE: " << t.value << std::endl;

                throw std::runtime_error("No prefix rule");
            }
            
            Expr* left = nudIt->second(t); // call the nud function to parse the token as a prefix expression
            while(pos < tokens.size()){ // while there are more tokens and the binding power of the next token is greater than the right binding power
                int nextBp = bp.contains(peek().type)? bp[peek().type] : 0; // get the binding power of the next token
                if(rbp >= nextBp) break; 
                t = advance(); // consume the next token

                auto ledIt = led.find(t.type);
                if(ledIt == led.end()) throw std::runtime_error("No infix rule for this token: ");
                left = ledIt->second(t,left); // call the led function to parse the token as an infix expression with left as the left operand
            }
            return left;
        }
    private:
        void registerRules(){

            // literals

            nud[TOK_NUM_LIT] = [&](Token t){
                auto node = new literals::IntExpr();  
                node->value = std::stoi(t.value);
                return (Expr*)node;
            };

            nud[TOK_STRING_LIT] = [&](Token t){
                auto node = new literals::StringExpr();
                node->value = t.value;
                return (Expr*)node;
            };

            // Boolean literals

            nud[TOK_TRUE] = [&](Token){
                return (Expr*)new TrueExpr();
            };

            nud[TOK_FALSE] = [&](Token){
                return (Expr*)new FalseExpr();
            };

            // identifiers

            nud[TOK_IDENTIFIER] = [&](Token t){
                auto node = new IdentifierExpr();
                node->name = t.value;
                return (Expr*)node;
            };

            // for parenthesis

            nud[TOK_OPEN_RD] = [&](Token t){
                Expr* e = parseExpression();
                if(!match(TOK_CLOSE_RD)) throw std::runtime_error("expected )");
                return e;
            };

            // for unary operators

            nud[TOK_NEG] = [&](Token t){
                auto node = new UnaryExpr();
                node->op = U_NEG;
                node->isPrefix = true;
                node->desendent = parseExpression(70);
                return (Expr*)node;
            };
             
            nud[TOK_UNARY_OP] = [&](Token t){
                auto node = new UnaryExpr();
                node->isPrefix = true;
                node->desendent = parseExpression(70);
                if(t.value == "++") node->op=U_INC;
                if(t.value == "--") node->op=U_DEC;
                if(t.value == "~") node->op=U_BIT_NOT;
                return (Expr*)node;
            };

            // Binary Power 

            bp[TOK_DOT] = 90;
            bp[TOK_OPEN_RD] = 85;
            bp[TOK_OPEN_BIG] = 85;
            bp[TOK_UNARY_OP] = 80;

            bp[TOK_ARTH_OP] = 50;
            bp[TOK_RELATION_OP] = 40;
            bp[TOK_LOGIC_OP] = 30; 

            // Binary

            led[TOK_ARTH_OP] = [&](Token t,Expr* left){
                auto node = new BinaryExpr();
                node->left = left;
                node->right = parseExpression(50);

                if(t.value == "+") node->op=B_ADD;
                if(t.value == "-") node->op=B_SUB;
                if(t.value == "*") node->op=B_MUL;
                if(t.value == "/") node->op=B_DIV;

                return (Expr*)node;
            };

            // Member access

            led[TOK_DOT] = [&](Token,Expr* left){
                Token name = advance();

                if(name.type != TOK_IDENTIFIER) throw std::runtime_error("Expected identifier after .");
                auto node = new ObjectMemberExpr();
                node->object = left;
                node->memberName = name.value;

                return (Expr*)node;
            };

            // function call

            led[TOK_OPEN_RD] = [&](Token,Expr* left){
                auto node = new CallExpr();
                node->callee = left;

                if(!match(TOK_CLOSE_RD)){
                    do{
                        node->args.push_back(parseExpression());
                    }
                    while(match(TOK_COMMA));
                    if(!match(TOK_CLOSE_RD)) throw std::runtime_error("Expected ) after function arguments");
                }
                return (Expr*)node;
            };

            // Array Index accessing

            led[TOK_OPEN_BIG] = [&](Token,Expr* left){

                auto node = new ObjectIdxExpr();
                node->object = left;
                Expr* idx = parseExpression();

                node->idx = idx; // store the index expression in the node

                if(!match(TOK_CLOSE_BIG)) throw std::runtime_error("Expected ] after array index");

                return (Expr*)node;

            };

            // postfix ++ / --

            led[TOK_UNARY_OP] = [&](Token t,Expr* left){
                if(t.value != "++" && t.value != "--") {
                    throw std::runtime_error("Invalid postfix unary operator: " + t.value);
                }

                auto node = new UnaryExpr();
                node->desendent = left;
                node->isPrefix = false;
                if(t.value == "++") node->op = U_INC;
                if(t.value == "--") node->op = U_DEC;
                return (Expr*)node;
            };
        }
};

#endif