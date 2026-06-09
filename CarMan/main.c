#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "CommonData.h"

#include "Network.h"

int main(void)
 {
    //ansi clear screen
    printf("\033[2J\033[H");
    
    //code
    NetworkProc();  // Temporary for test is run from main. Later will be redefined as a process.
    while(1)
     {

     }

    return 0;
 }