#include <stdio.h>
#include <stdlib.h>
#include "fook.h"


// fook declaration
FOOK_HOOK(access);

/**
 prototype function declaration to hook 

 int access(const char *path, int amode);

/!\ /!\ /!\ 
 it is mandatory to always use *orig_* and the symbol name for fook macros:
 
 here *orig_*access
/!\ /!\ /!\ 
**/

int (*orig_access)(char*,int) = NULL;

/**
 The function that will replace original symbol 
 
 you need to prefix it with FOOK_EXPORT to be visible by shared object

**/

FOOK_EXPORT int my_access(char *path,int flag){
    // function that disable function trampoline
    FOOK_ENTER(access);
    // call of legit function
    int ret = orig_access(path,flag);
    // hook restoration before function ret
    FOOK_RETURN(access);

    // hook code
    TRACE("path=%s",path);

    return ret;
}

/**
 FOOK_INIT is the entry point for hook execution after library injection

**/

FOOK_INIT void fook(){
    // add trampoline to global access function with local my_access from current shared object
    FOOK_ADD_TRAMPOLINE( access , my_access );
}
