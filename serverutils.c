//
// Created by Michael Reyes on 10/17/24.
//

#include "serverutils.h"

void ResetClientArray(Client clientArray[], int clientCount){
    for(int i = 0; i < clientCount; i++){
        clientArray[i].socket = NULL;
        memset(clientArray[i].username, 0, 64);
        clientArray[i].ready = false;
    }
}

void InitNetServer(TCPsocket* socket, IPaddress* ip){
    if (SDLNet_ResolveHost(ip, NULL, 3005) != 0) {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        exit(1);
    }
    *socket = SDLNet_TCP_Open(ip);
    if (!*socket) {
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        exit(2);
    }
}

void AddClient(SDLNet_SocketSet* socketSet, TCPsocket* clientSocket, TCPsocket* serverSocket, int* clientCount){
    if(*clientCount >= MAX_SOCKETS){
        return;
    }

    *clientSocket = SDLNet_TCP_Accept(*serverSocket);

    if(*clientSocket){
        SDLNet_TCP_AddSocket(*socketSet, *clientSocket);
        (*clientCount)++;
    }
    else{
        SDLNet_TCP_Close(*clientSocket);
    }
}

void RemoveClient(SDLNet_SocketSet* socketSet, TCPsocket* socket, int* clientCount){
    SDLNet_TCP_DelSocket(*socketSet, *socket);
    SDLNet_TCP_Close(*socket);
    *socket = NULL;
    (*clientCount)--;
}

uint8_t CheckWinner(const uint8_t* gameBoard){
    uint8_t winner = 3;
    for(int i = 0; i < 9; i++){
        if(!gameBoard[i]){
            winner = 0;
            break;
        }
    }
    for(int i = 0; i < 3; i++){
        if(gameBoard[i*3] && gameBoard[i*3] == gameBoard[i*3+1] && gameBoard[i*3] == gameBoard[i*3+2]){
            winner = gameBoard[i*3];
        }
        if(gameBoard[i] && gameBoard[i] == gameBoard[i+3] && gameBoard[i] == gameBoard[i+6]){
            winner = gameBoard[i];
        }
    }
    if(gameBoard[0] && gameBoard[0] == gameBoard[4] && gameBoard[0] == gameBoard[8]){
        winner = gameBoard[0];
    }
    if(gameBoard[2] && gameBoard[2] == gameBoard[4] && gameBoard[2] == gameBoard[6]){
        winner = gameBoard[2];
    }
    return winner;
}
