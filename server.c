#include <stdio.h>
#include "net.h"
#include "util.h"

int main(){
    Client clientArray[MAX_SOCKETS];
    TCPsocket serverSocket;
    IPaddress ip;

    bool shouldQuit = false;
    int clientCount = 0;
    int readyCount = 0;
    int gameState = 0;
    int gameStateBuffer = 0;
    uint8_t buffer[256] = { 0 };
    int dataLength[2] = {64, 64};
    uint8_t gameBoard[9] = { 0 };
    uint8_t dcpacket[2] = {0xFF, 0x00};

    for(int i = 0; i < MAX_SOCKETS; i++){
        clientArray[i].socket = NULL;
        clientArray[i].ready = false;
        clientArray[i].username[0] = 0;
    }

    InitSDLNet();
    InitNetServer(&serverSocket, &ip);
    SDLNet_SocketSet socketSet = NULL;
    socketSet = SDLNet_AllocSocketSet(MAX_SOCKETS + 1);
    if(socketSet == NULL){
        fprintf(stderr, "SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
        return 1;
    }
    if(SDLNet_TCP_AddSocket(socketSet, serverSocket) < 0){
        fprintf(stderr, "SDLNet_TCP_AddSocket: %s\n", SDLNet_GetError());
        return 1;
    }

    while(!shouldQuit){
        readyCount = SDLNet_CheckSockets(socketSet, 1);
        if(readyCount > 0){
            for(int i = 0; i < MAX_SOCKETS; i++){
                if(SDLNet_SocketReady(clientArray[i].socket)){
                    if(RecvLPacket(clientArray[i].socket, buffer, 256) <= 0){ // prevents program from accessing uninitialized memory
                        RemoveClient(&socketSet, &clientArray[i].socket, &clientCount);
                        gameState = 0;
                        clientArray[i].ready = false;
                        printf("A client disconnected\n");
                        for(int j = 0; j < MAX_SOCKETS; j++){
                            if(clientArray[j].socket){
                                SendLPacket(clientArray[j].socket,  dcpacket, 2);
                            }
                        }
                        printf("A client disconnected\n");
                    }
                    else{
                        if(gameState == 0){
                            StringFromPacket(buffer, clientArray[i].username, 2, 64);
                            StripUsernameE(clientArray[i].username);
                            memset(buffer, 0, 256);
                            StringToPacket(clientArray[i].username, StringLength(clientArray[i].username, 64), buffer, 0, 256);
                            SendTPacket(clientArray[i].socket, buffer, StringLength(clientArray[i].username, 64) + 1, 0x00);
                            printf("Sent a packet: %s\n", clientArray[i].username);
                            clientArray[i].ready = true;
                        }
                        else if(gameState == 1){
                            if(buffer[1] == 0){
                                if(!gameBoard[buffer[2]]){
                                    gameBoard[buffer[2]] = clientArray[i].playerID;
                                    for(int j = 0; j < MAX_SOCKETS; j++){
                                        SendLPacket(clientArray[j].socket, gameBoard, 9);
                                    }
                                }
                            }
                        }
                    }
                    memset(buffer, 0, 256);
                }
                else if(SDLNet_SocketReady(socketSet) && !clientArray[i].socket){
                    //printf("Adding a new client\n");
                    AddClient(&socketSet, &clientArray[i].socket, &serverSocket, &clientCount);
                }
                if(gameState == 0 && clientArray[0].ready && clientArray[1].ready){
                    for(int j = 0; j < MAX_SOCKETS; j++){
                        memset(buffer, 0, 256);
                        StringToPacket(clientArray[(j+1)%2].username, StringLength(clientArray[i].username, 64), buffer, 0, 256);
                        SendTPacket(clientArray[j].socket, buffer, 64, 0x01);
                    }
                    gameState = 1;
                }
            }
        }
    }

    QuitSDLNet();
    return 0;
}
