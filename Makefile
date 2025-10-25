TARGET = bin/final
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	./bin/final -f ./cleveldb.db -n
	./bin/final -f ./cleveldb.db
default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ) | bin
	gcc -o $@ $^

obj/%.o : src/%.c | obj
	gcc -c $< -o $@ -Iinclude

obj:
	mkdir -p obj

bin:
	mkdir -p bin