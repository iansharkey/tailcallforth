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


struct usefulstate;



#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

typedef void (*block)(struct usefulstate*, void*, void*, void**, void**);

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


struct usefulstate {
  struct word *latest;
  void **here;
  void **dp;
  int (*getnexttoken)(struct usefulstate*, void *ctx);
  char token[33];
  intptr_t length;
  void *ctx;
  enum INTERPRETER_STATE state;
};


// currentcodeword: block - pointer to the codeword within a definition
// datastack: void** - 
// 

#define PARAMS struct usefulstate *state, void* esi, void* eax, void** stacktop, void** retstacktop


static __attribute__((noinline)) void next(PARAMS) {
//static void next(PARAMS) {
  eax = *(void**)esi;
  esi = ((void**)esi)+1;
  block eax_ = *(block*)eax;

  __attribute__((musttail)) return eax_(state, esi, eax, stacktop, retstacktop);
    
}
#define NEXT __attribute__((musttail)) return next(state, esi, eax, stacktop, retstacktop)




void docol(PARAMS) {
  //    printf("docol'ing %.6s, currentcodeword: %p\n", currentword->name, currentcodeword);
    *(--retstacktop) = esi;
    eax = ((void**)eax)+1;
    esi = eax;
    NEXT;
}


void exit_(PARAMS) {
    esi = *retstacktop++;
    NEXT;
}


void swap(PARAMS) {
  void *a, *b;
  a = *stacktop++;
  b = *stacktop++;
  *(--stacktop) = a;
  *(--stacktop) = b;
  NEXT;
}


void over(PARAMS) {
  void *value = *(stacktop+2);
  *(--stacktop) = value;
  NEXT;
}

void twodrop(PARAMS) {
  stacktop += 2;
  NEXT;
}


void dupnz(PARAMS) {
  void *value = *stacktop;
  if (value) {
    *(--stacktop) = value;
  }
    
  NEXT;
}

void dup(PARAMS) {
  void *value = *stacktop;
  *(--stacktop) = value;
  NEXT;
}

void drop(PARAMS) {
    stacktop++;
    NEXT;
}



void lit(PARAMS) {
    *(--stacktop) = *(void**)esi;
    esi = (block*)esi+1;
    NEXT;
}

void add(PARAMS) {
  intptr_t a = *((intptr_t*)stacktop++);
  intptr_t b = *((intptr_t*)stacktop++);
  *(--stacktop) = (void*)(a+b);
  NEXT;
}

void sub(PARAMS) {
  intptr_t a = *((intptr_t*)stacktop++);
  intptr_t b = *((intptr_t*)stacktop++);
  *(--stacktop) = (void*)(a-b);
  NEXT;
}


void mul(PARAMS) {
  intptr_t a = *(intptr_t*)stacktop++;
  intptr_t b = *(intptr_t*)stacktop++;
  *(--stacktop) = (void*)(a*b);
  NEXT;
}


void divmod(PARAMS) {
  intptr_t numerator = *(intptr_t*)stacktop++;
  intptr_t denominator = *(intptr_t*)stacktop++;
  *(--stacktop) = (void*)(numerator / denominator);
  *(--stacktop) = (void*)(numerator % denominator);
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
  void *value = *((void**)*stacktop);
  *(--stacktop) = value;
  NEXT;
}

void comma(PARAMS) {
  void **dp = state->dp;
  *dp++ = *stacktop++;
  NEXT;
}

void lbrac(PARAMS) {
    state->state = IMMEDIATELY;
    NEXT;
}

void rbrac(PARAMS) {
    state->state = COMPILING;
    NEXT;
}

void terminate(PARAMS) {
  exit(*(intptr_t*)stacktop);
  NEXT;
}


void equ(PARAMS) {
  intptr_t a = *((intptr_t*)(stacktop++));
  intptr_t b = *((intptr_t*)(stacktop++));
  *(--stacktop) = (void*)(intptr_t)(a == b);
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
  *stacktop = (*(intptr_t*)stacktop) < 0;
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

  *(--stacktop) = *retstacktop;
  retstacktop++;
  NEXT;
}


