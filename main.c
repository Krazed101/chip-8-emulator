#include "chip8.h"

const int PIXEL_MULTIPLIER = 10;
const int SCREEN_WIDTH = 64 * 10;
const int SCREEN_HEIGHT = 32 * 10;

bool init();
bool close();


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
        
    }
    
}







int wmain ()
{


    if( SDL_Init( SDL_INIT_VIDEO ) < 0)
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        printf( "SDL Initialized\n");
        //Create Window
        window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if( window == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        }
        else
        {
            printf("SDL Window Created\n");
            //Get window surface
            screenSurface = SDL_GetWindowSurface( window);

            //Fill the surface white
            SDL_FillRect( screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

            //Update the surface
            SDL_UpdateWindowSurface(window);

            printf("SDL Window Drawn\n");

            //Wait two seconds
            SDL_Delay( 5000 );

            SDL_Rect fillRect = { SCREEN_WIDTH/4, SCREEN_HEIGHT/4, SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
            SDL_SetRenderDrawColor( renderer, 0xFF, 0x00, 0x00, 0xFF );
            SDL_RenderFillRect( renderer, &fillRect );
            SDL_RenderPresent( renderer );

            SDL_Delay( 5000 );

            printf("SDL Window Slept");
        }
    }
    

    cInitialize(&chip);
    cEmulateCycle(&chip);

    //Destroy Window
    SDL_DestroyWindow( window );
    SDL_DestroyRenderer( renderer );
    printf("SDL Window Destroyed");
    
    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}