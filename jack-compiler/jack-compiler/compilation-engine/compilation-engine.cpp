//
//  compilation-engine.cpp
//  jack-compiler
//
//  Created by Yuvraj Sakshith on 28/12/22.
//

#include "compilation-engine.hpp"



compilation_engine::compilation_engine(tokenizer T){
    filename = T.getCurrentFilename();
    tok = &T;
    compilation_engine_begin();
}

void compilation_engine::dumpVM(){
    
    // remove .jack from the filename;
    
    string outputFile = "";
    for(auto c: filename){
        if(c == '.') break;
        outputFile += c;
    }
    outputFile += ".vm";
    ofstream outputFileStream(outputFile);
    for(auto line : VMContent){
        outputFileStream << line << '\n';
    }
    
    outputFileStream.close();
    
}

inline void compilation_engine::addCurrentToken(){
    
}

vector<string> compilation_engine::getVMContent(){
    return VMContent;
}

void compilation_engine::compilation_engine_begin(){
    
    // we assume that all .jack files start with a class declaration.
    // hence, its enough to just process class declarations.
    
    while(tok->hasMoreTokens()){
        if(tok->getCurrentToken() == "class"){
            compileClass();
        }
        tok->advance();
    }
    
}

void compilation_engine::compileClass(){ //class : 'class' className { classVarDec*, subroutineDec* }
    
}

void compilation_engine::compileClassVarDec(){ // ('static | 'field') type varName (','varName)* ';'
    
   
    
}

void compilation_engine::compileType(){
    
    // 'int' | 'char' | 'boolean' | className
    
}

void compilation_engine::compileSubroutineDec(){ // ('constructor' | 'function' | 'method') ('void' | type) subroutineName '(' parameterList ')' subroutineBody
    
    
}

void compilation_engine::compileSubroutineBody(){ // '{' varDec* statements '}'
    
    
}

void compilation_engine::compileVarDec(){ // 'var' type varName (',', varName)*';'
    
    
    
    
}

void compilation_engine::compileStatements(){ // statement*
    
    
}

void compilation_engine::compileStatement(){ // if | while | let | do | return
    
    
    if(tok->getCurrentToken() == "if") compileIf();
    if(tok->getCurrentToken() == "while") compileWhile();
    if(tok->getCurrentToken() == "let") compileLet();
    if(tok->getCurrentToken() == "do") compileDo();
    if(tok->getCurrentToken() == "return") compileReturn();
}

void compilation_engine::compileIf(){ // 'if' '(' expression ')' '{' statements '}' ('else' '{' statements '}')?
    
   
    
}

void compilation_engine::compileWhile(){ // 'while' '(' expression ')' '{' statements '}'
    
  
    
}

void compilation_engine::compileLet(){ // 'let' varName ('[' expression ']')? '=' expression ';'
    

    
}

void compilation_engine::compileDo(){ // do subroutineCall ';'
    
    
}

void compilation_engine::compileReturn(){ // return expression?';'
    
    
    
}

void compilation_engine::compileExpression(){ // term (op term)*
    
}

void compilation_engine::compileTerm(){ // integerConstant | stringConstant | keywordConstant | varName | varName '[' expression ']' | subroutineCall | '(' expression ')' | unaryOp term
    
    
     
}

void compilation_engine::compileExpressionList(){ // (expression (',' expression)*)?
    
   
}

void compilation_engine::compileSubroutineCall(){ // subroutineName '(' expressionList ')' | (className | varName) '.' subroutineName '(' expressionList ')'
    
    
}

void compilation_engine::compileParameterList(){ // ((type varName) (',' type VarName)*)?
    
    
}

