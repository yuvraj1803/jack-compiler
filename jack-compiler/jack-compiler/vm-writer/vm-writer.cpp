//
//  vm-writer.cpp
//  jack-compiler
//
//  Created by Yuvraj Sakshith on 29/12/22.
//

#include "vm-writer.hpp"

// this file is pretty self-explanatory provided, you are used to the hack vm.

vm_writer::vm_writer(){
}


vector<vector<string>> vm_writer::get_vm_buffer(){
    return vm_code_buffer;
}

void vm_writer::writePush(int segment, int index){
    string memory_segment;
    
    switch (segment) {
        case CONST:
            memory_segment = "constant";
            break;
            
        case FIELD:
            memory_segment = "this";
            break;
            
        case ARG:
            memory_segment = "argument";
            break;
            
        case STATIC:
            memory_segment = "static";
            break;
        
        case THIS:
            memory_segment = "this";
            break;
            
        case THAT:
            memory_segment = "that";
            break;
            
        case POINTER:
            memory_segment = "pointer";
            break;
            
        case TEMP:
            memory_segment = "temp";
            break;
            
        default:
            memory_segment = "local";
            break;
    }
    
    vector<string> vm_command;
    vm_command.push_back("push ");
    vm_command.push_back(memory_segment);
    vm_command.push_back(" ");
    vm_command.push_back(to_string(index));
    
    vm_code_buffer.push_back(vm_command);
    
}

void vm_writer::writePop(int segment, int index){
    string memory_segment;
    
    switch (segment) {
        case CONST:
            memory_segment = "constant";
            break;
            
        case FIELD:
            memory_segment = "this";
            break;
            
        case ARG:
            memory_segment = "argument";
            break;
        
        case STATIC:
            memory_segment = "static";
            break;
        
        case THIS:
            memory_segment = "this";
            break;
            
        case THAT:
            memory_segment = "that";
            break;
            
        case POINTER:
            memory_segment = "pointer";
            break;
            
        case TEMP:
            memory_segment = "temp";
            break;
            
        default:
            memory_segment = "local";
            break;
    }
    
    vector<string> vm_command;
    vm_command.push_back("pop ");
    vm_command.push_back(memory_segment);
    vm_command.push_back(" ");
    vm_command.push_back(to_string(index));
    
    vm_code_buffer.push_back(vm_command);
    
}

void vm_writer::writeArithmetic(int command){
    
    if(command == ADD){
        vm_code_buffer.push_back({"add"});
    }else if(command == SUB){
        vm_code_buffer.push_back({"sub"});
    }else if(command == NEG){
        vm_code_buffer.push_back({"neg"});
    }else if(command == EQ){
        vm_code_buffer.push_back({"eq"});
    }else if(command == GT){
        vm_code_buffer.push_back({"gt"});
    }else if(command == LT){
        vm_code_buffer.push_back({"lt"});
    }else if(command == AND){
        vm_code_buffer.push_back({"and"});
    }else if(command == OR){
        vm_code_buffer.push_back({"or"});
    }else if(command == NOT){
        vm_code_buffer.push_back({"not"});
    }
    
}

void vm_writer::writeLabel(string label){
    vm_code_buffer.push_back({"label ", label});
}

void vm_writer::writeGoto(string label){
    vm_code_buffer.push_back({"goto ", label});
}

void vm_writer::writeIf(string label){
    vm_code_buffer.push_back({"if-goto ", label});
}

void vm_writer::writeCall(string name, int nArgs){
    vm_code_buffer.push_back({"call ", name, " ", to_string(nArgs)});
}

void vm_writer::writeFunction(string name, int nLocals){
    vm_code_buffer.push_back({"function ", name , " ", to_string(nLocals)});
}

void vm_writer::writeReturn(){
    vm_code_buffer.push_back({"return"});
}

