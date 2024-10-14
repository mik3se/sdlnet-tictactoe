#include "tictactoe.h"
#include "net.h"
#include "util.h"

int main() {
    const int screenWidth = 720;
    const int screenHeight = 720;

    char username[64];
    char opponent[64] = { 0 };
    uint8_t buffer[256] = { 0 };
    uint8_t gameBoard[9] = { 0 };

    float deltaTime;
    float speedDivisor = 100.0f;
    uint8_t turn = 1;
    uint8_t playingAs = 1;
    uint8_t winner = 0;

    SetTraceLogLevel(LOG_ERROR);
    //SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    //Initialize SDLNet
    InitSDLNet();
    TCPsocket tcpsock;
    IPaddress ip;
    InitNetClient(&tcpsock, &ip);

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

    //if(SDLNet_SocketReady(tcpsock)){}
    RecvLPacket(tcpsock, buffer, 256);
    StringFromPacket(buffer, opponent, 2, 64);
    printf("You're playing against: %s\n", opponent);
    printf("Game started!\n");

    InitWindow(screenWidth, screenHeight, "tic tac toe - online multiplayer");

    while(!WindowShouldClose()){
        //Update
        deltaTime = GetFrameTime() * 1000.0f;
        //Update logic
        UpdateBoard(gameBoard, &playingAs, &turn, &winner);
        UpdateBoard(gameBoard, &turn, &turn, &winner);

        //Draw
        BeginDrawing();
        ClearBackground(BLACK);
        DrawHoverHighlight(gameBoard, &playingAs, &turn);

        DrawBoard();
        DrawGame(gameBoard, &winner);

        DrawText(TextFormat("%d",GetFPS()), 10, 10, 20, GRAY);

        EndDrawing();
    }

    CloseNetClient(&tcpsock);
    QuitSDLNet();

    return 0;
}