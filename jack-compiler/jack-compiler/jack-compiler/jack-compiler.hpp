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
    
};
#endif /* jack_compiler_hpp */
