#ifndef CHIP8
#define CHIP8
#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<time.h>
#include<windows.h>
#include"external/SDL/include/SDL.h"
#include"external/SDL/include/SDL_ttf.h"


//Shortcuts for chip8 emulator
#define register chip->V
#define operation chip->opcode
#define progcount chip->pc
#define mem chip->memory
#define graphics chip->gfx
#define stxpntr chip->sp
#define stx chip->stack
#define addrreg chip->I
#define dlytmr chip->delay_timer
#define sndtmr chip->sound_timer

//Chip 8 emulator struct
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

//Chip 8 emulator functions
void cInitialize(Chip8* chip);
void cEmulateCycle(Chip8* chip);
bool cLoadApplication(const char* filename, Chip8* chip);
void cDecrementTimers(Chip8* chip);

#endif