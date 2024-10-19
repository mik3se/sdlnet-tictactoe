//
// Created by Michael Reyes on 10/17/24.
//

#include "net.h"

#ifndef TICTACTOE_SERVERUTILS_H
#define TICTACTOE_SERVERUTILS_H

void ResetClientArray(Client clientArray[], int clientCount);
void InitNetServer(TCPsocket* socket, IPaddress* ip);
void AddClient(SDLNet_SocketSet* socketSet, TCPsocket* clientSocket, TCPsocket* serverSocket, int* clientCount);
void RemoveClient(SDLNet_SocketSet* socketSet, TCPsocket* socket, int* clientCount);
uint8_t CheckWinner(const uint8_t* gameBoard);

#endif //TICTACTOE_SERVERUTILS_H