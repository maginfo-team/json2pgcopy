#define READ_BUFSIZE 1024
#define TOKENS_COUNT 128

FieldConfig fields[] = {
    { "author", &convert_string},
    { "score", &convert_integer},
    { "edited" , &convert_bool }
};

