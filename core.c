#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

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
    char name[10];

    block codeword; // first impl block is machine word
    void* extra[];
};


struct usefulstate {
  struct word *latest;
  void **here;
  void **dp;
  int (*getnexttoken)(struct usefulstate*);
  char token[33];
  intptr_t length;
  void *ctx;
  enum INTERPRETER_STATE state;
};


// currentcodeword: block - pointer to the codeword within a definition
// datastack: void** - 
// 

#define PARAMS struct usefulstate *state, void* esi, void* eax, void** stacktop, void** retstacktop


#define ARGS state, esi, eax, stacktop, retstacktop

static __attribute__((noinline)) void next(PARAMS) {
//static void next(PARAMS) {
  eax = *(void**)esi;
  esi = ((void**)esi)+1;
  block eax_ = *(block*)eax;

  __attribute__((musttail)) return eax_(ARGS);
    
}
#define NEXT __attribute__((musttail)) return next(ARGS)




void docol(PARAMS) {
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

void rot(PARAMS) {
  void *n1 = *stacktop++;
  void *n2 = *stacktop++;
  void *n3 = *stacktop++;
  *(--stacktop) = n3;
  *(--stacktop) = n2;
  *(--stacktop) = n2;
  NEXT;
}

void nrot(PARAMS) {
  void *n1 = *stacktop++;
  void *n2 = *stacktop++;
  void *n3 = *stacktop++;
  *(--stacktop) = n2;
  *(--stacktop) = n3;
  *(--stacktop) = n1;
  NEXT;
}


void twodrop(PARAMS) {
  stacktop += 2;
  NEXT;
}

void twodup(PARAMS) {
  void *n1 = *stacktop;
  void *n2 = *(stacktop+1);
  *(--stacktop) = n1;
  *(--stacktop) = n2;
  
  NEXT;
}



void qdup(PARAMS) {
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
  *(--stacktop) = *((void**)esi);
  esi = (void**)esi+1;
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


void fetch(PARAMS) {
  void **addr = (void**)(*stacktop++);
  *(--stacktop) = *addr;
  NEXT;
}

void store(PARAMS) {
  void **addr = (void**)( *stacktop++);
  void *value = *stacktop++;
  *addr = value;
  NEXT;
}


void comma(PARAMS) {
  void **dp = state->dp;
  *dp++ = *stacktop++;
  state->dp = dp;
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
  intptr_t a = *((intptr_t*)(stacktop++));
  intptr_t b = *((intptr_t*)(stacktop++));
  *(--stacktop) = (void*)(intptr_t)(a & b);
  NEXT;
}


void bitor(PARAMS) {
  intptr_t a = *((intptr_t*)(stacktop++));
  intptr_t b = *((intptr_t*)(stacktop++));
  *(--stacktop) = (void*)(intptr_t)(a | b);
  NEXT;
}

void bitxor(PARAMS) {
  intptr_t a = *((intptr_t*)(stacktop++));
  intptr_t b = *((intptr_t*)(stacktop++));
  *(--stacktop) = (void*)(intptr_t)(a ^ b);

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
  void *value = stacktop;
  *(--stacktop) = value;
  NEXT;
}

void dspstore(PARAMS) {
  stacktop = (void**)*stacktop;
  NEXT;
}


void branch(PARAMS) {
  intptr_t offset = *(intptr_t*)esi;
  esi = ((void**)esi)+offset;

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
    esi = ((void**)esi)+1;
  }
  NEXT;
}

void _dodoes(PARAMS) {
  void *value = *(((void**)eax)+1);

  if ( value ) {
    *(--retstacktop) = esi;
    esi = *(((void**)eax)+1);
  }
  else {
    eax = ((void**)eax)+2;
    *(--stacktop) = eax;
  }
  
  NEXT;
}

void dodoes(PARAMS) {
  *(--stacktop) = &_dodoes;
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
 x dictionary (find, >CFA, >DFA)
 compile (:, ;, create, header_comma, dodoes, hidden, hide, tick)
 x branching (branch, 0branch)
 strings (litstring, tell)
 x interpreter (quit, interpret)
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
    if (((word->flags & F_HIDDEN) != F_HIDDEN) && (memcmp(word->name, *stacktop, 6) == 0)) {
      *(--stacktop) = word;
      NEXT;
    }
    word = word->prev;
  }

  *(--stacktop) = NULL;
  NEXT;
}

void tcfa(PARAMS) {
  struct word *word = (struct word*)*stacktop++;
  *(--stacktop) = &word->codeword;
  NEXT;
}

