# C-try-catch
C try / catch implementation.

## Features
- thread safe
- nested calls
- automatic throwing up if it is impossible to find match at its level

## Requirements

- POSIX-compatible environment
- C99 compiler with GNU extensions (GCC 2.95+, Clang 1.0+, MinGW)

## Quick Start

```c
TRY(
    THROW(int,4);
)CATCH(int,x)(
    printf("int %d\n",x);
)
```
>examples are in [main.c](main.c)

## Specific:
- in TRY block do not call return and goto
- EXCEPTION always rewrites by last one

