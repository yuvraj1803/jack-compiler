//
//  ref.h
//  jack-compiler
//
//  Created by Yuvraj Sakshith on 28/12/22.
//


#ifndef ref_h
#define ref_h

enum types{
    SYMBOL,
    INTEGER_CONSTANT,
    STRING_CONSTANT,
    IDENTIFIER,
    KEYWORD,
    CLASS,
    METHOD,
    FUNCTION,
    CONSTRUCTOR,
    INT,
    BOOLEAN,
    CHAR,
    VOID,
    NONVOID,
    LET,
    DO,
    IF,
    ELSE,
    WHILE,
    RETURN,
    
    __TRUE__,   // _'s have been added as these terms are already in use.
    __FALSE__,
    __NULL__,
    
    CONST,
    TEMP,
    VAR,
    STATIC,
    FIELD,
    LOCAL,
    ARG,
    THIS,
    THAT,
    POINTER,
    NONE,
    
    ADD,
    SUB,
    NEG,
    EQ,
    GT,
    LT,
    AND,
    OR,
    NOT,
    
    INVALID
};


#endif /* ref_h */
