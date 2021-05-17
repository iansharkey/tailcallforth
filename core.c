#include <stdio.h>
#include <stddef.h>
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

typedef void (*block)(void*, void**, void**, size_t, size_t);

//void* datastack_[256];
//void* returnstack_[256];
//void* w;

//size_t stacktop;
// void* returnstack[256]; // handled by system stack

//void* latest;
//void* dict[1024];


struct word {
    struct word* prev;
    unsigned int length:12;
    unsigned int flags:4;
    char name[14];

    block codeword; // first impl block is machine word
    void* extra[];
};


// currentcodeword: block - pointer to the codeword within a definition
// datastack: void** - 
// 

#define PARAMS void* currentcodeword, void** datastack, void** returnstack, size_t stacktop, size_t retstacktop


static __attribute__((noinline)) void next(PARAMS) {
//static void next(PARAMS) {
    block *codeword = (block*)currentcodeword;
    struct word* currentword = container_of(codeword, struct word, codeword);

    printf("executing word %s\n", currentword->name);

    __attribute__((musttail)) return (*codeword)((void*)(codeword+1), datastack, returnstack, stacktop, retstacktop);
}
#define NEXT __attribute__((musttail)) return next(currentcodeword, datastack, returnstack, stacktop, retstacktop)




void docol(PARAMS) {
    struct word* currentword = container_of(currentcodeword, struct word, codeword);
    returnstack[++retstacktop] = currentcodeword;
    currentcodeword = currentword->extra[0];
    NEXT;
}


void exit_(PARAMS) {
    currentcodeword = returnstack[retstacktop--];
    NEXT;
}


void dup(PARAMS) {
    datastack[stacktop+1] = datastack[stacktop];
    stacktop++;
    NEXT;
}

void drop(PARAMS) {
    stacktop--;
    NEXT;
}

void lit(PARAMS) {
    datastack[stacktop++] = (void*)currentcodeword;
    currentcodeword = (block*)currentcodeword+1;
    NEXT;
}

void add(PARAMS) {
    datastack[stacktop] = (void*)((size_t)datastack[stacktop] + (size_t)datastack[stacktop-1]);
    stacktop--;
    NEXT;
}

void peek(PARAMS) {
    datastack[stacktop] = *(void**)datastack[stacktop];
    NEXT;
}

void comma(PARAMS) {
    NEXT;
}



struct word PEEK = { .prev = NULL, .length = 0x8f0, .name = "PEEK",
                     .flags = 0, .codeword = peek };
struct word DUP = { .prev = &PEEK, .length = 3, .name = "DUP",
                     .flags = 0, .codeword =  dup };

struct word FIND = {  .prev = &DUP, .length = 4, .name = "FIND", .codeword = docol,
                          .extra = { &DUP.codeword } };

struct word LIT = {  .prev = &FIND, .length = 3, .name = "LIT", .codeword = lit };

struct word TWO = { .prev = &LIT, .length = 3, .name = "TWO", .codeword = docol, .extra = { &LIT.codeword, (void*)2 } };
                        
int main(int argc, char** argv)
{

  void* datastack[256];
  void* returnstack[256];
  void* buffer[256];
  void* currentip;
  void* here;

  void** stacktop = &datastack[0];
  void** retstacktop = &returnstack[0];
  void** here = &buffer[0];

  block* ip = &TWO.codeword;

  next(ip, stacktop, 
  
  
  return 0;
}
