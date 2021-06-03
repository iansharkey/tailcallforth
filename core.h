#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#ifndef CORE_H_
#define CORE_H_
enum INTERPRETER_STATE {
    COMPILING,
    IMMEDIATELY
};

enum WORD_FLAGS {
    F_HIDDEN = 1,
    F_IMMEDIATE = 2
};


struct usefulstate;



#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

typedef void (*block)(struct usefulstate*, void*, void*, void**, void**, void*);



//void* datastack_[256];
//void* returnstack_[256];
//void* w;

//size_t stacktop;
// void* returnstack[256]; // handled by system stack

//void* latest;
//void* dict[1024];


struct word {
    struct word* prev;
    char name[15];
    unsigned int flags:4;

    block codeword; // first impl block is machine word
    void* extra[];
};


#define PARAMS struct usefulstate *state, void* esi, void* eax, void** stacktop, void** retstacktop, void *next_


struct usefulstate {
  struct word *latest;
  void **here;
  void **dp;
  int (*getnexttoken)(PARAMS);
  char token[33];
  intptr_t length;
  void *ctx;
  enum INTERPRETER_STATE state;
  int base;
};




#define ARGS state, esi, eax, stacktop, retstacktop, next_


// currentcodeword: block - pointer to the codeword within a definition
// datastack: void** - 
// 

__attribute__((noinline)) void next(PARAMS);

extern void *defaultprogram[];
extern struct word *lastword;


#define NEXT __attribute__((musttail)) return ((block)(next_))(ARGS)

#endif

