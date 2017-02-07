objects := $(patsubst src/%.c,obj/%.o,$(wildcard src/*.c))
exe := tpane

all: $(exe)

clean:
	rm -r obj
	rm tpane

$(exe): $(objects)
	gcc -I inc -Wall -o $(exe) $(objects)

obj/%.o: src/%.c
	mkdir -p obj
	gcc -I inc -Wall -c -o $@ $<

.PHONY: all clean
