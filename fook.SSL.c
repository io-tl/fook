#include <stdio.h>
#include <stdlib.h>
#include "fook.h"

// int SSL_read(SSL *ssl, void *buf, int num);

FOOK_HOOK(SSL_read);
int (*orig_SSL_read)(void*,char*,int) = NULL;

FOOK_EXPORT int my_sslread(void* ctx,char *buf,int num){
    FOOK_ENTER(SSL_read);
    int ret = orig_SSL_read(ctx,buf,num);
    FOOK_RETURN(SSL_read);

    TRACE("%s",buf);

    return ret;
}

//int SSL_write(SSL *ssl, const void *buf, int num);

FOOK_HOOK(SSL_write);
int (*orig_SSL_write)(void*,char*,int) = NULL;

FOOK_EXPORT int my_sslwrite(void* ctx,char *buf,int num){
    FOOK_ENTER(SSL_write);
    int ret = orig_SSL_write(ctx,buf,num);
    FOOK_RETURN(SSL_write);

    TRACE("%s",buf);

    return ret;
}

FOOK_INIT void fook() {
    FOOK_ADD_TRAMPOLINE( SSL_read , my_sslread );
    FOOK_ADD_TRAMPOLINE( SSL_write , my_sslwrite );
}
