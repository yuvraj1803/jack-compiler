//
//  compilation-engine.hpp
//  jack-compiler
//
//  Created by Yuvraj Sakshith on 28/12/22.
//

#ifndef compilation_engine_hpp
#define compilation_engine_hpp

#include "../tokenizer/tokenizer.hpp"
#include <fstream>
#include "../symbol-table/symbol-table.hpp"

/*
  The compilation_engine class utilises the services of the tokenizer class.
    We generate an VM file after processing the tokens one by one.
    
 */

class compilation_engine{
    
public:
    
    compilation_engine(tokenizer T, symbol_table ST); // constructor
    void dumpVM(); // dumps the processed VM code into a .vm file
    vector<string> getVMContent();
    
private:
    
    tokenizer * tok; // pointer to the tokenizer that has processed the current file.
    symbol_table * sym;
    string filename; // the current file being processed.
    void compilation_engine_begin(); // starts generating VM code for the processed tokens.
    vector<string> VMContent; // holds the content that will be dumped into the VM file at the end. 
    
        
    void compileClass();
    void compileClassVarDec();
    void compileSubroutineDec();
    void compileType();
    void compileParameterList();
    void compileSubroutineBody();
    void compileVarDec();
    void compileStatements();
    void compileStatement();
    void compileWhile();
    void compileLet();
    void compileReturn();
    void compileDo();
    void compileIf();
    void compileExpression();
    void compileTerm();
    void compileSubroutineCall();
    void compileExpressionList();
    
};


#endif /* compilation_engine_hpp */
