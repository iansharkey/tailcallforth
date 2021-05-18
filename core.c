#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

enum INTERPRETER_STATE {
    COMPILING,
    IMMEDIATELY
};

enum WORD_FLAGS {
    HIDDEN,
    IMMEDIATE
};

#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

typedef void (*block)(void*, void*, void**, void**, void**, void**, enum INTERPRETER_STATE);

//void* datastack_[256];
//void* returnstack_[256];
//void* w;

//size_t stacktop;
// void* returnstack[256]; // handled by system stack

//void* latest;
//void* dict[1024];


struct word {
    struct word* prev;
    unsigned int flags:16;
    char name[6];

    block codeword; // first impl block is machine word
    void* extra[];
};


// currentcodeword: block - pointer to the codeword within a definition
// datastack: void** - 
// 

#define PARAMS void* ip, void* currentcodeword, void** stacktop, void** retstacktop, void** latest, void** here, enum INTERPRETER_STATE state


static __attribute__((noinline)) void next(PARAMS) {
//static void next(PARAMS) {
  block ip_ = **((block**)currentcodeword);
  
    
    __attribute__((musttail)) return (ip_)(currentcodeword, ((void**)currentcodeword)+1, stacktop, retstacktop, latest, here, state);
    
}
#define NEXT __attribute__((musttail)) return next(ip, currentcodeword, stacktop, retstacktop, latest, here, state)




void docol(PARAMS) {
  struct word* currentword = container_of(*(void**)ip, struct word, codeword);
  //    printf("docol'ing %.6s, currentcodeword: %p\n", currentword->name, currentcodeword);
    *(++retstacktop) = currentcodeword;
    currentcodeword = &currentword->extra[0];
    NEXT;
}


void exit_(PARAMS) {
    currentcodeword = *(retstacktop--);
    NEXT;
}


void swap(PARAMS) {
  void *a, *b;
  a = *stacktop;
  b = *(stacktop-1);
  *stacktop = b;
  *(stacktop-1) = a;
  NEXT;
}


void over(PARAMS) {
  stacktop++;
  *stacktop = *(stacktop-2);
  NEXT;
}

void twodrop(PARAMS) {
  stacktop -= 2;
  NEXT;
}


void dupnz(PARAMS) {
  void *value = *stacktop;
  if (value) {
    stacktop++;
    *stacktop = value;
  }
    
  NEXT;
}

void dup(PARAMS) {
    *(stacktop+1) = *stacktop;
    stacktop++;
    NEXT;
}

void drop(PARAMS) {
    stacktop--;
    NEXT;
}


void lit(PARAMS) {
    *(++stacktop) = *(void**)currentcodeword;
    currentcodeword = (block*)currentcodeword+1;
    NEXT;
}

void add(PARAMS) {
  *(stacktop-1) = (void*)(*((intptr_t*)stacktop-1)) + *(intptr_t*)stacktop;
  stacktop--;
  NEXT;
}

void sub(PARAMS) {
  *(stacktop-1) = (void*)(*((intptr_t*)stacktop-1)) - *(intptr_t*)stacktop;
  stacktop--;
  NEXT;
}


void mul(PARAMS) {
  *(stacktop-1) = (void*)(*((intptr_t*)stacktop-1) * *(intptr_t*)stacktop);
    stacktop--;
    NEXT;
}


void divmod(PARAMS) {
  intptr_t numerator = *(intptr_t*)(stacktop-1);
  intptr_t denominator = *(intptr_t*)stacktop;
  *(stacktop-1) = (void*)(numerator / denominator);
  *stacktop = (void*)(numerator % denominator);
    NEXT;
}

void incr(PARAMS) {
  *(intptr_t*)stacktop += 1;
  NEXT;
}

void decr(PARAMS) {
  *(intptr_t*)stacktop -= 1;
  NEXT;
}


void peek(PARAMS) {
    *stacktop = *((void**)*stacktop);
    NEXT;
}

void comma(PARAMS) {
    *(++here) = *(stacktop--);
    NEXT;
}

void lbrac(PARAMS) {
    state = IMMEDIATELY;
    NEXT;
}

void rbrac(PARAMS) {
    state = COMPILING;
    NEXT;
}

void terminate(PARAMS) {
  exit(*(intptr_t*)stacktop);
  NEXT;
}


void equ(PARAMS) {
  *(stacktop-1) = (void*)(*((intptr_t*)stacktop-1)) == *(intptr_t*)stacktop;
  stacktop--;
  NEXT;
}


void nequ(PARAMS) {
  *(stacktop-1) = (void*)(*((intptr_t*)stacktop-1)) != *(intptr_t*)stacktop;
  stacktop--;
  NEXT;
}

void lt(PARAMS) {
  *(stacktop-1) = (void*)(*((intptr_t*)stacktop-1)) < *(intptr_t*)stacktop;
  stacktop--;
  NEXT;
}

void gt(PARAMS) {
  *(stacktop-1) = (void*)(*((intptr_t*)stacktop-1)) > *(intptr_t*)stacktop;
  stacktop--;
  NEXT;
}


void lte(PARAMS) {
  *(stacktop-1) = (void*)(*((intptr_t*)stacktop-1)) <= *(intptr_t*)stacktop;
  stacktop--;
  NEXT;
}

void gte(PARAMS) {
  *(stacktop-1) = (void*)(*((intptr_t*)stacktop-1)) >= *(intptr_t*)stacktop;
  stacktop--;
  NEXT;
}

void zequ(PARAMS) {
  *stacktop = (void*)(*(intptr_t*)stacktop == 0);
  NEXT;
}

void znequ(PARAMS) {
  *stacktop = *(intptr_t*)stacktop != 0;
  NEXT;
}


