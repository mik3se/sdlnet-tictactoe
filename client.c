#include "tictactoe.h"
#include "net.h"
#include "util.h"

int main() {
    const int screenWidth = 720;
    const int screenHeight = 720;

    TCPsocket tcpsock;
    IPaddress ip;
    SDLNet_SocketSet socketSet;

    char username[64];
    char opponent[64] = { 0 };
    uint8_t buffer[256] = { 0 };
    uint8_t gameBoard[9] = { 0 };

    float deltaTime;
    float speedDivisor = 100.0f;
    int gameState = 0;
    int requestRematch = 2;
    uint8_t turn = 3;
    uint8_t playingAs = 0;
    uint8_t winner = 0;
    uint8_t emptypacket = 0;
    uint8_t move;

    SetTraceLogLevel(LOG_ERROR);
    //SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitSDLNet();
    InitNetClient(&tcpsock, &ip);
    InitSocketSet(&socketSet, &tcpsock, 1);

    printf("Please enter a data (63 characters or less): ");
    fgets(username, 64, stdin);
    //flush input buffer if the newline was not stored to string
    if(!strchr(username, '\n')) while (getchar() != '\n');

    StripUsernameE(username);
    memset(buffer, 0, 256);
    buffer[0] = (uint8_t)StringLength(username, 64);
    for(int i = 0; i < buffer[0]; i++){
        buffer[i + 1] = username[i];
    }
    SendTPacket(tcpsock, buffer, StringLength(username, 64) + 1, 0x00);

    memset(username, 0, 64);
    RecvLPacket(tcpsock, buffer, 256);
    StringFromPacket(buffer, username, 2, 64);
    printf("You're playing as: %s\n", username);
    printf("Waiting for opponent...\n");

    InitWindow(screenWidth, screenHeight, "tic tac toe - online multiplayer");

    while(!WindowShouldClose()){
        //Update
        deltaTime = GetFrameTime() * 1000.0f;
        //Update logic
        if(SDLNet_CheckSockets(socketSet, 0) && SDLNet_SocketReady(tcpsock)){
            memset(buffer, 0, 256);
            RecvLPacket(tcpsock, buffer, 256);
            if(gameState == 0){
                if(buffer[1] == 0x01){
                    playingAs = buffer[2];
                    turn = 1;
                    gameState = 1;
                    StringFromPacket(buffer, opponent, 3, 64);
                    printf("You're playing against: %s\n"
                           "Game started! %d\n", opponent, playingAs);
                }
            }
            else if(gameState == 1) {
                switch(buffer[1]){
                    case 0x00:
                        GameboardFromPacket(buffer, gameBoard, 2);
                        turn = playingAs;
                        break;
                    case 0x01:
                        turn = (playingAs % 2) + 1;
                        break;
                    case 0x03:
                        winner = buffer[2];
                        gameState = 2;
                        turn = 3;
                        playingAs = 0;
                        requestRematch = 0;
                        break;
                    case 0xFF:
                        memset(gameBoard, 0, 9);
                        memset(opponent, 0, 64);
                        turn = 3;
                        playingAs = 0;
                        gameState = 0;
                        printf("Opponent has disconnected\n"
                               "Waiting for opponent...\n");
                        break;
                }
            }
            else{
                switch(buffer[1]){
                    case 0x00:
                        memset(gameBoard, 0, 9);
                        playingAs = buffer[2];
                        turn = 1;
                        gameState = 1;
                        winner = 0;
                        printf("Game started! %d\n", playingAs);
                        break;
                    case 0xFF:
                        memset(gameBoard, 0, 9);
                        memset(opponent, 0, 64);
                        winner = 0;
                        gameState = 0;
                        printf("Opponent disconnected\n");
                        break;
                }
            }
        }

        if(requestRematch == 1){
            SendTPacket(tcpsock, &emptypacket, 1, 0x00);
            requestRematch = 2;
        }
        if(gameState == 1) {
            move = UpdateBoard(gameBoard, &playingAs, &turn, &winner);
            if (move <= 9) {
                SendTPacket(tcpsock, &move, 1, 0x00);
                //printf("%d\n", move);
            }
        }

        //Draw
        BeginDrawing();
        ClearBackground(BLACK);
        DrawHoverHighlight(gameBoard, &playingAs, &turn);

        DrawBoard();
        DrawGame(gameBoard, &winner);
        if(gameState == 2){
            DrawRematchButton(&requestRematch);
        }

        DrawText(TextFormat("%d",GetFPS()), 10, 10, 20, GRAY);

        EndDrawing();
    }

    CloseNetClient(&tcpsock);
    QuitSDLNet();

    return 0;
}