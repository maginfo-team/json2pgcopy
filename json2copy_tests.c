#include "minunit.h"
#include "json2copy.h"

/*char *test_jsmn() {
    jsmntok_t tokens[8];
    jsmnerr_t r;
    char str[] = "{\"field\": \"Test string\"}";
    int i;
    jsmn_parser parser;
    jsmn_init(&parser);
    r = jsmn_parse(&parser, str, strlen(str), tokens, 8);
    for (i = 0; i < r; i++ ) {
        debug("type=%d, start=%d, end=%d, size=%d", tokens[i].type, tokens[i].start, tokens[i].end, tokens[i].size);
    }
    return NULL;
}
*/

char *test_convert_string()
{
    int i;
    char str[] = "Test string";
    FieldValue val = convert_string(str, strlen(str));
    mu_assert(val.bytes == strlen(str), "Incorrect value for length");
    mu_assert(strncmp(val.data, str, strlen(str)) == 0, "Incorrect string");
    return  NULL;
}

char *test_convert_integer_positive()
{
    char str[] = "255";
    char expect[] = { 0x00, 0x00, 0x00, 0xff };
    FieldValue val = convert_integer(str, strlen(str));
    mu_assert(val.bytes == 4, "Incorrect value for length");
    mu_assert(memcmp(val.data, expect, 4) == 0, "Incorrect value for data");
}

char *test_convert_integer_negative()
{
    char str[] = "-255";
    char expect[] = { 0xff, 0xff, 0xff, 0x01 };
    FieldValue val = convert_integer(str, strlen(str));
    mu_assert(val.bytes == 4, "Incorrect value for length");
    mu_assert(memcmp(val.data, expect, 4) == 0, "Incorrect value for data");
}

char *test_convert_integer_overflow()
{
    int i;
    char str[] = "2147483647";
    char expect[] = { 0xff, 0xff, 0xff, 0x01 };
    FieldValue val = convert_integer(str, strlen(str));
    mu_assert(val.bytes == 4, "Incorrect value for length");
    for ( i = 0; i < 4; i++ ) debug("%d %x",i,  (unsigned char)val.data[i]);
    mu_assert(memcmp(val.data, expect, 4) == 0, "Incorrect value for data");
}

char *all_tests() {
    mu_suite_start();

    //mu_run_test(test_jsmn);
    mu_run_test(test_convert_string);
    mu_run_test(test_convert_integer_positive);
    mu_run_test(test_convert_integer_negative);
    //mu_run_test(test_convert_integer_overflow);

    return NULL;
}

RUN_TESTS(all_tests);
