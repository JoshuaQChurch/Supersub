###############################################################################
# purpose: Assignment 1 Ed Solution
# created: 8-25-2015
#  author: J. Edward Swan II
# edited: 1-29-2017 E.A. Luke
###############################################################################
.PRECIOUS = parser.cpp lexer.h parseTree.h lexer.cpp

parser.exe: lexer.o parser.o
	g++ -g -o $@ $^

lexer.o: lexer.cpp lexer.h
	g++ -o $@ -O3 -c lexer.cpp

parser.o: parser.cpp lexer.h parseTree.h
	g++ -o $@ -O3 -c parser.cpp

clean: 
	$(RM) *.o *~ parser.exe

