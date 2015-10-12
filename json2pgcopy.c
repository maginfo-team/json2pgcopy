#include "json2pgcopy.h"
#include "dbg.h"

#include "../jsmn/jsmn.h"

FieldValue convert_string(char *source, int len) {
    char *data = strndup(source, len);
    FieldValue ret = { .data = data, .bytes = len };
    return ret;
}

FieldValue convert_int(char *source, int len, int int_size) {
    int i;
    FieldValue ret;
    char *raw_val = strndup(source, len);
    char *errptr;
    int64_t val = strtol(raw_val, &errptr, 10);
    check(*errptr == '\0', "Failed to convert field to long");
    char *data;
    data = malloc(int_size);
    for ( i = 0; i < int_size; i++ ) {
        data[i] = (val >> (int_size*8 - 8*(i+1))) & 0xff;
    }
    ret.data = data;
    ret.bytes = int_size;
    free(raw_val);
    return ret;

error:
    free(raw_val);
    ret.data = NULL;
    ret.bytes = 0;
    return ret;
}

FieldValue convert_smallint(char *source, int len) {
    return convert_int(source, len, 2);
}

FieldValue convert_integer(char *source, int len) {
    return convert_int(source, len, 4);
}

FieldValue convert_bigint(char *source, int len) {
    return convert_int(source, len, 8);
}

FieldValue convert_bool(char *source, int len) {
    FieldValue ret;
    char *data = malloc(1);
    if ( source[0] == 't' ) {
        data[0] = '\x1';
    } else if ( source[0] == 'f' ) {
        data[0] = '\0';
    } else {
        free(data);
        ret.data = NULL;
        ret.bytes = 0;
        return ret;
    }
    ret.data = data;
    ret.bytes = 1;
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
            free(line);
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
    check(count += fwrite(&len, 4, 1, fp), "Failed to write value size to stdout");
    check(count += fwrite(val.data, val.bytes, 1, fp), "Failed to write value to stdout");
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
