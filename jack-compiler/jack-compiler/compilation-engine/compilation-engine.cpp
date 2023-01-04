//
//  compilation-engine.cpp
//  jack-compiler
//
//  Created by Yuvraj Sakshith on 28/12/22.
//

#include "compilation-engine.hpp"



compilation_engine::compilation_engine(tokenizer * T, symbol_table * ST, vm_writer * VMW){
    filename = T->getCurrentFilename();
    filesystem::path file_path(filename);
    
    filename_stem = file_path.stem();
    
    tok = T;
    sym = ST;
    vmw = VMW;


}

void compilation_engine::scanSubroutines(){
    
    while(tok->hasMoreTokens()){
    
        if(tok->getCurrentToken() == "function" or tok->getCurrentToken() == "constructor" or tok->getCurrentToken() == "method"){
            
            int kind = NONE;
            int type = NONE;
            string name;
            
            if(tok->getCurrentToken() == "function") kind = FUNCTION;
            if(tok->getCurrentToken() == "method") kind = METHOD;
            if(tok->getCurrentToken() == "constructor") kind = CONSTRUCTOR;
            
            tok->advance(); // (function | constructor | method)
            
            if(tok->getCurrentToken() == "void") type = VOID;
            else type = NONVOID;
            
            tok->advance(); // (void | type)
            
            name = tok->getCurrentToken(); // subroutineName
            
            sym->define(name, type == VOID ? "void" : "nonvoid", kind);
            
            tok->advance(); // subroutineName
            
        }else{
            tok->advance();
        }
        
    }
    
    
    tok->reset();
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
    
    string outputFile = filename.substr(0, filename.size() - 5);
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
    sym->reset_subroutine_info_table(); // reset all information about subroutines.
    
    scanSubroutines(); // we do a pass through the class, analysing each subroutine, and recording its info into symbol_table::subroutine_info_ST
    
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
    // reset the current_subroutine_info object.
    current_subroutine_info.name = "";
    current_subroutine_info.kind = NONE;
    current_subroutine_info.type = NONE;
    
    
    if(tok->getCurrentToken() == "constructor"){

        current_subroutine_info.kind = CONSTRUCTOR;
        
    }else if(tok->getCurrentToken() == "function"){
        
        current_subroutine_info.kind = FUNCTION;
        
    }else if(tok->getCurrentToken() == "method"){
        
        current_subroutine_info.kind = METHOD;
        
    }
    
    tok->advance(); // (constructor | function | method)
    
    if(tok->getCurrentToken() == "void"){
        current_subroutine_info.type = VOID;
    }else{
        current_subroutine_info.type = NONVOID;
    }
    tok->advance(); // (void | type)
    
    string subroutineName = filename_stem+ "." + tok->getCurrentToken();
    current_subroutine_info.name = subroutineName;
        
    sym->define(subroutineName, current_subroutine_info.type == VOID ? "void" : "non-void", current_subroutine_info.kind); // add an entry to the subroutine_info symbol table.
    
    tok->advance();
    tok->advance(); // (
    
    compileParameterList(); // parameterList
    
    tok->advance(); // )
    

    
    compileSubroutineBody(subroutineName, current_subroutine_info.kind, current_subroutine_info.type == VOID); // subroutineBody
    

    
}

void compilation_engine::compileSubroutineBody(string subroutine_name, int subroutine_kind, bool void_subroutine){ // '{' varDec* statements '}'
    
    
    tok->advance(); // {
    int local_var_count = 0;
    
    if(tok->getCurrentToken() == "let" or tok->getCurrentToken() == "do" or tok->getCurrentToken() == "while" or tok->getCurrentToken() == "return" or tok->getCurrentToken() == "if"){
        // dont do anything as there are no variable declarations. |varDec| = 0.
    }else{
        while(tok->getCurrentToken() == "var")local_var_count += compileVarDec();
    }
    
    vmw->writeFunction(subroutine_name,  local_var_count);
    
    // if the subroutine is a method, we need to align the base-address of the object that called the method with the THIS memory segment.
    if(subroutine_kind == METHOD){
        vmw->writePush(ARG, 0);
        vmw->writePop(POINTER, 0);
    }else if(subroutine_kind == CONSTRUCTOR){ // if the subroutine is a constructor, we need to allocate space for the object. here, it is the number of field variables present.
        vmw->writePush(CONST, sym->var_count(FIELD));
        vmw->writeCall("Memory.alloc", 1);
        vmw->writePop(POINTER, 0); // anchor the object to the THIS memory segment.
    }
    
    compileStatements(); // statements
    

    tok->advance(); // }
}

