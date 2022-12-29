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
    
    if(tok->getCurrentToken() == "constructor"){
        tok->advance();
        
        
        
    }else if(tok->getCurrentToken() == "function"){
        
    }else if(tok->getCurrentToken() == "method"){
        
    }
    
    
    
    
}

void compilation_engine::compileSubroutineBody(){ // '{' varDec* statements '}'
    compileVarDec();
    compileStatements();
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
    
    while(   tok->getCurrentToken() == "if"
          or tok->getCurrentToken() == "while"
          or tok->getCurrentToken() == "let"
          or tok->getCurrentToken() == "do"
          or tok->getCurrentToken() == "return"
          )
    {
        compileStatement();
    }
}

void compilation_engine::compileStatement(){ // if | while | let | do | return
    
    
    if(tok->getCurrentToken() == "if") compileIf();
    if(tok->getCurrentToken() == "while") compileWhile();
    if(tok->getCurrentToken() == "let") compileLet();
    if(tok->getCurrentToken() == "do") compileDo();
    if(tok->getCurrentToken() == "return") compileReturn();
}

void compilation_engine::compileIf(){ // 'if' '(' expression ')' '{' statements '}' ('else' '{' statements '}')?
    
    tok->advance(); // if
    tok->advance(); // (
    compileExpression(); // expression
    tok->advance(); // )
    tok->advance(); // {
    
    
    vmw->writeIf("IF_" + to_string(if_else_label_count)); // if-goto IF_x
    vmw->writeGoto("ELSE_" + to_string(if_else_label_count)); // goto ELSE_x
    
    vmw->writeLabel("IF_" + to_string(if_else_label_count)); // label for the if statement's body
    compileStatements(); // statements
    vmw->writeGoto("IF_END_" + to_string(if_else_label_count)); // continue with the code
    tok->advance(); // }
    
    if(tok->getCurrentToken() == "else"){
        tok->advance(); // else
        tok->advance(); // {
        
        vmw->writeLabel("ELSE_" + to_string(if_else_label_count));
        compileStatements();
        vmw->writeGoto("IF_END_" + to_string(if_else_label_count)); // continue with the code
        tok->advance(); // }
    }
    
    
    vmw->writeLabel("IF_END_" + to_string(if_else_label_count));// label to jump to the rest of the code
    if_else_label_count++;
}

void compilation_engine::compileWhile(){ // 'while' '(' expression ')' '{' statements '}'
    
    tok->advance(); //  while
    tok->advance(); // (
    
    vmw->writeLabel("WHILE_" + to_string(while_label_count));
    compileExpression(); // expression
    
    vmw->writeArithmetic(NOT); // not
    vmw->writeIf("WHILE_END_" + to_string(while_label_count)); // if statement is false, goto WHILE_END_x
 
    tok->advance(); // )
    tok->advance(); // {
    
    compileStatements(); // statements
    
    vmw->writeGoto("WHILE_" + to_string(while_label_count)); // goto WHILE_x
    
    tok->advance(); // }
    
    vmw->writeLabel("WHILE_END_" + to_string(while_label_count)); // label to exit the while loop
    while_label_count++;
    
}

void compilation_engine::compileLet(){ // 'let' varName ('[' expression ']')? '=' expression ';'
    
    tok->advance(); // let
    
    string varName = tok->getCurrentToken(); // varName;
    tok->advance();
    
    if(tok->getCurrentToken() == "["){ // varName[expression]
        
        vmw->writePush(sym->kind_of(varName), sym->index_of(varName)); // push varName
        
        tok->advance(); // [
        
        compileExpression(); // expression
        
        tok->advance(); // ]
        
        vmw->writeArithmetic(ADD); // add
        
        tok->advance(); // =
        
        compileExpression(); // expression
        
        vmw->writePop(TEMP, 0);
        vmw->writePop(POINTER, 1);
        vmw->writePush(TEMP, 0);
        vmw->writePop(THAT, 0);
        
        tok->advance(); // ;
        
        
    }else{ // varName = expression;
        tok->advance(); // =
        compileExpression(); // expression
        
        vmw->writePop(sym->kind_of(varName), sym->index_of(varName)); // pop the value of the expression into the variable varName after consulting the symbol table
        
    }
    
    tok->advance(); // ;
    
    
}

void compilation_engine::compileDo(){ // do subroutineCall ';'
    
    tok->advance(); // do
    
    compileSubroutineCall(); // subroutineCall
    
    tok->advance(); // ';'
    
}

void compilation_engine::compileReturn(){ // return expression?';'
    
    tok->advance();
    
    if(tok->getCurrentToken() == ";"){
        vmw->writeReturn();             // if there aint no expression
        return;
    }
    
    compileExpression();
    
    vmw->writeReturn();
    
    tok->advance(); // ;
    
    
}

