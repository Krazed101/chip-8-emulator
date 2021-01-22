#include "chip8.h"
#undef main     //Needed because SDL2 defines main and confuses compiler

//Define Constants
#define PIXEL_MULTIPLIER 10                                             //The multiplier to expand the size of the pixels
#define SCREEN_WIDTH_MODIFIER 300                                       //The modifier for adding area for fps and future debugging panel
#define SCREEN_WIDTH (64 * PIXEL_MULTIPLIER + SCREEN_WIDTH_MODIFIER)    //The overall width of the window
#define SCREEN_HEIGHT (32 * PIXEL_MULTIPLIER)                           //The overall height of the window
#define SCREEN_FPS 60                                                   //The FPS desired
#define SCREEN_TICKS_PER_FRAME (1000 / SCREEN_FPS)                      //The ticks needed per frame

//Function Declerations
int main( int argc, char* args[] );
bool init();
bool loadFromRenderedText(const char* textureText, SDL_Color textColor);
bool loadMedia();
void render(int x, int y, double angle, SDL_Point* center, SDL_RendererFlip flip);
void drawScreen();
void close();

//SDL Structs
SDL_Window* window = NULL;              //The window we're rendering to
SDL_Renderer* renderer = NULL;          //The renderer that does the drawing
SDL_Texture* texture = NULL;            //The texture that gets rendered
TTF_Font* font = NULL;                  //Where we store the font

int width,height;                       //Used for rendering the text image

int countedFrames = 0;                  //The amount of frames the game has had

Chip8 chip;                             //The Chip8 struct


/*****************************************
 * Name:    init
 * Args:    Void
 * Return:  If initialization was succesful it returns true
 *          else it returns false
 * Desc:    Initializes SDL Components and creates the window
 *          object
 *****************************************/
bool init()
{
    //Initialize SDL
    if(SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		return false;        
    }
    //Set texture filtering to linear
    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
    {
        printf( "Warning: Linear texture filtering not enabled!" );
    }
    //Initialize TTF
    if( TTF_Init() == -1 )
    {
        printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
        return false;
    }

    //Create Window
    window = SDL_CreateWindow( "Chip-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if( window == NULL )
    {
        printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
        return false;
    }

    //Create renderer for window
    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
    if( renderer == NULL )
    {
        printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
        return false;
    }

    //Initialize renderer color
    SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );

    return true;
}

/*****************************************
 * Name:    loadFromRenderedText
 * Args:    const char* textureText:    The text to be printed
 *          SDL_Color   textColor:      The color for the text to be
 * Return:  returns true if the texture was created
 * Desc:    Takes a c str and converts into into a surface with the
 *          text in the font and then converts it into a texture
 *****************************************/
bool loadFromRenderedText(const char* textureText, SDL_Color textColor)
{
    //Create Suface and render the text onto it
    SDL_Surface* textSurface = TTF_RenderText_Solid( font, textureText, textColor);
    if( textSurface == NULL )
    {
        printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
    }
    else
    {
        //Create texture from surface pixels
        texture = SDL_CreateTextureFromSurface( renderer, textSurface );
        if( texture == NULL )
        {
            printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
        }
        else
        {
            //Get image dimensions
            width = textSurface->w;
            height = textSurface->h;
        }
        //Get rid of old surface
        SDL_FreeSurface( textSurface );
    }
    return texture != NULL;
}

/*****************************************
 * Name:    loadMedia
 * Args:    Void
 * Return:  returns true if the font was succesfully loaded
 * Desc:    Opens the font and saves to global variable to 
 *          be used
 *****************************************/
bool loadMedia()
{
    //Loading success flag
    bool success = true;

    //Open the font
    font = TTF_OpenFont( "Resources\\Roboto-Black.ttf", 20);
    if( font == NULL )
    {
        printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }

    return success;
}

/*****************************************
 * Name:    render
 * Args:    int x:                  The x position to render
 *          int y:                  The y position to render
 *          double angle:           Rotation of the texture
 *          SDL_Point* center:      Point that the object will be rotated around
 *          SDL_RendererFlip flip:  Used to flip the texture
 * Return:  VOID
 * Desc:    Renders the texture given
 *****************************************/
