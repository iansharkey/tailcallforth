#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#ifndef CORE_H_
#define CORE_H_
enum INTERPRETER_STATE {
			IMMEDIATELY = 0,
  	    COMPILING = 1,

};

enum WORD_FLAGS {
    F_HIDDEN = 1,
    F_IMMEDIATE = 2
};


struct usefulstate;



#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

typedef void (*block)(void*, void*, void**, void**, void*, struct usefulstate*);



//void* datastack_[256];
//void* returnstack_[256];
//void* w;

//size_t stacktop;
// void* returnstack[256]; // handled by system stack

//void* latest;
//void* dict[1024];

#define TAILCALL_FORTH_PLATFORM_NAME "macos"
#define TAILCALL_FORTH_ARCH_NAME "x86_64"


struct word {
  struct word* prev;
  unsigned int flags:3;
  unsigned int len:5;
  char name[15];

  block codeword; // first impl block is machine word
  void* extra[];
};


#define PARAMS void* ip, void* xt, void** stacktop, void** retstacktop, void *next_, struct usefulstate *state


struct usefulstate {
  struct word *latest;
  void **stackbase;
  void **dpbase;
  void **dp;
  FILE *instream;
  FILE *outstream;
  int (*filllinebuffer)(struct usefulstate *);
  int (*error)(struct usefulstate *);
  char token[32];
  int tokenlen;
  char line[1024];
  size_t pos;
  intptr_t length;
  void *ctx;
  void *rv;
  enum INTERPRETER_STATE state;
  int base;
};




#define ARGS ip, xt, stacktop, retstacktop, next_, state


// currentcodeword: block - pointer to the codeword within a definition
// datastack: void** - 
// 

__attribute__((noinline)) void next(PARAMS);

extern void *defaultprogram[];
extern struct word *lastword;
extern struct word RET;


#define NEXT __attribute__((musttail)) return ((block)(next_))(ARGS)

#endif