void fromr(PARAMS) {
  *(--retstacktop) = *stacktop++;

  NEXT;
}

void rspfetch(PARAMS) {
  *(--stacktop) = *retstacktop;
  NEXT;
}

void rspstore(PARAMS) {
  *retstacktop = *stacktop++;
  NEXT;
}


void rspdrop(PARAMS) {
  retstacktop--;
  NEXT;
}


void dspfetch(PARAMS) {

  *(--stacktop) = stacktop;
  NEXT;
}

void dspstore(PARAMS) {
  stacktop = (void**)*stacktop;
  NEXT;
}


void branch(PARAMS) {
  intptr_t offset = *(intptr_t*)esi;
  esi += offset;

  NEXT;
}

void zbranch(PARAMS) {
  void* value = *stacktop++;
  if (value)
  {
     intptr_t offset = *(intptr_t*)esi;
     esi += offset;
  }
  else
  {
    esi += 1;
  }
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
 input/output (word, number, emit)
 dictionary (find, >CFA, >DFA)
 compile (:, ;, create, header_comma, dodoes, hidden, hide, tick)
 x branching (branch, 0branch)
 strings (litstring, tell)
 interpreter (quit, interpret)
 misc (execute, 
 
 

 */


void display_number(PARAMS) {
  int num = *((int*)stacktop++);
  printf("%d\n", num);
  NEXT;
}

void find(PARAMS) {
  struct word *word = state->latest;
    
    while (word) {
      if (((word->flags & HIDDEN) != HIDDEN) && (memcmp(word->name, *stacktop, 6) == 0)) {
            *stacktop = word;
            NEXT;
        }
        word = word->prev;
    }
    *stacktop = NULL;
    NEXT;
}


void headercomma(PARAMS) {
  int length = *((intptr_t*)stacktop++);
  char *name = *((char**)stacktop++);
  void **dp = state->dp;
  
  struct word *newword = (struct word*)*dp;
  memcpy(newword->name, name, 6);
  newword->prev = state->latest;
  newword->flags = 0;

  state->dp = (void**)&newword->codeword;
  state->latest = newword;

  NEXT;
}

void immediate(PARAMS) {
  state->latest->flags ^= IMMEDIATE;
  NEXT;
}

void word(PARAMS) {
  char *name;
  intptr_t length = state->getnexttoken(state, state->ctx);

  *(--stacktop) = state->token;
  *(--stacktop) = (void*)state->length;
  
  NEXT;
}



void  number(PARAMS) {
  intptr_t length = *stacktop++;
  char *s = *stacktop++;
  long value = strtol(s, NULL, 10);
  *(--stacktop) = value;
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

struct word BRANCH = {.prev = &DECR, .name = "BRANCH", .codeword = branch };

struct word WORD = {.prev = &BRANCH, .name = "WORD", .codeword = word };

struct word NUMBER = {.prev = &WORD, .name = "NUMBER", .codeword = number };

int scanf_token(struct usefulstate *state, void *ctx) {
  int length;
  int rv = scanf(" %32s%n", state->token, &length);
  state->token[32] = 0;
  state->length = length;
  return rv;  
}


int main(int argc, char** argv)
{

  void* datastack[256];
  void* returnstack[256];
  void* buffer[256];

  void** stacktop = &datastack[255];
  void** retstacktop = &returnstack[255];
  void** here = &buffer[0];

  struct usefulstate state;
  state.getnexttoken = scanf_token;
  state.here = here;
  state.dp = buffer;
  state.latest = &BRANCH;
  
  void* ip[] = { &WORD.codeword, &NUMBER.codeword, &QUADRUPLE.codeword, &INCR.codeword, &DUP.codeword, &LIT.codeword, (void*)-1, &MUL.codeword, &DISPLAY_NUMBER.codeword, &TERMINATE.codeword };
  
  next(&state, &ip[0], 0, stacktop, retstacktop);
  
  
  return 0;
}
