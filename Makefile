INC=./includes
SRC=./src
OUT=./bin

$(OUT)/lab: $(OUT)/main.o $(OUT)/cmd.o
	gcc -o $(OUT)/lab $(OUT)/main.o $(OUT)/cmd.o

$(OUT)/main.o: $(SRC)/main.c $(INC)/lab2.h
	gcc -I$(INC) -o $(OUT)/main.o -c $(SRC)/main.c

$(OUT)/cmd.o: $(SRC)/cmd.c $(INC)/lab2.h
	gcc -I$(INC) -o $(OUT)/cmd.o -c $(SRC)/cmd.c

clean:
	rm $(OUT)/*.o $(OUT)/lab