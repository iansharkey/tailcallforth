This is an implementation of a Forth using Clang 13+ `musttail` attribute to generate efficient machine code representations of the code words.

It's based on Jonesforth, but the core words are written in C rather than x86 assembly as  a threaded interpreter.

To compile: 
```
make
```

To use on a Posix system:

```
cat core.f posix.f - | ./tailcallforth
```

