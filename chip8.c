#include "chip8.h"


//The Chip8 fontset
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


/*****************************************
 * Name:    cInitialize
 * Args:    Chip8* chip:    The Chip8 object being initialized
 * Return:  Void
 * Desc:    Initializes the Chip8 object to prepare for emulation
 *****************************************/
void cInitialize(Chip8* chip)
{
    //Initialize variables
    progcount = 0x0200;     //Initialize to 0x0200 which is where .ch8 files start in memory
    addrreg = 0;
    operation = 0;
    stxpntr = 0;
    dlytmr = 0;
    sndtmr = 0;


    //Initialize Display
    for (int i = 0; i < 2048; i++)
    {
        graphics[i] = 0;
    }

    //Initialize stack
    for(int i = 0; i < 16; i++)
    {
        stx[i] = 0;
    }

    //Initialize registers
    for (int i = 0; i < 16; i++)
    {
        register[i] = 0;
    }

    //Initialize memory
    for (int i = 0; i < 4096; i++)
    {
        mem[i] = 0;
    }

    //Load Font
    for (int i = 0; i < 80; i++)
    {
        mem[i] = chip8_fontset[i];
    }
    //Initialize rand
    srand((unsigned) time(NULL));
}


/*****************************************
 * Name:    cEmulateCycle
 * Args:    Chip8* chip:    The chip that is being emulated
 * Return:  Void
 * Desc:    Emulates a clock cycle by getting the next opcode
 *          parsing it and performing the said operation
 *****************************************/
