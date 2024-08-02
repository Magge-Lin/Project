#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/socket.h>
#include <string.h>

// 编译时生成共享库
// gcc -shared -fPIC -o hook.so hook.c -ldl

//LD_PRELOAD=./hook.so ./build/dataRelaySrs 8888
const char* hookRecvStr = " --hook Recv--";
const char* hookSendStr = " --hook Send--";

typedef ssize_t (*recv_ptr)(int sockfd, void *buf, size_t len, int flags);
typedef ssize_t (*send_ptr)(int sockfd, const void *buf, size_t len, int flags);

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
    static recv_ptr original_recv = NULL;

    if (original_recv == NULL) {
        original_recv = (recv_ptr)dlsym(RTLD_NEXT, "recv");
    }

    char* newbuf = (char*)malloc(strlen(hookRecvStr) + len);

    ssize_t received = original_recv(sockfd, newbuf, len, flags);
    if (received > 0) {
        strcpy(newbuf + received, hookRecvStr);
        received = received + strlen(hookRecvStr);
    }

    memcpy(buf, newbuf, received + strlen(hookRecvStr));

    free(newbuf);

    return received;
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
    static send_ptr original_send = NULL;

    if (original_send == NULL) {
        original_send = (send_ptr)dlsym(RTLD_NEXT, "send");
    }
    ssize_t sent = 0;
    if (len > strlen(hookRecvStr)) {
        size_t newlen = strlen(hookSendStr) + len - strlen(hookRecvStr);
        char* newbuf = (char*)malloc(newlen);
        strcpy(newbuf, (const char*)buf);
        strcpy(newbuf + len - strlen(hookRecvStr), hookSendStr);
    
        sent = original_send(sockfd, newbuf, newlen, flags);

        free(newbuf);
    }
    else {
        sent = original_send(sockfd, buf, len, flags);
    }

    return sent;
}
