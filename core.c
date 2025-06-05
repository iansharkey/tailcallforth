#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include "core.h"

// in the usual case (docol word A consisting of a list of words references b,c,d,...)
//  ip points to the next word references in word A
//  xt points to the current executing word reference's code field 

// in the case of an executing primitive word
//  ip points to the next word to execute after finishing the primitive
//  xt points to the currently executing word's code field

/**
 * NEXT - The core of the Forth inner interpreter
 * Fetches the next word to execute and tail-calls it
 */
__attribute__((noinline)) void next(PARAMS) {
//static void next(PARAMS) {
  xt = *(void**)ip;
  ip = ((void**)ip)+1;
  block xt_ = *(block*)xt;

  __attribute__((musttail)) return xt_(ARGS);
    
}



/**
 * DOCOL - The code field for colon definitions
 * Saves the current instruction pointer to the return stack
 * and sets up to execute the words in the definition
 */
void docol(PARAMS) {
    *(--retstacktop) = ip;
    xt = ((void**)xt)+1;

    ip = xt;

    NEXT;
}


/**
 * EXIT - Exits the current word definition
 * Restores the instruction pointer from the return stack
 */
void exit_(PARAMS) {
    ip = *retstacktop++;
    NEXT;
}


/**
 * SWAP - ( x1 x2 -- x2 x1 )
 * Exchanges the top two items on the stack
 */
void swap(PARAMS) {
  void *a, *b;
  a = *stacktop++;
  b = *stacktop++;
  *(--stacktop) = a;
  *(--stacktop) = b;
  NEXT;
}


/**
 * OVER - ( x1 x2 -- x1 x2 x1 )
 * Copies the second item on the stack to the top
 */
void over(PARAMS) {
  void *value = *(((void**)stacktop)+1);
  *(--stacktop) = value;
  NEXT;
}

/**
 * ROT - ( x1 x2 x3 -- x2 x3 x1 )
 * Rotates the top three items on the stack
 */
void rot(PARAMS) {
  void *n3 = *stacktop++;
  void *n2 = *stacktop++;
  void *n1 = *stacktop++;
  *(--stacktop) = n3;
  *(--stacktop) = n1;
  *(--stacktop) = n2;
  NEXT;
}

/**
 * -ROT - ( x1 x2 x3 -- x3 x1 x2 )
 * Rotates the top three items on the stack in the opposite direction
 */
void nrot(PARAMS) {
  void *n3 = *stacktop++;
  void *n2 = *stacktop++;
  void *n1 = *stacktop++;
  *(--stacktop) = n2;
  *(--stacktop) = n3;
  *(--stacktop) = n1;
  NEXT;
}


/**
 * 2DROP - ( x1 x2 -- )
 * Removes the top two items from the stack
 */
void twodrop(PARAMS) {
  stacktop += 2;
  NEXT;
}

/**
 * 2DUP - ( x1 x2 -- x1 x2 x1 x2 )
 * Duplicates the top two items on the stack
 */
void twodup(PARAMS) {
  void *n1 = *stacktop;
  void *n2 = *(stacktop+1);
  *(--stacktop) = n2;
  *(--stacktop) = n1;
  
  NEXT;
}



/**
 * ?DUP - ( x -- x x | 0 )
 * Duplicates the top item if it is non-zero
 */
void qdup(PARAMS) {
  void *value = *stacktop;
  if (value) {
    *(--stacktop) = value;
  }
    
  NEXT;
}

/**
 * DUP - ( x -- x x )
 * Duplicates the top item on the stack
 */
static void dup(PARAMS) {
  void *value = *stacktop;
  *(--stacktop) = value;
  NEXT;
}

/**
 * DROP - ( x -- )
 * Removes the top item from the stack
 */
void drop(PARAMS) {
    stacktop++;
    NEXT;
}



/**
 * LIT - ( -- x )
 * Pushes the literal value that follows in the instruction stream
 */
void lit(PARAMS) {
  *(--stacktop) = *((void**)ip);
  ip = (void**)ip+1;
  NEXT;
}

/**
 * + - ( n1 n2 -- n3 )
 * Adds the top two numbers on the stack
 */
void add(PARAMS) {
  intptr_t a = *((intptr_t*)stacktop++);
  intptr_t b = *((intptr_t*)stacktop++);
  *(--stacktop) = (void*)(a+b);
  NEXT;
}