void cEmulateCycle(Chip8* chip)
{
    //fetching the opcode
    operation = mem[progcount] << 8 | mem[progcount + 1];

    //decode the opcode
    switch(operation & 0xF000)
    {
        case 0x0000:
            switch(operation &  0x000F)
            {
                //Clears the Screen
                case 0x0000:
                    //Zero out all the graphics
					for(int i = 0; i < 2048; ++i)
						graphics[i] = 0x0;
                    //Set the draw flag
					chip->drawFlag = true;
					progcount += 2;
				    break;

                //Return from subroutine
                case 0x000E:
                    //Decrement stack pointer
                    stxpntr--;
                    //Set program counter to the previous address
                    progcount = stx[stxpntr];
                    progcount += 2;
                    break;
                
                default:
                    printf("Unkown opcode [0x0000]: 0x%X\n",operation);
            }
            break;

        //Jumps to address 0x0NNN
        case 0x1000:
            progcount = operation & 0x0FFF;
            break;

        //Calls subroutine at 0x0NNN
        case 0x2000:
            //Save location of program on the stack
            stx[stxpntr] = progcount;
            //Increment the stack pointer
            stxpntr++;
            //Go to the subroutine
            progcount = operation & 0x0FFF;
            break;

        //Skips next instruction if register 0x0N00 equals 0x00NN
        case 0x3000:
            if (register[(operation & 0x0F00) >> 8] == (operation & 0x00FF))
            {
                progcount += 4;
            }
            else
            {
                progcount += 2;
            }
            break;

        //Skips next instruction if register 0x0N00 deosn't equal 0x00NN
        case 0x4000:
            if (register[(operation & 0x0F00) >> 8] != (operation & 0x00FF))
            {
                progcount += 4;
            }
            else
            {
                progcount += 2;
            }
            break;

        //Skips next instruction if register 0x0N00 equals register 0x00N0
        case 0x5000:
            if (register[(operation & 0x0F00) >> 8] == register[(operation & 0x00F0) >> 4])
            {
                progcount += 4;
            }
            else
            {
                progcount += 2;
            }
            break;

        //Sets register 0x0N00 to 0x00NN   
        case 0x6000:
            register[(operation & 0x0F00) >> 8] = operation & 0x00FF;
            progcount += 2;
            break;

        //Adds 0x00NN to register 0x0N00, No carry flag
        case 0x7000:
            register[(operation & 0x0F00) >> 8] += (operation & 0x00FF);
            progcount += 2;
            break;

        //Register Math and Bit operations
        case 0x8000:
            switch(operation & 0x000F)
            {
                //Register 0x0N00 equals register 0x00N0
                case 0x0000:
                    register[(operation & 0x0F00) >> 8] = register[(operation & 0x00F0) >> 4];
                    progcount += 2;
                    break;

                //Bitwise or register 0x0N00 and register 0x00N0 stored in register 0x0N00
                case 0x0001:
                    register[(operation & 0x0F00) >> 8] = register[(operation & 0x0F00) >> 8] | register[(operation & 0x00F0) >> 4];
                    progcount += 2;
                    break;

                //Bitwise and register 0x0N00 and register 0x00N0 stored in register 0x0N00
                case 0x0002:
                    register[(operation & 0x0F00) >> 8] = register[(operation & 0x0F00) >> 8] & register[(operation & 0x00F0) >> 4];
                    progcount += 2;
                    break;

                //Bitwise exclusive or register 0x0N00 and register 0x00N0 stored in register 0x0N00
                case 0x0003:
                    register[(operation & 0x0F00) >> 8] = register[(operation & 0x0F00) >> 8] ^ register[(operation & 0x00F0) >> 4];
                    progcount += 2;
                    break;

                //Adds register 0x00N0 to register 0x0N00 and sets the carry flag when needed
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
                    progcount += 2;
                    break;

                //Subtracts register 0x00N0 from register 0x0N00 and sets the carry flag when needed
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
                    progcount += 2;
                    break;

                //BitShift right 1 on register 0x0N00
                case 0x0006:
                    register[0xF] = register[(operation & 0x0F00) >> 8] * 0x1;
                    register[(operation & 0x0F00) >> 8] >>= 1;
                    progcount += 2;
                    break;

                //Sets register 0x0N00 to register 0x00N0 minus register 0x0N00, sets carry flag if there's a borrow
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
                    progcount += 2;
                    break;

                //BitShift left 1 on register 0x0N00 and stores the bit in the carry register
                case 0x000E:
                    register[0xF] = register[(operation & 0x0F00) >> 8] >> 7;
                    register[(operation & 0x0F00) >> 8] <<= 1;
                    progcount += 2;
                    break;

                default:
                    printf("Unkown opcode [0x8000]: 0x%X\n",operation);
                    break;
            }
            break;

        //Skips the next instruction if register 0x0N00 doesn't equal register 0x00N0
        case 0x9000:
            if (register[(operation & 0x0F00) >> 8] != register[(operation & 0x00F0) >> 4])
            {
                progcount += 4;
            }
            else
            {
                progcount += 2;
            }
            break;

        //Sets I(address register) to the address 0x0NNN
        case 0xA000:
            addrreg = operation & 0x0FFF;
            progcount += 2;
            break;

        //Jumps to the address 0x0NNN + register[0]
        case 0xB000:
            progcount = operation & 0x0FFF + register[0];
            progcount += 2;
            break;

        //Sets register 0x0N00 to a bitwise and of rand umber and 0x00NN
        case 0xC000:
            register[(operation & 0x0F00) >> 8] = (rand() & 0xFF) & (operation & 0x00FF);
            progcount += 2;
            break;

        //Draw sprite at coordinate (register 0x0N00, register 0x00N0) and sets the correct pixels
        case 0xD000:
            unsigned short x = register[(operation & 0x0F00) >> 8];
            unsigned short y = register[(operation & 0x00F0) >> 4];
            unsigned short height = operation & 0x000F;
            unsigned short pixel;

            register[0xF] = 0;
            for (int yline = 0; yline < height; yline++)
            {
                pixel = mem[addrreg + yline];
                for (int xline = 0; xline < 8; xline++)
                {
                    if((pixel & (0x80 >> xline)) != 0)
                    {
                        if(graphics[(x + xline + ((y + yline) * 64))] == 1)
                        {
                            register[0xF] = 1;
                        }
                        graphics[(x + xline + ((y + yline) * 64))] ^= 1;
                    }
                }
            }

            chip->drawFlag = true;
            progcount +=2;
            break;

        //Key Operations
        case 0xE000:
            switch (operation & 0x00FF)
            {
                //Skips the next instruction if the key stored in register 0x0N00 is pressed
                case 0x009E:
                    if(chip->key[register[(operation & 0x0F00) >> 8]] != 0)
                    {
                        progcount += 4;
                    }
                    else
                    {
                        progcount += 2;
                    }
                    break;

                //Skips the next isntruction if the key stored in register 0x0N00 isn't pressed
                case 0x00A1:
                    if(chip->key[register[(operation & 0x0F00) >> 8]] == 0)
                    {
                        progcount += 4;
                    }
                    else
                    {
                        progcount += 2;
                    }
                    break;
            
                default:
                    printf("Unkown opcode [0xE000]: 0x%X\n",operation);
                    break;
            }
            break;

        //Misc operations
        case 0xF000:
            switch(operation & 0x00FF)
            {
                //Sets register 0x0N00 to the value of delay timer
                case 0x0007:
                    register[(operation & 0x0F00) >> 8] = dlytmr;
                    progcount += 2;
                    break;

                //BLOCKING: awaits a key that is then stored in register 0x0N00
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

					progcount += 2;
                    break;
                //Sets the delay timer to register 0x0N00
                case 0x0015:
                    dlytmr = register[(operation & 0x0F00) >> 8];
                    progcount += 2;
                    break;

                //Sets the sound timer to register 0x0N00
                case 0x0018:
                    sndtmr = register[(operation & 0x0F00) >> 8];
                    progcount += 2;
                    break;

                //Adds register 0x0N00 to I(memory address)
                case 0x001E:
                    addrreg += register[(operation & 0x0F00) >> 8];
                    progcount += 2;
                    break;

                //Sets I(memory address) to the location of the sprite for char in register 0x0N00
                case 0x0029:
                    addrreg = register[(operation & 0x0F00) >> 8] * 0x5;
                    progcount += 2;
                    break;

                //Stores the binary coded decimal representation of register 0x0N00
                case 0x0033:
                    mem[addrreg] = register[(operation & 0x0F00) >> 8] / 100;
					mem[addrreg + 1] = (register[(operation & 0x0F00) >> 8] / 10) % 10;
					mem[addrreg + 2] = (register[(operation & 0x0F00) >> 8] % 100) % 10;					
					progcount += 2;
                    break;
                
                //Stores registers 0 to register 0x0N00 in memory starting at address I(memory address)
                case 0x0055:
                	for (int i = 0; i <= ((operation & 0x0F00) >> 8); i++)
						mem[addrreg + i] = register[i];	

					// On the original interpreter, when the operation is done, I = I + X + 1.
					addrreg += ((operation & 0x0F00) >> 8) + 1;
					progcount += 2;
				    break;

                //Fills register 0 to register 0x0N00 with values from memory starting at address I(memory address)
                case 0x0065:
                    for (int i = 0; i <= ((operation & 0x0F00) >> 8); i++)
						register[i] = mem[addrreg + i];			

					// On the original interpreter, when the operation is done, I = I + X + 1.
					addrreg += ((operation & 0x0F00) >> 8) + 1;
					progcount += 2;
				    break;
            }
            break;

        default:
            printf("Unkown opcode: 0x%X\n",operation);
    }
}