void zlt(PARAMS) {
  *stacktop = *(intptr_t*)stacktop < 0;
  NEXT;
}

void zgt(PARAMS) {
  *stacktop = *(intptr_t*)stacktop > 0;
  NEXT;
}


void zlte(PARAMS) {
  *stacktop = *(intptr_t*)stacktop <= 0;
  NEXT;
}


void zgte(PARAMS) {
  *stacktop = (void*)(*(intptr_t*)stacktop >= 0);
  NEXT;
}


void bitand(PARAMS) {
  *(stacktop-1) = (void*)((*((intptr_t*)stacktop-1)) & *(intptr_t*)stacktop);
  stacktop--;
  NEXT;
}


void bitor(PARAMS) {
  *(stacktop-1) = (void*)((*((intptr_t*)stacktop-1)) | *(intptr_t*)stacktop);
  stacktop--;
  NEXT;
}

void bitxor(PARAMS) {
  *(stacktop-1) = (void*)((*((intptr_t*)stacktop-1)) ^ *((intptr_t*)stacktop));
  stacktop--;
  NEXT;
}


void bitnot(PARAMS) {
  *stacktop = (void*)~(*(intptr_t*)stacktop);
  NEXT;
}



void tor(PARAMS) {
  stacktop++;
  *stacktop = *retstacktop;
  retstacktop--;
  NEXT;
}


void fromr(PARAMS) {
  retstacktop++;
  *retstacktop = *stacktop;
  stacktop--;
  NEXT;
}

void rspfetch(PARAMS) {
  stacktop++;
  *stacktop = *retstacktop;
  NEXT;
}

void rspstore(PARAMS) {
  *retstacktop = *stacktop;
  stacktop--;
  NEXT;
}


void rspdrop(PARAMS) {
  retstacktop--;
  NEXT;
}


void dspfetch(PARAMS) {
  stacktop++;
  *stacktop = stacktop;
  NEXT;
}

void dspstore(PARAMS) {
  stacktop = (void**)*stacktop;
  NEXT;
}


/*
TODO
 x sub, divmod, incr, decr
 x logical operators
 x comparison operators
 stack operators (swap, rot, over, dsp@, dsp!)  
 memory operators (store, addstore, substore, byte store, byte copy, byte move)
 builtin variables (state, here, latest, s0, base)
 return stack (>R, R>,  RSP@, RSP!, RDROP)
 input/output
 dictionary (find, >CFA, >DFA)
 compile (:, ;, create, header_comma, dodoes, hidden, hide, tick)
 branching (branch, 0branch)
 strings (litstring, tell)
 interpreter (quit, interpret)
 misc (execute, 
 
 

 */


void display_number(PARAMS) {
  int num = *((int*)stacktop--);
  printf("%d\n", num);
  NEXT;
}

void find(PARAMS) {
    struct word* word = container_of(*latest, struct word, codeword);
    
    while (word) {
        if (memcmp(word->name, *stacktop, 6) == 0) {
            *stacktop = word;
            NEXT;
        }
        word = word->prev;
    }
    *stacktop = NULL;
    NEXT;
}


struct word PEEK = { .prev = NULL, .name = "PEEK",
                     .codeword = peek };
struct word DUP = { .prev = &PEEK, .name = "DUP",
                      .codeword =  dup };

struct word FIND = {  .prev = &DUP, .name = "FIND", .codeword = find };

struct word LIT = {  .prev = &FIND, .name = "LIT", .codeword = lit };


                        
struct word MUL = { .prev = &LIT, .name = "*", .codeword = mul };

struct word EXIT = { .prev = &MUL, .name = ";", .codeword = exit_ };

struct word SQUARE = { .prev = &EXIT,  .name = "SQUARE", .codeword = docol, .extra = { &DUP.codeword, &MUL.codeword, &EXIT.codeword } };



struct word COMMA = { .prev = &SQUARE, .name = ",", .codeword = comma };

struct word LBRAC = { .prev = &COMMA, .name = "[", .codeword = lbrac, .flags = IMMEDIATE };

struct word RBRAC = { .prev = &LBRAC, .name = "]", .codeword = rbrac };

struct word QUADRUPLE = { .prev = &RBRAC, .name = "QUAD", .codeword = docol, .extra = { &SQUARE.codeword, &SQUARE.codeword, &EXIT.codeword } };


struct word TERMINATE = { .prev = &QUADRUPLE, .name = "TERM", .codeword = terminate };

struct word ADD = { .prev = &TERMINATE, .name = "+", .codeword = add };
struct word SUB = { .prev = &ADD, .name = "-", .codeword = sub };

struct word FOUR = { .prev = &SUB, .name = "FOUR", .codeword = docol, .extra = { &LIT.codeword, (void*)2, &DUP.codeword, &ADD.codeword, &EXIT.codeword } };

struct word DISPLAY_NUMBER = { .prev = &FOUR, .name = ".", .codeword = display_number };
struct word INCR = { .prev = &DISPLAY_NUMBER, .name = "1+", .codeword = incr };
struct word DECR = { .prev = &INCR, .name = "1-", .codeword = decr };



int main(int argc, char** argv)
{

  void* datastack[256];
  void* returnstack[256];
  void* buffer[256];
  void* currentip;


  void** stacktop = &datastack[0];
  void** retstacktop = &returnstack[0];
  void** here = &buffer[0];
  void** latest = buffer;
  void* ip[] = { &FOUR.codeword, &QUADRUPLE.codeword, &INCR.codeword, &DUP.codeword, &LIT.codeword, (void*)-1, &MUL.codeword, &DISPLAY_NUMBER.codeword, &TERMINATE.codeword };
  block blah = docol;
  
  
  next(ip, &ip, stacktop, retstacktop, latest, here, IMMEDIATELY);
  
  
  return 0;
}
