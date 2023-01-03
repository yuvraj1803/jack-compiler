//
//  jack-compiler.cpp
//  jack-compiler
//
//  Created by Yuvraj Sakshith on 29/12/22.
//

#include "jack-compiler.hpp"


jack_compiler::jack_compiler(){
    sym = new symbol_table();
}

void jack_compiler::compile_file(string file){
    
    string vm_file = file.substr(0,file.size()-5) + ".vm"; // remove the .jack extention and add .vm extention
    
    tokenizer * tok = new tokenizer(file);
    vm_writer * vmw = new vm_writer();
    
    
    compilation_engine * CE = new compilation_engine(tok, sym, vmw);
    CE->compilation_engine_begin();
    CE->compilation_engine_end();
    CE->dumpVM();
    
    free(CE);
    free(tok);
    free(vmw);
    
}


