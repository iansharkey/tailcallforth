#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dlfcn.h>
#include <string.h>

#include "core.h"

int scanf_token(struct usefulstate *state) {
  int length;
  int rv = scanf(" %32s%n", state->token, &length);
  if ( rv < 0 ) {
    return rv;
  }
  state->token[32] = 0;
  state->length = length;
  return rv;  
}


void display_number(PARAMS) {
  int num = *((int*)stacktop++);
  printf("%d\n", num);
  NEXT;
}

void emit(PARAMS) {
  char c = (char*)(*stacktop++);
  putchar(c);
  NEXT;
}

void *libc_handle;

void libc_dlsym(PARAMS) {
  char *str;
  *(stacktop--) = dlsym(libc_handle, str);
  NEXT;
}

extern void litstring(PARAMS);
int main(int argc, char** argv)
{

  void* datastack[256];
  void* returnstack[256];
  void* buffer[256] = { 0 };

  void** stacktop = &datastack[255];
  void** retstacktop = &returnstack[255];
  void** here = &buffer[0];

  libc_handle = dlopen("libc.dylib", RTLD_LAZY);

  struct word DISPLAY_NUMBER = { .prev = lastword, .name = ".", .codeword = display_number };

  struct word EMIT = { .prev = &DISPLAY_NUMBER, .name = "emit", .codeword = emit };

  
  struct word *BLAH = malloc(sizeof(struct word) + sizeof(void*)+5);

  BLAH->prev = &EMIT;
  strcpy(&BLAH->name,"blah");
  BLAH->codeword = litstring;
  BLAH->extra[0] = (void*)5;
  
  strcpy(&(BLAH->extra[1]), "yeah");
  
  struct usefulstate state;
  state.getnexttoken = scanf_token;
  state.here = here;
  state.dp = buffer;
  //  state.latest = &DISPLAY_NUMBER;
  state.latest = BLAH;
  state.state = IMMEDIATELY;
  
  //void* ip[] = { &WORD.codeword, &NUMBER.codeword, &QUADRUPLE.codeword, &INCR.codeword, &DUP.codeword, &LIT.codeword, (void*)-1, &MUL.codeword, &DISPLAY_NUMBER.codeword, &TERMINATE.codeword };

  
  next(&state, &defaultprogram[0], 0, stacktop, retstacktop, &next);
  
  
  return 0;
}
