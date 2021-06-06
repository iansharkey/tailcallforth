#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "core.h"

// in the usual case (docol word A consisting of a list of words references b,c,d,...)
//  esi points to the next word references in word A
//  eax points to the current executing word reference's code field 

// in the case of an executing primitive word
//  esi points to the next word to execute after finishing the primitive
//  eax points to the currently executing word's code field

__attribute__((noinline)) void next(PARAMS) {
//static void next(PARAMS) {
  eax = *(void**)esi;
  esi = ((void**)esi)+1;
  block eax_ = *(block*)eax;

  __attribute__((musttail)) return eax_(ARGS);
    
}



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
  void *n3 = *stacktop++;
  void *n2 = *stacktop++;
  void *n1 = *stacktop++;
  *(--stacktop) = n3;
  *(--stacktop) = n1;
  *(--stacktop) = n2;
  NEXT;
}

void nrot(PARAMS) {
  void *n3 = *stacktop++;
  void *n2 = *stacktop++;
  void *n1 = *stacktop++;
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
  *(--stacktop) = n2;
  *(--stacktop) = n1;
  
  NEXT;
}



void qdup(PARAMS) {
  void *value = *stacktop;
  if (value) {
    *(--stacktop) = value;
  }
    
  NEXT;
}

static void dup(PARAMS) {
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
  intptr_t n2 = *((intptr_t*)stacktop++);
  intptr_t n1 = *((intptr_t*)stacktop++);
  *(--stacktop) = (void*)(n1-n2);
  NEXT;
}


void mul(PARAMS) {
  intptr_t a = *(intptr_t*)stacktop++;
  intptr_t b = *(intptr_t*)stacktop++;
  *(--stacktop) = (void*)(a*b);
  NEXT;
}


void _div(PARAMS) {
  intptr_t numerator = *(intptr_t*)stacktop++;
  intptr_t denominator = *(intptr_t*)stacktop++;
  *(--stacktop) = (void*)(numerator / denominator);
  NEXT;
}


