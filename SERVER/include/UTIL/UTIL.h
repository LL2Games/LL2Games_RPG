#include <iconv.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

namespace UTIL
{
    char* ANSIToUTF8(const char* pszCode);
    char* UTF8ToANSI(const char* pszCode);   
    void  FREE(char *p);
}