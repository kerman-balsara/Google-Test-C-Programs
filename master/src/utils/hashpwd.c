#include <errno.h>
#include <stddef.h>

#include <crypt.h>
#include <stdlib.h>
#include <string.h>

#include <errcodes.h>
#include <typedefs.h>

#include <cyclicstr.h>
#include <hashpwd.h>
#include <sysexit.h>

string_t * HashPwd(constr_t passwd)
{
    char *setting = crypt_gensalt_ra("$y$", 10, NULL, 0);
    if (!setting)
    {
        int sysErrNo = errno;
        SysExit(__func__, sysErrNo, SYSCALL_CRYPT_GENSALT_RA);
    }

    void *cryptData = NULL;
    int szCryptData = 0;
    char * hashPwd = crypt_ra(passwd, setting, &cryptData, &szCryptData);
    if (!hashPwd)
    {
        int sysErrNo = errno;
        free(setting);
        free(cryptData);
        SysExit(__func__, sysErrNo, SYSCALL_CRYPT_RA);
    }

    free(setting);
    free(cryptData);

    return(CyclicStrAdd(hashPwd));
}

int ValidatePwd(constr_t passwd, constr_t setting)
{
    void *cryptData = NULL;
    int szCryptData = 0;
    char * hashPwd = crypt_ra(passwd, setting, &cryptData, &szCryptData);
    if (!hashPwd)
    {
        int sysErrNo = errno;
        free(cryptData);
        SysExit(__func__, sysErrNo, SYSCALL_CRYPT_RA);
    }

    if (strcmp(hashPwd, setting))
    {
        free(cryptData);
        return(1);
    }
        
    free(cryptData);
    return(0);
}