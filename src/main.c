#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>

#include "fook.h"

#ifdef PROD
void trace(const char* format, ...);
#else
#include <sys/time.h>
#include <time.h>
#include <sys/stat.h>
#include <stdarg.h>

void trace(const char* format, ...) {
    va_list param;
    struct timeval tv;
    struct tm *nowtm;
    time_t nowtime;
    char tmbuf[64], buf[512];
    gettimeofday(&tv, NULL);
    nowtime = tv.tv_sec;
    nowtm = localtime(&nowtime);
    strftime(tmbuf, sizeof tmbuf, "%H:%M:%S", nowtm);
    snprintf(buf, sizeof buf, "%s.%03ld", tmbuf, tv.tv_usec);

    FILE *out = fopen("/tmp/debug","a+");
    if(out == NULL)
        return;
    chmod("/tmp/debug",0777);

    //FILE *out = fopen("/dev/stdout","a+");
    va_start(param, format);
    fprintf(out, "(%d) %s: " , getpid(), buf);
    vfprintf(out, format, param);
    fprintf(out, "\n");
    va_end(param);
    fflush(out);
    fclose(out);
}
#endif


FOOK_EXPORT const char* locallib(){
    Dl_info dl_info;
    dladdr((char *)locallib, &dl_info);
    return dl_info.dli_fname;
}


int inject(int pid) {

    injector_t *injector;
    char dso[4096] =  {0};
    readlink(PROCSELF,dso,4096);

    int ret = -1;
    if ((ret = injector_attach(&injector, pid)) != 0) {
        perror("attach");
        return 1;
    }

    printf("[+] attached to %i\n",pid);

    if (!(ret = injector_inject(injector, dso, NULL)) == 0) {
        perror("inj");
        return 1;
    }
    printf("[+] %s injected !\n",dso);
    return 0;
}

int main(int argc,char *argv[],char **env){
    if(argc == 2){
        int pid = atoi(argv[1]);
        inject(pid);
        return 1;
    }
    return 0;
}
