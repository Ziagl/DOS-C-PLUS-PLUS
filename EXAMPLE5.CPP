/*
 * this example extends example 4 and adds an object that is drawn before
 * the background. It is moveable by cursor keys
 */
#include <conio.h>
#include <dos.h>
#include <alloc.h>
#include <mem.h>

typedef unsigned char byte;
typedef struct {
    byte color;
    int x, y;
    int width;
    byte *buffer;
} square;

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
#define INPUT_STATUS 0x3DA      /* CRT controller 6845 status register */
#define VRETRACE_BIT 0x08       /* retrace scanline bit */

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

/* copy memory from VGA buffer to square */
void blit2mem(byte far *buffer, int x, int y, int width, int height)
{
    int i;
    byte far *src = VGA + x + y * SCREEN_WIDTH;
    byte far *dest = buffer;
    for(i = y; i < y + height; ++i)
    {
        movedata(FP_SEG(src), FP_OFF(src),
                 FP_SEG(dest), FP_OFF(dest), width);
        src += SCREEN_WIDTH;
        dest += width;
    }
}

/* copy memory from square to VGA buffer */
void blit2vga(byte far *buffer, int x, int y, int width, int height)
{
    int i;
    byte far *src = buffer;
    byte far *dest = VGA + x + y * SCREEN_WIDTH;
    for(i = y; i < y + height; ++i)
    {
        movedata(FP_SEG(src), FP_OFF(src),
                 FP_SEG(dest), FP_OFF(dest), width);
        src += width;
        dest += SCREEN_WIDTH;
    }
}

/* store background behind square into buffer */
void store_square(square *s)
{
    blit2mem(s->buffer, s->x, s->y, s->width, s->width);
}

/* restore background from square buffer */
void restore_square(square *s)
{
    blit2vga(s->buffer, s->x, s->y, s->width, s->width);
}

/* draws a square object with given color and dimensions into VGA buffer */
void draw_square(square *s)
{
    int i, j;
    for(j=s->y; j < s->y + s->width; ++j)
    {
        for(i=s->x; i < s->x + s->width; ++i)
        {
            byte far *dest = VGA + i + j * SCREEN_WIDTH;
            *dest = s->color;
        }
    }
}

/* wait till ram access is allowed to fix flickering */
void wait_for_retrace()
{
    while (inp(INPUT_STATUS) & VRETRACE_BIT);
    while (!(inp(INPUT_STATUS) & VRETRACE_BIT));
}

int main()
{
    char kc = 0;
    byte *pal;
    square square1;

    square1.width = 50;
    square1.x = 5;
    square1.y = 5;
    square1.color = 255;
    square1.buffer = (byte*)malloc(square1.width * square1.width);

    /* set video mode and create color palette */
    set_mode(VGA_320_200_256);
    pal = get_color_palette();
    set_color_palette(pal);

    draw_background();
    store_square(&square1);
    
    /* game loop */
    while(kc != 0x1b)
    {
        /* no flickering */
        wait_for_retrace();
        /* restore background */
        restore_square(&square1);

        /* get keyboard input */
        if(kbhit())
        {
            kc = getch();
            if( kc == (char)0)
            {
                kc = getch();
                /* movement of square */
                switch(kc)
                {
                    case 0x48: /* up */
                        square1.y -= 2;
                        if(square1.y < 0)
                        {
                            square1.y = 0;
                        }
                        break;
                    case 0x4b: /* left */
                        square1.x -= 2;
                        if(square1.x < 0)
                        {
                            square1.x = 0;
                        }
                        break;
                    case 0x4d: /* right */
                        square1.x += 2;
                        if(square1.x + square1.width > SCREEN_WIDTH)
                        {
                            square1.x = SCREEN_WIDTH - square1.width;
                        }
                        break;
                    case 0x50:  /* down */
                        square1.y += 2;
                        if(square1.y + square1.width > SCREEN_HEIGHT)
                        {
                            square1.y = SCREEN_HEIGHT - square1.width;
                        }
                        break;
                }
            }
        }

        /* draw scene */
        store_square(&square1);
        draw_square(&square1);
    }

    /* restore video mode */
    set_mode(TEXT_MODE);
    return 0;
}