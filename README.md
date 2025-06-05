This is an implementation of a Forth using Clang `musttail` attribute to generate efficient machine code representations of the code words.

It's based on Jonesforth, but the core words are written in C rather than x86 assembl as  a threaded interpreter.

To compile: clang -o tailcallforth -Os core.c crt.c -fomit-frame-points -fno-asynchhronous-unwind-tables -fno-unwind-tables -Wno-int-conversions

To use on a Posix system:

cat core.f - | ./tailcallforth

