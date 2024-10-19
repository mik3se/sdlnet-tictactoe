//
// Created by Michael Reyes on 9/24/24.
//
#include "util.h"

void StripUsernameC(char* copyTo, const char* data){
    for(int i = 0, j = 0; i < 64; i++){
        if(data[i] == 0){
            copyTo[j] = 0;
            break;
        }
        else if((data[i] >= '0' && data[i] <= '9') || (data[i] >= 'A' && data[i] <= 'Z') || (data[i] >= 'a' && data[i] <= 'z') || data[i] == '_'){
            copyTo[j] = data[i];
            j++;
        }
    }
}

void StripUsernameE(char* data){
    for(int i = 0, j = 0; i < 64; i++){
        if(data[i] == 0){
            data[j] = 0;
            break;
        }
        else if((data[i] >= '0' && data[i] <= '9') || (data[i] >= 'A' && data[i] <= 'Z') || (data[i] >= 'a' && data[i] <= 'z') || data[i] == '_'){
            data[j] = data[i];
            j++;
        }
    }
}

int StringLength(const char* str, int maxlen){
    for(int i = 0; i < maxlen; i++){
        if(str[i] == 0){
            return i;
        }
    }
    return -1;
}

int StringFromPacket(const uint8_t* data, char* copyto, int strpos, int maxlen){
    int strlen = maxlen > data[strpos] ? data[strpos] : maxlen;
    for(int i = 0; i < strlen; i++) {
        copyto[i] = (char)data[strpos + 1 + i];
    }
    copyto[maxlen - 1] = '\0';
    return strpos + strlen + 1;
}

int StringToPacket(const char* source, int strlen, uint8_t* dest, int pos, int maxlen){
    strlen = maxlen >= strlen + 1 ? strlen : maxlen - 1;
    // i trust you're giving me the correct length of the string
    dest[pos] = strlen;
    for(int i = 0; i < strlen; i++){
        dest[pos + i + 1] = source[i];
    }
    return pos + strlen + 1;
}