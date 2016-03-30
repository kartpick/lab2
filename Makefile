INC=./includes
SRC=./src
OUT=./bin

$(OUT)/lab: $(OUT)/main.o $(OUT)/cmd.o $(OUT)/cmd_as.o
	gcc -o $(OUT)/lab $(OUT)/main.o $(OUT)/cmd.o $(OUT)/cmd_as.o

$(OUT)/main.o: $(SRC)/main.c $(INC)/lab2.h
	gcc -I$(INC) -o $(OUT)/main.o -c $(SRC)/main.c

$(OUT)/cmd.o: $(SRC)/cmd.c $(INC)/lab2.h
	gcc -I$(INC) -o $(OUT)/cmd.o -c $(SRC)/cmd.c

$(OUT)/cmd_as.o: $(SRC)/cmd_as.c $(INC)/lab2.h
	gcc -I$(INC) -o $(OUT)/cmd_as.o -c $(SRC)/cmd_as.c -D_GNU_SOURCE

clean:
	rm $(OUT)/*.o $(OUT)/lab