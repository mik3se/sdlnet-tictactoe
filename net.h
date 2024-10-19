#include <SDL2/SDL_net.h>
#include <stdbool.h>

#define MAX_SOCKETS 2

#ifndef ONLINE1V1_NET_HPP
#define ONLINE1V1_NET_HPP

typedef struct {
    TCPsocket socket;
    char username[64];
    bool ready;
    uint8_t playerID;
}Client;

void InitSDLNet();
void InitNetClient(TCPsocket* socket, IPaddress* ip);
void InitSocketSet(SDLNet_SocketSet* socketSet, TCPsocket* toAdd, int maxsockets);
void CloseNetClient(TCPsocket* socket);
void QuitSDLNet();
int SendTPacket(TCPsocket sock, const uint8_t* data, int maxlen, uint8_t type);
int RecvLPacket(TCPsocket sock, uint8_t *data, int maxlen);
int SendLPacket(TCPsocket sock, const uint8_t *data, int maxlen);

#endif //ONLINE1V1_NET_HPP