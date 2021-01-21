#include "chip8.h"
#undef main

const int PIXEL_MULTIPLIER = 10;
const int SCREEN_WIDTH_MODIFIER = 300;
const int SCREEN_WIDTH = 64 * 10 + 300;
const int SCREEN_HEIGHT = 32 * 10;
const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / 60;


int main( int argc, char* args[] );
bool init();
bool loadFromRenderedText(const char* textureText, SDL_Color textColor);
bool loadMedia();
void render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip);
void drawScreen();
void close();

//The window we'll be rendering to
SDL_Window* window = NULL;
    
//The window renderer
SDL_Renderer* renderer = NULL;

SDL_Texture* texture = NULL;

TTF_Font* font = NULL;

int width,height;

int countedFrames = 0;

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
        if( TTF_Init() == -1 )
        {
            printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
            success = false;
        }

        //Create Window
        window = SDL_CreateWindow( "Chip-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

bool loadFromRenderedText(const char* textureText, SDL_Color textColor)
{
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

void render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
    //Set rendering space and render to screen
    SDL_Rect renderQuad = { x, y, width, height };

    //Set clip rendering dimensions
    if( clip != NULL )
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    //Render to Screen
    SDL_RenderCopyEx( renderer, texture, clip, & renderQuad, angle, center, flip );
}

void drawScreen()
{
    //Clear Screen
    SDL_SetRenderDrawColor( renderer, 0x80, 0x80, 0x80, 0xFF );
    SDL_RenderClear( renderer );

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
    render( (SCREEN_WIDTH - SCREEN_WIDTH_MODIFIER), (SCREEN_HEIGHT - height)/2, NULL, 0.0, NULL,  SDL_FLIP_NONE );
    SDL_RenderPresent(renderer);
    countedFrames++;
    chip.drawFlag = false;
}

void close()
{
    //Destroy Window
    TTF_CloseFont( font );
    font = NULL;

    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    window = NULL;
    renderer = NULL;

    //Quit SDL subsystems
    TTF_Quit();
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
        if( !loadMedia() )
        {
            printf( "Failed to load media!\n" );
        }
        else
        {
            bool quit = false;
            Uint32 startTime = SDL_GetTicks();
            Uint32 frameTime = 0;
            char buffer[50];

            SDL_Event e;

            while ( !quit )
            {
                frameTime = SDL_GetTicks();
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
                    float avgFPS = countedFrames / ( (SDL_GetTicks() - startTime) / 1000.f);
                    if( avgFPS > 2000000 )
                    {
                        avgFPS = 0;
                    }
                    snprintf(buffer, 50, "FPS: %f", avgFPS);
                            //Render Text
                    SDL_Color textColor = {0, 0, 0};
                    if( !loadFromRenderedText(buffer, textColor ) )
                    {
                        printf("Failed to render text texture!\n" );
                    }
                    drawScreen(&chip);
                    //If frame finished early
                    int frameTicks = SDL_GetTicks() - frameTime;
                    if( frameTicks < SCREEN_TICKS_PER_FRAME)
                    {
                        //Wait Remaining time
                        SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks );
                    }
                }
                //Sleep(15);
            }
        }
        
    }

    close();

    return 0;
}