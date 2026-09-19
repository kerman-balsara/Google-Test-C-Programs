// Merge two gcov files

// File1 (extract)
// "        -:    0:Source:/home/kerman/projects/GoogleTestC/master/src/utils/pathstatus.c"
// "        1:   27:bool_t PathExists(constr_t path, struct stat * const pathStatus)"
// "        -:   28:{"
// "       1*:   31:        if (errno == ENOENT) return(0);"
// "        1:   33:        SysExit(__func__, sysErrNo, SYSCALL_STAT);"
// "    #####:   36:    return(1);"


// File2 (extract)
// "        -:    0:Source:/home/kerman/projects/GoogleTestC/master/src/utils/pathstatus.c"
// "       27:   27:bool_t PathExists(constr_t path, struct stat * const pathStatus)"
// "        -:   28:{"
// "       13:   31:        if (errno == ENOENT) return(0);"
// "    #####:   33:        SysExit(__func__, sysErrNo, SYSCALL_STAT);"
// "       14:   36:    return(1);"

// Merged file (extract)
// "        -:    0:Source:/home/kerman/projects/GoogleTestC/master/src/utils/pathstatus.c"
// "       28:   27:bool_t PathExists(constr_t path, struct stat * const pathStatus)"
// "        -:   28:{"
// "       14:   31:        if (errno == ENOENT) return(0);"
// "        1:   33:        SysExit(__func__, sysErrNo, SYSCALL_STAT);"
// "       14:   36:    return(1);"

// ---------------------
// External definitions.
// ---------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#define SZ_FULL_PATH_NAME 256
#define SZ_BUFFER         256
typedef struct
{
    char name[SZ_FULL_PATH_NAME];
    FILE * fptr;

    char inbuf[SZ_BUFFER];
    char outbuf[SZ_BUFFER];

    int read;

    int skipline;
    int eof;
    int hitcnt;
    int hitstar;
    int lineno;
    int totallinecnt;
} filen_t;

static int readline(filen_t * const filen);

