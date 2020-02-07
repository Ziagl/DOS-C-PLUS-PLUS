/*
 * this example shows how to use VGA graphics mode on DOS
 * and how to draw with color on screen
 */
#include <conio.h>
#include <dos.h>

typedef unsigned char byte;

#define VIDEO_INT 0x10          /* graphics mode interrupt number */
#define SET_MODE 0x00           /* high byte */
#define VGA_320_200_256 0x13    /* low byte 320x200 256 colors */
#define VGA_640_480_16 0x14     /* low byte 640x480  16 colors */
#define TEXT_MODE 0x03          /* 80x25 characters 16 colors */
#define SCREEN_HEIGHT 200
#define SCREEN_WIDTH 320

byte far *VGA = (byte far *)0xA0000000L;    /* pointer to VGA memory */

#define SETPIX(x,y,c) *(VGA+(x)+(y)*SCREEN_WIDTH)=c
#define GETPIX(x,y) *(VGA+(x)+(y)*SCREEN_WIDTH)

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

int main()
{
    set_mode(VGA_320_200_256);
    draw_background();
    getch();
    set_mode(TEXT_MODE);
    return 0;
}