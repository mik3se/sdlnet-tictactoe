#include "tictactoe.h"
#include "net.h"

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

void DrawRematchButton(int* requestRematch){
    Rectangle button = {0, 480, (float)(40 + MeasureText("Rematch?", 60)), 100};
    button.x = 360 - button.width/2;
    if(*requestRematch == 0){
        if(CheckCollisionPointRec(GetMousePosition(), button) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
            *requestRematch = 1;
        }
        if(CheckCollisionPointRec(GetMousePosition(), button) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            DrawRectangleRec(button, DARKGRAY);
        }
        else{
            DrawRectangleRec(button, LIGHTGRAY);
        }
    }
    else{
        DrawRectangleRec(button, GRAY);
    }
    DrawText("Rematch?", button.x + 20, button.y + 20, 60, WHITE);
}
void DrawConnectButton(int* connect){
    Rectangle button = {0, 480, (float)(40 + MeasureText("Connect", 60)), 100};
    button.x = 360 - button.width/2;
    if(*connect <= 0){
        if((CheckCollisionPointRec(GetMousePosition(), button) && (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) || IsKeyPressed(KEY_ENTER))){
            *connect = 1;
        }
        if(CheckCollisionPointRec(GetMousePosition(), button) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            DrawRectangleRec(button, DARKGRAY);
        }
        else{
            DrawRectangleRec(button, LIGHTGRAY);
        }
    }
    else{
        DrawRectangleRec(button, GRAY);
    }
    DrawText("Connect", button.x + 20, button.y + 20, 60, WHITE);
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

int GameboardFromPacket(const uint8_t* data, uint8_t* copyto, int boardpos){
    for(int i = 0; i < 9; i++) {
        copyto[i] = data[boardpos + i];
    }
    return boardpos + 9;
}

uint8_t UpdateBoard(uint8_t* gameBoard, const uint8_t* playingAs, const uint8_t* turn, uint8_t* winner){
    for(int i = 0; i < 9; i++){
        Vector2 origin = GetOriginFromI(i);
        if(CheckCollisionPointRec(GetMousePosition(), (Rectangle){origin.x, origin.y, 232, 232})){
            if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && !gameBoard[i] && *turn == *playingAs){
                gameBoard[i] = *playingAs;
                return i;
            }
        }
    }
    return 10;
}

Vector2 GetOriginFromI(int iterator){
    return (Vector2){(float)((iterator % 3) * 240 + 4 * (iterator%3)), (float)((iterator / 3) * 240 + 4 * (iterator/3))};
}

void DrawTextBoxPro(TextBoxPro* tb, Color foreground, Color background, Color inactive){
    int charPressed;
    int index;
    if(CheckCollisionPointRec(GetMousePosition(), tb->box) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) tb->isActive = true;
    else if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_ESCAPE)) tb->isActive = false;
    if(tb->isActive) {
        charPressed = GetCharPressed();
        index = TextLength(tb->text);
        while (charPressed) {
            if (((charPressed >= '0' && charPressed <= '9') || (charPressed >= 'A' && charPressed <= 'Z') ||
                 (charPressed >= 'a' && charPressed <= 'z') || charPressed == '_' || charPressed == '.') && index < (tb->maxtextlen - 1)) {
                tb->text[index] = (char) charPressed;
                tb->text[index + 1] = '\0';
                index++;
            }
            charPressed = GetCharPressed();
        }
        if ((IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) && index > 0) {
            tb->text[index - 1] = '\0';
            index--;
        }
    }
    if(tb->borderwidth > 0) DrawRectangleLinesEx(tb->box, tb->borderwidth, tb-> isActive? foreground : inactive);
    //use BLANK for no background
    if(inactive.a > 0) DrawRectangle((int)tb->box.x + tb->borderwidth, (int)tb->box.y + tb->borderwidth, (int)tb->box.width - (2*tb->borderwidth), (int)tb->box.height - (2*tb->borderwidth), background);
    if(!tb->text[0] && tb->defaulttext){
        DrawRestrictedText(tb->defaulttext, (int)tb->box.x + tb->borderwidth, (int)tb->box.y + tb->borderwidth + tb->margin, tb->box.height - (2.0F*(float)tb->borderwidth) - (2.0F*(float)tb->margin), (int)tb->box.width - (2*tb->borderwidth) - tb->margin, tb->margin, inactive);
    }
    else{
        DrawRestrictedText(tb->text, (int)tb->box.x + tb->borderwidth, (int)tb->box.y + tb->borderwidth + tb->margin, tb->box.height - (2.0F*(float)tb->borderwidth) - (2.0F*(float)tb->margin), (int)tb->box.width - (2*tb->borderwidth) - tb->margin, tb->margin, tb->isActive ? foreground : inactive);
    }
}

