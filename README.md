# Jack Compiler

The Jack Compiler is a software tool which converts code written in the **Jack Programming Language into the Hack Virtual Machine Code**.

The Jack Language is a simple object-based language which can be used to write high-level programs.

The Compiler converts the Jack code into VM code, which is then passed to the VM Translator, which then converts the code into the assembly code.

This Assembly code is passed on to the Hack Assembler, which finally generates the machine code.


![compilation-process](https://user-images.githubusercontent.com/37622719/210441255-0cd352fd-a675-4b81-a8c0-1d1e65d93049.png)

Hence, the code undergoes a **two-tier compilation process**.

<img width="1407" alt="compiler-design" src="https://user-images.githubusercontent.com/37622719/210438650-1999db82-980a-4c23-b8a6-326a0793f2d4.png">

Sub-modules: 

**Tokenizer** : Reads the given .jack file and breaks it into tokens. It can provide the next token sequentially by the Tokenizer-API.

Tokenizer-API:

           * tokenizer::advance() -> moves to the next token

           * tokenizer::retreat() -> moves to the previous token
           
           * tokenizer::hasMoreTokens() -> returns true if more tokens remaining, otherwise false
           
           * tokenizer::getCurrentToken() -> returns the current token
           
           * tokenizer::tokenType() -> returns the token type. (types present in ref/ref.h)
           
**Symbol Table** : keeps track of all variable declarations in the code. It defines two scopes, namely class-level and subroutine-level.
        Utilizes the services of a hash-map.
        
        
**VM Writer**  : provides an API to generate VM code for various operations such as push, pop, add, sub, call, return, etc.

**Compilation Engine** : is the heart of the compiler. It utilises the Tokenizer to advance from one token to another, following the Jack Grammar and generating VM code, which is done using the VM Writer's API.

It also takes care of variable declarations and uses the services of the symbol-table to store their information.


**Jack Grammar** : is a LL-Grammar which is LL(1) in all cases except while handling expressions, in which it becomes LL(2). (Parsed from LEFT TO RIGHT and by constructing the LEFT MOST DERIVATION of the sentence.)

Refer to the Jack Grammar and the design of the compiler in the image below.

![jack-grammar](https://user-images.githubusercontent.com/37622719/210438527-4d12e2fa-a241-48b4-a242-0a301478be5b.png)

 
**References**

**Hack Architecture** : https://en.wikipedia.org/wiki/Hack_computer

