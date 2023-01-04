//
//  jack-compiler.hpp
//  jack-compiler
//
//  Created by Yuvraj Sakshith on 29/12/22.
//

#ifndef jack_compiler_hpp
#define jack_compiler_hpp

#include "../compilation-engine/compilation-engine.hpp"
#include "../tokenizer/tokenizer.hpp"
#include "../vm-writer/vm-writer.hpp"
#include "../symbol-table/symbol-table.hpp"

#include <string>

using namespace std;

class jack_compiler{
    
public:
    
    jack_compiler();
    void compile_file(string file); // compiles the input jack file and creates the .vm file
    
public:
    
    int common_if_else_label_count = -1; // common for all files in order to avoid clashes
    int common_while_label_count = -1;
    
    symbol_table * sym; // we need a common symbol table for all jack files, as we dont want to overwrite any variables in memory that belongs to an already processed file.
    
};
#endif /* jack_compiler_hpp */
