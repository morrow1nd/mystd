# Makeifle for mystd test

CPP=g++
CPPFLAG=-std=c++14 -I../include/ 

TEST_PROGRAMS=$(shell find . -name "*.cpp")

tem:
	$(foreach program, $(TEST_PROGRAMS), echo $(program); )

test: clean
	$(foreach program, $(TEST_PROGRAMS), $(CPP) $(CPPFLAG) $(program) -o $(program).out; echo -n TEST $(program); ./$(program).out; if [ $$? = 0 ]; then echo -e "\033[32m  SUCCESS \033[0m"; fi; )

clean:
	-rm *.out;
	-rm *.stackdump;

.phony: test clean