//
//  symbol-table.cpp
//  jack-compiler
//
//  Created by Yuvraj Sakshith on 29/12/22.
//

#include "symbol-table.hpp"


symbol_table::symbol_table(){
    
}

int symbol_table::var_count(int kind){ // count of variables with given kind.
    int count = 0;
    if(kind == ARG or kind == VAR){
        for(auto element : subroutine_level_ST){
            if(element.second.kind == kind) count++;
        }
    }
    
    if(kind == STATIC or kind == FIELD){
        for(auto element : class_level_ST){
            if(element.second.kind == kind) count++;
        }
    }
    
    return count;
}

int symbol_table::kind_of(string name){ // returns the kind of the variable 'name'
    if(class_level_ST.find(name) != class_level_ST.end()) return class_level_ST[name].kind;
    
    if(subroutine_level_ST.find(name) != subroutine_level_ST.end()) return subroutine_level_ST[name].kind;
    
    return NONE;
    
}

string symbol_table::type_of(string name){ // returns the type of varible 'name'
    if(class_level_ST.find(name) != class_level_ST.end()) return class_level_ST[name].type;
    
    if(subroutine_level_ST.find(name) != subroutine_level_ST.end()) return subroutine_level_ST[name].type;
    
    return "INVALID"; // if the variable is not found.
    
}

int symbol_table::index_of(string name){ // returns the index of variable 'name'
    if(class_level_ST.find(name) != class_level_ST.end()) return class_level_ST[name].index;
    
    if(subroutine_level_ST.find(name) != subroutine_level_ST.end()) return subroutine_level_ST[name].index;
    
    return -1; // if the variable is not found.
    
    
}

void symbol_table::define(string name, string type, int kind){ // adds variable with given information
    if(kind == ARG or kind == VAR){
        subroutine_level_ST[name].type = type;
        subroutine_level_ST[name].kind = kind;
        subroutine_level_ST[name].index = nextIndex[kind]++; // assign the next available index and increment it.
    }
    
    if(kind == FIELD or kind == STATIC){
        class_level_ST[name].type = type;
        class_level_ST[name].kind = kind;
        class_level_ST[name].index = nextIndex[kind]++; // assign the next available index and increment it.
    }
}

void symbol_table::reset_class_table(){ // sets all values to default in the class table
    class_level_ST.clear();
    
    // have to reset the indices of the STATIC and FIELD variables.
    
    nextIndex[STATIC] = 0;
    nextIndex[FIELD] = 0;
    
}

void symbol_table::reset_subroutine_table(){ // sets all values to default in the subroutine table
    subroutine_level_ST.clear();
    
    // have to reset the indices of the VAR and ARG variables
    nextIndex[ARG] = 0;
    nextIndex[VAR] = 0;
    
}


