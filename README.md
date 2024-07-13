# C-try-catch
C try / catch implementation.

 - almost single-header (need macro.h and define TRY_CATCH_IMPL before include trycatch.h)
 - thread safety
 - nested calls
 - finally is optional
 - automatic throwing up if it is impossible to find catch at its level

The compiler must support statement expressions. a=({ ... });
Ooold GCC 2.95.3 have it as extension, so it should compile everywhere with >=C89

