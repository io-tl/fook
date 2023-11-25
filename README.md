# FOOK
**THIS IS JUST FOR EDUCATIONAL PURPOSE, USE IT ONLY ON YOUR COMPUTER.**

Fook is a toy framework to simplify the process of live function hooking under linux x86_64.
It uses MACRO to handle live injection, hooking and trampoline complexity to let user concentrate on useful code.

Example of hooking code :
```C
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

```

To make :

```bash
$ DEBUG=1 make FOOK=fook.access.c 
DEBUG BUILD
compiling & linking p2so
CC src/main.o
CC src/subhook.o
make[1]: Entering directory '/home/user/work/fook/injector/src/linux'
cc -Wall -fPIC -I../../include   -c -o elf.o elf.c
cc -Wall -fPIC -I../../include   -c -o injector.o injector.c
cc -Wall -fPIC -I../../include   -c -o ptrace.o ptrace.c
cc -Wall -fPIC -I../../include   -c -o remote_call.o remote_call.c
cc -Wall -fPIC -I../../include   -c -o util.o util.c
cc -shared -o libinjector.so elf.o injector.o ptrace.o remote_call.o util.o
make[1]: Leaving directory '/home/user/work/fook/injector/src/linux'
linking fook with fook.access.c
./fook converted to dso ./fook.so

```

Usage :

```text
TERM1 $ python
Python 3.11.5 (main, Sep  2 2023, 14:16:33) [GCC 13.2.1 20230801] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> import os
>>> os.getpid()
1491953

TERM2 $ sudo ./fook.so 1491953
[+] attached to 1491953
[+] /home/user/work/fook/fook.so injected !

TERM1 
>>> os.access("/etc/passwd",os.F_OK)
True
>>> os.access("/etc/test",os.F_OK)
False
>>> 

TERM2 $ cat /tmp/debug
(1491953) 09:40:34.510066: fook.access.c     fook              #51  	access@ 0x7fce0811c9c0 -> my_access@ 0x7fce0881f404
(1491953) 09:41:15.273087: fook.access.c     my_access         #39  	path=/etc/passwd
(1491953) 09:41:20.389026: fook.access.c     my_access         #39  	path=/etc/test

```

Enjoy !

![fook.jpg](fook.jpg)