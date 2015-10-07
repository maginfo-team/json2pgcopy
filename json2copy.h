#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

typedef struct {
    char *data;
    int32_t bytes;
} FieldValue;

typedef FieldValue(*convert_func)(char *source, int bytes);

typedef struct {
    char *source_field;
    convert_func func;
} FieldConfig;

char *read_line(FILE *fp, size_t init_bufsize);
size_t write_val(FILE *fp, FieldValue val);
size_t write_null(FILE *fp);

FieldValue convert_string(char *source, int bytes);
