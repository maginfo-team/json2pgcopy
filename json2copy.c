#include "json2copy.h"
#include "dbg.h"

FieldValue convert_string(char *source, int bytes) {
    FieldValue ret = { .data = source, .bytes = bytes };
    return ret;
}

char *read_line(FILE *fp, size_t init_bufsize) {
    char *line;
    char ch;
    size_t len = 0;
    int bufsize = init_bufsize;

    line = malloc(init_bufsize);
    while ( 1 ) {
        ch = fgetc(fp);
        if ( ch == EOF && len == 0 ) {
            return NULL;
        }
        if ( ch == EOF || ch == '\n') {
            line[len] = '\0';
            return line;
        }
        if ( len == bufsize ) {
            bufsize += bufsize;
            line = realloc(line, bufsize);
        } 
        line[len++] = ch;
    }
}

size_t write_val(FILE *fp, FieldValue val) {
    char len[4];
    size_t count = 0;
    len[0] = (val.bytes >> 24) & 0xff;
    len[1] = (val.bytes >> 16) & 0xff;
    len[2] = (val.bytes >> 8) & 0xff;
    len[3] = val.bytes & 0xff;
    check(count += fwrite(&len, 4, 1, stdout), "Failed to write value size to stdout");
    check(count += fwrite(val.data, val.bytes, 1, stdout), "Failed to write value to stdout");
    return count;

error:
    return 0;
}

size_t write_null(FILE *fp) {
    int32_t n = -1;
    size_t count = 0;
    check(count += fwrite((char *)&n, 4, 1, fp), "Failed to write NULL");
    return count;

error:
    return 0;
}
