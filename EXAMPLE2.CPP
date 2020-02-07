/*
 * this example shows how to get and compute keyboard
 * input on MS DOS and how to deal with special keys
 */
#include <conio.h>
#include <stdio.h>
#include <string.h>

int main()
{
    char kc = 0;
    char s[255];

    clrscr();
    printf("Keyboard test program!\n");

    /* game loop stops with ESC */
    while( kc != 0x1b)
    {
        if(kbhit()) 
        {
            kc = getch();
            if(kc == (char)0)
            {
                kc = getch();
                switch(kc)
                {
                    case 0x48: /* up arrow */
                        strcpy(s, "up arrow");
                        break;
                    case 0x50: /* down arrow */
                        strcpy(s, "down arrow");
                        break;
                    default:
                        sprintf(s, "00 %02x", kc);
                        break;
                }
            } else {
                sprintf(s, "%02x", kc);
            }
            printf("Key pressed: %s\n", s);
        }
    }

    return 0;
}