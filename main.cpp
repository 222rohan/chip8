/*

    The main driver program for the chip8 emulator

    Takes "program.ch8" and runs it in the game loop

*/

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include "chip8.h"

#include<SDL2/SDL.h>

using namespace std;

#define MODE_VRB        000000001
#define MODE_SND        000000010
#define MODE_STP        000000100
#define PIX_ON_COLOR    0xbff9fff5    /* Pixel ON color value: ARGB                    */
#define PIX_OFF_COLOR   0xbf001e23    /* Pixel OFF color value: ARGB                   */
#define REFRESH_TIME    1300          /* Refresh time in milliseconds                  */

/* State of the machine, will be used for trace, and running. */
enum MACHINESTATE {EMU_ON, EMU_READY, EMU_RUN, EMU_STOP, EMU_OFF, EMU_UNDEF};
MACHINESTATE STATE = EMU_OFF; 

/* SDL2 paramters, window width and height. */
#define WIN_WD 960
#define WIN_HT 480
uint8_t keymap[16] = {
/*
    this is the keypad layout of CHIP8            mapped to keyboard
            1   2   3   C                           1   2   3   4
            4   5   6   D               ----->      Q   W   E   R
            7   8   9   E                           A   S   D   F
            A   0   B   F                           Z   X   C   V               
*/
    SDLK_x, SDLK_1, SDLK_2, SDLK_3,
    SDLK_q, SDLK_w, SDLK_e, SDLK_a,
    SDLK_s, SDLK_d, SDLK_z, SDLK_c,
    SDLK_4, SDLK_r, SDLK_f, SDLK_v,
};

struct STRUCT_SDL
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture *texture;
};


void    parse_commands(int, char*[], uint8_t*);
int     setup_rom(CHIP8*, char*, uint8_t);
int     setup_window(struct STRUCT_SDL*);
int     run_gameloop(CHIP8*, struct STRUCT_SDL*, int );
void    close_window(struct STRUCT_SDL*);

int main(int argc, char *argv[]) {
    STATE = EMU_ON;
    /*
        A byte which determines the various modes (see options)

        Default                                 (00000000).
        Verbose ON : right-most bit     ON      (00000001).
        Sound   OFF: 2nd from right bit ON.     (00000010).
    */
    uint8_t MODE = 0;
    parse_commands(argc,argv, &MODE);

    STRUCT_SDL sdl_setupvar;
    CHIP8 chip8_instance;
    
    if(setup_rom(&chip8_instance, argv[1], MODE) == -1) {
        cerr<<std::endl<<"could not open ROM file.";
        exit(1);
    }

    if(setup_window(&sdl_setupvar) == -1) {
        cerr<<std::endl<<"could not setup SDL2 window.";
        exit(1);
    }

    if(run_gameloop(&chip8_instance, &sdl_setupvar, REFRESH_TIME) == -1) {
        cerr <<"error running game loop.";
    }
    close_window(&sdl_setupvar);

    return 0;
}

void parse_commands(int argc, char* argv[], uint8_t *MODE){
    if(argc < 2) {
        cout<<"usage: ./chip8 <rom> <-options[hvac]>"<<endl;
        exit(0);
    }

    if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "help") == 0) {
        cout<<"usage: ./chip8 <rom> <-options[hvac]>"<<endl;
        cout<<"options:"<<endl;
        cout<<"\t-h : shows this message."<<endl;
        cout<<"\t-v : verbose mode, shows interal trace."<<endl;
        cout<<"\t-a : disables audio."<<endl;
        cout<<"\t-c : displays controls."<<endl;
        exit(0);
    }

    if(argc >= 3 && argv[2][0] == '-'){
        bool option_correct = false;
        string options = argv[2];

        if(options.find("h") != string::npos){
            cout<<"usage: ./chip8 <rom> <-options[hvac]>"<<endl;
            cout<<"options:"<<endl;
            cout<<"\t-h : shows this message."<<endl;
            cout<<"\t-v : verbose mode, shows interal trace."<<endl;
            cout<<"\t-a : disables audio."<<endl;
            cout<<"\t-c : displays controls."<<endl;
            cout<<endl;
            option_correct = true;
        }

        if(options.find("v") != string::npos) {
            cout<<"VERBOSE(trace) mode is ON."<<endl;
            *MODE |= MODE_VRB;
            option_correct = true;
        }
    
        if(options.find("a") != string::npos) {
            cout<<"AUDIO is OFF."<<endl;
            *MODE |= MODE_SND;
            option_correct = true;
        }
    
        if(options.find("c") != string::npos) {
            cout << "CONTROLS:";
            option_correct = true;
        }

        if(!option_correct) {
            cout << "invalid option. check valid options using ./chip -h"<<endl;
            return;
        }
    }
    
}

