//
//  vm-writer.hpp
//  jack-compiler
//
//  Created by Yuvraj Sakshith on 29/12/22.
//

#ifndef vm_writer_hpp
#define vm_writer_hpp

#include <string>
#include <fstream>
#include "../ref/ref.h"
#include <vector>

using namespace std;

/*
 
    * The vm_writer class provides the VM code for various different operations such as push, pop, arithmetic, ... for different memory-segments
    such as argument, local, this, that, etc...
 
    * vm_writer provides an API through which we can obtain the VM code for various different operations.
 
    * The functions in the API return a vector of the VM code. Which are in the form of strings.
 
    * A file can be provided, into which the VM code will be written.
 
 */

class vm_writer{
    
public:
    
    vm_writer(string file); // constructor. sets the filename, resets the vm_code_buffer. passed file is a .vm file.
    
    void writePush(int segment, int index); // push variable of index of given memory segment
    void writePop(int segment, int index); // pop variable of index of given memory segment
    void writeArithmetic(int command); // ADD, SUB, EQ, NEG, ...
    void writeLabel(string label); // generate label
    void writeGoto(string label); // goto label
    void writeIf(string label); // if-goto label
    void writeCall(string name, int nArgs); // call function nArgs(number of arguments)
    void writeFunction(string name, int nLocals); // declare function nLocals(number of local variables)
    void writeReturn(); // return from current subroutine
    
    void flush_vm_writer(); // flushes code from vm_code_buffer to the file. closes the file
    
    
private:
    
    vector<vector<string>> vm_code_buffer; // all the written code is stored in this buffer. everything is flushed into the file while closing it.
    string filename;
    
};


#endif /* vm_writer_hpp */