/**
 * - - ( n1 n2 -- n3 )
 * Subtracts the top number from the second number
 */
void sub(PARAMS) {
  intptr_t n2 = *((intptr_t*)stacktop++);
  intptr_t n1 = *((intptr_t*)stacktop++);
  *(--stacktop) = (void*)(n1-n2);
  NEXT;
}


/**
 * * - ( n1 n2 -- n3 )
 * Multiplies the top two numbers on the stack
 */
void mul(PARAMS) {
  intptr_t a = *(intptr_t*)stacktop++;
  intptr_t b = *(intptr_t*)stacktop++;
  *(--stacktop) = (void*)(a*b);
  NEXT;
}


/**
 * / - ( n1 n2 -- n3 )
 * Divides the second number by the top number
 */
void _div(PARAMS) {
  intptr_t numerator = *(intptr_t*)stacktop++;
  intptr_t denominator = *(intptr_t*)stacktop++;
  *(--stacktop) = (void*)(numerator / denominator);
  NEXT;
}


/**
 * % - ( n1 n2 -- n3 )
 * Returns the remainder of dividing the second number by the top number
 */
void mod(PARAMS) {
  intptr_t numerator = *(intptr_t*)stacktop++;
  intptr_t denominator = *(intptr_t*)stacktop++;
  *(--stacktop) = (void*)(numerator % denominator);
  NEXT;
}


/**
 * 1+ - ( n1 -- n2 )
 * Increments the top number on the stack
 */
void incr(PARAMS) {
  *(intptr_t*)stacktop += 1;
  NEXT;
}

/**
 * 1- - ( n1 -- n2 )
 * Decrements the top number on the stack
 */
void decr(PARAMS) {
  *(intptr_t*)stacktop -= 1;
  NEXT;
}


/**
 * @ - ( addr -- x )
 * Fetches the value at the given address
 */
void fetch(PARAMS) {
  void **addr = (void**)(*stacktop++);
  *(--stacktop) = *addr;
  NEXT;
}

/**
 * ! - ( x addr -- )
 * Stores the value at the given address
 */
void store(PARAMS) {
  void **addr = (void**)( *stacktop++);
  void *value = *stacktop++;
  *addr = value;
  NEXT;
}

/**
 * +! - ( n addr -- )
 * Adds the number to the value at the given address
 */
void addstore(PARAMS) {
  intptr_t *addr = (intptr_t*)( *stacktop++);
  intptr_t value = (intptr_t)(*stacktop++);

  *addr += value;
  NEXT;
}

/**
 * -! - ( n addr -- )
 * Subtracts the number from the value at the given address
 */
void substore(PARAMS) {
  intptr_t *addr = (intptr_t*)( *stacktop++);
  intptr_t value = (intptr_t)(*stacktop++);

  *addr -= value;
  NEXT;
}

/**
 * , - ( x -- )
 * Compiles a value into the dictionary
 */
void comma(PARAMS) {
  void **dp = state->dp;
  *dp++ = *stacktop++;
  state->dp = dp;
  NEXT;
}

/**
 * [ - ( -- )
 * Enters immediate mode
 */
void lbrac(PARAMS) {
    state->state = IMMEDIATELY;
    NEXT;
}

/**
 * ] - ( -- )
 * Enters compilation mode
 */
void rbrac(PARAMS) {
    state->state = COMPILING;
    NEXT;
}

/**
 * BYE - ( n -- )
 * Terminates the Forth system with the given exit code
 */
void terminate(PARAMS) {
  exit(*(intptr_t*)stacktop);
  NEXT;
}



#define logicalimpl(name, op) \
void name(PARAMS) { \
  intptr_t n2 = (intptr_t)(*stacktop++); \
  intptr_t n1 = (intptr_t)(*stacktop++); \
  *(--stacktop) = (void*)(n1 op n2); \
  \
  NEXT; \
}

logicalimpl(lt, <);
logicalimpl(gt, >);
logicalimpl(lte, <=);
logicalimpl(gte, >=);
logicalimpl(equ, ==);
logicalimpl(nequ, !=);


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


/**
 * AND - ( x1 x2 -- x3 )
 * Performs bitwise AND between the top two items
 */
void bitand(PARAMS) {
  intptr_t a = *((intptr_t*)(stacktop++));
  intptr_t b = *((intptr_t*)(stacktop++));
  *(--stacktop) = (void*)(intptr_t)(a & b);
  NEXT;
}