/*****************************************
 * Name:    cDecrementTimers
 * Args:    Chip8* chip:    The chip that the timers need decremented on
 * Return:  Void
 * Desc:    Decrements the delay_timer and sound_timer if they need to be
 *          decremented
 *****************************************/
void cDecrementTimers(Chip8* chip)
{
    //Update Timers
    if(dlytmr > 0)
    {
        dlytmr--;
    }
    if(sndtmr > 0)
    {
        if(sndtmr == 1)
        {
            //find a way to beep that doesn't slow the program down
        }
        sndtmr--;
    }
}

/*****************************************
 * Name:    cLoadApplication
 * Args:    const char* fileName:   The .ch8 file to be loaded into memory
 *          Chip8* chip:            the chip8 emulator that the file will be loaded into
 * Return:  true on success
 * Desc:    Loads a .ch8 file into emulator memory to be ran
 *****************************************/
bool cLoadApplication(const char* filename, Chip8* chip)
{
    //Initialize the chip
    cInitialize(chip);

    //Open the file in Read Bytes mode
    FILE* pFile = fopen(filename, "rb");
    if(pFile == NULL)
    {
        printf("File Error");
        return false;
    }

    //Check File size
    fseek(pFile, 0, SEEK_END);
    long lSize = ftell(pFile);
    rewind(pFile);

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
            mem[i + 512] = buffer[i];
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