#ifndef MSGLENIND_H
#define MSGLENIND_H

// Message is null terminated (there is no message length field).
// For a message ABC, the length transmitted is 4 bytes.
// The message length is 3.
#define MSG_WITH_NULL             'N'

// Message length field of size SZ_MSGLEN_IN_MSG begins the message.
// For a message ABC, the length transmitted is 3 + SZ_MSGLEN_IN_MSG bytes.
// The message length field is 3 + SZ_MSGLEN_IN_MSG.
// The message length is 3.
#define MSG_WITH_TOTLEN           'T'

// Message length field of size SZ_MSGLEN_IN_MSG begins the message.
// For a message ABC, the length transmitted is 3 + SZ_MSGLEN_IN_MSG bytes.
// The message length field is 3.
// The message length is 3.
#define MSG_WITH_MSGLEN           'M'

#endif	// MSGLENIND_H
