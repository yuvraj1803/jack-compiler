//
//  main.cpp
//  jack-compiler
//
//  Created by Yuvraj Sakshith on 29/12/22.
//

#include <iostream>
#include <filesystem>
#include <vector>

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
    
    
    
    
    return 0;
}