/**
 * OR - ( x1 x2 -- x3 )
 * Performs bitwise OR between the top two items
 */
void bitor(PARAMS) {
  intptr_t a = *((intptr_t*)(stacktop++));
  intptr_t b = *((intptr_t*)(stacktop++));
  *(--stacktop) = (void*)(intptr_t)(a | b);
  NEXT;
}

/**
 * XOR - ( x1 x2 -- x3 )
 * Performs bitwise XOR between the top two items
 */
void bitxor(PARAMS) {
  intptr_t a = *((intptr_t*)(stacktop++));
  intptr_t b = *((intptr_t*)(stacktop++));
  *(--stacktop) = (void*)(intptr_t)(a ^ b);
  NEXT;
}

/**
 * INVERT - ( x1 -- x2 )
 * Performs bitwise NOT on the top item
 */
void bitnot(PARAMS) {
  *stacktop = (void*)~(*(intptr_t*)stacktop);
  NEXT;
}

/**
 * >R - ( x -- ) ( R: -- x )
 * Moves the top item from the data stack to the return stack
 */
void tor(PARAMS) {
  *(--retstacktop) = *stacktop++;
  NEXT;
}

/**
 * R> - ( -- x ) ( R: x -- )
 * Moves the top item from the return stack to the data stack
 */
void fromr(PARAMS) {
  *(--stacktop) = *retstacktop++;
  NEXT;
}

/**
 * RSP@ - ( -- addr )
 * Returns the address of the top of the return stack
 */
void rspfetch(PARAMS) {
  *(--stacktop) = retstacktop;
  NEXT;
}

/**
 * RSP! - ( addr -- )
 * Sets the return stack pointer to the given address
 */
void rspstore(PARAMS) {
  retstacktop = *stacktop++;
  NEXT;
}

/**
 * RDROP - ( -- ) ( R: x -- )
 * Removes the top item from the return stack
 */
void rspdrop(PARAMS) {
  retstacktop--;
  NEXT;
}

struct litstring {
 intptr_t length;
 char str[];
};


/**
 * LITSTRING - ( -- addr len )
 * Pushes the address and length of the string literal that follows
 */
void litstring(PARAMS) {
  struct litstring *str = (struct litstring*)ip;
  
  *(--stacktop) = (void*)&str->str;
  *(--stacktop) = (void*)str->length;

  ip = (char*)ip + sizeof(struct litstring) + str->length + 1;
  int cellsize = sizeof(void*);
  ip = (void*)(((intptr_t)ip + (cellsize - 1)) & ~(cellsize - 1));
  
  NEXT;
}

/**
 * DSP@ - ( -- addr )
 * Returns the address of the top of the data stack
 */
void dspfetch(PARAMS) {
  void *value = stacktop;
  *(--stacktop) = value;
  NEXT;
}

/**
 * DSP! - ( addr -- )
 * Sets the data stack pointer to the given address
 */
void dspstore(PARAMS) {
  stacktop = (void**)*stacktop;
  NEXT;
}

/**
 * BRANCH - ( -- )
 * Unconditionally branches to the offset that follows
 */
void branch(PARAMS) {
  intptr_t offset = *(intptr_t*)ip;
  ip += offset;
  NEXT;
}

/**
 * 0BRANCH - ( flag -- )
 * Branches to the offset that follows if flag is zero
 */
void zbranch(PARAMS) {
  void* value = *stacktop++;
  if (!value)
  {
     intptr_t offset = *(intptr_t*)ip;
     ip += offset;
  }
  else
  {
    ip = ((void**)ip)+1;
  }
  NEXT;
}

void _dodoes(PARAMS) {
  void *value = *(((void**)xt)+1);

  if ( value ) {
    *(--retstacktop) = ip;
    ip = value;
  }

  xt = ((void**)xt)+2;
  *(--stacktop) = xt;

  
  NEXT;
}

/**
 * DODOES - ( -- )
 * Sets up a DOES> word to execute
 */
void dodoes(PARAMS) {
  *(--stacktop) = &_dodoes;
  NEXT;
}

/**
 * DP - ( -- addr )
 * Returns the address of the dictionary pointer
 */
void dp(PARAMS) {
  *(--stacktop) = &state->dp;
  NEXT;
}

/**
 * RET - ( x -- )
 * Returns from the current word with the given value
 */
