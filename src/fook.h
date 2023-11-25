#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include "injector.h"
#include "subhook.h"

#define PROCSELF "/proc/self/exe"

#define FOOK_EXPORT __attribute__ ((visibility ("default")))
#define FOOK_INIT   __attribute__((constructor))
#define FOOK_EXIT  __attribute__((destructor))

#define APPEND_HOOK(a) a##_hook
#define PREPEND_ORIG(a) orig_##a

const char* locallib();

#define FOOK_HOOK(f) \
    subhook_t APPEND_HOOK(f) = NULL;

#define FOOK_ENTER( f ) \
    subhook_remove( APPEND_HOOK(f) );

#define FOOK_RETURN( f ) \
    subhook_install( APPEND_HOOK(f) );

#define FOOK_ADD_TRAMPOLINE(sym,dest_sym) \
    do{ \
        if ( (  PREPEND_ORIG(sym)   = dlsym(NULL, #sym )) != NULL ){ \
            APPEND_HOOK(sym) = subhook_new((void *)PREPEND_ORIG(sym), (void *)dest_sym, 0); \
            subhook_install(APPEND_HOOK(sym)); \
            TRACE(#sym"@ %p -> "#dest_sym"@ %p",PREPEND_ORIG(sym),dest_sym); \
        } \
    }while(0)

#define FOOK_ADD_PRELOAD \
        setenv("LD_PRELOAD",locallib(),0);


#ifdef PROD
    #define TRACE   (void)sizeof
#else
    void trace(const char* format, ...);
    #define TRACE( fmt , args... ) \
        trace("\033[1;36m%-18s\033[0;33m%-18s\033[0;32m#%d  \t\033[0m" fmt , __FILE__ , __FUNCTION__ , __LINE__ , ##args );
#endif
