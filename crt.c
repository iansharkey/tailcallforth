#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dlfcn.h>
#include <string.h>

#include "core.h"





int getline_line(struct usefulstate *state) {
  char *buf = NULL;
  size_t length = 0;
  state->length = getline(&buf, &length, stdin);

  if (state->line) {
    free(state->line);
  }
  state->line = buf;
  state->pos = 0;
  
  return state->length;  
}


int print_error(struct usefulstate *state) {
  printf("parse error: %s", state->token);
  return 0;
}

void tell(PARAMS) {
  intptr_t length = (intptr_t)(*stacktop++);
  char *addr = (char*)(*stacktop++);

  fwrite(addr, 1, length, stdout);
  NEXT;
}


void display_number(PARAMS) {
  void *num = *stacktop++;
  printf("%p ", num);
  NEXT;
}

void emit(PARAMS) {
  char c = (char)(*stacktop++);
  putchar(c);
  NEXT;
}

void *libc_handle;

void libc_dlsym(PARAMS) {
  intptr_t length = (intptr_t)(*stacktop++);
  char *addr = (char*)(*stacktop++);

  void *func = dlsym(libc_handle, addr);
  
  *(--stacktop) = func;
  NEXT;
}


void _stdout(PARAMS) {
  *(--stacktop) = (void*)stdout;
  NEXT;
}

void invoke_c(PARAMS) {
  intptr_t (*func)() = (intptr_t (*)())*stacktop++; 
  void *a = *stacktop;
  void *b = *(stacktop+1);
  void *c = *(stacktop+2);
  void *d = *(stacktop+3);
  void *e = *(stacktop+4);
  void *f = *(stacktop+5);

  intptr_t rv = func(a,b,c,d,e,f);

  *(--stacktop) = (void*)rv;

  
  NEXT;
}


extern void litstring(PARAMS);
int main(int argc, char** argv)
{

  void* datastack[256];
  void* returnstack[256];
  void* buffer[2048] = { 0 };

  void** stacktop = &datastack[255];
  void** retstacktop = &returnstack[255];

  libc_handle = dlopen("libc.dylib", RTLD_LAZY);

  struct word DISPLAY_NUMBER = { .prev = lastword, .name = ".", .codeword = display_number };

  struct word TELL = { .prev = &DISPLAY_NUMBER, .name = "tell", .codeword = tell };

  struct word DLSYM = { .prev = &TELL, .name = "dlsym", .codeword = libc_dlsym };

  struct word INVOKE_C = {.prev = &DLSYM, .name = "c-invoke", .codeword = invoke_c };

  struct word STDOUT = {.prev = &INVOKE_C, .name = "stdout", .codeword = _stdout };
  
  struct word EMIT = { .prev = &STDOUT, .name = "emit", .codeword = emit };

  struct word *BLAH = malloc(sizeof(struct word) + sizeof(void*)+5);

  BLAH->prev = &EMIT;
  strcpy((char*)&BLAH->name,"blah");
  BLAH->codeword = litstring;
  BLAH->extra[0] = (void*)4;
  
  strcpy((char*)&(BLAH->extra[1]), "yeah");
  
  struct usefulstate state = { 0 };
  state.getnexttoken = getline_line;
  state.error = print_error;
  state.dp = buffer;
  state.dpbase = buffer;
  state.stackbase = stacktop;
  //  state.latest = &DISPLAY_NUMBER;
  state.latest = BLAH;
  state.state = IMMEDIATELY;
  
  //void* ip[] = { &WORD.codeword, &NUMBER.codeword, &QUADRUPLE.codeword, &INCR.codeword, &DUP.codeword, &LIT.codeword, (void*)-1, &MUL.codeword, &DISPLAY_NUMBER.codeword, &TERMINATE.codeword };

  
  next(&state, &defaultprogram[0], 0, stacktop, retstacktop, &next);
  
  
  return 0;
}