void ret(PARAMS) {
  state->rv = *stacktop;
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
  xt = *(void**)ip;
  ip = ((void**)ip)+1;
  *(--stacktop) = xt;
  NEXT;
}

void nextaddr(PARAMS) {
  *(--stacktop) = &next;
  NEXT;
}

/**
 * FIND - ( addr len -- word|0 )
 * Searches the dictionary for a word with the given name
 * Returns the word if found, 0 if not found
 */
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
    int rv = state->filllinebuffer(state);
    if (rv < 0) {
      return rv;
    }
  }

  *c = state->line[state->pos++];
  return 1;
}



/**
 * KEY - ( -- char )
 * Reads a single character from the input stream
 */
void key(PARAMS) {
  char c;
  int rv = getkey(state, &c);
  *(--stacktop) = (void*)c;
  NEXT;
}



/**
 * >CFA - ( word -- addr )
 * Converts a word to its code field address
 */
void tcfa(PARAMS) {
  struct word *word = (struct word*)*stacktop++;
  *(--stacktop) = &word->codeword;
  NEXT;
}

/**
 * >DFA - ( word -- addr )
 * Converts a word to its data field address
 */
void tdfa(PARAMS) {
  struct word *word = (struct word*)*stacktop++;
  *(--stacktop) = &word->extra;
  NEXT;
}

/**
 * HEADER, - ( addr len -- )
 * Creates a new word header in the dictionary
 */
void headercomma(PARAMS) {
  int length = *((intptr_t*)stacktop++);
  char *name = *((char**)stacktop++);
  void **dp = state->dp;

  // add a new word 
  struct word *newword = (struct word*)dp;
  if (length > 0 && name ) {
    __builtin_memcpy(newword->name, name, 15);
  }
  newword->prev = state->latest;
  newword->flags = 0;
  newword->len = length;

  state->dp = (void**)&newword->codeword;
  state->latest = newword;

  NEXT;
}

/**
 * IMMEDIATE - ( -- )
 * Makes the most recently defined word immediate
 */
void immediate(PARAMS) {
  state->latest->flags ^= F_IMMEDIATE;
  NEXT;
}


/**
 * CELLSIZE - ( -- n )
 * Returns the size of a cell in bytes
 */
void cellsize(PARAMS) {
  *(--stacktop) = sizeof(void*);
  NEXT;
}


/**
 * PLATFORM - ( -- addr len )
 * Returns the name of the current platform
 */
void platform(PARAMS) {
  *(--stacktop) = TAILCALL_FORTH_PLATFORM_NAME;
  *(--stacktop) = sizeof(TAILCALL_FORTH_PLATFORM_NAME);
  NEXT;
}

/**
 * ARCH - ( -- addr len )
 * Returns the name of the current architecture
 */
void arch(PARAMS) {
  *(--stacktop) = TAILCALL_FORTH_ARCH_NAME;
  *(--stacktop) = sizeof(TAILCALL_FORTH_ARCH_NAME);
  NEXT;
}

/**
 * DOCOL - ( -- addr )
 * Returns the address of the DOCOL word
 */
void docol_addr(PARAMS) {
  *(--stacktop) = (void*)docol;
  NEXT;
}

/**
 * S0 - ( -- addr )
 * Returns the address of the bottom of the data stack
 */
void stackbase(PARAMS) {
  *(--stacktop) = state->stackbase;
  NEXT;
}

/**
 * DP0 - ( -- addr )
 * Returns the address of the bottom of the dictionary
 */
void dpbase(PARAMS) {
  *(--stacktop) = state->dpbase;
  NEXT;
}



/**
 * HIDDEN - ( word -- )
 * Toggles the hidden flag of a word
 */
void hidden(PARAMS) {
  struct word *word = (struct word*)(*stacktop++);
  word->flags ^= F_HIDDEN;
  NEXT;
}

/**
 * CMOVE - ( addr1 addr2 len -- )
 * Copies len bytes from addr1 to addr2
 */
void cmove(PARAMS) {
  intptr_t length = (intptr_t)(*stacktop++);
  char *dest = (char*)(*stacktop++);
  char *src =  (char*)(*stacktop++);

  while(length--)
    *dest++ = *src++;

    
  NEXT;
}

/**
 * C@ - ( addr -- char )
 * Fetches a byte from the given address
 */
void fetchbyte(PARAMS) {
  unsigned char *addr = (unsigned char*)*stacktop++;
  *(--stacktop) = (void*)(*addr);
  NEXT;
}

