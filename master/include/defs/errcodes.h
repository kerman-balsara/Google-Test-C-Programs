#ifndef ERRCODES_H
#define ERRCODES_H

#define INVALID_PERSON_REC                              100
#define INVALID_PMTTXN_REC                              101
#define INVALID_RECPRM_REC                              102
#define INVALID_RECPRM_ARCHIVE                          103
#define INVALID_RECPRM_EMAIL                            104
#define RECEIPT_ANOTHER_USER                            105

#define SYSCMD_FAILED                                   110

#define STR_SIZE_EXCEEDED                               120
#define STR_QUOTES_INVALID                              123
#define LENGTH_INVALID                                  124
#define MSGLEN_IND_INVALID                              125

#define INVALID_CLIENT_FD                               130

#define INVALID_LOGPKG                                  140
#define INVALID_HEX_CHAR                                141

#define INVALID_DB_MSG                                  150
#define INVALID_BANK_MSG                                151

#define SYSCALL_POPEN                                   1000
#define SYSCALL_FGETS                                   1001
#define SYSCALL_PCLOSE                                  1002
#define SYSCALL_STAT                                    1003
#define SYSCALL_VSNPRINTF                               1004
#define SYSCALL_FOPEN                                   1005
#define SYSCALL_FPRINTF                                 1006
#define SYSCALL_VFPRINTF                                1007
#define SYSCALL_FPUTC                                   1008
#define SYSCALL_FPUTS                                   1009
#define SYSCALL_FCLOSE                                  1010
#define SYSCALL_MALLOC                                  1011
#define SYSCALL_REALLOC                                 1012
#define SYSCALL_FORK                                    1013
#define SYSCALL_SETSID                                  1014
#define SYSCALL_CLOSE                                   1015
#define SYSCALL_GETRLIMIT                               1016
#define SYSCALL_SETRLIMIT                               1017
#define SYSCALL_GETCWD                                  1018
#define SYSCALL_OPEN                                    1019
#define SYSCALL_DUP2                                    1020
#define SYSCALL_UNLINK                                  1021
#define SYSCALL_SOCKET                                  1022
#define SYSCALL_BIND                                    1023
#define SYSCALL_LISTEN                                  1024
#define SYSCALL_ACCEPT                                  1025
#define SYSCALL_INET_PTON                               1026
#define SYSCALL_SEND                                    1027
#define SYSCALL_RECV                                    1028
#define SYSCALL_POLL                                    1029
#define SYSCALL_CALLOC                                  1030
#define SYSCALL_SHUTDOWN                                1031
#define SYSCALL_EVENTFD                                 1032
#define SYSCALL_READ                                    1033
#define SYSCALL_WRITE                                   1034
#define SYSCALL_GETADDRINFO                             1035
#define SYSCALL_CONNECT                                 1036
#define SYSCALL_FCNTL                                   1037
#define SYSCALL_GETSOCKOPT                              1038
#define SYSCALL_CRYPT_GENSALT_RA                        1039
#define SYSCALL_CRYPT_RA                                1040

#define SYSCALL_PT_CREATE                               1200
#define SYSCALL_PT_MUTEX_INIT                           1201
#define SYSCALL_PT_MUTEX_DESTROY                        1202
#define SYSCALL_PT_MUTEX_LOCK                           1203
#define SYSCALL_PT_MUTEX_UNLOCK                         1204
#define SYSCALL_PT_COND_INIT                            1205
#define SYSCALL_PT_COND_WAIT                            1206
#define SYSCALL_PT_COND_SIGNAL                          1207
#define SYSCALL_PT_BARRIER_INIT                         1208
#define SYSCALL_PT_BARRIER_WAIT                         1209
#define SYSCALL_PT_SIGMASK                              1210
#define SYSCALL_PT_KILL                                 1211

#define SYSCALL_SIGACTION                               1400
#define SYSCALL_SIGFILLSET                              1401
#define SYSCALL_SIGEMPTYSET                             1402
#define SYSCALL_SIGADDSET                               1403
#define SYSCALL_SIGWAIT                                 1404
#define SYSCALL_SIGWAITINFO                             1405
#endif	// ERRCODES_H
