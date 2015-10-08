#include "minunit.h"
#include "json2copy.h"

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
    char str[] = "abc";
    FieldValue val = convert_integer(str, strlen(str));
    mu_assert(val.bytes == 0, "Value size should be set to 0 on error");
    mu_assert(val.data == NULL, "Value data should be NULL on error");
}

char *test_convert_smallint()
{
    char str[] = "255";
    char expect[] = { 0x00, 0xff };
    FieldValue val = convert_smallint(str, strlen(str));
    mu_assert(val.bytes == 2, "Value length should be equal to 2");
    mu_assert(memcmp(val.data, expect, 2) == 0, "Value data s incorrect");
}

char *test_convert_bigint()
{
    char str[] = "2147483648";
    char expect[] = { 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00 };
    FieldValue val = convert_bigint(str, strlen(str));
    mu_assert(val.bytes == 8, "Value length should be equal to 2");
    mu_assert(memcmp(val.data, expect, 8) == 0, "Value data s incorrect");
}

char *all_tests() {
    mu_suite_start();

    //mu_run_test(test_jsmn);
    mu_run_test(test_convert_string);
    mu_run_test(test_convert_integer_positive);
    mu_run_test(test_convert_integer_negative);
    mu_run_test(test_convert_integer_incorrect);
    mu_run_test(test_convert_smallint);
    mu_run_test(test_convert_bigint);

    return NULL;
}

RUN_TESTS(all_tests);
