//
//  symbol-table.hpp
//  jack-compiler
//
//  Created by Yuvraj Sakshith on 29/12/22.
//

#ifndef symbol_table_hpp
#define symbol_table_hpp


/*
    * The symbol_table class stores the information of variables in the current jack file.
    * Ofcourse, we have two different scopes, class-level and subroutine level, hence we have two tables for each of them respectively.
 
    * The table is represented in the form of a hash table. In order to not sophesticate things unnecessarily, we go with the std::map data-structure for the implementation.
 
 
    * Table contains 4 columns => NAME, TYPE, KIND, INDEX.
 
    * Each type of variable has a fixed scope in which it shall be defined.
      This is given in the table below.
 
    TYPE        SCOPE
 
    STATIC      CLASS
    FIELD       CLASS
    ARG         SUBROUTINE
    LOCAL         SUBROUTINE
 
 */

#include <map>
#include <vector>
#include "../ref/ref.h"
#include <string>

using namespace std;

class table_element{ // for each variable, this class holds its elements.
public:
    
    string type;
    int kind;
    int index;
    
    table_element(){
        type = "";
        kind = NONE;
        index = 0;
    }
};

class symbol_table{
    
public:
    
    symbol_table();
    
    // ---------API--------------------------------
    
    int var_count(int kind); // returns the count of variables of given kind.
    int kind_of(string name); // returns the kind of variable 'name' is.
    int index_of(string name); // return the index of the variable 'name'.
    
    string type_of(string name); // returns the type of variable 'name' is.
    
    void define(string name, string type, int kind); // adds the given variable into the appropriate symbol_table.
    void reset_subroutine_table(); // resets the class-level symbol table.
    void reset_class_table(); // resets the subroutine-level symbol table.
    
    // --------------------------------------------
    
    
private:
    
    map<string, table_element> class_level_ST; // hash table for variables in the class scope.
    map<string, table_element> subroutine_level_ST; // hash table for variables in the subroutine scope.
                    
    map<int,int> nextIndex; // maps nextIndex[kind] to the next available index for a variable of its kind. (remember 'kind' is an integer defined in "../ref/ref.h"
    
    
    
};


#endif /* symbol_table_hpp */
