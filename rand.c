
/*
*Originally created for testing purpose.
*Made by Kritpal Singh.
*For any further query email me @ kritpal.sing@gmail.com
 Originally created for Openwrt SDK 
 Get random number using urandom device .
*/
#include <stdio.h>
#include <stdlib.h>
 #include <time.h>
 #include <fcntl.h>
int main() {
  int c, n;
  srand(time(NULL));
    n = rand() % 150 + 1;
    printf("%d\n", n);
FILE *urandom; 
unsigned int seed; 

urandom = fopen ("/dev/urandom", "r"); 
if (urandom == NULL) { 
fprintf (stderr, "Cannot open /dev/urandom!\n"); 
exit (1); 
} 
fread (&seed, sizeof (seed), 1, urandom); 

srand (seed); /* seed the pseudo-random number generator */ 

printf ("Random number from 1 to 100: %d\n", floor (rand() * 100.0 / ((double) RAND_MAX + 1) )+ 1); 

  return 0;
}
