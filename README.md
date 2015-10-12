# json2pgcopy

## Purpose

This program converts json data to PostgreSQL binary [COPY format](http://www.postgresql.org/docs/9.4/static/sql-copy.html).
It can be used for bulk loading large amounts of data to PostgreSQL database. Originally, this was done for 
[Reddit comment archive](https://www.reddit.com/r/datasets/comments/3bxlg7/i_have_every_publicly_available_reddit_comment/),
but can be customized to use different data and table structure.

## Building

To build program, you need [jsmn library](http://zserge.com/jsmn.html). Check it out to `../jsmn` directory and build
`libjsmn.a` according to installation instructions.

To build json2copy, create `config.h` (see below) and type `make`.

## Usage

Program reads data from stdin and prints result to stdout. Every line of input data should be a valid JSON and 
converted to single table row according to configuration (see below). If some error occurs, line is skipped and warning
message is printed.

## Configuration

Program doesn't have external confituration file and configured by editing `config.h` and re-building.
The main thing to configure is `fields` array. For every field in resulting file, a value should be added to this array.
Every value is a structure consisting of JSON field name and convert function to process it. Program will extract field value from json and pass it to function to convert to PGCOPY format. `null` json values are converted to SQL NULL.
If `NULL` is passed instead of JSON field name, the whole json is passed to convert function.

Currently, the following convert functions exist:

* `convert_smallint`, `convert_integer`, `convert_bigint` - to convert values to postgresql integer types
* `convert_string` - to convert to character types (also works for enums)
* `convert_boolean` - to convert to postgresql boolean type

You can implement your own convert function to convert to column type you need or to make some pre-processing of source value.
See `json2pgcopy.c` for examples.

See `config.sample.h` for configuration example and other settings.

## Example

Suppose, you have json data with the following structure:

    { "author": "Ivan Petrov", "score": 10, "edited": false, "body": "Very smart comment" }

And you want to bulk load it to table:

    CREATE TABLE comments (
        id serial,
        author text,
        score smallint,
        edited boolean,
        txt text
    );

You should configure `fields` as follows:

    FieldConfig fields[] = {
        { "author", &convert_string},
        { "score",  &convert_integer},
        { "edited", &convert_bool }
        { "body",   &convert_string }
    };

Then build the program and run it as follows

    cat data.json | ./json2pgcopy | psql -c 'COPY comments (author, score, edited, txt) FROM STDIN WITH (FORMAT binary);