void compilation_engine::compileExpression(){ // term (op term)*  // end_token tells went to stop looking for the next term
    
    /*
     
     (no operator precedence followed !!!)
     
     we are going to process this in a postfix fashion.
     
     if expression is T1 op1 T2 op2 T3
     where Ti is the term and opi is the operation,
     
     we process this as,
     
     T1 T2 op1 T3 op2 ...
     
     
     for more info, refer to "Postfix Expressions"
     
     */
    
    string current_operation; // keeps track of the operation we need to perform after the current term.
    
    compileTerm(); // term
    
        while(tok->getCurrentToken() == "+"
           or tok->getCurrentToken() == "-"
           or tok->getCurrentToken() == "*"
           or tok->getCurrentToken() == "/"
           or tok->getCurrentToken() == "&"
           or tok->getCurrentToken() == "|"
           or tok->getCurrentToken() == "<"
           or tok->getCurrentToken() == ">"
           or tok->getCurrentToken() == "=")
        {
            current_operation = tok->getCurrentToken(); // op
            tok->advance();
            
            compileTerm(); // term
            tok->advance();
            
            if(current_operation == "+"){
                vmw->writeArithmetic(ADD); // add
            }else if(current_operation == "-"){
                vmw->writeArithmetic(SUB); // sub
            }else if(current_operation == "*"){
                vmw->writeCall("Math.multiply", 2); // call Math.multiply 2
            }else if(current_operation == "/"){
                vmw->writeCall("Math.divide", 2); // call Math.divide 2
            }else if(current_operation == "&"){
                vmw->writeArithmetic(AND); // and
            }else if(current_operation == "|"){
                vmw->writeArithmetic(OR); // or
            }else if(current_operation == "<"){
                vmw->writeArithmetic(LT); // lt
            }else if(current_operation == ">"){
                vmw->writeArithmetic(GT); // gt
            }else if(current_operation == "="){
                vmw->writeArithmetic(EQ); // eq
            }
            
        }
    
    
}

void compilation_engine::compileTerm(){ // integerConstant | stringConstant | keywordConstant | varName | varName '[' expression ']' | subroutineCall | '(' expression ')' | unaryOp term
    
    bool is_int_const = true;
    for(auto c : tok->getCurrentToken()){
        if(!isdigit(c)) is_int_const = false; // to check if the term is an integer constant.
    }
    
    
    // term => integerConstant
    if(is_int_const){
        vmw->writePush(CONST, stoi(tok->getCurrentToken()));
        tok->advance();
        return;
    }
    // term => stringConstant
    if(tok->getCurrentToken()[0] == '"' and tok->getCurrentToken().back() == '"'){
        int str_len = (int)tok->getCurrentToken().size(); // length of the stringConstant
        
        vmw->writePush(CONST, str_len); // push the length of the string
        vmw->writeCall("String.new", 1); // call String.new 1
        
        for(char c : tok->getCurrentToken()){ // we need to push the int value of every char and call String.appendChar 2
            
            vmw->writePush(CONST, (int)c); // push the ascii value of the character c.
            vmw->writeCall("String.appendChar", 2);
        }
        return;
    }
    
    //term => keywordConstant
    if(tok->getCurrentToken() == "true"){ // true is equivalent to -1 --> NOT(0)
        vmw->writePush(CONST, 0);
        vmw->writeArithmetic(NOT);
    }
    if(tok->getCurrentToken() == "false"){ // false is equivalent to 0
        vmw->writePush(CONST, 0);
    }
    if(tok->getCurrentToken() == "null"){ // null is equivalent to 0
        vmw->writePush(CONST, 0);
    }
    if(tok->getCurrentToken() == "this"){ // this is stored in pointer 0
        vmw->writePush(POINTER, 0);
    }
    
    // term=> unaryOp term
    if(tok->getCurrentToken() == "~"){
        tok->advance(); // term
        vmw->writeArithmetic(NOT);
    }else if(tok->getCurrentToken() == "-"){
        tok->advance(); // term
        vmw->writeArithmetic(NEG);
    }
    
    // term => '(' expression ')'
    
    if(tok->getCurrentToken() == "("){
        tok->advance(); // '('
        compileExpression();
        tok->advance(); // ')'
    }
    
    string current_token = tok->getCurrentToken(); // this can be varName, varName['expression'], or a subroutine call.
    // grammar is LL(2) in this part.
    
    tok->advance();
    
    if(tok->getCurrentToken() == "["){ // its varName['expression']
        tok->advance(); // '['
        
        compileExpression(); // push the result of the expression;
        
        vmw->writePush(LOCAL, sym->index_of(current_token)); // push the base-address of the array
        vmw->writeArithmetic(ADD);
        
        tok->advance(); // ']'
        
    }else if(tok->getCurrentToken() == "(" or tok->getCurrentToken() == "."){ // has to be a subroutine call
        tok->retreat();
        
        compileSubroutineCall();
        
    }else{ // gotta be a varName
        vmw->writePush(LOCAL, sym->index_of(current_token)); // pushing into local segment since its a 'var'
    }
    
     
}

void compilation_engine::compileExpressionList(){ // (expression (',' expression)*)?
    
    compileExpression(); // expression
    tok->advance();
    
    while(tok->getCurrentToken() == ","){ // (',' expression)*
        
        tok->advance();
        
        compileExpression();
        
        tok->advance();
    }
   
}

void compilation_engine::compileSubroutineCall(){ // subroutineName '(' expressionList ')' | (className | varName) '.' subroutineName '(' expressionList ')'
    
    
    tok->advance();
    
    if(tok->getCurrentToken() == "("){ //  has to be subroutineName
        tok->retreat();
        
        
        
    }else if(tok->getCurrentToken() == "."){ //   has to be (className | varName)
        
        tok->retreat();
        
    }
    
    
    
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
