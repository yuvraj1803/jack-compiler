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
    
    
    // to have unique indices for labels generated among all files
    CE->if_else_label_count = common_if_else_label_count;
    CE->while_label_count = common_while_label_count;
    
    CE->compilation_engine_begin();
    CE->compilation_engine_end();
    
    common_if_else_label_count = max(common_if_else_label_count, CE->max_if_label_used+1); // update the indices
    common_while_label_count = max(common_while_label_count, CE->max_while_label_used+1); // we need to have indices higher than what we used for the current file when processing the next file, otherwise the same label will appear in both the files and a clash will happen.
    
    CE->dumpVM();
    
    free(CE);
    free(tok);
    free(vmw);
    
}