/**
 * C! - ( char addr -- )
 * Stores a byte at the given address
 */
void storebyte(PARAMS) {
  unsigned char *addr = (unsigned char*)(*stacktop++);
  intptr_t c = (intptr_t)(*stacktop++);
  *addr = c;
  NEXT;
}

/**
 * COPYBYTE - ( addr1 addr2 -- addr1+1 addr2+1 )
 * Copies a byte from addr1 to addr2 and increments both addresses
 */
void copybyte(PARAMS) {
  unsigned char *dest = (unsigned char*)(*stacktop++);
  unsigned char *src = (unsigned char*)(*stacktop++);
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


/**
 * WORD - ( -- addr len )
 * Reads a word from the input stream
 */
void word(PARAMS) {
  int rv = _word(ARGS);
  
  *(--stacktop) = (void*)state->token;
  *(--stacktop) = (void*)state->tokenlen;
  
  NEXT;
}



/**
 * NUMBER - ( addr len -- n )
 * Converts a string to a number
 */
void number(PARAMS) {
  intptr_t length = *((intptr_t*)stacktop++);
  char *s = *stacktop++;
  long value = strtol(s, NULL, 0);
  *(--stacktop) = (void*)value;
  NEXT;
}




/**
 * STATE - ( -- addr )
 * Returns the address of the interpreter state
 */
void compilestate(PARAMS) {
  *(--stacktop) = &state->state;
  NEXT;
}

/**
 * LATEST - ( -- addr )
 * Returns the address of the most recently defined word
 */
void latest(PARAMS) {
  *(--stacktop) = &state->latest;

  NEXT;
}


/**
 * LINE-FN - ( -- addr )
 * Returns the address of the line buffer fill function
 */
void filllinebuffer(PARAMS) {
  *(--stacktop) = &state->filllinebuffer;
  NEXT;
}

/**
 * LINE-BUF - ( -- addr1 addr2 )
 * Returns the addresses of the line buffer and position
 */
void linebuffer(PARAMS) {
  *(--stacktop) = &state->line;
  *(--stacktop) = &state->pos;
  NEXT;
}

/**
 * (DO) - ( limit start -- ) ( R: -- start limit )
 * Sets up a DO loop
 */
void paren_do(PARAMS) {
  void *a = *stacktop++;
  void *b = *stacktop++;
  *(--retstacktop) = b;
  *(--retstacktop) = a;
  NEXT;
}

/**
 * (LOOP) - ( -- ) ( R: start limit -- start limit )
 * Increments the loop counter and branches if not done
 */
void paren_loop(PARAMS) {
  intptr_t limit = (intptr_t)(*retstacktop++);
  intptr_t index = (intptr_t)(*retstacktop++);
  index++;
  if (index != limit) {
    ip = ((void**)ip) + *((intptr_t*)ip);
    *(--retstacktop) = (void*)index;
    *(--retstacktop) = (void*)limit;
  }
  else {
    ip = ((void**)ip)+1;
  }
  
  NEXT;
}

/**
 * UNLOOP - ( -- ) ( R: start limit -- )
 * Removes the loop control parameters from the return stack
 */
void unloop(PARAMS) {
  retstacktop++;
  retstacktop++;
  NEXT;  
}

/**
 * I - ( -- n ) ( R: start limit -- start limit )
 * Returns the current loop index
 */
void inner_index(PARAMS) {
  *(--stacktop) = *retstacktop;
  NEXT;
}

/**
 * J - ( -- n ) ( R: start1 limit1 start2 limit2 -- start1 limit1 start2 limit2 )
 * Returns the outer loop index
 */
void outer_index(PARAMS) {
  *(--stacktop) = *((void**)retstacktop+1);
  NEXT;
}

/**
 * EXECUTE - ( xt -- )
 * Executes the word at the given execution token
 */
void execute(PARAMS) {
  xt = (block*)(*stacktop++);
  block xt_ = *(block*)xt;
  __attribute__((musttail)) return xt_(ARGS);

  NEXT;
}


/**
 * SHL - ( x1 x2 -- x3 )
 * Shifts x1 left by x2 bits
 */
void shl(PARAMS) {
  uintptr_t amount = (intptr_t)*stacktop++;
  uintptr_t v = (intptr_t)*stacktop++;
  *(--stacktop) = v << amount;
  NEXT;
}

/**
 * SHR - ( x1 x2 -- x3 )
 * Shifts x1 right by x2 bits
 */
void shr(PARAMS) {
  uintptr_t amount = (intptr_t)*stacktop++;
  uintptr_t v = (intptr_t)*stacktop++;
  *(--stacktop) = v >> amount;
  NEXT;
}



#define simpleprim(sname, fname, cword, last)				\
  struct word sname = { .prev = &last, .name = fname, .len=sizeof(fname)-1, .codeword = cword }
#define immediateprim(sname, fname, cword, last)				\
  struct word sname = { .prev = &last, .name = fname, .len=sizeof(fname)-1, .codeword = cword, .flags= F_IMMEDIATE }


struct word FETCH = {  .prev = NULL, .name = "@", .len = 1, .codeword = fetch };

simpleprim(DUP, "dup", dup, FETCH);
simpleprim(FIND_, "(find)", find, DUP);
simpleprim(LIT, "lit", lit, FIND_);
simpleprim(MUL, "*", mul, LIT);
simpleprim(EXIT, "exit", exit_, MUL);
simpleprim(COMMA, ",", comma, EXIT);
immediateprim(LBRAC, "[", lbrac, COMMA);
simpleprim(RBRAC, "]", rbrac, LBRAC);
simpleprim(TERMINATE, "bye", terminate, RBRAC);
simpleprim(ADD, "+", add, TERMINATE);
simpleprim(SUB, "-", sub, ADD);
simpleprim(INCR, "1+", incr, SUB);
simpleprim(DECR, "1-", decr, INCR);
simpleprim(BRANCH, "branch", branch, DECR);
simpleprim(ZBRANCH, "0branch", zbranch, BRANCH);
simpleprim(WORD, "word", word, ZBRANCH);
simpleprim(KEY, "key", key, WORD);
simpleprim(NUMBER, "number", number, KEY);

simpleprim(HEADERCOMMA, "header,", headercomma, NUMBER);
simpleprim(BRACKET_TICK, "[']", bracket_tick, HEADERCOMMA);
simpleprim(STATE, "state", compilestate, BRACKET_TICK);
simpleprim(PAREN_DO, "(do)", paren_do, STATE);
simpleprim(PAREN_LOOP, "(loop)", paren_loop, PAREN_DO);
simpleprim(INNER_INDEX, "i", inner_index, PAREN_LOOP);
simpleprim(OUTER_INDEX, "j", outer_index, INNER_INDEX);
simpleprim(LATEST, "latest", latest, OUTER_INDEX);
simpleprim(DP, "dp", dp, LATEST);
simpleprim(TCFA, ">cfa", tcfa, DP);
simpleprim(TDFA, ">dfa", tdfa, TCFA);
simpleprim(HIDDEN, "hidden", hidden, TDFA);
immediateprim(IMMEDIATE_, "immediate", immediate, HIDDEN);

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

/**
 * INTERPRET - ( -- )
 * The main interpreter loop
 */
void interpret(PARAMS) {
  errno = 0;
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
    char *endptr = NULL;
    // this doesn't belong!!
    long v = strtol(state->token, &endptr, 0);
    if ( v == 0 ) { // check for error
      if ( errno == EINVAL || state->token == endptr ) {
	state->error(state);
	NEXT;
      }
    }
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
      xt = &word->codeword;
      __attribute__((musttail)) return word->codeword(ARGS);
    }
  }
  NEXT; 
}