void tdfa(PARAMS) {
  struct word *word = (struct word*)*stacktop++;
  *(--stacktop) = &word->extra;
  NEXT;
}

void headercomma(PARAMS) {
  int length = *((intptr_t*)stacktop++);
  char *name = *((char**)stacktop++);
  void **dp = state->dp;

  // add a new word 
  struct word *newword = (struct word*)dp;
  __builtin_memcpy(newword->name, name, 10);
  newword->prev = state->latest;
  newword->flags = 0;

  state->dp = (void**)&newword->codeword;
  state->latest = newword;

  NEXT;
}

void immediate(PARAMS) {
  state->latest->flags ^= F_IMMEDIATE;
  NEXT;
}

void hidden(PARAMS) {
  struct word *latest = state->latest;
  latest->flags ^= F_HIDDEN;
  NEXT;
}


void __attribute__((always_inline)) _word(struct usefulstate *state) {
  state->getnexttoken(state);
}

void word(PARAMS) {
  char *name;

  _word(state);

  *(--stacktop) = state->token;
  *(--stacktop) = (void*)state->length;
  
  NEXT;
}



void  number(PARAMS) {
  intptr_t length = *((intptr_t*)stacktop++);
  char *s = *stacktop++;
  long value = strtol(s, NULL, 10);
  *(--stacktop) = value;
  NEXT;
}


void latest(PARAMS) {
  *(--stacktop) = &state->latest;

  NEXT;
}

struct word FETCH = { .prev = NULL, .name = "@",
                     .codeword = fetch };

struct word DUP = { .prev = &FETCH, .name = "DUP",
                      .codeword =  dup };

struct word FIND = {  .prev = &DUP, .name = "FIND", .codeword = find };

struct word LIT = {  .prev = &FIND, .name = "LIT", .codeword = lit };


                        
struct word MUL = { .prev = &LIT, .name = "*", .codeword = mul };

struct word EXIT = { .prev = &MUL, .name = "EXIT", .codeword = exit_ };

struct word SQUARE = { .prev = &EXIT,  .name = "SQUARE", .codeword = docol, .extra = { &DUP.codeword, &MUL.codeword, &EXIT.codeword } };



struct word COMMA = { .prev = &SQUARE, .name = ",", .codeword = comma };

struct word LBRAC = { .prev = &COMMA, .name = "[", .codeword = lbrac, .flags = F_IMMEDIATE };

struct word RBRAC = { .prev = &LBRAC, .name = "]", .codeword = rbrac };

struct word TERMINATE = { .prev = &RBRAC, .name = "bye", .codeword = terminate };

struct word ADD = { .prev = &TERMINATE, .name = "+", .codeword = add };
struct word SUB = { .prev = &ADD, .name = "-", .codeword = sub };

struct word DISPLAY_NUMBER = { .prev = &SUB, .name = ".", .codeword = display_number };

struct word INCR = { .prev = &DISPLAY_NUMBER, .name = "1+", .codeword = incr };
struct word DECR = { .prev = &INCR, .name = "1-", .codeword = decr };

struct word BRANCH = {.prev = &DECR, .name = "branch", .codeword = branch };

struct word WORD = {.prev = &BRANCH, .name = "word", .codeword = word };

struct word NUMBER = {.prev = &WORD, .name = "number", .codeword = number };

struct word HEADERCOMMA = {.prev = &WORD, .name = "header,", .codeword = headercomma };

struct word LATEST = {.prev = &HEADERCOMMA, .name = "latest", .codeword = latest };

struct word TCFA = {.prev = &LATEST, .name = ">cfa", .codeword = tcfa };

struct word TDFA = {.prev = &TCFA, .name = ">dfa", .codeword = tdfa };

struct word HIDDEN = {.prev = &TDFA, .name = "hidden", .codeword = hidden };

struct word COLON = {.prev = &HIDDEN, .name = ":", .codeword = docol,
		     .extra = { &WORD.codeword, &HEADERCOMMA.codeword,
			        &LIT.codeword, docol, &COMMA.codeword,
			        &LATEST.codeword, &FETCH.codeword,
			        &HIDDEN.codeword,
			        &RBRAC.codeword, &EXIT.codeword } };

struct word SEMICOLON = { .prev = &COLON, .flags = F_IMMEDIATE, .name = ";", .codeword = docol,
		     .extra = { &LIT.codeword, &EXIT.codeword, &COMMA.codeword,
				&LATEST.codeword, &FETCH.codeword,
				&HIDDEN.codeword,
				&LBRAC.codeword, &EXIT.codeword } };

