#include "tictactoe.h"

// DIAGFORMULAX(w, pos)  ((2.0*pow(l, 2) + pow(w, 2)/4.0) * cos(M_PI_4 + atan(w/2.0*M_SQRT2*pos)))
// DIAGFORMULAY(w, pos)  ((2.0*pow(l, 2) + pow(w, 2)/4.0) * sin(M_PI_4 + atan(w/2.0*M_SQRT2*pos)))

void DrawBoard(){
    DrawRectangle(GetScreenWidth()/3 - 8, 0, 12, GetScreenHeight(), WHITE);
    DrawRectangle(2*GetScreenWidth()/3 - 4, 0, 12, GetScreenHeight(), WHITE);
    DrawRectangle(0, GetScreenHeight()/3 - 8, GetScreenWidth(), 12, WHITE);
    DrawRectangle(0, 2*GetScreenHeight()/3 - 4, GetScreenWidth(), 12, WHITE);
}

void DrawX(Vector2 origin){
    //I think they're not perfectly symmetrical because of a rounding issue, but I'm too lazy to fix this
    //uses DIAGFORMULAX and DIAGFORMULAY
    DrawRectanglePro((Rectangle){26 + origin.x, 33 + origin.y, 10, 243}, (Vector2){0.0f, 0.0f}, -45, RED);
    DrawRectanglePro((Rectangle){232 - 33 + origin.x, 26 + origin.y, 10, 243}, (Vector2){0.0f, 0.0f}, 45, RED);
}

void DrawO(Vector2 origin){
    DrawRing((Vector2){232.0f - 116.0f + origin.x, 232.0f - 116.0f + origin.y}, 91, 101, 0, 360, 0, BLUE);
}

void DrawHoverHighlight(const uint8_t* gameBoard, const uint8_t* playingAs, const uint8_t* turn){
    for(int i = 0; i < 9; i++){
        Vector2 origin = GetOriginFromI(i);
        Rectangle toCheck = {origin.x, origin.y, 232, 232};
        if(CheckCollisionPointRec(GetMousePosition(), toCheck) && *turn == *playingAs){
            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !gameBoard[i]){
                DrawRectangleRec(toCheck, (Color){ 40, 40, 40, 255 });
            }
            else {
                DrawRectangleRec(toCheck, (Color){ 60, 60, 60, 255 });
            }
        }
    }
}

void DrawGame(const uint8_t* gameBoard, const uint8_t* winner){
    for(int i = 0; i < 9; i++){
        if(gameBoard[i] == 1){
            DrawX(GetOriginFromI(i));
        }
        else if(gameBoard[i] == 2){
            DrawO(GetOriginFromI(i));
        }
    }
    //draw lines for 3 in a row
    for(int i = 0; i < 3; i++){
        if(gameBoard[i*3] && gameBoard[i*3] == gameBoard[i*3+1] && gameBoard[i*3] == gameBoard[i*3+2]){
            DrawRectangle(10, (int)GetOriginFromI(i*3).y + 116 - 4, 700, 8, GRAY);
        }
        if(gameBoard[i] && gameBoard[i] == gameBoard[i+3] && gameBoard[i] == gameBoard[i+6]){
            DrawRectangle((int)GetOriginFromI(i).x + 116 - 4, 10, 8, 700, GRAY);
        }
    }
    //uses diagonal formulas
    if(gameBoard[0] && gameBoard[0] == gameBoard[4] && gameBoard[0] == gameBoard[8]){
        DrawRectanglePro((Rectangle){12, 18, 8, 975}, (Vector2){0.0f, 0.0f}, -45, GRAY);
    }
    if(gameBoard[2] && gameBoard[2] == gameBoard[4] && gameBoard[2] == gameBoard[6]){
        DrawRectanglePro((Rectangle){720 - 18, 12, 8, 975}, (Vector2){0.0f, 0.0f}, 45, GRAY);
    }
    if(*winner == 1){
        DrawText("X Wins!", 360 - MeasureText("X Wins!", 160)/2, 360 - 80, 160, PURPLE);
    }
    else if(*winner == 2){
        DrawText("O Wins!", 360 - MeasureText("O Wins!", 160)/2, 360 - 80, 160, PURPLE);
    }
    else if(*winner == 3){
        DrawText("Draw!", 360 - MeasureText("Draw!", 160)/2, 360 - 80, 160, PURPLE);
    }
}

void UpdateBoard(uint8_t* gameBoard, const uint8_t* playingAs, uint8_t* turn, uint8_t* winner){
    for(int i = 0; i < 9; i++){
        Vector2 origin = GetOriginFromI(i);
        if(CheckCollisionPointRec(GetMousePosition(), (Rectangle){origin.x, origin.y, 232, 232})){
            if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && !gameBoard[i] && *turn == *playingAs){
                gameBoard[i] = *playingAs;
                *turn = *playingAs == 1 ? 2 : 1;
            }
        }
    }
    /*if(!*winner){
        *winner = 3;
        for(int i = 0; i < 9; i++){
            if(!gameBoard[i]){
                *winner = 0;
                break;
            }
        }
        for(int i = 0; i < 3; i++){
            if(gameBoard[i*3] && gameBoard[i*3] == gameBoard[i*3+1] && gameBoard[i*3] == gameBoard[i*3+2]){
                *winner = gameBoard[i*3];
            }
            if(gameBoard[i] && gameBoard[i] == gameBoard[i+3] && gameBoard[i] == gameBoard[i+6]){
                *winner = gameBoard[i];
            }
        }
        if(gameBoard[0] && gameBoard[0] == gameBoard[4] && gameBoard[0] == gameBoard[8]){
            *winner = gameBoard[0];
        }
        if(gameBoard[2] && gameBoard[2] == gameBoard[4] && gameBoard[2] == gameBoard[6]){
            *winner = gameBoard[2];
        }
    }
    if(*winner){
        *turn = 0;
    }*/
}

Vector2 GetOriginFromI(int iterator){
    return (Vector2){(float)((iterator % 3) * 240 + 4 * (iterator%3)), (float)((iterator / 3) * 240 + 4 * (iterator/3))};
}