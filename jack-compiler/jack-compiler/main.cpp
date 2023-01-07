//
//  main.cpp
//  jack-compiler
//
//  Created by Yuvraj Sakshith on 29/12/22.
//

#include <iostream>
#include <filesystem>
#include <vector>

#include "jack-compiler/jack-compiler.hpp"

using namespace std;

int main(int argc, const char * argv[]) {

   if(argc < 2){
       cout << "./jack_compiler <path>\n";
       exit(1);
   }
    
    string path = argv[1];
    
    vector<string> jack_files;
    
    if(filesystem::is_directory(path)){
        for(auto file : filesystem::directory_iterator(path)){
            if(file.path().extension() == ".jack"){
                jack_files.push_back(file.path());
            }
        }
    }else{
        if(path.substr(path.size()-5,path.size()-1) == ".jack"){
            jack_files.push_back(path);
        }else{
            cout << "not a jack file \n";
            exit(1);
        }
    }
    
    jack_compiler * __JACK_COMPILER__ = new jack_compiler();
        
    for(auto j_file : jack_files){
        __JACK_COMPILER__->compile_file(j_file);
    }
    free(__JACK_COMPILER__);
    
    
    return 0;
}
