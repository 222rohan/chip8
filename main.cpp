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

#define MODE_VRB 000000001
#define MODE_SND 000000010

/* State of the machine, will be used for trace, and running. */
enum MACHINESTATE {EMU_ON, EMU_READY, EMU_RUN, EMU_STOP, EMU_OFF, EMU_UNDEF};
MACHINESTATE STATE = EMU_OFF; 

/* SDL2 paramters, window width and height. */
#define WIN_WD 1280
#define WIN_HT 640
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

void    parse_commands(int, char*[], uint8_t*);
int     setup_rom(CHIP8*, char*, uint8_t);
int     setup_window();
int     exit_emu(int );

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

    CHIP8 chip8_instance;
    
    if(setup_rom(&chip8_instance, argv[1], MODE) == -1) {
        cerr<<std::endl<<"could not open ROM file.";
        exit(1);
    }

    if(setup_window() == -1) {
        cerr<<std::endl<<"could not setup SDL2 window.";
        exit(1);
    }

    /*
        SETUP WINDOW using SDL2
        SDL window, Pointers
        References: 1. https://thenumb.at/cpp-course/index.html#sdl
                    2. https://lazyfoo.net/tutorials/SDL/
    */
    SDL_Window* window = NULL;
    SDL_Renderer* renderer;
    SDL_Texture *texture;

    if ( SDL_Init( SDL_INIT_EVERYTHING ) < 0 ) {
        cerr << "Error initializing SDL: " << SDL_GetError() << endl;
        system("pause");
        return -1;
    } 

    window = SDL_CreateWindow( "CHIP8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIN_WD, WIN_HT, SDL_WINDOW_SHOWN );

	if ( !window ) {
		cout << "Error creating window: " << SDL_GetError()  << endl;
		system("pause");
		return 1;
	}

    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
	if ( !renderer ) {
		cout << "Error creating renderer: " << SDL_GetError() << endl;
		return false;
	}

    SDL_RenderSetLogicalSize(renderer, WIN_WD, WIN_HT);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, MAX_WIDTH, MAX_HEIGHT);
    if (texture == NULL)
    {
        std::cerr << "Error in setting up texture " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    while(STATE == EMU_ON){
        chip8_instance.cycle();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT){
                STATE = EMU_OFF;
            }

            if(event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    STATE = EMU_OFF;
                }

                for (int i = 0; i < MAX_KEYCOUNT; i++)
                {
                    if (event.key.keysym.sym == keymap[i])
                    {
                        chip8_instance.set_key(i, KEY_DOWN);
                    }
                }                
            }

            if(event.type == SDL_KEYUP) {
                for (int i = 0; i < MAX_KEYCOUNT; i++)
                {
                    if (event.key.keysym.sym == keymap[i])
                    {
                        chip8_instance.set_key(i, KEY_UP);
                    }
                } 
            }
        }
        /*
            Update screen if drawflag is set.
        */
        if(chip8_instance.get_drawflag()) {
            uint32_t video_buffer[MAX_DISPSIZE];
            for(int i=0; i<MAX_DISPSIZE; i++){
                video_buffer[i] = chip8_instance.get_pixel(i);
            }
            
            SDL_UpdateTexture(texture, NULL, video_buffer, MAX_WIDTH * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture , NULL, NULL);
            SDL_RenderPresent(renderer);

            chip8_instance.set_drawflag(false);
        }

        sleep(1);
    }


    //SDL_Quit();
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
            exit(1);
        }
    }
    
    

}

int setup_rom(CHIP8 *chip8_instance, char *rom, uint8_t MODE) {
    //for now call load_rom directly, add fancy path checkers later
    bool sound = false;
    bool verbose = false;

    if(MODE & MODE_SND) {
        sound = true;
    }
    if(MODE & MODE_VRB) {
        verbose = true;
    }

    return chip8_instance->load_rom(rom, sound, verbose);
}

int setup_window() {
    return 0;
}

int exit_emu(int code) {
    exit(code);
}