simpleprim(INTERPRET, "interpret", interpret, SEMICOLON);
simpleprim(DODOES, "dodoes", dodoes, INTERPRET);
simpleprim(OVER, "over", over, DODOES);
simpleprim(SWAP, "swap", swap, OVER);
simpleprim(ROT, "rot", rot, SWAP);
simpleprim(NROT, "-rot", nrot, ROT);
simpleprim(DROP, "drop", drop, NROT);
simpleprim(TWODROP, "2drop", twodrop, DROP);
simpleprim(TWODUP, "2dup", twodup, TWODROP);
simpleprim(QDUP, "?dup", qdup, TWODUP);
simpleprim(DIV, "/", _div, QDUP);
simpleprim(MOD, "%", mod, DIV);
simpleprim(STORE, "!", store, MOD);
simpleprim(ADDSTORE, "+!", addstore, STORE);

simpleprim(SUBSTORE, "-!", substore, ADDSTORE);
simpleprim(EXECUTE, "execute", execute, SUBSTORE);
simpleprim(CMOVE, "cmove", cmove, EXECUTE);
simpleprim(FETCHBYTE, "c@", fetchbyte, CMOVE);
simpleprim(STOREBYTE, "c!", storebyte, FETCHBYTE);
simpleprim(LITSTRING, "litstring", litstring, STOREBYTE);

