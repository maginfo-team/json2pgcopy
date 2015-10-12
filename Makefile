CFLAGS=-g
TEST_DB=su
json2pgcopy: json2pgcopy.o main.o ../jsmn/libjsmn.a

clean:
	rm -rf json2pgcopy json2pgcopy_tests *.o

json2copy_tests: json2pgcopy.o

tests: json2pgcopy_tests
	./json2pgcopy_tests

it: json2pgcopy
	psql $(TEST_DB) < sample.sql
	cat sample.json | ./json2pgcopy | psql -c 'COPY comments (author, score, edited, txt) FROM STDIN WITH (FORMAT binary);' $(TEST_DB)
	psql -c 'DROP TABLE comments;' $(TEST_DB)