void mod(PARAMS) {
  intptr_t numerator = *(intptr_t*)stacktop++;
  intptr_t denominator = *(intptr_t*)stacktop++;
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

void addstore(PARAMS) {
  intptr_t *addr = (intptr_t*)( *stacktop++);
  intptr_t value = (intptr_t)(*stacktop++);

  *addr += value;
  NEXT;
}

void substore(PARAMS) {
  intptr_t *addr = (intptr_t*)( *stacktop++);
  intptr_t value = (intptr_t)(*stacktop++);

  *addr -= value;
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
  *(stacktop-1) = (void*)(*((intptr_t*)stacktop-1) != *(intptr_t*)stacktop);
  stacktop--;
  NEXT;
}

void lt(PARAMS) {
  *(stacktop-1) = (void*)(*((intptr_t*)stacktop-1) < *(intptr_t*)stacktop);
  stacktop--;
  NEXT;
}

void gt(PARAMS) {
  *(stacktop-1) = (void*)(*((intptr_t*)stacktop-1) > *(intptr_t*)stacktop);
  stacktop--;
  NEXT;
}


void lte(PARAMS) {
  *(stacktop-1) = (void*)(*((intptr_t*)stacktop-1) <= *(intptr_t*)stacktop);
  stacktop--;
  NEXT;
}

void gte(PARAMS) {
  *(stacktop-1) = (void*)(*((intptr_t*)stacktop-1) >= *((intptr_t*)stacktop));
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



void fromr(PARAMS) {

  *(--stacktop) = *retstacktop;
  retstacktop++;
  NEXT;
}


void tor(PARAMS) {
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

struct litstring {
 intptr_t length;
 char str[];
};


void litstring(PARAMS) {
  // get current
  struct litstring *str = (struct litstring*)((void**)eax+1);
  *(--stacktop) = (void*)str->length;
  *(--stacktop) = (void*)&str->str;
  
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
  esi += offset;

  NEXT;
}

void zbranch(PARAMS) {
  void* value = *stacktop++;
  if (!value)
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
    esi = value;
  }

  eax = ((void**)eax)+2;
  *(--stacktop) = eax;

  
  NEXT;
}

void dodoes(PARAMS) {
  *(--stacktop) = &_dodoes;
  NEXT;
}

void dp(PARAMS) {
  *(--stacktop) = &state->dp;
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



void bracket_tick(PARAMS) {
  eax = *(void**)esi;
  esi = ((void**)esi)+1;
  *(--stacktop) = eax;
  NEXT;
}

void find(PARAMS) {
  struct word *word = state->latest;

  intptr_t length = *stacktop++;
  char *s = *stacktop++;
    
  while (word) {
    if (((word->flags & F_HIDDEN) != F_HIDDEN) && (__builtin_strncmp(word->name, s, 15) == 0)) {
      *(--stacktop) = word;
      NEXT;
    }
    word = word->prev;
  }

  *(--stacktop) = NULL;
  NEXT;
}

int getkey(struct usefulstate *state, char *c) {
  if (state->pos == state->length) {
    int rv = state->getnexttoken(state);
    if (rv < 0) {
      return rv;
    }
  }

  *c = state->line[state->pos++];
  return 1;
}



void key(PARAMS) {
  char c;
  int rv = getkey(state, &c);
  *(--stacktop) = (void*)c;
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


void cells(PARAMS) {
  *(--stacktop) = sizeof(void*);
  NEXT;
}

void docol_addr(PARAMS) {
  *(--stacktop) = (void*)docol;
  NEXT;
}

void stackbase(PARAMS) {
  *(--stacktop) = state->stackbase;
  NEXT;
}


void hidden(PARAMS) {
  struct word *latest = state->latest;
  latest->flags ^= F_HIDDEN;
  NEXT;
}

void cmove(PARAMS) {
  intptr_t length = (intptr_t)(*stacktop++);
  char *dest = (char*)(*stacktop++);
  char *src =  (char*)(*stacktop++);

  while(length--)
    *dest++ = *src++;

    
  NEXT;
}

void fetchbyte(PARAMS) {
  char *addr = (char*)*stacktop++;
  *(--stacktop) = (void*)(*addr);
  NEXT;
}

void storebyte(PARAMS) {
  char *addr = (char*)(*stacktop++);
  intptr_t c = (intptr_t)(*stacktop++);
  *addr = c;
  NEXT;
}

void copybyte(PARAMS) {
  char *dest = (char*)(*stacktop++);
  char *src = (char*)(*stacktop++);
  *dest = *src;
  *(--stacktop) = src++;
  *(--stacktop) = dest++;
  NEXT;
}


int _word(PARAMS) {
  int comment = 0;
  int i = 0;
  int rv;
  char c;

  
  do {
    rv = getkey(state, &c);
    if (rv < 0) {
      return rv;
    }

    if (!comment) {
      if (c == '\\') {
	comment = 1;
	continue;
      }

      if (c != ' ' && c != '\t' && c != '\r' && c != '\n') {
	break;
      }
    }
    else {
      if ( c == '\r' || c == '\n') {
	comment = 0;
      }
    }
  } while (rv >= 0);

  // skip comments

  while (c != ' ' && c != '\t' && c != '\r' && c != '\n') {
    state->token[i++] = c;
    rv = getkey(state, &c);
  }
  state->token[i] = '\0';

  state->tokenlen = i;

  return rv;
}


void word(PARAMS) {
  int rv = _word(ARGS);
  
  *(--stacktop) = (void*)state->token;
  *(--stacktop) = (void*)state->tokenlen;
  
  NEXT;
}



void number(PARAMS) {
  intptr_t length = *((intptr_t*)stacktop++);
  char *s = *stacktop++;
  long value = strtol(s, NULL, 10);
  *(--stacktop) = (void*)value;
  NEXT;
}




void compilestate(PARAMS) {
  *(--stacktop) = &state->state;
  NEXT;
}

void latest(PARAMS) {
  *(--stacktop) = &state->latest;

  NEXT;
}

void paren_do(PARAMS) {
  void *a = *stacktop++;
  void *b = *stacktop++;
  *(--retstacktop) = b;
  *(--retstacktop) = a;
  NEXT;
}

void paren_loop(PARAMS) {
  intptr_t limit = (intptr_t)(*retstacktop++);
  intptr_t index = (intptr_t)(*retstacktop++);
  index++;
  if (index != limit) {
    esi = ((void**)esi) + *((intptr_t*)esi);
    *(--retstacktop) = (void*)index;
    *(--retstacktop) = (void*)limit;
  }
  else {
    esi = ((void**)esi)+1;
  }
  
  NEXT;
}

void unloop(PARAMS) {
  retstacktop++;
  retstacktop++;
  NEXT;  
}

void inner_index(PARAMS) {
  *(--stacktop) = *retstacktop;
  NEXT;
}

void outer_index(PARAMS) {
  *(--stacktop) = *((void**)retstacktop+1);
  NEXT;
}

void execute(PARAMS) {
  eax = (block*)(*stacktop++);
  block eax_ = *(block*)eax;
  __attribute__((musttail)) return eax_(ARGS);

  NEXT;
}


struct word FETCH = { .prev = NULL, .name = "@",
                     .codeword = fetch };

struct word DUP = { .prev = &FETCH, .name = "dup",
                      .codeword =  dup };

static struct word FIND = {  .prev = &DUP, .name = "(find)", .codeword = find };

struct word LIT = {  .prev = &FIND, .name = "lit", .codeword = lit };


                        
struct word MUL = { .prev = &LIT, .name = "*", .codeword = mul };

struct word EXIT = { .prev = &MUL, .name = "EXIT", .codeword = exit_ };

struct word COMMA = { .prev = &EXIT, .name = ",", .codeword = comma };

struct word LBRAC = { .prev = &COMMA, .name = "[", .codeword = lbrac, .flags = F_IMMEDIATE };

struct word RBRAC = { .prev = &LBRAC, .name = "]", .codeword = rbrac };

struct word TERMINATE = { .prev = &RBRAC, .name = "bye", .codeword = terminate };

struct word ADD = { .prev = &TERMINATE, .name = "+", .codeword = add };
struct word SUB = { .prev = &ADD, .name = "-", .codeword = sub };

struct word INCR = { .prev = &SUB, .name = "1+", .codeword = incr };
struct word DECR = { .prev = &INCR, .name = "1-", .codeword = decr };

struct word BRANCH = {.prev = &DECR, .name = "branch", .codeword = branch };



struct word ZBRANCH = {.prev = &BRANCH, .name = "0branch", .codeword = zbranch };

struct word WORD = {.prev = &ZBRANCH, .name = "word", .codeword = word };

struct word KEY = {.prev= &WORD, .name = "key", .codeword = key };

struct word NUMBER = {.prev = &KEY, .name = "number", .codeword = number };

struct word HEADERCOMMA = {.prev = &NUMBER, .name = "header,", .codeword = headercomma };

struct word BRACKET_TICK = {.prev = &HEADERCOMMA, .name = "[']", .codeword = bracket_tick };

struct word STATE = {.prev = &BRACKET_TICK, .name = "state", .codeword = compilestate };

struct word PAREN_DO = {.prev = &STATE, .name = "(do)", .codeword = paren_do };

struct word PAREN_LOOP = {.prev = &PAREN_DO, .name = "(loop)", .codeword = paren_loop };

struct word INNER_INDEX = {.prev = &PAREN_LOOP, .name = "i", .codeword = inner_index };

struct word OUTER_INDEX = {.prev = &INNER_INDEX, .name = "j", .codeword = outer_index };


struct word LATEST = {.prev = &OUTER_INDEX, .name = "latest", .codeword = latest };

struct word DP = {.prev = &LATEST, .name = "dp", .codeword = dp };

struct word TCFA = {.prev = &DP, .name = ">cfa", .codeword = tcfa };

struct word TDFA = {.prev = &TCFA, .name = ">dfa", .codeword = tdfa };

struct word HIDDEN = {.prev = &TDFA, .name = "hidden", .codeword = hidden };

struct word IMMEDIATE_ = {.prev = &HIDDEN, .flags = F_IMMEDIATE, .name = "immediate", .codeword = immediate };

struct word COLON = {.prev = &IMMEDIATE_, .name = ":", .codeword = docol,
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
  int rv = _word(ARGS);
  if (rv < 0) {
    return;
  }

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
    if ( (state->state == COMPILING) && (word->flags & F_IMMEDIATE) != F_IMMEDIATE ) {
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

struct word SWAP = {.prev = &OVER, .name = "swap", .codeword = swap };

struct word ROT = { .prev = &SWAP, .name = "rot", .codeword = rot };

struct word NROT = { .prev = &ROT, .name = "-rot", .codeword = nrot };

struct word DROP = { .prev = &NROT, .name = "drop", .codeword = drop };

struct word TWODROP = { .prev = &DROP, .name = "2drop", .codeword = twodrop };

struct word TWODUP = { .prev = &TWODROP, .name = "2dup", .codeword = twodup };

struct word QDUP = { .prev = &TWODUP, .name = "?dup", .codeword = qdup };

struct word DIV = {.prev = &QDUP, .name = "/", .codeword = _div };

struct word MOD = {.prev = &DIV, .name = "%", .codeword = mod };

struct word STORE = {.prev = &MOD, .name = "!", .codeword = store };

struct word ADDSTORE = {.prev = &STORE, .name = "+!", .codeword = addstore };

struct word SUBSTORE = {.prev = &ADDSTORE, .name = "-!", .codeword = substore };

struct word EXECUTE = {.prev = &SUBSTORE, .name = "execute", .codeword = execute };

struct word CMOVE = {.prev = &EXECUTE, .name = "cmove", .codeword = cmove };

struct word FETCHBYTE = {.prev = &CMOVE, .name = "c@", .codeword = fetchbyte };

struct word STOREBYTE = {.prev = &FETCHBYTE, .name = "c!", .codeword = storebyte };

#define logicalop(last, sname, fname, cword) \
  struct word sname = { .prev = &last, .name = fname, .codeword = cword }

logicalop(STOREBYTE, EQU, "=", equ);
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
logicalop(DSPFETCH, CELLS, "cells", cells);
logicalop(CELLS, DOCOL_ADDR, "docol", docol_addr);
logicalop(DOCOL_ADDR, STACKBASE, "s0", stackbase);
logicalop(STACKBASE, DSPSTORE, "dsp!", dspstore);


void* defaultprogram[] = { &INTERPRET.codeword, &BRANCH.codeword, (void*)(-2*sizeof(void*)), &TERMINATE.codeword };

struct word *lastword = &DSPSTORE;
