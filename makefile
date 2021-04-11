all:
	cc asciidash.c -lncurses -g -o asciidash
run: all
	./asciidash level
debug: all
	gdb -tui --args ./asciidash level
