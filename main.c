#include "chip8.h"
#undef main

const int PIXEL_MULTIPLIER = 10;
const int SCREEN_WIDTH = 64 * 10;
const int SCREEN_HEIGHT = 32 * 10;

uint8_t keymap[16] =
{
    SDLK_x,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v,
};

bool init();
void drawScreen();
void close();
//The window we'll be rendering to
SDL_Window* window = NULL;
    
//The window renderer
SDL_Renderer* renderer = NULL;

Chip8 chip;

bool init()
{
    //Initialization flag
    bool success = true;

    if(SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;        
    }
    else
    {
        //Set texture filtering to linear
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
        {
            printf( "Warning: Linear texture filtering not enabled!" );
        }

        //Create Window
        window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if( window == NULL )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
        }
        else
        {
            //Create renderer for window
            renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
            if( renderer == NULL )
            {
                printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
            }
            
        }
        
    }
    
    return success;
}

void drawScreen()
{
    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            if(chip.gfx[(y*64) + x] != 0)
            {
                SDL_Rect fillRect = { x * PIXEL_MULTIPLIER , y * PIXEL_MULTIPLIER, PIXEL_MULTIPLIER, PIXEL_MULTIPLIER };
                SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0xFF );
                SDL_RenderFillRect( renderer, &fillRect );
            }
            else
            {
                SDL_Rect fillRect = { x * PIXEL_MULTIPLIER , y * PIXEL_MULTIPLIER, PIXEL_MULTIPLIER, PIXEL_MULTIPLIER };
                SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderFillRect( renderer, &fillRect );
            }
        }
        
    }
    SDL_RenderPresent(renderer);
    chip.drawFlag = false;
}

void close()
{
    //Destroy Window
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    window = NULL;
    renderer = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}






int main( int argc, char* args[] )
{
    cLoadApplication(args[1], &chip);
    //Start up SDL and create window
    if( !init() )
    {
        printf( "Failed to initialize!\n" );
    }
    else
    {
        bool quit = false;

        SDL_Event e;

        while ( !quit )
        {
            while( SDL_PollEvent( &e ) != 0 )
            {
                if(e.type == SDL_QUIT )
                {
                    quit = true;
                }
                else if(e.type == SDL_KEYDOWN)
                {
                    switch(e.key.keysym.sym)
                    {
                        case SDLK_x:
                            chip.key[0x0] = 1;
                            break;
                        case SDLK_1:
                            chip.key[0x1] = 1;
                            break;
                        case SDLK_2:
                            chip.key[0x2] = 1;
                            break;
                        case SDLK_3:
                            chip.key[0x3] = 1;
                            break;
                        case SDLK_q:
                            chip.key[0x4] = 1;
                            break;
                        case SDLK_w:
                            chip.key[0x5] = 1;
                            break;
                        case SDLK_e:
                            chip.key[0x6] = 1;
                            break;
                        case SDLK_a:
                            chip.key[0x7] = 1;
                            break;
                        case SDLK_s:
                            chip.key[0x8] = 1;
                            break;
                        case SDLK_d:
                            chip.key[0x9] = 1;
                            break;
                        case SDLK_z:
                            chip.key[0xA] = 1;
                            break;
                        case SDLK_c:
                            chip.key[0xB] = 1;
                            break;
                        case SDLK_4:
                            chip.key[0xC] = 1;
                            break;
                        case SDLK_r:
                            chip.key[0xD] = 1;
                            break;
                        case SDLK_f:
                            chip.key[0xE] = 1;
                            break;
                        case SDLK_v:
                            chip.key[0xF] = 1;
                            break;
                    }
                }
                else if(e.type == SDL_KEYUP)
                {
                    switch(e.key.keysym.sym)
                    {
                        case SDLK_x:
                            chip.key[0x0] = 0;
                            break;
                        case SDLK_1:
                            chip.key[0x1] = 0;
                            break;
                        case SDLK_2:
                            chip.key[0x2] = 0;
                            break;
                        case SDLK_3:
                            chip.key[0x3] = 0;
                            break;
                        case SDLK_q:
                            chip.key[0x4] = 0;
                            break;
                        case SDLK_w:
                            chip.key[0x5] = 0;
                            break;
                        case SDLK_e:
                            chip.key[0x6] = 0;
                            break;
                        case SDLK_a:
                            chip.key[0x7] = 0;
                            break;
                        case SDLK_s:
                            chip.key[0x8] = 0;
                            break;
                        case SDLK_d:
                            chip.key[0x9] = 0;
                            break;
                        case SDLK_z:
                            chip.key[0xA] = 0;
                            break;
                        case SDLK_c:
                            chip.key[0xB] = 0;
                            break;
                        case SDLK_4:
                            chip.key[0xC] = 0;
                            break;
                        case SDLK_r:
                            chip.key[0xD] = 0;
                            break;
                        case SDLK_f:
                            chip.key[0xE] = 0;
                            break;
                        case SDLK_v:
                            chip.key[0xF] = 0;
                            break;
                    }
                }
            }

            cEmulateCycle(&chip);
            if(chip.drawFlag)
            {
                drawScreen(&chip);
            }
            Sleep(1);
        }
        
    }

    close();

    return 0;
}