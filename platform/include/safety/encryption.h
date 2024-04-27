#ifndef ENCRYPTION_H
#define ENCRYPTION_H

extern void DES_SetKey(char*);
extern void DES_Encryption(char TextMessage[8], char HexMessage[16]);
extern void DES_Decryption(char TextMessage[8], char HexMessage[16]);







#endif