//
// Created by Michael Reyes on 9/24/24.
//
#include <stdint.h>

#ifndef TICTACTOE_UTIL_H
#define TICTACTOE_UTIL_H

void StripUsernameC(char* copyTo, const char* data);
void StripUsernameE(char* data);
int StringLength(const char* str, int maxlen);
int StringFromPacket(const uint8_t* data, char* copyto, int strpos, int maxlen);
int StringToPacket(const char* source, int strlen, uint8_t* dest, int pos, int maxlen);

#endif //TICTACTOE_UTIL_H