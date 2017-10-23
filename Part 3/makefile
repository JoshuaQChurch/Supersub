###############################################################################
# purpose: Assignment 1 Ed Solution
# created: 8-25-2015
#  author: J. Edward Swan II
###############################################################################
.PRECIOUS = parser.cpp lexer.h lexer.cpp interpreter.cpp interpreter.h

supersub.exe: lexer.o parser.o interpreter.o
	g++ -g -o $@ $^

lexer.o: lexer.cpp lexer.h interpreter.h
	g++ -o $@ -O3 -c lexer.cpp

interpreter.o: interpreter.cpp lexer.h interpreter.h
	g++ -o $@ -O3 -c interpreter.cpp

parser.o: parser.cpp lexer.h interpreter.h
	g++ -o $@ -O3 -c parser.cpp

clean: 
	$(RM) *.o *~ supersub.exe

