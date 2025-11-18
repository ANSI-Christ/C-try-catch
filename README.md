# C-try-catch
C try / catch implementation.

 - cross-platform
 - thread safety
 - nested calls
 - automatic throwing up if it is impossible to find match at its level

Specific:
 - in TRY block do not call return and goto
 - EXCEPTION always rewrites by last one
 - finally block can be handmade by using goto in CATCH block

Look for usage examples in main.c
