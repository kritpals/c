/*
*Originally created for testing purpose.
*Made by Kritpal Singh.
*For any further query email me @ kritpal.sing@gmail.com
 Originally created for Mediatek SDK 
cdecoder.c - c source to a base64 decoding algorithm implementation

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
typedef enum
{
	step_a, step_b, step_c, step_d
} base64_decodestep;

typedef struct
{
	base64_decodestep step;
	char plainchar;
} base64_decodestate;
char *rawHash = "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";
void base64_init_decodestate(base64_decodestate* state_in);

int base64_decode_value(char value_in);

int base64_decode_block(const char* code_in, const int length_in, char* plaintext_out, base64_decodestate* state_in);


int base64_decode_value(char value_in)
{
	static const signed char decoding[] = {62,-1,-1,-1,63,52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-2,-1,-1,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51};
	static const char decoding_size = sizeof(decoding);
	value_in -= 43;
	if (value_in < 0 || value_in >= decoding_size) return -1;
	return decoding[(int)value_in];
}

void base64_init_decodestate(base64_decodestate* state_in)
{
	state_in->step = step_a;
	state_in->plainchar = 0;
}

int base64_decode_block(const char* code_in, const int length_in, char* plaintext_out, base64_decodestate* state_in)
{
	const char* codechar = code_in;
	char* plainchar = plaintext_out;
	signed char fragment;
	
	*plainchar = state_in->plainchar;
	
	switch (state_in->step)
	{
		while (1)
		{
	case step_a:
			do {
				if (codechar == code_in+length_in)
				{
					state_in->step = step_a;
					state_in->plainchar = *plainchar;
					return plainchar - plaintext_out;
				}
				fragment = (char)base64_decode_value(*codechar++);
			} while (fragment < 0);
			*plainchar    = (fragment & 0x03f) << 2;
	case step_b:
			do {
				if (codechar == code_in+length_in)
				{
					state_in->step = step_b;
					state_in->plainchar = *plainchar;
					return plainchar - plaintext_out;
				}
				fragment = (char)base64_decode_value(*codechar++);
			} while (fragment < 0);
			*plainchar++ |= (fragment & 0x030) >> 4;
			*plainchar    = (fragment & 0x00f) << 4;
	case step_c:
			do {
				if (codechar == code_in+length_in)
				{
					state_in->step = step_c;
					state_in->plainchar = *plainchar;
					return plainchar - plaintext_out;
				}
				fragment = (char)base64_decode_value(*codechar++);
			} while (fragment < 0);
			*plainchar++ |= (fragment & 0x03c) >> 2;
			*plainchar    = (fragment & 0x003) << 6;
	case step_d:
			do {
				if (codechar == code_in+length_in)
				{
					state_in->step = step_d;
					state_in->plainchar = *plainchar;
					return plainchar - plaintext_out;
				}
				fragment = (char)base64_decode_value(*codechar++);
			} while (fragment < 0);
			*plainchar++   |= (fragment & 0x03f);
		}
	}
	/* control should not reach here */
	return plainchar - plaintext_out;
}

char * decode(const char* input) {
    /* set up a destination buffer large enough to hold the encoded data */
    char* output = (char*) calloc(strlen(input) + 1, sizeof (char));
    /* keep track of our decoded position */
    char* c = output;
    /* store the number of bytes decoded by a single call */
    int cnt = 0;
    /* we need a decoder state */
    base64_decodestate s;

    /*---------- START DECODING ----------*/
    /* initialise the decoder state */
    base64_init_decodestate(&s);
    /* decode the input data */
    cnt = base64_decode_block(input, strlen(input), c, &s);
    c += cnt;
    /* note: there is no base64_decode_blockend! */
    /*---------- STOP DECODING  ----------*/

    /* we want to print the decoded data, so null-terminate it: */
    *c = 0;

    return output;
}
int main(){
printf("Decoding  hash\n");
char *plainS = decode(rawHash);
int i;
printf("Plain length=%d, hash=%s\n",strlen(plainS),plainS);
	for ( i = 0; i < strlen(plainS); i = i + 4) {
        char print[10]="";
        strncpy(print,plainS + i, 4);
        printf("%s",print);
        }
}

