#include "tictactoe.h"
#include "net.h"
#include "util.h"

int main() {
    const int screenWidth = 720;
    const int screenHeight = 720;

    TCPsocket tcpsock;
    IPaddress ip;
    SDLNet_SocketSet socketSet;

    char username[64] = { 0 };
    char opponent[64] = { 0 };
    char ipAddress[128] = { 0 };
    TextBoxPro usernameTB = {{60, 120, 600, 100}, 10, 10, username, 64, "username", false};
    TextBoxPro ipTB = {{60, 240, 600, 100}, 10, 10, ipAddress, 128, "ip address", false};
    uint8_t buffer[256] = { 0 };
    uint8_t gameBoard[9] = { 0 };

    float deltaTime;
    float speedDivisor = 100.0f;
    int gameState = 0;
    int requestRematch = 2;
    int connect = 0;
    uint8_t turn = 3;
    uint8_t playingAs = 0;
    uint8_t winner = 0;
    uint8_t emptypacket = 0;
    uint8_t move;

    bool boardIsEmpty = true;
    bool displayOpponentName = false;

    SetTraceLogLevel(LOG_ERROR);
    //SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitSDLNet();
    InitWindow(screenWidth, screenHeight, "tic tac toe - online multiplayer");
    //SetExitKey(KEY_NULL);

    while(!WindowShouldClose()){
        //Update
        deltaTime = GetFrameTime() * 1000.0f;
        //Update logic
        if(connect == 1){
            if(!InitNetClient(&tcpsock, &ip, ipAddress)){
                InitSocketSet(&socketSet, &tcpsock, 1);
                buffer[0] = (uint8_t)StringLength(username, 64);
                for(int i = 0; i < buffer[0]; i++){
                    buffer[i + 1] = username[i];
                }
                SendTPacket(tcpsock, buffer, StringLength(username, 64) + 1, 0x00);
                memset(username, 0, 64);
                RecvLPacket(tcpsock, buffer, 256);
                StringFromPacket(buffer, username, 2, 64);
                connect = 2;
            }
            else connect = -1;
        }
        else if(connect == 2 && SDLNet_CheckSockets(socketSet, 0) && SDLNet_SocketReady(tcpsock)){
            memset(buffer, 0, 256);
            RecvLPacket(tcpsock, buffer, 256);
            if(gameState == 0){
                if(buffer[1] == 0x01){
                    playingAs = buffer[2];
                    turn = 1;
                    gameState = 1;
                    StringFromPacket(buffer, opponent, 3, 64);
                    displayOpponentName = true;
                }
            }
            else if(gameState == 1) {
                switch(buffer[1]){
                    case 0x00:
                        GameboardFromPacket(buffer, gameBoard, 2);
                        boardIsEmpty = false;
                        displayOpponentName = false;
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
                        boardIsEmpty = true;
                        memset(opponent, 0, 64);
                        turn = 3;
                        playingAs = 0;
                        gameState = 0;
                        break;
                }
            }
            else{
                switch(buffer[1]){
                    case 0x00:
                        memset(gameBoard, 0, 9);
                        boardIsEmpty = true;
                        playingAs = buffer[2];
                        turn = 1;
                        gameState = 1;
                        winner = 0;
                        break;
                    case 0xFF:
                        memset(gameBoard, 0, 9);
                        boardIsEmpty = true;
                        memset(opponent, 0, 64);
                        winner = 0;
                        gameState = 0;
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
                boardIsEmpty = false;
                displayOpponentName = false;
            }
        }

        //Draw
        BeginDrawing();

        if(connect < 2){
            ClearBackground(WHITE);
            DrawTextBoxPro(&usernameTB, BLACK, BLANK, GRAY);
            DrawTextBoxPro(&ipTB, BLACK, BLANK, GRAY);
            if(connect < 0){
                DrawText("Could not connect!", 360 - MeasureText("Could not connect!", 60)/2, 380, 60, RED);
            }
            DrawConnectButton(&connect);
        }
        else {
            ClearBackground(BLACK);
            DrawHoverHighlight(gameBoard, &playingAs, &turn);
            DrawBoard();
            DrawGame(gameBoard, &winner);
            if(gameState == 0){
                DrawText(TextFormat("You're playing as: %s", username), 360 - MeasureText(TextFormat("You're playing as: %s", username), 40)/2, 300, 40, PURPLE);
                DrawText("Waiting for opponent...", 360 - MeasureText("Waiting for opponent...", 40)/2, 380, 40, PURPLE);
            }
            else if(gameState == 1 && boardIsEmpty){
                if(displayOpponentName) DrawText(TextFormat("You're playing against: %s", opponent), 360 - MeasureText(TextFormat("You're playing against: %s", opponent), 40)/2, 300, 40, PURPLE);
                DrawText("Game started!", 360 - MeasureText("Game started!", 40)/2, displayOpponentName? 380 : 340, 40, PURPLE);
            }
            else if (gameState == 2) {
                DrawRematchButton(&requestRematch);
            }
        }

        //DrawText(TextFormat("%d",GetFPS()), 10, 10, 20, GRAY);

        EndDrawing();
    }
    if(connect == 2){
        CloseNetClient(&tcpsock);
    }
    QuitSDLNet();

    return 0;
}