//
//  tokenizer.cpp
//  jack-compiler
//
//  Created by Yuvraj Sakshith on 28/12/22.
//

#include "tokenizer.hpp"


tokenizer::tokenizer(string file){
    
    // reads the file line by line
    // breaks the words in each line into induvidual words
    // using std::ifsteam for this purpose.
    
    filename = file;
    
    vector<string> file_data;
    ifstream fileStream(file);
    
    if(fileStream.is_open()){
        string line;
        
        bool multiLineCommentActive = false; // we make this true if a multi-line comment was initialised but not closed.
        
        while(getline(fileStream, line)){
            
            // removing spaces in the start of the string.
            
            while(*line.begin() == ' ') line.erase(line.begin());
            
            // we have to deal with comments first.
            // as we process each line of the file, we parallely remove the comments.
            
            if(multiLineCommentActive){ // we check if the multi-lined comment ends in the current line.
                int indexCommentFinished = (int)line.size(); // if it does, then we need to consider the code ahead of it.
                for(int charIndex = 0;charIndex<(int)line.size();charIndex++){
                    if(charIndex + 1 < (int)line.size() and line[charIndex] == '*' and line[charIndex+1] == '/'){
                        multiLineCommentActive = false;
                        indexCommentFinished = charIndex + 2;
                    }
                    
                    // after we come out of the comment, we begin consider the code ahead for tokenisation.
                    
                    string lineWOComments;
                    for(int charIndex = indexCommentFinished;  charIndex<(int)line.size(); charIndex++){
                        lineWOComments += line[charIndex];
                    }
                    
                    line = lineWOComments;
                
                }
            }
            bool insideString = false;
            string temp;
            for(int charIndex = 0;charIndex<(int)line.size();charIndex++){
                
                if(line[charIndex] == '/' and (charIndex + 1 < (int)line.size())){ // if we find a pattern such as '//' or '/*', we ignore the
                    if(line[charIndex+1] == '/') break;                            // characters ahead. we also check if the comment is multi-lined.
                    if(line[charIndex+1] == '*'){
                        multiLineCommentActive = true;
                        
                        // here, we deal with multiline comments that end on the same line
                        // the end of the multiline comment has to be found and the code ahead of it has to be considered.
                        // this is done by a temporary index which scans and checks for the end.
                        // then it updates the charIndex accordingly.
                        
                        for(int tempIndex = charIndex + 2; tempIndex < (int)line.size();tempIndex++){
                            if(multiLineCommentActive and tempIndex+1 < (int)line.size() and line[tempIndex] == '*' and line[tempIndex+1] == '/'){
                                multiLineCommentActive = false;
                                charIndex = tempIndex + 2;
                                if(charIndex >= (int)line.size()) break;
                            }
                        }
                    }
                }
                // spaces and tabs are not removed if we are inside a string constant. to handle this, we use the insideString boolean value.
                
                if(!insideString and (line[charIndex] == ' ' or line[charIndex] == '\t')){ // removing spaces and tabs handled here.
                    file_data.push_back(temp);
                    temp = string();
                }else{
                    if(line[charIndex] == '"') insideString ^= 1;
                    temp += line[charIndex];
                }
            }
            if(!temp.empty()) file_data.push_back(temp);
        }
    }else{
        cout << "couldn't open file. check permissions and its existance.\n";
    }
    
    
    fileStream.close();
    
    // every space-seperated word is not a token. they might be binded to symbols such as '()'. we need to seperate them.
    // refer to the Jack Grammar for a better picture.
    
    
    vector<string> seperatedTokens; // will break each token into smaller parts to check if it has been mixed up with symbols.
    
    for(auto s : file_data){
        seperatedTokens.clear();
        filterToken(seperatedTokens, s);
        for(auto tok : seperatedTokens){
            if(isCleanToken(tok)) tokenlist.push_back(tok);
        }
    }
    
    if(tokenlist.size()) currentToken = tokenlist[nextTokenPosition++]; // setting the current token to the first token in tokenlist.
    
}

void tokenizer::dumpXML(){
    // remove .jack from the file name.
    string outputFile = "";
    for(auto c: filename){
        if(c == '.') break;
        outputFile += c;
    }
    // add T at the end of the filename to denote 'T'okenizer output.
    outputFile += "T.xml";
    ofstream outputFileStream(outputFile);
    outputFileStream << "<tokens>\n";
    for(auto token : tokenlist){
        outputFileStream << "<" << tokenTypeString(token) << "> " << token << " </" << tokenTypeString(token) << ">\n";
    }
    outputFileStream << "</tokens>";
    
    outputFileStream.close();
    
}

