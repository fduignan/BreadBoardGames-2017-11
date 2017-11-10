// serial.h
// Functions available in the stm32f0discovery serial library
// Author: Frank Duignan
#define NEWLINE 0x0d
#define LINEFEED 0x0a

void initUART(int BaudRate);
int ReadCom(int Max,char *Buffer);
int WriteCom(int Count,const char *Buffer);
int eputs(const char *String);
int egets(char *String, int size);
