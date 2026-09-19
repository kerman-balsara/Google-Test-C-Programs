// ---------------------
// External definitions.
// ---------------------

#include <errno.h>

#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <errcodes.h>
#include <syssizes.h>
#include <typedefs.h>

#include <logrtns.h>
#include <strutils.h>
#include <sysexit.h>

// --------------------------------------------------------------------
// Check whether length of passed string is within the passed range
// minLen and maxLen. The function fails for:
// - Passed minLen <= 0
// - Passed maxLen <= 0
// - Passed minLen > maxLen
//
// Return:
// 1 - Valid
// 0 - Invalid
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/strutils.cpp
// --------------------------------------------------------------------

bool_t IsValidStrLenMinMax(constr_t str,
                           const int minLen,
                           const int maxLen)
{
    if (minLen <= 0)
    {
        LogErrorF("Negative or zero length. Min Len: %d", minLen, maxLen);
        SysExit(__func__, 0, LENGTH_INVALID);
    }
    if (maxLen <= 0)
    {
        LogErrorF("Negative or zero length. Max Len: %d", maxLen);
        SysExit(__func__, 0, LENGTH_INVALID);
    }
    if (minLen > maxLen)
    {
        LogErrorF("Minimum length exceeds maximum. Min Len: %d, Max Len: %d", minLen, maxLen);
        SysExit(__func__, 0, LENGTH_INVALID);
    }

   int len = strlen(str);

   return(len >= minLen && len <= maxLen);
}

// --------------------------------------------------------------------
// Hide user name part of email address.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/strutils.cpp
// --------------------------------------------------------------------

void EmailAddressUserHide(string_t emailAddress[SZ_EMAIL_ADDRESS + 1])
{
    int lenEmail = strlen(emailAddress);
    const string_t * strAfterAtSign = strchr(emailAddress, '@');
    int lenAfterAtSign = strlen(strAfterAtSign);

    memset(emailAddress + 1, '*', lenEmail - lenAfterAtSign - 1);
}

// --------------------------------------------------------------------
// Hide document number.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/strutils.cpp
// --------------------------------------------------------------------

void PmtDocNoHide(string_t docNo[SZ_PMT_DOC_NO + 1])
{
    memset(docNo, '*', strlen(docNo) - 2);
}

// --------------------------------------------------------------------
// Remove trailing new-line from passed string.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/strutils.cpp
// --------------------------------------------------------------------

string_t * StrRmTrailNewLine(string_t * const str)
{
   	string_t * charBeforeNull = strchr(str, '\0') - 1;
    if (*charBeforeNull == '\n') *charBeforeNull = 0;
    return(str);
}

// --------------------------------------------------------------------
// Convert short value to buf.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/strutils.cpp
// --------------------------------------------------------------------

void ShortToBuf(const short value, buffer_t buf[2])
{
    // Extract LSB from value
    buf[1] = (unsigned char) (value & 0xFF);

    // Extract MSB from value
    buf[0] = (unsigned char) ((value >> 8) & 0xFF);
}

// --------------------------------------------------------------------
// Return short value for passed buf.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/strutils.cpp
// --------------------------------------------------------------------

short BufToShort(const buffer_t buf[2])
{
    unsigned char msb = buf[0];
    unsigned char lsb = buf[1];

    int16_t int16Value = (int16_t)((msb << 8) | lsb);

    return((short) int16Value);
}

void BufPrintf(buffer_t * const buf, const int bufLen, constr_t format, ...)
{
    if (bufLen <= 0)
    {
        LogErrorF("Negative or zero length. bufLen: %d", bufLen);
        SysExit(__func__, 0, LENGTH_INVALID);
    }

    string_t str[bufLen + 1];

    va_list arg;
    va_start(arg, format);
    int ret = vsnprintf(str, sizeof(str), format, arg);
    va_end(arg);

    if (ret < 0)
    {
        int sysErrNo = errno;
        SysExit(__func__, sysErrNo, SYSCALL_VSNPRINTF);
    }

    if (ret >= sizeof(str))
    {
        LogErrorF("String length exceeds passed buffer length. strLen: %d, bufLen: %d", ret, bufLen);
        SysExit(__func__, 0, STR_SIZE_EXCEEDED);
    }

    StrToBuf(str, buf, bufLen);
}

