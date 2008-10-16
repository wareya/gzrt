#include <stdio.h>
#include <windows.h>

int main ( int argc, char ** argv )
{
    static STARTUPINFO k;
    static PROCESS_INFORMATION j;
    char file[] = "resources\\bin\\gzrt.exe";
    CreateProcess( file, NULL, 0, 0, FALSE, 0, NULL, NULL, &k, &j );
    return 0;
}