simpleprim(EQU, "=", equ, LITSTRING);
simpleprim(NEQU, "<>", nequ, EQU);
simpleprim(LT, "<", lt, NEQU);
simpleprim(GT, ">", gt, LT);
simpleprim(LTE, "<=", lte, GT);
simpleprim(GTE, ">=", gte, LTE);
simpleprim(ZEQU, "0=", zequ, GTE);
simpleprim(ZNEQU, "0<>", znequ, ZEQU);
simpleprim(ZLT,  "0<", zlt, ZNEQU);
simpleprim(ZGT, "0>", zgt, ZLT);

simpleprim(ZLTE, "0<=", zlte, ZGT);
simpleprim(ZGTE, "0>=", zgte, ZLTE);

simpleprim(BITAND, "and", bitand, ZGTE);
simpleprim(BITOR, "or", bitor, BITAND);
simpleprim(BITXOR, "xor", bitxor, BITOR);
simpleprim(BITNOT, "invert", bitnot, BITXOR);
simpleprim(SHR, "shr", shr, BITNOT);
simpleprim(SHL, "shl", shl, SHR);

simpleprim(TOR, ">r", tor, SHL);
simpleprim(FROMR, "r>", fromr, TOR);

simpleprim(RSPFETCH, "rsp@", rspfetch, FROMR);
simpleprim(RSPSTORE, "rsp!", rspstore, RSPFETCH);
simpleprim(RDROP, "rdrop", rspdrop, RSPSTORE); 

simpleprim(DSPFETCH, "dsp@", dspfetch, RDROP);
simpleprim(CELLS, "cellsize", cellsize, DSPFETCH);
simpleprim(PLATFORM, "platform", platform, CELLS);
simpleprim(ARCH, "arch", arch, PLATFORM);
simpleprim(DOCOL_ADDR, "docol", docol_addr, ARCH);
simpleprim(STACKBASE, "s0", stackbase, DOCOL_ADDR);
simpleprim(DPBASE, "dp0", dpbase, STACKBASE);
simpleprim(RET, "ret", ret, DPBASE);
simpleprim(NEXTADDR, "next", nextaddr, RET);
simpleprim(FILLLINEBUFFER, "line-fn", filllinebuffer, NEXTADDR);
simpleprim(LINEBUFFER, "line-buf", linebuffer, FILLLINEBUFFER);
simpleprim(DSPSTORE, "dsp!", dspstore, LINEBUFFER);



struct invoke_forth_context {
  struct usefulstate *state;
  void *xt_codeword;
  void *xt_context;
};

void *invoke_forth(void *context, void *a, void *b, void *c, void *d) {
  struct invoke_forth_context *ifc = (struct invoke_forth_context*)context;
  struct usefulstate *state = ifc->state;
  void *datastack[128];
  void *returnstack[64];


  void **stacktop = &datastack[127];
  void **retstacktop = &returnstack[63];

  *(--stacktop) = d;
  *(--stacktop) = c;
  *(--stacktop) = b;
  *(--stacktop) = a;
  *(--stacktop) = ifc->xt_context;
  *(--stacktop) = ifc->xt_codeword;

  void* xtprogram[] = { &EXECUTE.codeword, &RET.codeword };

  next(&xtprogram[0], 0, stacktop, retstacktop, &next, state);
  return state->rv;
}

void invoke_forth_addr(PARAMS) {
  *(--stacktop) = &invoke_forth;
  NEXT;
}

void get_forth_state(PARAMS) {
  *(--stacktop) = state;
  NEXT;
}



simpleprim(INVOKE_FORTH, "invoke-forth", invoke_forth_addr, DSPSTORE);
simpleprim(FORTH_STATE, "forth-state@", get_forth_state, INVOKE_FORTH);

void* defaultprogram[] = { &INTERPRET.codeword, &BRANCH.codeword, (void*)(-2*sizeof(void*)), &TERMINATE.codeword };

struct word *lastword = &FORTH_STATE;
