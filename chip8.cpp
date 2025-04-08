/*

    CHIP8 

*/

#include "chip8.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>

/*

    Initialize CHIP8 

*/
CHIP8::CHIP8() {

    std::cout<< "Initializing CHIP8 instance..."<<std::endl;
    /*
        
        CPU Data

    */
    std::memset(V, 0, sizeof(V));
    
    PC   = PC_STARTADR;
    I    = 0;
    DT   = 0;
    ST   = 0;
    
    /*
        
        MEM Data
        
    */
    memset(STACK, 0, sizeof(STACK));
   
    SP   = 0;

    /*
        
        I/O Data
        
    */
    memset(DISP, 0, sizeof(DISP));
    memset(KEYP, 0, sizeof(KEYP));

    draw_flag = false;

    /* initialise font set */
    uint8_t font_set[MAX_FONTCOUNT] {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    /* load font set in memory from 0x050 to 0x09F */
    for(int i=0; i < 80; i++) {
        MEM[i + 0x50] = font_set[i];
    }

}

CHIP8::~CHIP8() {
    std::cout<<"CHIP8 instance stopped.";
}

int CHIP8::load_rom(char* path, bool SND, bool VRB) {
    std::ifstream rom_file(path, std::ios::binary);

    if(!rom_file.is_open()){
        std::cerr<<std::endl<< "file does not exist.";
        return -1;
    }

    char data;
    for(int i=PC_STARTADR; rom_file.get(data); i++){
        if(i >= MAX_MEMSIZE){
            std::cerr<< "file size too large";
            return -1;
        }
        MEM[i] = (uint8_t)data;
    }
    return 0;
}

uint16_t CHIP8::bit_mask(uint16_t word, uint16_t mask, int rshift) {
    return ((word & mask) >> rshift) ;
}

int CHIP8::cycle() {

    /*
        Each insturction is 16-bits, big-endian.
        so we get the first byte and LEFTSHIFT it by 8,
        then get the next byte and OR it.
        then we send this instruction to the exec unit,
        which decodes it and executes it
    */

    if(PC >= MAX_MEMSIZE) {
        std::cerr << "memory overflow";
        return -1;
    }

    uint16_t instruction = ( ( MEM[PC] << 8 ) ||  MEM[PC+1] );
    /* go to next address +2 bytes */
    PC += 2;

    if(instr_exec(instruction) == -1) {
        std::cerr<<"error executing instruction: <print instruction here>";
        return -1;
    }

    if(DT > 0) {
        DT--;
    } else {

    }

    if(ST > 0) {
        ST--;
    } else {

    }

    return 0;
}

/*
    sets KEYP (key pressed to 1).
*/
void CHIP8::set_key(int key, int val) {
    KEYP[key] = val;
}


/*
    Represents EXEC of ONE instruction
    A large nested switch is used to group instructions.
*/
int CHIP8::instr_exec(uint16_t instruction) {

    /*
        Large number of switch cases to group instructions with the same nibble.
        first we need to get all the nibbles, here we use 8-bit variables to store the following:
    */

    uint8_t  M    = bit_mask(instruction, 0xF000, 12);  //      M   : most significant nibble
    uint8_t  X    = bit_mask(instruction, 0x0F00,  8);  //      X   : second nibble -> corresponds to VX register (first  register)
    uint8_t  Y    = bit_mask(instruction, 0x00F0,  4);  //      Y   : third  nibble -> corresponds to VY register (second register)
    uint8_t  N    = bit_mask(instruction, 0x000F,  0);  //      N   : Fourth nibble
    uint8_t  KK   = bit_mask(instruction, 0x00FF,  0);  //      KK  : Lowest 8-bit (2 nibbles)
    uint16_t NNN  = bit_mask(instruction, 0x0FFF,  0);  //      NNN : Lowest 12-bit, corresponds to an address.

    /*
    
        We have all the segments of the instructions,
        which can be checked using nested switches.

    */

    /* main switch*/
    switch(M) {

        case 0x0:
            {
                switch(instruction){
                
                    /*
                        INSTR(1): 00E0 - CLS
                        Clear the display.
                    */
                    case 0x00E0:
                        {
                            for(int i=0; i<MAX_DISPSIZE; i++){
                                DISP[i] = (uint8_t)PIX_OFF;
                            }
                            break;
                        }                     
                    
                    /*
                        INSTR(2): 00EE - RET
                        Return from a subroutine.
                    */
                    case 0x00EE:
                        {
                            PC = STACK[SP];
                            SP--;
                            break;
                        }
                }
                break;
            }
            
            case 0x1:
                {
                    /*
                        INSTR(3): 1nnn - JP addr
                        Jump to location nnn.
                    */
                    PC = NNN;
                    break;
                }
            
            case 0x2:
                {
                    /*
                        INSTR(4): 2nnn - CALL addr
                        Call subroutine at nnn.
                    */
                   SP++;
                   STACK[SP] = PC;
                   PC = NNN;
                   break;
                }

            case 0x3:
                {
                    /*
                        INSTR(5): 3xkk - SE Vx, byte
                        Skip next instruction if Vx = kk.
                    */
                    if(V[X] == KK) {
                        PC += 2;
                    }
                    break;
                }

            case 0x4:
                {
                   /*
                        INSTR(6): 4xkk - SNE Vx, byte
                        Skip next instruction if Vx != kk.
                   */ 
                    if(V[X] != KK) {
                        PC += 2;
                    }
                    break;
                }

            case 0x5:
                {
                    /*
                        INSTR(7): 5xy0 - SE Vx, Vy
                        Skip next instruction if Vx = Vy.
                    */
                    if(V[X] == V[Y]) {
                        PC += 2;
                    }
                    break;
                }

            case 0x6:
                {
                    /*
                        INSTR(8): 6xkk - LD Vx, byte
                        Set Vx = kk.
                    */
                    V[X] = KK;
                    break;
                }
            
            case 0x7:
                {
                    /*
                        INSTR(9): 7xkk - ADD Vx, byte
                        Set Vx = Vx + kk.
                    */
                    V[X] = V[X] + KK;
                    break;
                }

            case 0x8:
                {
                    switch(N) {
                        /*
                            INSTR(10): 8xy0 - LD Vx, Vy
                            Set Vx = Vy.
                        */
                        case 0x0:
                            {
                                V[X] = V[Y];
                                break;
                            }
                        
                        /*
                            INSTR(11): 8xy1 - OR Vx, Vy
                            Set Vx = Vx OR Vy.
                        */
                        case 0x1:
                        {
                            V[X] = V[X] | V[Y];
                            break;
                        }
                        /*
                            INSTR(12): 8xy2 - AND Vx, Vy
                            Set Vx = Vx AND Vy.
                        */
                        case 0x2:
                        {
                            V[X] = V[X] & V[Y];
                            break;
                        }
                        /*
                            INSTR(13): 8xy3 - XOR Vx, Vy
                            Set Vx = Vx XOR Vy. 

                        */
                        case 0x3:
                        {
                            V[X] = V[X] ^ V[Y];
                            break;
                        }
                   
                        /*
                            INSTR(14): 8xy4 - ADD Vx, Vy
                            Set Vx = Vx + Vy, set VF = carry.
                        */
                        case 0x4:
                        {
                            V[0xF] = V[X]; //temporarily use Flag Register

                            V[X] = V[X] + V[Y];
                            /* carry check, sum won't be equal if V[X] overflows.*/
                            if(V[X] - V[0xF] != V[Y]) {
                                V[0xF] = 0x1;
                            } else {
                                V[0xF] = 0x0;
                            }
                            break;
                        }
                        /*
                            INSTR(15): 8xy5 - SUB Vx, Vy
                            Set Vx = Vx - Vy, set VF = NOT borrow.          
                        */
                        case 0x5:
                        {
                            /* carry flag is set when there is no borrow. */
                            if(V[X] > V[Y]) {
                                V[0xF] = 0x1;
                            } else {
                                V[0xF] = 0x0;
                            }

                            V[X] = V[X] - V[Y];

                            break;
                        }
                        /*
                            INSTR(16): 8xy6 - SHR Vx {, Vy}
                            Set Vx = Vx SHR 1. 
                        */
                        case 0x6:
                        {
                            V[0xF] = V[X] & 1;
                            V[X] = V[X] >> 1;
                            break;
                        }
                        
                        /*
                            INSTR(17): 8xy7 - SUBN Vx, Vy
                            Set Vx = Vy - Vx, set VF = NOT borrow. 
                        */
                        case 0x7:
                        {
                            /* carry flag is set when there is no borrow. */
                            if(V[Y] > V[X]) {
                                V[0xF] = 0x1;
                            } else {
                                V[0xF] = 0x0;
                            }

                            V[X] = V[Y] - V[X];

                            break;
                        }
                        /*
                            INSTR(18): 8xyE - SHL Vx {, Vy}
                            Set Vx = Vx SHL 1. 
                        */
                        case 0x8:
                        {    
                            V[0xF] = (V[X] & 010000000) >> 7; 
                            V[X] = V[X] << 1;
                            break;
                        }
                    }
                    break;
                }
                
            case 0x9:
                {
                    /*
                        INSTR(19): 9xy0 - SNE Vx, Vy
                        Skip next instruction if Vx != Vy.
                    */
                    if(V[X] != V[Y]) {
                        PC += 2;
                    }
                    break;
                }

            case 0xA:
                {
                    /*
                        INSTR(20): Annn - LD I, addr
                        Set I = nnn.
                    */
                    I = NNN;
                    break;
                }

            case 0xB:
                {
                    /*
                        INSTR(21): Bnnn - JP V0, addr
                        Jump to location nnn + V0. 
                    */
                    PC = NNN + (uint16_t) V[0];
                    break;
                }

            case 0xC:
                {
                    /*
                        INSTR(22): Cxkk - RND Vx, byte
                        Set Vx = random byte AND kk. 
                    */
                    V[X] = KK & (uint8_t) (ST+DT);  /* the RAND number here is (ST+DT) */
                    break;
                }

            case 0xD:
                {
                    /*
                        INSTR(23): Dxyn - DRW Vx, Vy, nibble
                        Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision. 
                    */
                    
                    /* 
                        starting location is MEM[I], until MEM[I+N-1].
                        then these same bytes are copied onto starting point V[X], V[Y].
                    */
                    for(int i=0;i<N;i++) {
                        
                    }
                    
                    break;
                }
                
            case 0xE:
                {
                    switch(KK) {

                        /*
                            INSTR(24): Ex9E - SKP Vx
                            Skip next instruction if key with the value of Vx is pressed.
                        */
                        case 0x9E:
                            {   
                                if(KEYP[V[X]] == KEY_DOWN) {
                                    PC += 2;
                                }
                                break;
                            }
                        
                        /*
                            INSTR(25): ExA1 - SKNP Vx
                            Skip next instruction if key with the value of Vx is not pressed.
                        */
                        case 0xA1:
                            {
                                if(KEYP[V[X]] == KEY_UP) {
                                    PC += 2;
                                }
                                break;
                            }
                    }
                    break;
                }
            
            case 0xF:
                {
                    switch(KK) {
                        /*
                            INSTR(26): Fx07 - LD Vx, DT
                            Set Vx = delay timer value.
                        */
                        case 0x07: 
                            {
                                V[X] = DT;
                                break;
                            }

                        /*
                            INSTR(27): Fx0A - LD Vx, K
                            Wait for a key press, store the value of the key in Vx.
                        */
                        case 0x0A: 
                        {
                            /* this checks if any key is pressed at the moment, then breaks once found*/
                            int key_index = 0;
                            while(true) {
                                if(KEYP[key_index] == KEY_DOWN){
                                    break;
                                }
                                key_index = (key_index + 1) % MAX_KEYCOUNT;
                            }
                            V[X] = key_index;
                            
                            break;
                        }

                        /*
                            INSTR(28): Fx15 - LD DT, Vx
                            Set delay timer = Vx.
                        */
                        case 0x15:
                            {
                                DT = V[X];
                                break;
                            }

                        /*
                            INSTR(29): Fx18 - LD ST, Vx
                            Set sound timer = Vx.    
                        */
                        case 0x18:
                            {
                                ST = V[X];
                                break;
                            }

                        /*
                            INSTR(30): Fx1E - ADD I, Vx
                            Set I = I + Vx.
                        */
                        case 0x1E:
                            {
                                I = I + V[X];
                                break;
                            }

                        /*
                            INSTR(31): Fx29 - LD F, Vx
                            Set I = location of sprite for digit Vx.
                        */
                        case 0x29:
                            {
                                break;
                            }

                        /*
                            INSTR(32): Fx33 - LD B, Vx
                            Store BCD representation of Vx in memory locations I, I+1, and I+2. 
                        */
                        case 0x33:
                            {
                                break;
                            }

                        /*
                        
                        */

                    }
                    break;
                }
    }
    
}