int main(int argc, char ** argv)
{
    int ret;
    if (argc != 4)
    {
        printf("Usage: mergegcov infile1 infile2 outfile\n");
        exit(1);
    }

    if (strnlen(argv[1], SZ_FULL_PATH_NAME) == SZ_FULL_PATH_NAME)
    {
        printf("File name (infile1) too long\n");
        exit(1);
    }
    if (strnlen(argv[2], SZ_FULL_PATH_NAME) == SZ_FULL_PATH_NAME)
    {
        printf("File name (infile2) too long\n");
        exit(1);
    }
    if (strnlen(argv[3], SZ_FULL_PATH_NAME) == SZ_FULL_PATH_NAME)
    {
        printf("File name (outfile) too long\n");
        exit(1);
    }
    filen_t file1;
    filen_t file2;

    memset(&file1, 0, sizeof(filen_t));
    memset(&file2, 0, sizeof(filen_t));

    strlcpy(file1.name, argv[1], sizeof(file1.name));
    strlcpy(file2.name, argv[2], sizeof(file2.name));

    char outfilename[SZ_FULL_PATH_NAME];
    strlcpy(outfilename, argv[3], sizeof(outfilename));

    if (!strcmp(file1.name, file2.name) || !strcmp(file1.name, outfilename) || !strcmp(file2.name, outfilename))
    {
        printf("Specified files cannot be the same\n");
        exit(1);
    }

    file1.fptr = fopen(file1.name, "r");
    if (file1.fptr == NULL)
    {
        printf("Error opening %s\n", file1.name);
        exit(1);
    }

    file2.fptr = fopen(file2.name, "r");
    if (file2.fptr == NULL)
    {
        printf("Error opening %s\n", file2.name);
        (void) fclose(file1.fptr);
        exit(1);
    }

    FILE * outfptr = fopen(outfilename, "w");
    if (outfptr == NULL)
    {
        printf("Error opening %s\n", outfilename);
        (void) fclose(file1.fptr);
        (void) fclose(file2.fptr);
        exit(1);
    }

    int exitcode = 0;

    file1.read = 1;
    file2.read = 1;

    int execlinecnt = 0;

    char srcfilename[SZ_FULL_PATH_NAME];
    memset(srcfilename, 0, sizeof(srcfilename));

    while(1)
    {
        if (!file1.eof && file1.read)
        {
            ret = readline(&file1);
            if (ret == 1)
            {
                exitcode = 1;
                break;
            }
            if (file1.skipline)
                continue;
        }
      
        if (!file2.eof && file2.read)
        {
            ret = readline(&file2);
            if (ret == 1)
            {
                exitcode = 1;
                break;
            }
            if (file2.skipline)
                continue;
        }

        if (file1.eof && file2.eof)
            break;

        if (file1.lineno < file2.lineno)
        {
            file1.read = 1;
            file2.read = 0;

            continue;
        }

        if (file1.lineno > file2.lineno)
        {
            file1.read = 0;
            file2.read = 1;

            continue;
        }
      
        file1.read = 1;
        file2.read = 1;

        if (file1.lineno == 0)
        {
            // "        -:    0:Source:/home/kerman/projects/GoogleTestC/master/src/utils/pathstatus.c"
            if (!memcmp(file1.outbuf, "Sour", 4))
            {
                if (strcmp(file1.outbuf, file2.outbuf))
                {
                    printf("Source files for %s (%s) and %s (%s) do not match\n",
                           file1.name, file1.outbuf,
                           file2.name, file2.outbuf);
                    exitcode = 1;
                    break;      
                }

                fprintf(outfptr, "%s", file1.inbuf);

                strlcpy(srcfilename, file1.outbuf + 7, sizeof(srcfilename));
                char * newlinechar = strchr(srcfilename, '\n');
                if (newlinechar) *newlinechar = '\0';
            }

            // Skip all other lines with line number 0
            continue;
        }

        if (!strcmp(file1.inbuf, file2.inbuf) && file1.hitcnt == 0 && file2.hitcnt == 0)
        {
            fprintf(outfptr, "%s", file1.inbuf);
            continue;
        }

        int outhitcnt = file1.hitcnt + file2.hitcnt;
        int addstar = file1.hitstar && file2.hitstar;

        if (addstar)
        {
            execlinecnt += 5;
            fprintf(outfptr, "%8d*:%5d:%s", outhitcnt, file1.lineno, file1.outbuf);
        }
        else
        {
            execlinecnt += 10;
            fprintf(outfptr, "%9d:%5d:%s", outhitcnt, file1.lineno, file1.outbuf);
        }
    }

    if (exitcode == 0 && *srcfilename == '\0')
    {
        printf("Files %s and %s do not contain a source file name\n", file1.name, file2.name);
        exitcode = 1;
    }

    if (exitcode == 0 && file1.totallinecnt != file2.totallinecnt)
    {
        printf("File line counts for %s (%d) and %s (%d) do not match",
               file1.name, file1.totallinecnt,
               file2.name, file2.totallinecnt);
        exitcode = 1;
    }

    if (exitcode == 0)
    {
        printf("File '%s'\n", srcfilename);
        int pcnti = (100 * execlinecnt * 10) / file1.totallinecnt;
        double pcntd = pcnti / 100.0;
        printf("Lines executed: %.2f%% of %d\n", pcntd, file1.totallinecnt);
        printf("Created '%s'\n", outfilename);
    }

    (void) fclose(file1.fptr);
    (void) fclose(file2.fptr);
    (void) fclose(outfptr);
    
    return(exitcode);
}

static int readline(filen_t * const filen)
{
    filen->skipline = 0;

    if (!fgets(filen->inbuf, SZ_BUFFER - 1, filen->fptr))
    {
        if (ferror(filen->fptr))
        {
            printf("Error reading %s\n", filen->name);
            return(1);
        }

        filen->eof = 1;
    }
    else
    {
        char wkbuf[SZ_BUFFER];

        strlcpy(wkbuf, filen->inbuf, sizeof(wkbuf));
        char *colonptr1 = strchr(wkbuf, ':');
        if (!colonptr1 || !*(colonptr1 + 1))
        {
            filen->skipline = 1;
            return(0);
        }
        char *colonptr2 = strchr(colonptr1 + 1, ':');
        if (!colonptr2)
        {
            filen->skipline = 1;
            return(0);
        }

        int hithyphen = 0;
        int hithash = 0;

        filen->hitcnt = 0;
        filen->hitstar = 0;
        filen->lineno = 0;

        *colonptr1 = '\0';
        if (*(colonptr1 - 1) == '-')
        {
            hithyphen = 1;
        }
        else if (*(colonptr1 - 1) == '#')
        {
            hithash = 1;
            ++filen->totallinecnt;
        }
        else if (*(colonptr1 - 1) == '*')
        {
            filen->hitstar = 1;
            *(colonptr1 - 1) = '\0';
            ++filen->totallinecnt;
        }
        else
        {
            ++filen->totallinecnt;
        }

        if (!hithyphen && !hithash)
            filen->hitcnt = atoi(wkbuf);

        *colonptr2 = '\0';
        filen->lineno = atoi(colonptr1 + 1);

        strlcpy(filen->outbuf, colonptr2 + 1, sizeof(filen->outbuf));
    }

    return(0);
}