int setup_rom(CHIP8 *chip8_instance, char *rom, uint8_t MODE) {
    //for now call load_rom directly, add fancy path checkers later
    bool sound = false;
    bool verbose = false;
    bool step = false;

    if(MODE & MODE_SND) {
        sound = true;
    }
    if(MODE & MODE_VRB) {
        verbose = true;
    }
    if(MODE & MODE_STP) {
        step = true;
    }

    return chip8_instance->load_rom(rom, sound, verbose, step);
}

int setup_window(struct STRUCT_SDL* sdl_setupvar) {
    if ( SDL_Init( SDL_INIT_EVERYTHING ) < 0 ) {
        cerr << "Error initializing SDL: " << SDL_GetError() << endl;
        system("pause");
        return -1;
    } 

    sdl_setupvar->window = SDL_CreateWindow( "CHIP8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIN_WD, WIN_HT, SDL_WINDOW_SHOWN );

	if ( !sdl_setupvar->window ) {
		cout << "Error creating window: " << SDL_GetError()  << endl;
		system("pause");
		return -1;
	}

    sdl_setupvar->renderer = SDL_CreateRenderer( sdl_setupvar->window, -1, 0 );
	if ( !sdl_setupvar->renderer ) {
		cout << "Error creating renderer: " << SDL_GetError() << endl;
		return -1;
	}

    SDL_RenderSetLogicalSize(sdl_setupvar->renderer, WIN_WD, WIN_HT);

    sdl_setupvar->texture = SDL_CreateTexture(sdl_setupvar->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, MAX_WIDTH, MAX_HEIGHT);
    if (sdl_setupvar->texture == NULL)
    {
        std::cerr << "Error in setting up texture " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }
    return 0;
}

int run_gameloop(CHIP8 *chip8_instance, struct STRUCT_SDL* sdl_setupvar, int refresh_time) {
    while(STATE == EMU_ON){
        if(chip8_instance->cycle() == -1) {
            cerr << "Error in CHIP8 cycle.";
            return -1;
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT){
                STATE = EMU_OFF;
                return 0;
            }

            if(event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    STATE = EMU_OFF;
                    return 0;
                }

                for (int i = 0; i < MAX_KEYCOUNT; i++)
                {
                    if (event.key.keysym.sym == keymap[i])
                    {
                        chip8_instance->set_key(i, KEY_DOWN);
                    }
                }                
            }

            if(event.type == SDL_KEYUP) {
                for (int i = 0; i < MAX_KEYCOUNT; i++)
                {
                    if (event.key.keysym.sym == keymap[i])
                    {
                        chip8_instance->set_key(i, KEY_UP);
                    }
                } 
            }
        }
        /*
            Update screen if drawflag is set.
        */
        if(chip8_instance->get_drawflag() == true) {
            uint32_t video_buffer[MAX_DISPSIZE];
            for(int i=0; i<MAX_DISPSIZE; i++){
                if(chip8_instance->get_pixel(i) == PIX_ON) {
                    video_buffer[i] = PIX_ON_COLOR;
                } else {
                    video_buffer[i] = PIX_OFF_COLOR;
                }
            }
            
            SDL_UpdateTexture(sdl_setupvar->texture, NULL, &video_buffer, MAX_WIDTH * sizeof(uint32_t));
            SDL_RenderClear(sdl_setupvar->renderer);
            SDL_RenderCopy(sdl_setupvar->renderer, sdl_setupvar->texture , NULL, NULL);
            SDL_RenderPresent(sdl_setupvar->renderer);

            chip8_instance->set_drawflag(false);
        }

        usleep(refresh_time);

        if(chip8_instance->get_STP() == true) {
            std::string temp;
            getline(std::cin, temp);
            if (temp[0] == 27) //esc
            {
                //STATE = EMU_OFF
            }
        }
    }

    return 0;
}
void close_window(struct STRUCT_SDL* sdl_setupvar) {
    SDL_DestroyTexture(sdl_setupvar->texture);
    SDL_DestroyRenderer(sdl_setupvar->renderer);
    SDL_DestroyWindow(sdl_setupvar->window);

    sdl_setupvar->texture = NULL;
    sdl_setupvar->renderer = NULL;
    sdl_setupvar->window = NULL;

    SDL_Quit();
    return;
}