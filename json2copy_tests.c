#include "minunit.h"
#include "json2copy.h"

char *test_convert_string()
{
    char str[] = "Test string";
    FieldValue val = convert_string(str, strlen(str));
    mu_assert(val.bytes = strlen(str), "Incorrect value for length");
    mu_assert(strncmp(val.data, str, strlen(str)) == 0, "Incorrect string");
    return  NULL;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_convert_string);

    return NULL;
}

RUN_TESTS(all_tests);
