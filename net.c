//
// Created by Michael Reyes on 9/21/24.
//

#include "net.h"

void InitSDLNet(){
    if (SDLNet_Init() == -1) {
        fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
        exit(2);
    }
}

int InitNetClient(TCPsocket* socket, IPaddress* ip, const char* ipAddress){
    if (SDLNet_ResolveHost(ip, ipAddress, 3005) != 0) {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        return 1;
    }
    *socket = SDLNet_TCP_Open(ip);
    if (!*socket) {
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        return 2;
    }
    return 0;
}

void InitSocketSet(SDLNet_SocketSet* socketSet, TCPsocket* toAdd, int maxsockets){
    *socketSet = SDLNet_AllocSocketSet(maxsockets);
    if(*socketSet == NULL){
        fprintf(stderr, "SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
        exit(1);
    }
    if(SDLNet_TCP_AddSocket(*socketSet, *toAdd) < 0){
        fprintf(stderr, "SDLNet_TCP_AddSocket: %s\n", SDLNet_GetError());
        exit(1);
    }
}

void CloseNetClient(TCPsocket* socket){
    SDLNet_TCP_Close(*socket);
}

void QuitSDLNet(){
    SDLNet_Quit();
}

int RecvLPacket(TCPsocket sock, uint8_t *data, int maxlen){ //please do not use maxlen longer than 254
    uint8_t len;
    if(SDLNet_TCP_Recv(sock, &len, 1) <= 0) return -1;
    int recvLen = len > maxlen - 1 ? maxlen - 1 : len;
    if(SDLNet_TCP_Recv(sock, data, recvLen) <= 0) return -2;
    for(int i = maxlen - 1; i > 0; i--){
        data[i] = data[i - 1];
    }
    data[0] = len;
    return recvLen + 1;
}
int SendLPacket(TCPsocket sock, const uint8_t* data, int maxlen){ //ditto
    int result;
    uint8_t* sendData = malloc(maxlen + 1);
    for(int i = 0; i < maxlen; i++){
        sendData[i + 1] = data[i];
    }
    sendData[0] = (uint8_t)maxlen;
    result = SDLNet_TCP_Send(sock, sendData, maxlen + 1);
    free(sendData);
    return result;
}
int SendTPacket(TCPsocket sock, const uint8_t* data, int maxlen, uint8_t type){
    int result;
    uint8_t* sendData = malloc(maxlen + 2);
    for(int i = 0; i < maxlen; i++){
        sendData[i + 2] = data[i];
    }
    sendData[0] = (uint8_t)maxlen + 1;
    sendData[1] = type;
    result = SDLNet_TCP_Send(sock, sendData, maxlen + 2);
    free(sendData);
    return result;
}