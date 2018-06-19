all :
	mkdir bin
	gcc tests/tests.c -o tests/tests
	mv tests/tests bin/

clean :
	rm -r bin/

