CC = gcc
TARGET = intro_exe

$(TARGET) : main.o rv32i.o
        $(CC) -o $(TARGET) rv32i.o main.o

rv32i.o : rv32i.c
        $(CC) -c -o rv32i.o rv32i.c

main.o : main.c
        $(CC) -c -o main.o main.c

clean :
        rm *.o intro_exe
