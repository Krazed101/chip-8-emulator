#include "chip8.h"


unsigned char chip8_fontset[80] =
{ 
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

void cInitialize(Chip8* chip)
{
    chip->pc = 0x0200;
    chip->I = 0;
    operation = 0;
    chip->sp = 0;

    //Initialize Display
    for (int i = 0; i < 2048; i++)
    {
        chip->gfx[i] = 0;
    }

    //Initialize stack
    for(int i = 0; i < 16; i++)
    {
        chip->stack[i] = 0;
    }

    //Initialize registers
    for (int i = 0; i < 16; i++)
    {
        chip->V[i] = 0;
    }

    //Initialize memory
    for (int i = 0; i < 4096; i++)
    {
        chip->memory[i] = 0;
    }

    //Load Font
    for (int i = 0; i < 80; i++)
    {
        chip->memory[i] = chip8_fontset[i];
    }
    

    //Reset timers
    chip->delay_timer = 0;
    chip->sound_timer = 0;

    srand((unsigned) time(NULL));

    printf("Chip-8 Initialized\n");
}

void cEmulateCycle(Chip8* chip)
{
    //fetching the opcode
    operation = chip->memory[chip->pc] << 8 | chip->memory[chip->pc + 1];

    //decode the opcode
    switch(operation & 0xF000)
    {
        case 0x0000:
            switch(operation &  0x000F)
            {
                case 0x0000:
					for(int i = 0; i < 2048; ++i)
						chip->gfx[i] = 0x0;
					chip->drawFlag = true;
					chip->pc += 2;
				    break;

                case 0x000E:
                    --chip->sp;
                    chip->pc = chip->stack[chip->sp];
                    chip->pc += 2;
                    break;
                
                default:
                    printf("Unkown opcode [0x0000]: 0x%X\n",operation);
            }
            break;

        case 0x1000:
            chip->pc = operation & 0x0FFF;
            break;

        case 0x2000:
            chip->stack[chip->sp] = chip->pc;
            ++chip->sp;
            chip->pc = operation & 0x0FFF;
            break;

        case 0x3000:
            if (register[(operation & 0x0F00) >> 8] == (operation & 0x00FF))
            {
                chip->pc += 4;
            }
            else
            {
                chip->pc += 2;
            }
            break;

        case 0x4000:
            if (register[(operation & 0x0F00) >> 8] != (operation & 0x00FF))
            {
                chip->pc += 4;
            }
            else
            {
                chip->pc += 2;
            }
            break;

        case 0x5000:
            if (register[(operation & 0x0F00) >> 8] == register[(operation & 0x00F0) >> 4])
            {
                chip->pc += 4;
            }
            else
            {
                chip->pc += 2;
            }
            break;
            
        case 0x6000:
            register[(operation & 0x0F00) >> 8] = operation & 0x00FF;
            chip->pc += 2;
            break;

        case 0x7000:
            register[(operation & 0x0F00) >> 8] += (operation & 0x00FF);
            chip->pc += 2;
            break;

        case 0x8000:
            switch(operation & 0x000F)
            {
                case 0x0000:
                    register[(operation & 0x0F00) >> 8] = register[(operation & 0x00F0) >> 4];
                    chip->pc += 2;
                    break;

                case 0x0001:
                    register[(operation & 0x0F00) >> 8] = register[(operation & 0x0F00) >> 8] | register[(operation & 0x00F0) >> 4];
                    chip->pc += 2;
                    break;

                case 0x0002:
                    register[(operation & 0x0F00) >> 8] = register[(operation & 0x0F00) >> 8] & register[(operation & 0x00F0) >> 4];
                    chip->pc += 2;
                    break;

                case 0x0003:
                    register[(operation & 0x0F00) >> 8] = register[(operation & 0x0F00) >> 8] ^ register[(operation & 0x00F0) >> 4];
                    chip->pc += 2;
                    break;

                case 0x0004:
                    if(register[(operation & 0x00F0) >> 4] > (0xFF - register[(operation & 0x0F00) >> 8]))
                    {
                        register[0xF] = 1;//carry
                    }
                    else
                    {
                        register[0xF] = 0;
                    }
                    register[(operation & 0x0F00) >> 8] += register[(operation & 0x00F0) >> 4];
                    chip->pc += 2;
                    break;

                case 0x0005:
                    if(register[(operation & 0x00F0) >> 4] > (0xFF - register[(operation & 0x0F00) >> 8]))
                    {
                        register[0xF] = 0;//carry
                    }
                    else
                    {
                        register[0xF] = 1;
                    }
                    register[(operation & 0x0F00) >> 8] -= register[(operation & 0x00F0) >> 4];
                    chip->pc += 2;
                    break;

                case 0x0006:
                    register[0xF] = register[(operation & 0x0F00) >> 8] * 0x1;
                    register[(operation & 0x0F00) >> 8] >>= 1;
                    chip->pc += 2;
                    break;

                case 0x0007:
                    if(register[(operation & 0x0F00) >> 8] > register[(operation & 0x00F0) >> 4])
                    {
                        register[0xF] = 0;//carry
                    }
                    else
                    {
                        register[0xF] = 1;
                    }
                    register[(operation & 0x0F00) >> 8] = register[(operation & 0x00F0) >> 4] - register[(operation & 0x0F00) >> 8];
                    chip->pc += 2;
                    break;

                case 0x000E:
                    register[0xF] = register[(operation & 0x0F00) >> 8] >> 7;
                    register[(operation & 0x0F00) >> 8] <<= 1;
                    chip->pc += 2;
                    break;

                default:
                    printf("Unkown opcode [0x8000]: 0x%X\n",operation);
                    break;
            }
            break;

        case 0x9000:
            if (register[(operation & 0x0F00) >> 8] != register[(operation & 0x00F0) >> 4])
            {
                chip->pc += 4;
            }
            else
            {
                chip->pc += 2;
            }
            break;

        case 0xA000:
            chip->I = operation & 0x0FFF;
            chip->pc += 2;
            break;

        case 0xB000:
            chip->pc = operation & 0x0FFF + register[0];
            chip->pc += 2;
            break;

        case 0xC000:
            register[(operation & 0x0F00) >> 8] = (rand() & 0xFF) & (operation & 0x00FF);
            chip->pc += 2;
            break;

        case 0xD000:
            unsigned short x = register[(operation & 0x0F00) >> 8];
            unsigned short y = register[(operation & 0x00F0) >> 4];
            unsigned short height = operation & 0x000F;
            unsigned short pixel;

            register[0xF] = 0;
            for (int yline = 0; yline < height; yline++)
            {
                pixel = chip->memory[chip->I + yline];
                for (int xline = 0; xline < 8; xline++)
                {
                    if((pixel & (0x80 >> xline)) != 0)
                    {
                        if(chip->gfx[(x + xline + ((y + yline) * 64))] == 1)
                        {
                            register[0xF] = 1;
                        }
                        chip->gfx[(x + xline + ((y + yline) * 64))] ^= 1;
                    }
                }
            }

            chip->drawFlag = true;
            chip->pc +=2;
            break;

        case 0xE000:
            switch (operation & 0x00FF)
            {
                case 0x009E:
                    if(chip->key[register[(operation & 0x0F00) >> 8]] != 0)
                    {
                        chip->pc += 4;
                    }
                    else
                    {
                        chip->pc += 2;
                    }
                    break;

                case 0x00A1:
                    if(chip->key[register[(operation & 0x0F00) >> 8]] == 0)
                    {
                        chip->pc += 4;
                    }
                    else
                    {
                        chip->pc += 2;
                    }
                    break;
            
                default:
                    printf("Unkown opcode [0xE000]: 0x%X\n",operation);
                    break;
            }
            break;

        case 0xF000:
            switch(operation & 0x00FF)
            {
                case 0x0007:
                    register[(operation & 0x0F00) >> 8] = chip->delay_timer;
                    chip->pc += 2;
                    break;

                case 0x000A:
                    bool keyPress = false;

					for(int i = 0; i < 16; ++i)
					{
						if(chip->key[i] != 0)
						{
							register[(operation & 0x0F00) >> 8] = i;
							keyPress = true;
						}
					}

					// If we didn't received a keypress, skip this cycle and try again.
					if(!keyPress)						
						return;

					chip->pc += 2;
                    break;

                case 0x0015:
                    chip->delay_timer = register[(operation & 0x0F00) >> 8];
                    chip->pc += 2;
                    break;

                case 0x0018:
                    chip->sound_timer = register[(operation & 0x0F00) >> 8];
                    chip->pc += 2;
                    break;

                case 0x001E:
                    chip->I += register[(operation & 0x0F00) >> 8];
                    chip->pc += 2;
                    break;

                case 0x0029:
                    chip->I = register[(operation & 0x0F00) >> 8] * 0x5;
                    chip->pc += 2;
                    break;

                case 0x0033:
                    chip->memory[chip->I] = register[(operation & 0x0F00) >> 8] / 100;
					chip->memory[chip->I + 1] = (register[(operation & 0x0F00) >> 8] / 10) % 10;
					chip->memory[chip->I + 2] = (register[(operation & 0x0F00) >> 8] % 100) % 10;					
					chip->pc += 2;
                    break;
                
                case 0x0055:
                	for (int i = 0; i <= ((operation & 0x0F00) >> 8); i++)
						chip->memory[chip->I + i] = register[i];	

					// On the original interpreter, when the operation is done, I = I + X + 1.
					chip->I += ((operation & 0x0F00) >> 8) + 1;
					chip->pc += 2;
				    break;

                case 0x0065:
                    for (int i = 0; i <= ((operation & 0x0F00) >> 8); i++)
						register[i] = chip->memory[chip->I + i];			

					// On the original interpreter, when the operation is done, I = I + X + 1.
					chip->I += ((operation & 0x0F00) >> 8) + 1;
					chip->pc += 2;
				    break;
            }
            break;


        default:
            printf("Unkown opcode: 0x%X\n",operation);
    }

    //Update Timers
    if(chip->delay_timer > 0)
    {
        --chip->delay_timer;
    }
    if(chip->sound_timer > 0)
    {
        if(chip->sound_timer == 1)
        {
            Beep(500,800);
            //printf("Beep!");
        }
        --chip->sound_timer;
    }
    //printf("Emulation Cycle Complete\n");
}

bool cLoadApplication(const char* filename, Chip8* chip)
{
    cInitialize(chip);
    printf("Loading: %s\n", filename);

    //open file
    FILE* pFile = fopen(filename, "rb");
    if(pFile == NULL)
    {
        printf("File Error");
        return false;
    }

    //Check file size
    fseek(pFile, 0, SEEK_END);
    long lSize = ftell(pFile);
    rewind(pFile);
    printf("Filesize: %d\n", (int)lSize);

    //Allocate memory to contain the whole file
    char * buffer = (char *)malloc(sizeof(char) * lSize);
    if(buffer == NULL)
    {
        printf("Memory Error\n");
        return false;
    }

    //Copy the file into the buffer
    size_t result = fread(buffer, 1, lSize, pFile);
    if(result != lSize)
    {
        printf("Reading error\n");
        return false;
    }

    //Copy buffer to Chip8 memory
    if((4096-512) > lSize)
    {
        for (int i = 0; i < lSize; i++)
        {
            chip->memory[i + 512] = buffer[i];
        }
    }
    else
    {
        printf("Error: ROM too big for memory");
    }

    fclose(pFile);
    free(buffer);

    return true;
}