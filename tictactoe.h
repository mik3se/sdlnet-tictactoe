#include <stdlib.h>
#include <raylib.h>
#include <math.h>

#ifndef ONLINE1V1_TICTACTOE_H
#define ONLINE1V1_TICTACTOE_H

typedef struct {
    Rectangle box;
    int margin;
    int borderwidth;
    char* text;
    int maxtextlen;
    char* defaulttext;
    bool isActive;
}TextBoxPro;

void DrawBoard();
void DrawX(Vector2 origin);
void DrawO(Vector2 origin);
void DrawHoverHighlight(const uint8_t* gameBoard, const uint8_t* playingAs, const uint8_t* turn);
void DrawRematchButton(int* requestRematch);
void DrawConnectButton(int* connect);
void DrawGame(const uint8_t* gameBoard, const uint8_t* winner);
int GameboardFromPacket(const uint8_t* data, uint8_t* copyto, int boardpos);
uint8_t UpdateBoard(uint8_t* gameBoard, const uint8_t* playingAs, const uint8_t* turn, uint8_t* winner);
Vector2 GetOriginFromI(int iterator);
void DrawTextBoxPro(TextBoxPro* tb, Color foreground, Color background, Color inactive);
void DrawTextWidth(const char *text, Vector2 position, float fontSize, int width, Color tint);
void DrawTextCodepointOffset(Font font, int codepoint, Vector2 position, float fontSize, Color tint, float offset);
void DrawRestrictedText(const char *text, int posX, int posY, float fontSize, int width, int margin, Color tint);

#endif //ONLINE1V1_TICTACTOE_H