#ifndef LOGRTNS_H
#define LOGRTNS_H

// ---------------------
// Internal definitions.
// ---------------------

#include <loglvldefs.h>
#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

int LogLvlEnable(const int loglvl);
int LogLvlDisable(const int loglvl);
int LogLvlsEnabled(void);
string_t * LogLvlsInfo(void);
int LoglvlFromChar(const char loglvlChar);

void LogErrorF(constr_t format, ...);
void LogError(constr_t str);
void LogWarnF(constr_t format, ...);
void LogWarn(constr_t str);
void LogInfoF(constr_t format, ...);
void LogInfo(constr_t str);
void LogTraceF(constr_t format, ...);
void LogTrace(constr_t str);

#ifdef __cplusplus
}
#endif

#endif // LOGRTNS_H