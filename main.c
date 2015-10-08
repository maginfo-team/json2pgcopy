#include "json2copy.h"
#include "../jsmn/jsmn.h"
#include "dbg.h"

const char PGCOPY_HEADER[19] = { 'P', 'G', 'C', 'O', 'P', 'Y', '\n', 0xff, '\r', '\n', '\0' };
const char PGCOPY_TRAILER[] = { 0xff, 0xff };
jsmn_parser parser;


size_t process_field(char *json, jsmntok_t *tokens, jsmnerr_t tokens_num, FieldConfig field_config) {
    int j;
    for ( j = 0; j < tokens_num; j++ ) {
        if (tokens[j].type == JSMN_STRING && 
                (int) strlen(field_config.source_field) == tokens[j].end - tokens[j].start && 
                strncmp(json + tokens[j].start, field_config.source_field, tokens[j].end - tokens[j].start) == 0) {
            if ( tokens[j+1].type == JSMN_PRIMITIVE &&
                *(json + tokens[j+1].start) == 'n') {
                return write_null(stdout);
            }
            return write_val(stdout, field_config.func(json + tokens[j+1].start, tokens[j+1].end - tokens[j+1].start));
        }
    }
}

#include "config.h"

int main(int argc, char *argv[]) {
    char *line;
    size_t i, j;
    jsmntok_t tokens[TOKENS_COUNT];
    jsmnerr_t r;
    uint16_t fields_count = sizeof(fields) / sizeof(FieldConfig);
    char fields_count_bytes[2];
    fields_count_bytes[0] = ( fields_count >> 8 ) & 0xff;
    fields_count_bytes[1] = fields_count & 0xff;

    check(fwrite(PGCOPY_HEADER, sizeof(PGCOPY_HEADER), 1, stdout), "Failed to write header to stdout");
    while ( 1 ) {
        line = read_line(stdin, READ_BUFSIZE);
        if ( line == NULL ) {
            break;
        } else {
            jsmn_init(&parser);
            r = jsmn_parse(&parser, line, strlen(line), tokens, TOKENS_COUNT);
            if ( r == JSMN_ERROR_INVAL || r == JSMN_ERROR_PART ) {
                log_warn("Invalid json: %s\n", line);
                continue;
            } else if ( r == JSMN_ERROR_NOMEM ) {
                log_warn("Not enought token for line %s. Consider increasing TOKENS_COUNT");
                continue;
            }
            check(fwrite(&fields_count_bytes, sizeof(uint16_t), 1, stdout), "Failed to write fields count to stdout");
            for ( i = 0; i < fields_count; i++ ) {
                /*if ( fields[i].raw_data != NULL ) {
                    write_val(stdout, fields[i].raw_data);
                }*/
                check(process_field(line, tokens, r, fields[i]), "Failed to process field %s in %s", fields[i].source_field, line);
            }

        }
        free(line);
    }
    check(fwrite(PGCOPY_TRAILER, sizeof(PGCOPY_TRAILER), 1, stdout), "Failed to write trailer to stdout");

error:
    return 1;
}