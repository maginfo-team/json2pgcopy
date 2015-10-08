#include "json2copy.h"
#include "dbg.h"

#include "../jsmn/jsmn.h"

FieldValue convert_string(char *source, int len) {

    char *data = strndup(source, len);
    FieldValue ret = { .data = data, .bytes = len };
    return ret;
}

FieldValue convert_integer(char *source, int len) {
    FieldValue ret;
    char *raw_val = strndup(source, len);
    char *errptr;
    int64_t val = strtol(raw_val, &errptr, 10);
    check(*errptr == '\0', "Failed to convert field to long");
    char *data;
    data = malloc(4);
    data[0] = (val >> 24) & 0xff;
    data[1] = (val >> 16) & 0xff;
    data[2] = (val >> 8) & 0xff;
    data[3] = val & 0xff;
    ret.data = data;
    ret.bytes = 4;
    free(raw_val);
    return ret;

error:
    ret.data = NULL;
    ret.bytes = 0;
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
