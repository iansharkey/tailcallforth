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

  state->line = buf;
  state->pos = 0;
  
  return state->length;  
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

extern void litstring(PARAMS);
int main(int argc, char** argv)
{

  void* datastack[256];
  void* returnstack[256];
  void* buffer[2048] = { 0 };

  void** stacktop = &datastack[255];
  void** retstacktop = &returnstack[255];
  void** here = &buffer[0];

  libc_handle = dlopen("libc.dylib", RTLD_LAZY);

  struct word DISPLAY_NUMBER = { .prev = lastword, .name = ".", .codeword = display_number };

  struct word TELL = { .prev = &DISPLAY_NUMBER, .name = "tell", .codeword = tell };

  struct word DLSYM = { .prev = &TELL, .name = "dlsym", .codeword = libc_dlsym };
  
  struct word EMIT = { .prev = &DLSYM, .name = "emit", .codeword = emit };

  struct word *BLAH = malloc(sizeof(struct word) + sizeof(void*)+5);

  BLAH->prev = &EMIT;
  strcpy((char*)&BLAH->name,"blah");
  BLAH->codeword = litstring;
  BLAH->extra[0] = (void*)4;
  
  strcpy((char*)&(BLAH->extra[1]), "yeah");
  
  struct usefulstate state = { 0 };
  state.getnexttoken = getline_line;
  state.here = here;
  state.dp = buffer;
  state.stackbase = stacktop;
  //  state.latest = &DISPLAY_NUMBER;
  state.latest = BLAH;
  state.state = IMMEDIATELY;
  
  //void* ip[] = { &WORD.codeword, &NUMBER.codeword, &QUADRUPLE.codeword, &INCR.codeword, &DUP.codeword, &LIT.codeword, (void*)-1, &MUL.codeword, &DISPLAY_NUMBER.codeword, &TERMINATE.codeword };

  
  next(&state, &defaultprogram[0], 0, stacktop, retstacktop, &next);
  
  
  return 0;
}
