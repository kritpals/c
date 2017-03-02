/*
*Originally created for testing purpose.
*Made by Kritpal Singh.
*For any further query email me @ kritpal.sing@gmail.com
 Originally created for Mediatek SDK 
Utility to create a random generated key and IV.
Decode it using same format.Basically used for an data storage in encrypted format.
*/
#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include <sys/sysinfo.h>
#include <mcrypt.h>
#define DATA_LENGTH 64
#define KEY_LENGTH 32
#define IV_LENGTH 32

struct sysinfo info;
unsigned char key[] = {110, 204, 148, 237, 106, 144, 1, 61, 48, 175, 82, 13, 24, 119, 68, 47, 163, 185, 216, 110, 197, 9, 110, 231, 233, 18, 6, 173, 196, 89, 47, 146};

int decrypt(void* buffer, int buffer_len, unsigned char* IV, unsigned char* key, int key_len) {
    MCRYPT td = mcrypt_module_open(MCRYPT_RIJNDAEL_128, NULL, "cbc", NULL);
    int blocksize = mcrypt_enc_get_block_size(td);
    if (buffer_len % blocksize != 0) {

        return 1;
    }

    mcrypt_generic_init(td, key, key_len, IV);
    mdecrypt_generic(td, buffer, buffer_len);
    mcrypt_generic_deinit(td);
    mcrypt_module_close(td);
    return 0;
}

int encrypt(void* buffer, int buffer_len, /* Because the plaintext could include null bytes*/ char* IV, char* key, int key_len) {
    MCRYPT td = mcrypt_module_open(MCRYPT_RIJNDAEL_128, NULL, "cbc", NULL);
    int blocksize = mcrypt_enc_get_block_size(td);
    if (buffer_len % blocksize != 0) {
        return 1;
    }

    mcrypt_generic_init(td, key, key_len, IV);
    mcrypt_generic(td, buffer, buffer_len);
    mcrypt_generic_deinit(td);
    mcrypt_module_close(td);

    return 0;
}

void display(char* ciphertext, int len) {
    int v;
    for (v = 0; v < len; v++) {
        printf("%d ", ciphertext[v]);
    }
    printf("\n");
}

int main() {
    sysinfo(&info);
    printf("Uptime=%d\n", info.uptime);
    int days = info.uptime / 3600 / 24;
    int hours = (info.uptime / 3600) % 24;
    int minutes = (info.uptime / 60) % 60;
    int sec = info.uptime % 60;
    printf("Day=%d,Hours=%d,Min=%d,Sec=%d\n", days, hours, minutes, sec);
    char MAC[50] = "251176215904536";
    char SSID[50] = "¢©®Almond+ 5GHz-gvWezw";
    char SSID2[50] = "Almond+ 2.4GHz-abxsgsd";
    unsigned char IV[33];
    unsigned char deIV[33];
    unsigned char uptime[33];
    sprintf(uptime, "%d", info.uptime);
    printf("Uptime string=%s\n", uptime);
    memset(IV, '\0', sizeof (IV));
    memset(deIV, '\0', sizeof (deIV));
    int i;
    for (i = 0; i < 32; i++) {
        IV[i] = (uptime[i] + MAC[i] + SSID[i] + SSID2[i]) % 94 + 33;
    }
    printf("IV=%s\n", IV);
    unsigned char encryptBuff[32] = "admin";

    encrypt(encryptBuff, sizeof (encryptBuff), IV, key, KEY_LENGTH);
    // printf("Encrypted buff=%s\n", encryptBuff);
    display(encryptBuff, strlen(encryptBuff));
    int epoc = 0;
    epoc = days * 3600 * 24 + hours * 3600 + minutes * 60 + sec;
    printf("calculated time=%d\n", epoc);
    sprintf(uptime, "%d", epoc);
    for (i = 0; i < 32; i++) {
        deIV[i] = (uptime[i] + MAC[i] + SSID[i] + SSID2[i]) % 94 + 33;
    }

    printf("deIV=%s\n", deIV);
    decrypt(encryptBuff, strlen(encryptBuff), deIV, key, KEY_LENGTH);
    printf("decrypted buff=%s\n", encryptBuff);
}