int compilation_engine::compileVarDec(){ // 'var' type varName (',', varName)*';'
    
    int var_count = 0; // number of variables
    
    tok->advance(); // var
    
    string current_type = tok->getCurrentToken(); // type
    
    tok->advance(); // varName
    
    string varName = tok->getCurrentToken();
    
    var_count++;
    sym->define(varName, current_type, VAR);
    
    tok->advance();
    
    while(tok->getCurrentToken() != ";"){
        tok->advance(); // ','
        
        varName = tok->getCurrentToken(); // varName;
        
        var_count++;
        sym->define(varName, current_type, VAR);
        
        tok->advance();
        
    }
    
    tok->advance(); // ';'
    
    
    return var_count;
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
    
    if_else_label_count++; //since the if-else statements can be nested, and compiled in a recursive fashion, we need unique labels, hence we increment this.
    
    max_if_label_used = max(max_if_label_used, if_else_label_count);
    
    tok->advance(); // if
    tok->advance(); // (
    compileExpression(); // expression
    tok->advance(); // )
    tok->advance(); // {
    
        
    vmw->writeIf("IF_TRUE" + to_string(if_else_label_count)); // if-goto IF_TRUEx
    
    
    vmw->writeGoto("IF_FALSE" + to_string(if_else_label_count)); // goto IF_FALSEx
    
    vmw->writeLabel("IF_TRUE" + to_string(if_else_label_count)); // label for the if statement's body
    
    compileStatements(); // statements
    
    tok->advance(); // }
    
    bool else_statement_present = false;
    
    if(tok->getCurrentToken() == "else"){
        else_statement_present = true;
        
        vmw->writeGoto("IF_END" + to_string(if_else_label_count)); // goto IF_ENDx
        vmw->writeLabel("IF_FALSE" + to_string(if_else_label_count)); // label IF_FALSEx
        
        tok->advance(); // else
        tok->advance(); // {
        compileStatements();
        tok->advance(); // }
    }
    
    
    if(else_statement_present) vmw->writeLabel("IF_END" + to_string(if_else_label_count));// label to jump to the rest of the code
    else vmw->writeLabel("IF_FALSE" + to_string(if_else_label_count));
    
    if_else_label_count--;
}

void compilation_engine::compileWhile(){ // 'while' '(' expression ')' '{' statements '}'
    
    while_label_count++; // since the while statements can be nested, and compiled in a recursive fashion, we need unique labels, hence we increment this.
    
    max_while_label_used = max(max_while_label_used, while_label_count);
    
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
    
    
    while_label_count--;
    
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
    
    vmw->writePop(TEMP, 0);
    
    
    tok->advance(); // ';'
    
}

void compilation_engine::compileReturn(){ // return expression?';'
    
    tok->advance();
    
    if(tok->getCurrentToken() == ";"){ // if there aint no expression
        if(current_subroutine_info.type == VOID) vmw->writePush(CONST, 0); // pushing constant 0 as a dummy value, as void functions also have to return "something".
        
        if(current_subroutine_info.kind == CONSTRUCTOR) vmw->writePush(POINTER, 0); // when returning from a consructor, we return 'this'. hence we return its base-address, i.e, pointer 0.
        
        vmw->writeReturn();
        tok->advance(); // ;
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
        int str_len = (int)tok->getCurrentToken().size()-2; // length of the stringConstant (-2 as we dont consider the quotation marks)
        string currentStringConstant = tok->getCurrentToken().substr(1,tok->getCurrentToken().size()-2);
        vmw->writePush(CONST, str_len); // push the length of the string
        vmw->writeCall("String.new", 1); // call String.new 1
        
        for(char c : currentStringConstant){ // we need to push the int value of every char and call String.appendChar 2
            
            vmw->writePush(CONST, (int)c); // push the ascii value of the character c.
            vmw->writeCall("String.appendChar", 2);
        }
        tok->advance(); // stringConstant
        return;
    }
    
    //term => keywordConstant
    if(tok->getCurrentToken() == "true"){ // true is equivalent to -1 --> NOT(0)
        vmw->writePush(CONST, 0);
        vmw->writeArithmetic(NOT);
        tok->advance();
        return;
    }
    if(tok->getCurrentToken() == "false"){ // false is equivalent to 0
        vmw->writePush(CONST, 0);
        tok->advance();
        return;
    }
    if(tok->getCurrentToken() == "null"){ // null is equivalent to 0
        vmw->writePush(CONST, 0);
        tok->advance();
        return;
    }
    if(tok->getCurrentToken() == "this"){ // this is stored in pointer 0
        vmw->writePush(POINTER, 0);
        tok->advance();
        return;
    }
    
    // term=> unaryOp term
    if(tok->getCurrentToken() == "~"){
        tok->advance(); // op
        compileTerm(); // term
        vmw->writeArithmetic(NOT);
        return;
    }else if(tok->getCurrentToken() == "-"){
        tok->advance(); // op
        compileTerm(); // term
        vmw->writeArithmetic(NEG);
        return;
    }
    
    // term => '(' expression ')'
    
    if(tok->getCurrentToken() == "("){
        tok->advance(); // '('
        compileExpression();
        tok->advance(); // ')'
        return;
    }
    
    string current_token = tok->getCurrentToken(); // this can be varName, varName['expression'], or a subroutine call.
    // grammar is LL(2) in this part.
    
    tok->advance();
    
    if(tok->getCurrentToken() == "["){ // its varName['expression']
        tok->advance(); // '['
        
        compileExpression(); // push the result of the expression;
        
        vmw->writePush(LOCAL, sym->index_of(current_token)); // push the base-address of the array
        vmw->writeArithmetic(ADD);
        // we have the base-address of the array on top of the stack,  but we are interested in its value, hence these lines
        vmw->writePop(POINTER, 1);
        vmw->writePush(THAT, 0);
        
        tok->advance(); // ']'
        
    }else if(tok->getCurrentToken() == "(" or tok->getCurrentToken() == "."){ // has to be a subroutine call
        tok->retreat();
        
        compileSubroutineCall();
        
    }else{ // gotta be a varName
        vmw->writePush(sym->kind_of(current_token), sym->index_of(current_token));
    }
    
     
}

