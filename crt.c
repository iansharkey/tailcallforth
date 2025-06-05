#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dlfcn.h>
#include <string.h>

#include "core.h"





int getline_line(struct usefulstate *state) {
  size_t length = 0;
  char *rv = fgets(state->line, sizeof(state->line)-1, state->instream);
  if (rv == NULL ) {
    return -1;
  }
  state->length = strlen(state->line);
  state->pos = 0;
  
  return state->length;  
}


int print_error(struct usefulstate *state) {
  fprintf(state->outstream, "parse error: %s", state->token);
  return 0;
}

void tell(PARAMS) {
  intptr_t length = (intptr_t)(*stacktop++);
  char *addr = (char*)(*stacktop++);

  fwrite(addr, 1, length, state->outstream);
  fflush(state->outstream);
  NEXT;
}


void display_number(PARAMS) {
  void *num = *stacktop++;
  fprintf(state->outstream, "%p ", num);
  fflush(state->outstream);
  NEXT;
}

static void emit(PARAMS) {
  char c = (char)(*stacktop++);
  fputc(c, state->outstream);
  fflush(state->outstream);
  NEXT;
}


void libc_dlsym(PARAMS) {
  *(--stacktop) = (void*)dlsym;
  NEXT;
}


void _stdout(PARAMS) {
  *(--stacktop) = (void*)&state->outstream;
  NEXT;
}

void _stdin(PARAMS) {
  *(--stacktop) = (void*)&state->instream;
  NEXT;
}



void invoke_c(PARAMS) {
  intptr_t (*func)(void*, void*, void*, void*, void*, void*) = (intptr_t (*)(void*, void*, void*, void*, void*, void*))*stacktop++; 
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
  void* buffer[0x8000];;

  void** stacktop = &datastack[255];
  void** retstacktop = &returnstack[255];

  struct word DISPLAY_NUMBER = { .prev = lastword, .name = ".", .codeword = display_number };

  struct word TELL = { .prev = &DISPLAY_NUMBER, .name = "tell", .codeword = tell };

  struct word EMIT = { .prev = &TELL, .name = "emit", .codeword = emit };

  struct word DLSYM = { .prev = &EMIT, .name = "dlsym-addr", .codeword = libc_dlsym };

  struct word INVOKE_C = {.prev = &DLSYM, .name = "c-invoke", .codeword = invoke_c };

  struct word STDOUT = {.prev = &INVOKE_C, .name = "stdout", .codeword = _stdout };

  struct word STDIN = {.prev = &STDOUT, .name = "stdin", .codeword = _stdin };



  struct usefulstate state = { 0 };
  state.filllinebuffer = getline_line;
  state.error = print_error;
  state.dp = buffer;
  state.dpbase = buffer;
  state.instream = stdin;
  state.outstream = stdout;
  *(--retstacktop) = &RET.codeword;
  state.stackbase = stacktop;
  //  state.latest = &DISPLAY_NUMBER;
  state.latest = &STDIN;
  state.state = IMMEDIATELY;
  
  //void* ip[] = { &WORD.codeword, &NUMBER.codeword, &QUADRUPLE.codeword, &INCR.codeword, &DUP.codeword, &LIT.codeword, (void*)-1, &MUL.codeword, &DISPLAY_NUMBER.codeword, &TERMINATE.codeword };


  // for a C callback, we need:
  //  the address of some code to start a forth execution
  //    runs next(state, "execute ret", 0, newstacktop, newretstacktop, &next)
  //  therefore, context needs to hold pointer to state
  // 
  
  next(&defaultprogram[0], 0, stacktop, retstacktop, &next, &state);
  
  
  return 0;
}
