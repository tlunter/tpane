objects := $(patsubst src/%.c,obj/%.o,$(wildcard src/*.c))
exe := tpane

all: $(exe)

$(exe): $(objects)
	gcc -I inc -o $(exe) $(objects)

obj/%.o: src/%.c
	mkdir -p obj
	gcc -I inc -c -o $@ $<

.PHONY: all
