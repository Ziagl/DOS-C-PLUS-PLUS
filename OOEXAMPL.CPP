/*
 * this is a simple object oriented example to test if
 * Borland Turbo C++ 3.0 understand basic object oriented
 * concepts
 */

#include <stdio.h>

struct Testclass
{
    Testclass(){Testclass::i++;};

    static void helloFromClass()
    {
        printf("Hello from class. Current value: %d\n", i);
    }

    static int i;
};

int main()
{
    printf("Create class instance.\n");
    Testclass::i = 10;
    Testclass c1;
    c1.helloFromClass();
    Testclass c2;
    Testclass c3;
    c1.helloFromClass();
    printf("Exit.\n");
    return 0;
}