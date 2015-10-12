CFLAGS=-g
json2pgcopy: json2pgcopy.o main.o ../jsmn/libjsmn.a

clean:
	rm -rf json2pgcopy json2pgcopy_tests *.o

json2copy_tests: json2pgcopy.o

tests: json2pgcopy_tests
	./json2pgcopy_tests