void render(int x, int y, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
    //Set rendering space and render to screen
    SDL_Rect renderQuad = { x, y, width, height };
    //Render to Screen
    SDL_RenderCopyEx( renderer, texture, NULL, & renderQuad, angle, center, flip );
}

/*****************************************
 * Name:    drawScreen
 * Args:    void
 * Return:  void
 * Desc:    draws the screen for the chip8 emulator
 *****************************************/
void drawScreen()
{
    //Clear Screen to gray
    SDL_SetRenderDrawColor( renderer, 0x80, 0x80, 0x80, 0xFF );
    SDL_RenderClear( renderer );

    //Iterate through everyposition on the chip gfx and if the pixel is on make it black otherwise make it white
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
    //Render the screen now that we've set all the values
    render( (SCREEN_WIDTH - SCREEN_WIDTH_MODIFIER), (SCREEN_HEIGHT - height)/2, 0.0f, NULL,  SDL_FLIP_NONE );
    //Present it to the user
    SDL_RenderPresent(renderer);
    //Bam another frame down
    countedFrames++;
    //Don't need to draw anymore
    chip.drawFlag = false;
}

/*****************************************
 * Name:    close
 * Args:    Void
 * Return:  Void
 * Desc:    Closes all SDL2 programs and Quits them cleanly
 *****************************************/
void close()
{
    //Close SDL2 Items
    TTF_CloseFont( font );
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    font = NULL;
    window = NULL;
    renderer = NULL;

    //Quit SDL subsystems
    TTF_Quit();
    SDL_Quit();
}

/*****************************************
 * Name:    main
 * Args:    int argc:       The number of args passed
 *          char* args[]:   The list of arguments
 * Return:  returns 0 on clean execution
 * Desc:    The main function of the program that controls
 *          everything happening
 *****************************************/
int main( int argc, char* args[] )
{   
    //Load the application given in the arguements
    if( !cLoadApplication(args[1], &chip) )
    {
        printf("Loading Rom Error!\n");
        close();
        return 1;
    }
    //Start up SDL and create window
    if( !init() )
    {
        printf( "Failed to initialize!\n" );
        close();
        return 1;
    }
    //Load the font or other images if we used them
    if( !loadMedia() )
    {
        printf( "Failed to load media!\n" );
        close();
        return 1;
    }

    //Variables for main loop
    bool quit = false;                      //Tells the loop when the user says to escape
    Uint32 startTime = SDL_GetTicks();      //Used to tell time since program start
    Uint32 counterTime = SDL_GetTicks();    //Used to to keep counters at 60 htz
    Uint32 frameTime = 0;                   //Used to tell how long a frame took
    char buffer[50];                        //Buffer to store the fps text to be printed

    //SDL_Event for key presses
    SDL_Event e;

    while ( !quit )
    {
        //frame has started grab time
        frameTime = SDL_GetTicks();

        //Checking for keyboard event
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

        //Emulate a cycle on our chip8
        cEmulateCycle(&chip);

        //Get the time and check if enough time has passed to decrement timers
        int counterTicks = SDL_GetTicks() - counterTime;
        if( counterTicks > SCREEN_TICKS_PER_FRAME)
        {
            //Save the time for the next time we need to check
            counterTime = SDL_GetTicks();
            //Decrement timers
            cDecrementTimers(&chip);
        }

        //Check to see if we need to draw to the screen
        if(chip.drawFlag)
        {
            //Check the average fps and print it into the buffer
            float avgFPS = countedFrames / ( (SDL_GetTicks() - startTime) / 1000.f);
            if( avgFPS > 2000000 )
            {
                avgFPS = 0;
            }
            snprintf(buffer, 50, "FPS: %f", avgFPS);
            //Set the color to black and send the text to get rendered
            SDL_Color textColor = {0, 0, 0};
            if( !loadFromRenderedText(buffer, textColor ) )
            {
                printf("Failed to render text texture!\n" );
            }
            //Draw to the screen
            drawScreen(&chip);

            //If frame finished early
            int frameTicks = SDL_GetTicks() - frameTime;
            if( frameTicks < SCREEN_TICKS_PER_FRAME)
            {
                //Wait Remaining time
                SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks );
            }
        }
    }
    //Close SDL2 items
    close();
    return 0;
}