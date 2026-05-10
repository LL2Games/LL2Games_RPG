#include "UTIL.h"

char *UTIL::ANSIToUTF8(const char *pszCode)
{
    if (!pszCode)
        return nullptr;

    iconv_t cd = iconv_open("UTF-8", "CP949"); // to UTF-8, from CP949(=Windows-949)
    if (cd == (iconv_t)-1)
    {
        perror("iconv_open failed");
        return nullptr;
    }

    size_t inlen = strlen(pszCode);
    size_t outlen = inlen * 3 + 1; // UTF-8은 최대 3배 길이
    char *outbuf = new char[outlen];
    memset(outbuf, 0, outlen);

    char *pin = const_cast<char *>(pszCode);
    char *pout = outbuf;

    if (iconv(cd, &pin, &inlen, &pout, &outlen) == (size_t)-1)
    {
        perror("iconv error");
        delete[] outbuf;
        iconv_close(cd);
        return nullptr;
    }

    iconv_close(cd);
    return outbuf;
}

char *UTIL::UTF8ToANSI(const char *pszCode)
{
    if (!pszCode)
        return nullptr;

    iconv_t cd = iconv_open("CP949", "UTF-8"); // to CP949, from UTF-8
    if (cd == (iconv_t)-1)
    {
        perror("iconv_open failed");
        return nullptr;
    }

    size_t inlen = strlen(pszCode);
    size_t outlen = inlen * 3 + 1; // CP949 변환시 여유롭게
    char *outbuf = new char[outlen];
    memset(outbuf, 0, outlen);

    char *pin = const_cast<char *>(pszCode);
    char *pout = outbuf;

    if (iconv(cd, &pin, &inlen, &pout, &outlen) == (size_t)-1)
    {
        perror("iconv error");
        delete[] outbuf;
        iconv_close(cd);
        return nullptr;
    }

    iconv_close(cd);
    return outbuf;
}

void UTIL::FREE(char *p)
{
    if (p)
    {
        delete[] p;
        p = nullptr;
    }
}