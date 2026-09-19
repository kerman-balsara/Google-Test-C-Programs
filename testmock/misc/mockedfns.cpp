// ---------------------
// External definitions.
// ---------------------

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <mockedfns.hpp>

#include <syssizes.h>
#include <typedefs.h>

#include <person.h>

std::shared_ptr<MockedFns> MockedFnsObj;

extern "C"
{
    static void getTestSuiteName(string_t *name, const int nameLen)
    {
        snprintf(name, nameLen, "%s",
            testing::UnitTest::GetInstance()->current_test_suite()->name());
    }

    int DbPersonSelectKey(const int id, person_r * const person)
    {
        return(MockedFnsObj->DbPersonSelectKey(id, person));
    }

    int DbPersonPostReadDecrypt(const person_r * const person, string_t emailAddress[SZ_EMAIL_ADDRESS + 1])
    {
        return(MockedFnsObj->DbPersonPostReadDecrypt(person, emailAddress));
    }

    FILE * llfopen(constr_t fileName, constr_t mode)
    {
        return(MockedFnsObj->llfopen(fileName, mode));
    }

    int llfclose(FILE * const fPtr)
    {
        return(MockedFnsObj->llfclose(fPtr));
    }

    FILE * llpopen(constr_t cmd, constr_t type)
    {
        return(MockedFnsObj->llpopen(cmd, type));
    }

    int llpclose(FILE * const fPtr)
    {
        return(MockedFnsObj->llpclose(fPtr));
    }

    string_t * llfgets(string_t * str, const int len, FILE * const fPtr)
    {
        return(MockedFnsObj->llfgets(str, len, fPtr));
    }

    int llfputs(constr_t str, FILE *fPtr)
    {
        return(MockedFnsObj->llfputs(str, fPtr));
    }

    int llfputc(int c, FILE *fPtr)
    {
        return(MockedFnsObj->llfputc(c, fPtr));
    }

    int llvfprintf(FILE *fPtr, constr_t format, va_list arg)
    {
        return(MockedFnsObj->llvfprintf(fPtr, format, arg));
    }

    int llferror(FILE * const fPtr)
    {
        return(MockedFnsObj->llferror(fPtr));
    }

    void llclearerr(FILE * const fPtr)
    {
        return(MockedFnsObj->llclearerr(fPtr));
    }

    int llunlink(constr_t fileName)
    {
        return(MockedFnsObj->llunlink(fileName));
    }

    int llstat(constr_t path, struct stat * const pathStatus)
    {
        return(MockedFnsObj->llstat(path, pathStatus));
    }

    int llsocket(int domain, int type, int protocol)
    {
        return(MockedFnsObj->llsocket(domain, type, protocol));
    }

    int llbind(int sockfd, const struct sockaddr * const addr, socklen_t addrlen)
    {
        return(MockedFnsObj->llbind(sockfd, addr, addrlen));
    }

    int lllisten(int sockfd, int backlog)
    {
        return(MockedFnsObj->lllisten(sockfd, backlog));
    }

    int llconnect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
    {
        return(MockedFnsObj->llconnect(sockfd, addr, addrlen));
    }

    int llsend(const int fd, const char * const buf, const int buflen, const int flags)
    {
        return(MockedFnsObj->llsend(fd, buf, buflen, flags));
    }

    int llpoll(struct pollfd *fds, int nfds, int timeout)
    {
        return(MockedFnsObj->llpoll(fds, nfds, timeout));
    }

    int llrecv(const int fd, char * const buf, const int buflen, const int flags)
    {
        return(MockedFnsObj->llrecv(fd, buf, buflen, flags));
    }

    int llgetsockoptSocketError(int sockfd, int * so_error, socklen_t * so_len)
    {
        return(MockedFnsObj->llgetsockoptSocketError(sockfd, so_error, so_len));
    }

    int llshutdown(int sockfd, int how)
    {
        return(MockedFnsObj->llshutdown(sockfd, how));
    }

    int llfcntl(int fd, int op, int flags)
    {
        return(MockedFnsObj->llfcntl(fd, op, flags));
    }

    int llclose(int fd)
    {
        return(MockedFnsObj->llclose(fd));

    }
    void * llmalloc(const int size)
    {
        return(MockedFnsObj->llmalloc(size));
    }

    // For some tests, we want to do a real realloc().
    void * llrealloc(void * oldptr, const int size)
    {
        string_t name[128];
        getTestSuiteName(name, sizeof(name));
        if (!strcmp(name, "byclient/SocketRecvByClientF") ||
            !strcmp(name, "yield/SocketRecvYieldF"))
            return(realloc(oldptr, size));

        return(MockedFnsObj->llrealloc(oldptr, size));
    }

}