int compilation_engine::compileExpressionList(){ // (expression (',' expression)*)?    (returns the number of expressions processed)
    
    int expression_count = 0;
    
    if(tok->getCurrentToken() == ")"){ // there are no expressions in the expression list
        return expression_count;
    }
    
    compileExpression(); // expression
    
    expression_count++;
    
    while(tok->getCurrentToken() == ","){ // (',' expression)*
        
        tok->advance();
        
        compileExpression();
        expression_count++;
        
    }
    
    return expression_count;
   
}

void compilation_engine::compileSubroutineCall(){ // subroutineName '(' expressionList ')' | (className | varName) '.' subroutineName '(' expressionList ')'
    
    string subroutineName = tok->getCurrentToken();
    
    bool classNamePrefixAdded = false; // this we be set true of called subroutine is of the form (className | varName) . subrountineName(expressionList)
    
    tok->advance(); // subroutineName or (className | varName)
    
    int arg_count = 0; // number of arguments the subroutine takes.
    
    if(tok->getCurrentToken() == "."){
        classNamePrefixAdded = true;
        
        subroutineName += tok->getCurrentToken(); // .
        tok->advance(); //  .
        
        string class_var_name = subroutineName; //  (className | varName)
        subroutineName = tok->getCurrentToken();
        
        class_var_name.pop_back(); // remove '.'
        
        if(sym->exists(class_var_name)){
            vmw->writePush(sym->kind_of(class_var_name), sym->index_of(class_var_name)); // if we call a function through an object (method), we push the object's base address to the argument list.
            
            subroutineName = sym->type_of(class_var_name) + "." + subroutineName; // generate the actual subroutine name for the VMWriter::writeCall() function.
            
            arg_count++;
        }else{
            subroutineName = class_var_name + "." + subroutineName; // generate the actual subroutine name for the VMWriter::writeCall() function.
        }
        
        tok->advance(); // subroutineName
        
      
    }
    
   
    
    // moving ahead to '(' expressionList ')'
    
        tok->advance(); // (
        
        arg_count += compileExpressionList(); //  expressionList     (arg_count = the number of arguments passed into the subroutine)
        
        tok->advance(); // )
        
    if(sym->kind_of(subroutineName) == METHOD){
        vmw->writePush(POINTER, 0); // if we are working with a method call, we need to push the base-address of the class into the stack.
        arg_count++;
    }
            
        if(!classNamePrefixAdded) vmw->writeCall(filename_stem + "." + subroutineName, arg_count); // all subroutines are going to be named with a prefix of their filename.
        else{
            vmw->writeCall(subroutineName, arg_count);
        }
    // if the called function has a void return type, there will be a dummy return value at the top of the stack.
    // this has to popped somewhere, in some temporary location, hence we shall do pop temp 0.
    
        
    
}

void compilation_engine::compileParameterList(){ // ((type varName) (',' type VarName)*)?
    
    // these are going to be your arguments in a subroutine call, hence we shall add them to the symbol table as ARG variables
    
    // if the subroutine is a METHOD, we need to reserve the first argument, i.e argument 0 for the object reference.
    
    // for this purpose, we define a dummy variable subroutineName.baseAddress and add it into the ARG section of the symbol-table.
    
    if(current_subroutine_info.kind == METHOD){
        sym->define(current_subroutine_info.name + ".baseAddress", current_subroutine_info.name, ARG);
    }
    
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
