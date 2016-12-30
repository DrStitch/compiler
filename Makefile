parser: exp.tab.c lex.yy.c node.c inter.c target.c
	gcc exp.tab.c lex.yy.c node.c inter.c target.c -lfl -o parser

exp.tab.c: exp.y
	bison -d exp.y

lex.yy.c: lex.l
	flex lex.l

clean:
	rm exp.tab.c exp.tab.h lex.yy.c parser

cleanall:
	rm exp.tab.c exp.tab.h lex.yy.c parser *~ .*~

keep:
	tar -czf ../compiler.tar.gz .