//Start of modified code from raylib version 5.0
/*
 * Copyright (c) 2013-2024 Ramon Santamaria (@raysan5)
 *
 * This software is provided "as-is", without any express or implied warranty. In no event
 * will the authors be held liable for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose, including commercial
 * applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not claim that you
 *  wrote the original software. If you use this software in a product, an acknowledgment
 *  in the product documentation would be appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented
 *  as being the original software.
 *
 *  3. This notice may not be removed or altered from any source distribution.
 */
void DrawTextWidth(const char *text, Vector2 position, float fontSize, int width, Color tint)
{
    Font font = GetFontDefault();  // Security check in case of not valid font

    int spacing = fontSize/10;

    int size = TextLength(text);    // Total size in bytes of the text, scanned by codepoints in loop

    float textOffsetX = (float)width;       // Offset X to next character to draw

    float scaleFactor = fontSize/font.baseSize;         // Character quad scaling factor

    for (int i = size; i > 0;)
    {
        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetCodepointPrevious(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);
        if (codepoint != '\n')
        {
            if (i != size && font.glyphs[index].advanceX == 0) textOffsetX -= ((float)font.recs[index].width*scaleFactor + spacing);
            else if(i != size) textOffsetX -= ((float)font.glyphs[index].advanceX*scaleFactor + spacing);
            else if(font.glyphs[index].advanceX == 0) textOffsetX -= ((float)font.recs[index].width*scaleFactor);
            else textOffsetX -= ((float)font.glyphs[index].advanceX*scaleFactor + spacing);

            if(textOffsetX < 0){
                DrawTextCodepointOffset(font, codepoint, (Vector2){ position.x + textOffsetX, position.y }, fontSize, tint, 0 - textOffsetX);
                break;
            }

            if ((codepoint != ' ') && (codepoint != '\t'))
            {
                DrawTextCodepoint(font, codepoint, (Vector2){ position.x + textOffsetX, position.y }, fontSize, tint);
            }
        }

        i -= codepointByteCount;   // Move text bytes counter to next codepoint
    }
}

void DrawTextCodepointOffset(Font font, int codepoint, Vector2 position, float fontSize, Color tint, float offset)
{
    // Character index position in sprite font
    // NOTE: In case a codepoint is not available in the font, index returned points to '?'
    int index = GetGlyphIndex(font, codepoint);
    float scaleFactor = fontSize/font.baseSize;     // Character quad scaling factor

    // Character destination rectangle on screen
    // NOTE: We consider glyphPadding on drawing
    Rectangle dstRec = { position.x + font.glyphs[index].offsetX*scaleFactor - (float)font.glyphPadding*scaleFactor + offset,
                         position.y + font.glyphs[index].offsetY*scaleFactor - (float)font.glyphPadding*scaleFactor,
                         (font.recs[index].width + 2.0f*font.glyphPadding)*scaleFactor - offset,
                         (font.recs[index].height + 2.0f*font.glyphPadding)*scaleFactor };

    // Character source rectangle from font texture atlas
    // NOTE: We consider chars padding when drawing, it could be required for outline/glow shader effects
    Rectangle srcRec = { font.recs[index].x - (float)font.glyphPadding + ((font.baseSize/fontSize) * offset), font.recs[index].y - (float)font.glyphPadding,
                         font.recs[index].width + 2.0f*font.glyphPadding - ((font.baseSize/fontSize) * offset), font.recs[index].height + 2.0f*font.glyphPadding };

    // Draw the character texture on the screen
    DrawTexturePro(font.texture, srcRec, dstRec, (Vector2){ 0, 0 }, 0.0f, tint);
}

void DrawRestrictedText(const char *text, int posX, int posY, float fontSize, int width, int margin, Color tint){

    Font font = GetFontDefault();  // Security check in case of not valid font

    float textOffsetX = 0.0f;       // Offset X to next character to draw

    float scaleFactor = fontSize/font.baseSize;         // Character quad scaling factor

    int spacing = fontSize/10;

    for (int i = 0; i < TextLength(text);)
    {
        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetCodepointNext(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);

        if (codepoint != '\n')
        {
            if (font.glyphs[index].advanceX == 0) textOffsetX += ((float)font.recs[index].width*scaleFactor + spacing);
            else textOffsetX += ((float)font.glyphs[index].advanceX*scaleFactor + spacing);
        }

        if(textOffsetX > width){
            break;
        }

        i += codepointByteCount;   // Move text bytes counter to next codepoint
    }

    if(textOffsetX > width){
        DrawTextWidth(text, (Vector2){posX, posY}, fontSize, width, tint);
    }
    else{
        DrawText(text, posX + margin, posY, fontSize, tint);
    }
}
//End of modified code from raylib