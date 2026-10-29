#ifndef STDIO_FILE_H
#define STDIO_FILE_H

#include <sys/types.h>
#include <stdint.h>

#define FILE_FL_READ 0x01
#define FILE_FL_WRITE 0x02
#define FILE_FL_APPEND 0x04
#define FILE_FL_EOF 0x08
#define FILE_FL_ERR 0x10
#define FILE_FL_UNGOT 0x20
#define FILE_FL_NOBUF 0x40
#define FILE_FL_LINEBUF 0x80
#define FILE_FL_OWN_BUF 0x100

#define UNGET_NONE  (-1)

struct _FILE {
    int fd;
    int flags;
    int ungot;
    uint8_t* buf;
    size_t buf_size;
    size_t buf_pos;
    size_t buf_len;
};

#endif //STDIO_FILE_H
