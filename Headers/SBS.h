#pragma once
#ifndef  SBS
#define SBS

#ifndef IN
#define IN
#endif // ! IN

#ifndef  OUT
#define OUT
#endif // ! OUT


#ifndef SBSTYPE
#define SBSTYPE

//typedef unsigned int size_t;
typedef unsigned char BYTE;
typedef unsigned int UINT;
typedef unsigned short WORD;
typedef int* PINT;
typedef unsigned int* PSIZE_T;

#endif // !SBSTYPE

#ifdef _WIN32
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <Windows.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/stat.h>
#include <unistd.h>
#endif // ! _WIN32

#define FALSE 0
#define TRUE 1

#ifndef LibOPENSSL
#define LibOPENSSL
#include <openssl/buffer.h>
#include <openssl/bio.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#endif // !LibOPENSSL

// 공용 헤더 파일
#include "crypto.h"
#include "CSBS_Packet.h"

#endif // ! SBS

