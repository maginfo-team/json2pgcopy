json2copy: json2copy.o main.o ../jsmn/libjsmn.a

clean:
	rm -rf json2copy *.o

json2copy_tests: json2copy.o

tests: json2copy_tests
	./json2copy_tests
