#ifndef MOCKEDFNS_HPP
#define MOCKEDFNS_HPP

// ---------------------
// External definitions.
// ---------------------

#include <gmock/gmock.h>

#include <sys/stat.h>

#include <arpa/inet.h>
#include <stdarg.h>
#include <stdio.h>
#include <poll.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <syssizes.h>
#include <typedefs.h>

#include <person.h>

// Define the interface
class IMockedFns
{
    public:
        virtual ~IMockedFns(){ };

        // = 0 => pure virtual function
        virtual int DbPersonSelectKey(const int id, person_r * const person) = 0;
        virtual int DbPersonPostReadDecrypt(const person_r * const person, string_t emailAddress[SZ_EMAIL_ADDRESS + 1]) = 0;

        virtual FILE * llfopen(constr_t fileName, constr_t mode) = 0;
        virtual int llfclose(FILE * const fPtr) = 0;

        virtual FILE * llpopen(constr_t cmd, constr_t type) = 0;
        virtual int llpclose(FILE * const fPtr) = 0;

        virtual string_t * llfgets(string_t * str, const int len, FILE * const fPtr) = 0;
        virtual int llfputs(constr_t str, FILE *fPtr) = 0;
        virtual int llfputc(int c, FILE *fPtr) = 0;
        virtual int llvfprintf(FILE *fPtr, constr_t format, va_list arg) = 0;

        virtual int llferror(FILE * const fPtr) = 0;
        virtual void llclearerr(FILE * const fPtr) = 0;

        virtual int llunlink(constr_t fileName) = 0;

        virtual int llstat(constr_t path, struct stat * const pathStatus) = 0;

        virtual int llsocket(int domain, int type, int protocol) = 0;
        virtual int llbind(int sockfd, const struct sockaddr * const addr, socklen_t addrlen) = 0;
        virtual int lllisten(int sockfd, int backlog) = 0;
        virtual int llconnect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) = 0;
        virtual int llsend(const int fd, const char * const buf, const int buflen, const int flags) = 0;
        virtual int llrecv(const int fd, char * const, const int buflen, const int flags) = 0;
        virtual int llgetsockoptSocketError(int sockfd, int * so_error, socklen_t * so_len) = 0;
        virtual int llshutdown(int sockfd, int how) = 0;

        virtual int llpoll(struct pollfd *fds, int nfds, int timeout) = 0;

        virtual int llfcntl(int fd, int op, int flags) = 0;

        virtual int llclose(int fd) = 0;

        virtual void * llmalloc(const int size) = 0;
        virtual void * llrealloc(void * ptr, const int size) = 0;
};

// From Google Test Docs:
// Derive a class from the interface.
// Take a virtual function of the interface (while it’s possible to mock non-virtual methods using
// templates, it’s much more involved).
// In the public: section of the child class, write MOCK_METHOD();
// Now comes the fun part: you take the function signature, cut-and-paste it into the macro, and
// add two commas - one between the return type and the name, another between the name and the
// argument list.
// If you’re mocking a const method, add a 4th parameter containing (const) (the parentheses are required).
// Since you’re overriding a virtual method, we suggest adding the override keyword. For const methods
// the 4th parameter becomes (const, override), for non-const methods just (override). This isn’t
// mandatory.
// Repeat until all virtual functions you want to mock are done. (It goes without saying that all
// pure virtual methods in your abstract class must be either mocked or overridden.)
class MockedFns : public IMockedFns
{
    public:
        MOCK_METHOD(int, DbPersonSelectKey, (const int id, person_r * const person), (override));
        MOCK_METHOD(int, DbPersonPostReadDecrypt,
                    (const person_r * const person, string_t emailAddress[SZ_EMAIL_ADDRESS + 1]), (override));

        MOCK_METHOD(FILE *, llfopen, (constr_t fileName, constr_t mode), (override));
        MOCK_METHOD(int, llfclose, (FILE * const fPtr), (override));

        MOCK_METHOD(FILE *, llpopen, (constr_t cmd, constr_t type), (override));
        MOCK_METHOD(int, llpclose, (FILE * const fPtr), (override));

        MOCK_METHOD(string_t *,  llfgets, (string_t * str, const int len, FILE * const fPtr), (override));
        MOCK_METHOD(int, llfputs, (constr_t str, FILE *fPtr), (override));
        MOCK_METHOD(int, llfputc, (int c, FILE *fPtr), (override));
        MOCK_METHOD(int, llvfprintf, (FILE *fPtr, constr_t format, va_list arg), (override));

        MOCK_METHOD(int, llferror, (FILE * const fPtr), (override));
        MOCK_METHOD(void, llclearerr, (FILE * const fPtr), (override));

        MOCK_METHOD(int, llunlink, (constr_t fileName), (override));

        MOCK_METHOD(int, llstat, (constr_t path, struct stat * const pathStatus), (override));

        MOCK_METHOD(int, llsocket, (int domain, int type, int protocol), (override));
        MOCK_METHOD(int, llbind, (int sockfd, const struct sockaddr * const addr, socklen_t addrlen), (override));
        MOCK_METHOD(int, lllisten, (int sockfd, int backlog), (override));
        MOCK_METHOD(int, llconnect, (int sockfd, const struct sockaddr *addr, socklen_t addrlen), (override));
        MOCK_METHOD(int, llsend, (const int fd, const char * const buf, const int buflen, const int flags), (override));
        MOCK_METHOD(int, llrecv, (const int fd, char * const buf, const int buflen, const int flags), (override));
        MOCK_METHOD(int, llgetsockoptSocketError, (int sockfd, int * so_error, socklen_t * so_len), (override));
        MOCK_METHOD(int, llshutdown, (int sockfd, int how), (override));

        MOCK_METHOD(int, llpoll, (struct pollfd *fds, int nfds, int timeout), (override));

        MOCK_METHOD(int, llfcntl, (int fd, int op, int flags), (override));

        MOCK_METHOD(int, llclose, (int fd), (override));

        MOCK_METHOD(void *, llmalloc, (const int size), (override));
        MOCK_METHOD(void *, llrealloc, (void * oldptr, const int size), (override));
};

// Create a global mock object
extern std::shared_ptr<MockedFns> MockedFnsObj;

#endif // MOCKEDFNS_HPP
