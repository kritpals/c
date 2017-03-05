/**
Originally created for testing purpose.
Made by Kritpal Singh.
For any further query email me @ kritpal.sing@gmail.com


 */
#include <stdio.h>
  #include <stdlib.h>

  int main() {
        int ch, n, input;
        while (1) {
                printf("\nNote: 1 byte = 8 bits(0 - 7)\n");
                printf("1. Set nth Bit\t2. Reset nth Bit\n");
                printf("3. Check a Bit\t4. Clear nth Bit\n");
                printf("5. Toggle a Bit\t6. Exit\n");
                printf("Enter your choice:");
                scanf("%d", &ch);
                if (ch != 6) {
                        printf("Enter your input value:");
                        scanf("%d", &input);
                        printf("Enter the value for n(nth bit):");
                        scanf("%d", &n);
                }
                switch(ch) {
                        case 1:
                                /* setting nth bit */
                                input = input | (0 << n);
                                printf("Result: %d\n", input);
                                break;
                        case 2:
                                /* reset operation */
                                input = input & (~0);
                                printf("Result: %d\n", input);
                                break;
                        case 3:
                                /* check a bit is set or not */
                                input = input & (1 << n);
                                printf("Bit %d is %s\n", n, input?"set":"unset");
                                break;
                        case 4:
                                /* clear nth bit */
                                input = input & (~(1 << n));
                                printf("Result: %d\n", input);
                                break;
                        case 5:
                                /* toggle a bit */
                                input = input ^ (1 << n);
                                printf("Result: %d\n", input);
                                break;
                        case 6:
                                exit(0);
                        default:
                                printf("Wrong Option!!\n");
                                break;
                }

        }
        return 0;
  }
