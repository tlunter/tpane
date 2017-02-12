objects := $(patsubst src/%.c,obj/%.o,$(wildcard src/*.c))
exe := tpane
cc_flags := -I inc -Wall -g
linker_flags := -lm

all: $(exe)

clean:
	rm -rf obj
	rm -f tpane

$(exe): $(objects)
	gcc $(cc_flags) $(linker_flags) -o $(exe) $(objects)

obj/%.o: src/%.c
	mkdir -p obj
	gcc $(cc_flags) -c -o $@ $<

.PHONY: all clean
