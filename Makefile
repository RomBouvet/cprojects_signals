EXE1=primal
EXE2=user
CC=gcc
C_FLAGS=-Wall -pedantic -O3
C_LIBS=
OBJ1=conn_handler.o primal.o
OBJ2=user.o

.PHONY : clean mrproper

all: $(EXE1) $(EXE2)
	echo "Build finished"

$(EXE1):$(OBJ1)
	$(CC) $^ -o $@ $(C_LIBS)

$(EXE2):$(OBJ2)
	$(CC) $^ -o $@ $(C_LIBS)

%.o:%.c
	$(CC) $(C_FLAGS) -o $@ -c $<

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXE1) $(EXE2)
