//
//  compilation-engine.cpp
//  jack-compiler
//
//  Created by Yuvraj Sakshith on 28/12/22.
//

#include "compilation-engine.hpp"



compilation_engine::compilation_engine(tokenizer * T, symbol_table * ST, vm_writer * VMW){
    filename = T->getCurrentFilename();
    tok = T;
    sym = ST;
    vmw = VMW;
}

void compilation_engine::compilation_engine_end(){
    
    for(auto vm_line : vmw->get_vm_buffer()){ // store the vm code into the VMContent vector.
        string line;
        for(string vm_element : vm_line){
            line += vm_element;
        }
        VMContent.push_back(line);
    }
    vmw->flush_vm_writer();
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
    sym->reset_class_table(); // new class, new class-level symbol table
    
    tok->advance(); // className
    tok->advance(); // {
    tok->advance(); // classVarDec*
    
    while(tok->getCurrentToken() == "field" or tok->getCurrentToken() == "static") compileClassVarDec(); // classVarDec*
    while(tok->getCurrentToken() == "constructor" or tok->getCurrentToken() == "function" or tok->getCurrentToken() == "method") compileSubroutineDec(); // subroutineDec*
    
    tok->advance(); // }
    
}

void compilation_engine::compileClassVarDec(){ // ('static | 'field') type varName (','varName)* ';'
    
    int currentKind = NONE; // kind of variable
    string current_type; // type of variable
    
    if(tok->getCurrentToken() == "static") currentKind = STATIC;
    if(tok->getCurrentToken() == "field")  currentKind = FIELD;
    
    
    tok->advance();
    
    current_type = tok->getCurrentToken();
    
    tok->advance();
    
    string varName = tok->getCurrentToken();
    // add variable to the symbol_table
    sym->define(varName, current_type, currentKind);
    
    tok->advance();
    
    while(tok->getCurrentToken() != ";"){ // process the list of variables
        tok->advance(); // ','
        
        varName = tok->getCurrentToken();
        
        tok->advance();
        
        // add variable to the symbol_table
        sym->define(varName, current_type, currentKind);
    }
    
    tok->advance(); // ';'
    
}


void compilation_engine::compileSubroutineDec(){ // ('constructor' | 'function' | 'method') ('void' | type) subroutineName '(' parameterList ')' subroutineBody
    
    // new subroutine, new subroutine-level symbol-table.
    sym->reset_subroutine_table();
    
    
    
    
}

void compilation_engine::compileSubroutineBody(){ // '{' varDec* statements '}'
    
    
}

void compilation_engine::compileVarDec(){ // 'var' type varName (',', varName)*';'
    
    tok->advance(); // var
    
    string current_type = tok->getCurrentToken(); // type
    
    tok->advance(); // varName
    
    string varName = tok->getCurrentToken();
    
    sym->define(varName, current_type, VAR);
    
    tok->advance();
    
    while(tok->getCurrentToken() != ";"){
        tok->advance(); // ','
        
        varName = tok->getCurrentToken(); // varName;
        
        sym->define(varName, current_type, VAR);
        
        tok->advance();
        
    }
    
    tok->advance(); // ';'
    
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
    
    // we will first store the terms and the operators, later we will compile them in the postfix fashion.
    
    vector<string> terms;
    vector<char> ops;
    string current_term;
    
    
    
}

void compilation_engine::compileTerm(){ // integerConstant | stringConstant | keywordConstant | varName | varName '[' expression ']' | subroutineCall | '(' expression ')' | unaryOp term
    
    
     
}

void compilation_engine::compileExpressionList(){ // (expression (',' expression)*)?
    
   
}

void compilation_engine::compileSubroutineCall(){ // subroutineName '(' expressionList ')' | (className | varName) '.' subroutineName '(' expressionList ')'
    
    
}

void compilation_engine::compileParameterList(){ // ((type varName) (',' type VarName)*)?
    
    // these are going to be your arguments in a subroutine call, hence we shall add them to the symbol table as ARG variables
    
    if(tok->getCurrentToken() == ")") return; // no parameters provided.
    
    string current_type = tok->getCurrentToken();
    tok->advance();
    
    string varName = tok->getCurrentToken();
    tok->advance();
    
    sym->define(varName, current_type, ARG);
    
    while(tok->getCurrentToken() != ")"){
        tok->advance(); // ,
        
        current_type = tok->getCurrentToken();
        tok->advance();
        
        varName = tok->getCurrentToken();
        tok->advance();
        
        sym->define(varName, current_type, ARG);
    }
    
    
}
