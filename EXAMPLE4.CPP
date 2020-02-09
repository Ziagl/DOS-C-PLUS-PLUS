/*
 * this example extends example 3 and creates a new color palette
 * for drawing a nice background with sky and grass
 */
#include <conio.h>
#include <dos.h>
#include <alloc.h>

typedef unsigned char byte;

#define VIDEO_INT 0x10          /* graphics mode interrupt number */
#define SET_MODE 0x00           /* high byte */
#define VGA_320_200_256 0x13    /* low byte 320x200 256 colors */
#define VGA_640_480_16 0x14     /* low byte 640x480  16 colors */
#define TEXT_MODE 0x03          /* 80x25 characters 16 colors */
#define SCREEN_HEIGHT 200
#define SCREEN_WIDTH 320
#define NUM_COLORS 256          /* size of color palette */
#define PALETTE_INDEX 0x3C8     /* port number for VGA DAC PEL */
#define PALETTE_DATA 0x3C9      /* port number for VGA DAC data */

byte far *VGA = (byte far *)0xA0000000L;    /* pointer to VGA memory */

#define SETPIX(x,y,c) *(VGA+(x)+(y)*SCREEN_WIDTH)=c
#define GETPIX(x,y) *(VGA+(x)+(y)*SCREEN_WIDTH)
#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)<(y)?(x):(y))

/* changes video mode with given low byte*/
void set_mode(byte mode)
{
    union REGS regs;
    regs.h.ah = SET_MODE;   /* set high byte of register */
    regs.h.al = mode;       /* set low byte of register */
    int86(VIDEO_INT, &regs, &regs);
}

void draw_background()
{
    int x,y;

    for(y=0;y<SCREEN_HEIGHT;++y)
    {
        for(x=0;x<SCREEN_WIDTH;++x)
        {
            SETPIX(x,y,y);
        }
    }
}

/* creates a color palette for VGA color and returns a pointer to it */
byte* get_color_palette()
{
    byte* pal;
    int i;
    pal = (byte*)malloc(NUM_COLORS * 3);       /* RGB colors */

    /* sky */
    for(i=0; i<100; ++i)
    {
        /* 6-Bit colors! */
        pal[i*3 + 0] = MIN(63, i / 2);   /* red */
        pal[i*3 + 1] = MIN(63, i / 2);   /* green */
        pal[i*3 + 2] = 63;               /* blue */
    }
    /* ground */
    for(i=100; i<200; ++i)
    {
        pal[i*3 + 0] = 15;                  /* red */
        pal[i*3 + 1] = ((i-100) / 2) +15;   /* green */
        pal[i*3 + 2] = 15;                  /* blue */
    }

    return pal;
}

/* copy color palette array to VGA DAC PEL address */
void set_color_palette(byte* pal)
{
    int i;

    /* outp writes a byte to hardware port */
    outp(PALETTE_INDEX, 0);
    for(i=0; i<NUM_COLORS * 3; ++i)
    {
        outp(PALETTE_DATA, pal[i]);
    }
}

int main()
{
    byte *pal;

    set_mode(VGA_320_200_256);
    pal = get_color_palette();
    set_color_palette(pal);
    draw_background();
    getch();
    set_mode(TEXT_MODE);
    return 0;
}