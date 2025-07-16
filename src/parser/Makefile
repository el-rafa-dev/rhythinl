#precisa incluir mais arquivos.... por enquanto o makefile não ta funcionando como deveria

default_target: all
.PHONY: clean

cc_files := parser_test.cc r_parser.cc 
hpp_files := r_parser.hpp ../../src/includes/* ../../src/tokens/*
pname := ptest


all: $(pname)

$(pname):
	$(CXX) $(cc_files) $(hpp_files) -o $(pname)

clean:
	rm -rf ptest 