bool tokenizer::isCleanToken(string token){
    if(token.size() == 0) return false;
    if(token[0] == '\0' or token[0] == '\r') return false;
    
    return true;
}

string tokenizer::getCurrentFilename(){
    return filename;
}

void tokenizer::filterToken(vector<string> &seperatedTokens, string &s){
    
    string temp = "";
    
    bool insideString = false;
    
    while(insideString or (!s.empty() and tokenType(string(1,s[0])) != SYMBOL)){ // check until we find a SYMBOL
        if(s[0] == '\"'){
            insideString ^= 1; // if insideString is false, it becomes true, otherwise it becomes false.
        }
        temp += *s.begin();
        s.erase(s.begin());
    }
    
    if(temp.size()) seperatedTokens.push_back(temp); // push the processed part
    
    if(s.empty()) return; // base case for the recursion. we just return if the string is empty.
    
    //we have special names for  < > & " symbols. we dont want them to confuse the XML file processor.
    if(s[0] == '<'){
        seperatedTokens.push_back("&lt;");
    }else if(s[0] == '>'){
        seperatedTokens.push_back("&gt;");
    }else if(s[0] == '&'){
        seperatedTokens.push_back("&amp;");
    }else if(s[0] == '\"'){
        seperatedTokens.push_back("&quot;");
    }else{
        seperatedTokens.push_back(string(1,s[0])); // push the symbol into the tokenlist and remove it from the string
    }
    s.erase(s.begin()); // erase the symbol from the string to continue processing it further
    
    filterToken(seperatedTokens, s); // recursively process the string
    
}

vector<string>tokenizer::getAllTokens(){
    return tokenlist;
}

string tokenizer::tokenTypeString(string token){
    if(tokenType(token) == KEYWORD) return "keyword";
    if(tokenType(token) == SYMBOL)  return "symbol";
    if(tokenType(token) == INTEGER_CONSTANT) return "integerConstant";
    if(tokenType(token) == STRING_CONSTANT)  return "stringConstant";
    
    return "identifier";
}

int tokenizer::tokenType(string token){
    
    if(token[0] == '\"' and token.back() == '\"'){ // checking if it starts and begins with double quotes. in this case its a string constant.
        return STRING_CONSTANT;
    }
    
    if(  token == "class"
       ||token == "constructor"
       ||token == "function"
       ||token == "method"
       ||token == "field"
       ||token == "static"
       ||token == "var"
       ||token == "int"
       ||token == "char"
       ||token == "boolean"
       ||token == "void"
       ||token == "true"
       ||token == "false"
       ||token == "null"
       ||token == "this"
       ||token == "let"
       ||token == "do"
       ||token == "if"
       ||token == "else"
       ||token == "while"
       ||token == "return"
       )
    {
        return KEYWORD;
    }
    
    if(  token[0] == '{'
       ||token[0] == '}'
       ||token[0] == '('
       ||token[0] == ')'
       ||token[0] == '['
       ||token[0] == ']'
       ||token[0] == ','
       ||token[0] == ';'
       ||token[0] == '+'
       ||token[0] == '-'
       ||token[0] == '*'
       ||token[0] == '/'
       ||token[0] == '&'
       ||token[0] == '|'
       ||token[0] == '>'
       ||token[0] == '<'
       ||token[0] == '='
       ||token[0] == '~'
       ||token[0] == '.'
     )
    {
        return SYMBOL;
    }
    
    int isInt = 1;
    
    for(auto c : token){ // checking if the token is an integer. if all the characters in it are digits. checked using std::isdigit.
        if(!isdigit(c)){
            isInt = 0;
            break;
        }
    }
    
    if(isInt) return INTEGER_CONSTANT;
    
    // if none of the above conditions match, we conclude the token is an identifier.
    
    return IDENTIFIER;
}

void tokenizer::retreat(){ // goes back one step in the tokens list
    if(nextTokenPosition == 1) return; // we are at the first position. cant retreat further.
    currentToken = tokenlist[nextTokenPosition-2];
    nextTokenPosition--;
    
}

void tokenizer::advance(){
    if(!hasMoreTokens()) return; // we dont do anything if there aren't any more tokens. Otherwise we may end up overflowing the buffer.
    currentToken = tokenlist[nextTokenPosition];
    nextTokenPosition++;
    
}

bool tokenizer::hasMoreTokens(){
    return nextTokenPosition < (int) tokenlist.size();
}

string tokenizer::getCurrentToken(){
    return currentToken;
}
