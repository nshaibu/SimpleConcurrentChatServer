

/*
 * Compile me with:
 *   gcc -o helper helper.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
main( int    argc, char **argv )
{
    int i;
	char buf[100];
    for( i = 0; i < 10; i++ )
    {
        char stdout_string[] = "Normal message no:  .";
        char stderr_string[] = "Error message no:  .";
	
        stdout_string[19] = '0' + i;
        stderr_string[18] = '0' + i;
		  read(STDIN_FILENO, buf, sizeof(buf));
		 
        sleep( 1 );
        fprintf( stdout, "%s\n", stdout_string );
        sleep( 1 );
        fprintf( stderr, "%s\n", stderr_string );
        sleep(1);
        fprintf(stdout, "%s\n", buf);
    }

    return( 0 );
}
