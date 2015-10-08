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

char *test_convert_integer_incorrect()
{
    int i;
    char str[] = "abc";
    FieldValue val = convert_integer(str, strlen(str));
    mu_assert(val.bytes == 0, "Value size should be set to 0 on error");
    mu_assert(val.data == NULL, "Value data should be NULL on error");
}

char *all_tests() {
    mu_suite_start();

    //mu_run_test(test_jsmn);
    mu_run_test(test_convert_string);
    mu_run_test(test_convert_integer_positive);
    mu_run_test(test_convert_integer_negative);
    mu_run_test(test_convert_integer_incorrect);

    return NULL;
}

RUN_TESTS(all_tests);
