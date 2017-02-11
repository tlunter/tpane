objects := $(patsubst src/%.c,obj/%.o,$(wildcard src/*.c))
exe := tpane
flags := -I inc -Wall -lm -g

all: $(exe)

clean:
	rm -rf obj
	rm -f tpane

$(exe): $(objects)
	gcc $(flags) -o $(exe) $(objects)

obj/%.o: src/%.c
	mkdir -p obj
	gcc $(flags) -c -o $@ $<

.PHONY: all clean
