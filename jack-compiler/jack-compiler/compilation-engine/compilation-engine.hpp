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
#include "../vm-writer/vm-writer.hpp"

/*
  The compilation_engine class utilises the services of the tokenizer class.
    We generate an VM file after processing the tokens one by one.
    
 */

class compilation_engine{
    
public:
    
    compilation_engine(tokenizer * T, symbol_table * ST, vm_writer * VMW); // constructor
    ~compilation_engine(); // destructor.
    void dumpVM(); // dumps the processed VM code into a .vm file
    vector<string> getVMContent();
    void compilation_engine_begin(); // starts generating VM code for the processed tokens.
    void compilation_engine_end(); // frees all the defined structures. flushes code into .vm file
    
private:
    
    tokenizer * tok; // pointer to the tokenizer that has processed the current file.
    symbol_table * sym; // pointer to the symbol_table we use for the current file.
    vm_writer * vmw; // pointer to the vm_writer class that we use for the current file.
    string filename; // the current file being processed.
    
    vector<string> VMContent; // holds the content that will be dumped into the VM file at the end. 
    
        
    void compileClass();
    void compileClassVarDec();
    void compileSubroutineDec();
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