void interpret(PARAMS) {
  _word(state);

  struct word *word = state->latest;
  int found = 0;  
  while (word) {
    if ((word->flags & F_HIDDEN) != F_HIDDEN) {
      char *hay = word->name;
      char *needle = state->token;
      while (*hay)
      {
        // if characters differ, or end of the second string is reached
        if (*hay != *needle) {
            break;
        }
	hay++; needle++;
      }
      int rv = (*(const unsigned char*)hay - *(const unsigned char*)needle);
      
      if (rv == 0) {
	found = 1;
	break;
      }
    }
    word = word->prev;
  }

  if (!found) { // assume number
    long v = strtol(state->token, NULL, 10);
    if ( state->state == COMPILING ) {
      void **dp = state->dp;
      *dp++ = &LIT.codeword;
      *dp++ = (void*)v;
      state->dp = dp;
    }
    else {
      *(--stacktop) = (void*)v;
    }
    NEXT;    
  }
  else {
    if ( state->state == COMPILING && word->flags != F_IMMEDIATE ) {
      void **dp = state->dp;
      *dp++ = &word->codeword;
      state->dp = dp;
    }
    else {
      eax = &word->codeword;
      __attribute__((musttail)) return word->codeword(ARGS);
    }
  }
  NEXT; 
  
}


struct word INTERPRET = { .prev = &SEMICOLON, .name = "interpret", .codeword = interpret };

struct word DODOES = {.prev = &INTERPRET, .name = "dodoes", .codeword = dodoes };

struct word OVER = {.prev = &DODOES, .name = "over", .codeword = over };

struct word ROT = { .prev = &OVER, .name = "rot", .codeword = rot };

struct word NROT = { .prev = &ROT, .name = "-rot", .codeword = nrot };

struct word TWODROP = { .prev = &NROT, .name = "2drop", .codeword = twodrop };

struct word TWODUP = { .prev = &TWODROP, .name = "2dup", .codeword = twodup };

struct word QDUP = { .prev = &TWODUP, .name = "?dup", .codeword = qdup };

struct word DIVMOD =  {.prev = &QDUP, .name = "/mod", .codeword = divmod };

struct word STORE = {.prev = &DIVMOD, .name = "!", .codeword = store };

#define logicalop(last, sname, fname, cword) \
  struct word sname = { .prev = &last, .name = fname, .codeword = cword }

logicalop(STORE, EQU, "=", equ);
logicalop(EQU, NEQU, "<>", nequ);
logicalop(NEQU, LT, "<", lt);
logicalop(LT, GT, ">", gt);
logicalop(GT, LTE, "<=", lte);
logicalop(LTE, GTE, ">=", gte);
logicalop(GTE, ZEQU, "0=", zequ);
logicalop(ZEQU, ZNEQU, "0<>", znequ);
logicalop(ZNEQU, ZLT,  "0<", zlt);
logicalop(ZLT, ZGT, "0>", zgt);

logicalop(ZGT, ZLTE, "0<=", zlte);
logicalop(ZLTE, ZGTE, "0>=", zgte);

logicalop(ZGTE, BITAND, "and", bitand);
logicalop(BITAND, BITOR, "or", bitor);
logicalop(BITOR, BITXOR, "xor", bitxor);
logicalop(BITXOR, BITNOT, "invert", bitnot);

logicalop(BITNOT, TOR, ">r", tor);
logicalop(TOR, FROMR, "r>", fromr);

logicalop(FROMR, RSPFETCH, "rsp@", rspfetch);
logicalop(RSPFETCH, RSPSTORE, "rsp!", rspstore);
logicalop(RSPSTORE, RDROP, "rdrop", rspdrop); 

logicalop(RDROP, DSPFETCH, "dsp@", dspfetch);
logicalop(DSPFETCH, DSPSTORE, "dsp!", dspstore);


int scanf_token(struct usefulstate *state) {
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
  void* buffer[256] = { 0 };

  void** stacktop = &datastack[255];
  void** retstacktop = &returnstack[255];
  void** here = &buffer[0];

  struct usefulstate state;
  state.getnexttoken = scanf_token;
  state.here = here;
  state.dp = buffer;
  state.latest = &DSPSTORE;
  
  //void* ip[] = { &WORD.codeword, &NUMBER.codeword, &QUADRUPLE.codeword, &INCR.codeword, &DUP.codeword, &LIT.codeword, (void*)-1, &MUL.codeword, &DISPLAY_NUMBER.codeword, &TERMINATE.codeword };
  void* ip[] = { &INTERPRET.codeword, &BRANCH.codeword, (void*)-2, &TERMINATE.codeword };
  
  next(&state, &ip[0], 0, stacktop, retstacktop);
  
  
  return 0;
}
