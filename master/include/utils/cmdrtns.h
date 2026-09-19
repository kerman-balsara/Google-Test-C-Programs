#ifndef CMDRTNS_H
#define CMDRTNS_H

// ---------------------
// Internal definitions.
// ---------------------

#include <typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------
// Process commands that enable/disable log levels and log packages.
// The returned response is a malloc'd string so the caller has to
// pass an address of a pointer.
// 
// RETURN VALUE
// Length of the string excluding the terminating null character.
//
// SIDE EFFECTS
// Passed response string is set. The caller is responsible for
// freeing the string.
//
// DESCRIPTION
// Currently, the following commands are valid.
// ?log
// Response contains the current log levels enabled/disabled.
//
// ?log:<comma-separated list of log levels to be enabled/disabled>
// Response contains the current log levels.
//
// ?pkg
// Response contains the current packages enabled/disabled.
//
// ?pkg:<comma-separated list of log packages to be enabled/disabled>
// Response contains the current packages enabled/disabled.
//
// For invalid commands, the response starts with "ERROR: "
// followed by the reason for failure.
//
// --------------------------------------------------------------------
int CmdProc(const buffer_t * const cmd, const int cmdLen, string_t ** response);

#ifdef __cplusplus
}
#endif

#endif // CMDRTNS_H