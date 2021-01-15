#ifndef CHIP8
#define CHIP8
#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<time.h>
#include"external/SDL/include/SDL.h"
#pragma comment( lib, "SDL2.lib")

#define register chip->V

typedef struct chip8
{
    unsigned short opcode;
    unsigned char memory[4096];
    unsigned char V[16];
    unsigned short I;
    unsigned short pc;
    unsigned char delay_timer;
    unsigned char sound_timer;
    unsigned short stack[16];
    unsigned short sp;
    unsigned char key[16];
    unsigned char gfx[64 * 32];
    bool drawFlag;
}Chip8;

void cInitialize(Chip8* chip);
void cEmulateCycle(Chip8* chip);

#endif