// --------------------------------------------------------------------
// Convert passed buf to string str returning the length of resulting
// string. Any trailing spaces in buf are removed. The function fails
// for:
// - Passed bufLen <= 0
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/strutils.cpp
// --------------------------------------------------------------------

int BufToStr(const buffer_t * const buf, const int bufLen, string_t * const str)
{
    if (bufLen <= 0)
    {
        LogErrorF("Negative or zero length. bufLen: %d", bufLen);
        SysExit(__func__, 0, LENGTH_INVALID);
    }

    const int wklen = BufLen(buf, bufLen);
    const buffer_t * bp = buf;
    buffer_t * sp = str;

    while (bp < buf + wklen && *bp)
    	*sp++ = *bp++;

    *sp = 0;

    return(sp - str);
}

// --------------------------------------------------------------------
// Convert passed string str to buffer buf returning the length of 
// resulting buffer. The function fails for:
// - Passed bufLen <= 0
// - Length of passed string greater than passed bufLen.
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/strutils.cpp
// --------------------------------------------------------------------

int StrToBuf(constr_t str, buffer_t * const buf, const int bufLen)
{
    if (bufLen <= 0)
    {
        LogErrorF("Negative or zero length. bufLen: %d", bufLen);
        SysExit(__func__, 0, LENGTH_INVALID);
    }
    int strLen = strlen(str);
    if (strLen > bufLen)
    {
        LogErrorF("String length exceeds buffer length. strLen: %d, bufLen: %d", strLen, bufLen);
        SysExit(__func__, 0, LENGTH_INVALID);
    }

    memset(buf, ' ', bufLen);

    const char *strPtr = (char *) str;
    char *bufPtr = (char *) buf;

    int wklen = 0;
    while (*strPtr)
    {
        wklen++;
        *bufPtr++ = *strPtr++;
    }

    return(wklen);
}

// --------------------------------------------------------------------
// Return length of passed buf ignoring trailing spaces. The function
// fails for:
// - Passed bufLen <= 0
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/strutils.cpp
// --------------------------------------------------------------------

int BufLen(const buffer_t * const buf, const int bufLen)
{
    if (bufLen <= 0)
    {
        LogErrorF("Negative or zero length. bufLen: %d", bufLen);
        SysExit(__func__, 0, LENGTH_INVALID);
    }

    int wklen = bufLen;
    buffer_t *bufPtr = (buffer_t *) buf;

    for (bufPtr += wklen; wklen && *--bufPtr == ' '; wklen--);
    return(wklen);
}

// --------------------------------------------------------------------
// Convert passed buf to upper case. The function fails for:
// - Passed bufLen <= 0
//
// Testing:
// file:///home/kerman/projects/GoogleTestC/test/utils/strutils.cpp
// --------------------------------------------------------------------

void BufToUpperCase(buffer_t * const buf, const int bufLen)
{
    if (bufLen <= 0)
    {
        LogErrorF("Negative or zero length. bufLen: %d", bufLen);
        SysExit(__func__, 0, LENGTH_INVALID);
    }

    for (int i = 0; i < bufLen; i++)
        buf[i] = toupper(buf[i]);
}

//int NumericBufPack(const buffer_t * buf, const int bufLen, char * const packedBuf)
//{
//    int i;
//
//    char wkbuf[bufLen + 1];
//    int wkbufLen = bufLen;
//
//    if (bufLen % 2)
//    {
//        // In case of an odd length, pad with leading '0'.
//        wkbufLen++;
//        wkbuf[0] = '0';
//        memcpy(&wkbuf[1], buf, bufLen);
//    }
//    else
//    {
//        memcpy(wkbuf, buf, bufLen);
//    }
//    char *wkbufptr = wkbuf;
//
//    int maxItr = wkbufLen / 2;
//    for (i = 0; i < maxItr; i++)
//    {
//        packedBuf[i] = (HexToInt(wkbufptr[0]) << 4) + HexToInt(wkbufptr[1]);
//        wkbufptr += 2;
//    }
//    
//    return(i);
//}   
//
//int HexToInt(char hexChar)
//{
//    if (hexChar >= '0' && hexChar <= '9')
//        return(hexChar - '0');
//    if (hexChar >= 'A' && hexChar <= 'F')
//        return(hexChar - 'A' + 10);
//    if (hexChar >= 'a' && hexChar <= 'f')
//        return(hexChar - 'a' + 10);
//    SysExit(__func__, 0, INVALID_HEX_CHAR);
//}

