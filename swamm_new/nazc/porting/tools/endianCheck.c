#include <stdio.h>
#include <string.h>

char * endian(void)
{
    static long int str[2] = { 0x41424344,0x0 }; /* ASCII "ABCD" */
    if (strcmp("DCBA", (char *) str) == 0)
        return("little-endian");
    else if (strcmp("ABCD", (char *) str) == 0)
        return("big-endian");
    else if (strcmp("BADC", (char *) str) == 0)
        return("PDP-endian");
}

int main(void)
{
    static long int str[2] = { 0x41424344,0x0 }; /* ASCII "ABCD" */

    printf("if your machine does print : \n");
    printf("        DCBA = little-endian ( VAX, PC )\n");
    printf("        ABCD =    big-endian ( MC6800, Sun4, HP9000/715 )\n");
    printf("        BADC =    PDP-endian ( PDP-11 )\n\n");
    printf("your machine gives : ''%s'', so it is a ''%s'' one\n", (char *) str, endian());

	return 0;
}
