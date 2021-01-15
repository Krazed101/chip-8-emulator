#include "chip8.h"
#undef main

const int PIXEL_MULTIPLIER = 10;
const int SCREEN_WIDTH = 64 * 10;
const int SCREEN_HEIGHT = 32 * 10;

bool init();
void drawScreen();
void close();

enum KeyPressSurfaces
{
    KEY_PRESS_SURFACE_DEFAULT,
    KEY_PRESS_SURFACE_UP,
    KEY_PRESS_SURFACE_DOWN,
    KEY_PRESS_SURFACE_LEFT,
    KEY_PRESS_SURFACE_RIGHT,
    KEY_PRESS_SURFACE_TOTAL
};

//The window we'll be rendering to
SDL_Window* window = NULL;
    
//The window renderer
SDL_Renderer* renderer = NULL;

SDL_Surface* keyPressSurfaces[ KEY_PRESS_SURFACE_TOTAL ];
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

                SDL_RenderPresent( renderer );
            }
            else
            {
                SDL_Rect fillRect = { x * PIXEL_MULTIPLIER , y * PIXEL_MULTIPLIER, PIXEL_MULTIPLIER, PIXEL_MULTIPLIER };
                SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderFillRect( renderer, &fillRect );

                SDL_RenderPresent( renderer );
            }
        }
        
    }
    
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
            }

            cEmulateCycle(&chip);
            if(chip.drawFlag)
            {
                drawScreen(&chip);
            }
        }
        
    }

    close();

    return 0;
}