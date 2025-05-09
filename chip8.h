#ifndef CHIP8_H
#define CHIP8_H

#include<cstdint>   

/*

    CHIP-8 EMULATOR in C++

    References: 1. http://devernay.free.fr/hacks/chip8/C8TECH10.HTM 

    Specs (for reference)
    
    Opcode: 16-bit, 35 total 
    Memory: 4KB (12 bits addressable)
    Register: 8-bit general purpose, 16 total (V0 to VF) 
    Display: 64 x 32 pixels black or white
    Program counter: 16-bit, 0x000 to 0xFFF
    Index Register: 16-bit, 0x000 to 0xFFF
    Stack: 256B, 16-bit, with size 16
    Stack Pointer: 16-bit
    Delay Timer: 8-bit, 60Hz
    Sound Timer: 8-bit, 60Hz
    Keypad: 8-bit, 16 total
    Font: 16 characters, 5 nibbles per character

*/

/* CPU */
#define MAX_REGCOUNT    16          /* Maximum number of registers      */  

/* MEM */
#define MAX_MEMSIZE     4096        /* Maximum Memory Size              */  
#define MAX_STACKSIZE   16          /* Maximum Stack Size               */
#define PC_STARTADR     0x200       /* Program counter start address    */

/* I/O */
#define MAX_WIDTH       64          /* Maximum Width of Display (Pixels)    */
#define MAX_HEIGHT      32          /* Maximum Height of Display (Pixels)   */
#define MAX_DISPSIZE    64 * 32     /* Maximum Size of Display (Pixels)     */
#define MAX_KEYCOUNT    16          /* Maximum Key count                    */
#define MAX_FONTCOUNT   16 * 5      /* Maximum font count                   */
#define PIX_ON          0x1         /* Pixel ON value                       */
#define PIX_OFF         0x0         /* Pixel OFF value                      */
#define KEY_DOWN        1           /* Key DOWN value                       */
#define KEY_UP          0           /* Key UP value                         */
#define MAX_SPRITEWD    8           /* Maximum Sprite Width (Bits)          */

/*

    CHIP8 structure,
        will have all the SPECS mentioned above in PRIVATE,
        while these can be manipulated using member functions in PUBLIC

        Contains CPU and Memory.

*/
class CHIP8 {
    private:
        
        /*
        
            CPU Data

        */
        uint8_t     V[MAX_REGCOUNT];        /* 16 x 8-bit register          */
        uint16_t    PC;                     /* 16-bit program counter       */
        uint16_t    I;                      /* 16-bit index register        */
        uint8_t     ST;                     /* 8-bit sound timer register   */
        uint8_t     DT;                     /* 8-bit delay timer register   */

        /*
        
            MEM Data
        
        */
        uint8_t     MEM[MAX_MEMSIZE];       /* 4KB RAM                                  */
        uint16_t    STACK[MAX_STACKSIZE];   /* 16 x 16-bit addresses for function trace */
        int8_t      SP;                     /* 8-bit stack pointer                      */

        /*
        
            I/O Data

        */
        uint8_t    DISP[MAX_DISPSIZE];     /* 64 x 32 32-bit pixels each   */
        uint8_t    KEYP[MAX_KEYCOUNT];     /* 16 x 8-bit key pressed       */
        
        bool       draw_flag;              /* flag if display update       */

        /*
        
            Misc.
        
        */
        bool MODE_VRB;
        bool MODE_SND;
        bool MODE_STP;
        uint16_t bit_mask(uint16_t, uint16_t, int);     /* helper function to mask bits, takes the original 2 bytes, a mask, and a right-shift value*/

    public:
        
        /* Constructor  */
        CHIP8();

        /* Destructor   */
        ~CHIP8();

        /* Load the ROM into memory if it exists */
        int load_rom(char*, bool, bool, bool);

        /* getters and setters */
        bool     get_STP();
        uint32_t get_pixel(int );
        bool     get_drawflag();
        uint8_t  get_key(int );
        void     set_key(int , int );
        void     set_drawflag(bool );

        /* takes care of fetching the instruction and sending it to exec unit */
        int cycle();

        /* decodes the instruction and executes it */
        int instr_exec(uint16_t );
};

#endif //CHIP8_H