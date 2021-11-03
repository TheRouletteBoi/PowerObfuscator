#ifndef __POBF_API_H__
#define __POBF_API_H__

#include <types.h>
#include "Classes.h"
#include "Private.h"
#include "Internal.h"


struct __fnData
{
   int16_t count;
   int32_t start;
   int16_t offset[256];
};

extern __fnData fnData;

extern char PRIV_API_KEY[];

extern int decryptData_START;
extern int decryptData_SIZE;
extern int decryptData_CODE;

#define START_PATTERN 0xAABBCCDD, 0x12345678, 0xEEFFEEFF 
extern int EXPORTS_TOC[];

/*
struct 
{
   int32_t fnData_ptr;
   int32_t decryptData_start;
   int32_t decryptData_code;
};
*/


void pobf_Start(int suppressParameter);

#endif// __POBF_API_H__

