#include <stdlib.h>
#include <raylib.h>
#include <math.h>

#ifndef ONLINE1V1_TICTACTOE_H
#define ONLINE1V1_TICTACTOE_H

#endif //ONLINE1V1_TICTACTOE_H

void DrawBoard();
void DrawX(Vector2 origin);
void DrawO(Vector2 origin);
void DrawHoverHighlight(const uint8_t* gameBoard, const uint8_t* playingAs, const uint8_t* turn);
void DrawGame(const uint8_t* gameBoard, const uint8_t* winner);
void UpdateBoard(uint8_t* gameBoard, const uint8_t* playingAs, uint8_t* turn, uint8_t* winner);
Vector2 GetOriginFromI(int iterator);