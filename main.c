#include "json2copy.h"
#include "dbg.h"
#include "../jsmn/jsmn.h"

const char PGCOPY_HEADER[19] = { 'P', 'G', 'C', 'O', 'P', 'Y', '\n', 0xff, '\r', '\n', '\0' };
const char PGCOPY_TRAILER[] = { 0xff, 0xff };
jsmn_parser parser;


size_t process_field(char *json, jsmntok_t *tokens, jsmnerr_t tokens_num, FieldConfig field_config, FILE *stream) {
    int j;
    size_t r = 0;
    for ( j = 0; j < tokens_num; j++ ) {
        if (tokens[j].type == JSMN_STRING && 
                tokens[j].size > 0 && //This ensures that we found a key
                (int) strlen(field_config.source_field) == tokens[j].end - tokens[j].start && 
                strncmp(json + tokens[j].start, field_config.source_field, tokens[j].end - tokens[j].start) == 0) {
            if ( tokens[j+1].type == JSMN_PRIMITIVE &&
                *(json + tokens[j+1].start) == 'n') {
                r = write_null(stream);
            } else {
                FieldValue val = field_config.func(json+tokens[j+1].start,
                        tokens[j+1].end - tokens[j+1].start);
                check(val.bytes > 0, "Error processing field %s in %s", field_config.source_field, json);
                r = write_val(stream, val);
                free(val.data);
            }
            return r;
        }
    }
    log_warn("Didn't find field %s in json %s", field_config.source_field, json);
    return r;

error:
    return 0;
}

#include "config.h"

int main(int argc, char *argv[]) {
    FILE *out_buf_fp;
    char *line, *out_buf;
    size_t out_buf_size;
    size_t i, j;
    int err;
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
            out_buf_fp = open_memstream(&out_buf, &out_buf_size);
            check(out_buf_fp != NULL, "Failed to create output buffer stream");
            check(fwrite(&fields_count_bytes, sizeof(uint16_t), 1, out_buf_fp), 
                    "Failed to write fields count to buffer");
            err = 0;
            for ( i = 0; i < fields_count; i++ ) {
                if ( ! process_field(line, tokens, r, fields[i], out_buf_fp) ) {
                    log_warn("Failed to process field, skipping line", fields[i].source_field, line);
                    err = 1;
                    break;
                } 
            }
            if ( ! err ) {
                fclose(out_buf_fp);
                fwrite(out_buf, out_buf_size, 1, stdout);
                free(out_buf);
            }

        }
        free(line);
    }
    check(fwrite(PGCOPY_TRAILER, sizeof(PGCOPY_TRAILER), 1, stdout), "Failed to write trailer to stdout");
    return 0;

error:
    return 1;
}
