// Initial size of reading buffer
#define READ_BUFSIZE 1024
// Maximum tokens count in single json.
#define TOKENS_COUNT 128

// Fields configuration
FieldConfig fields[] = {
    { "author", &convert_string },
    { "score",  &convert_integer },
    { "edited", &convert_bool },
    { "body",  &convert_string }
};

