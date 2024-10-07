CC = gcc
CFLAGS = -pthread -Wall
TARGET = simulacao_incendio
OBJS = main.o funcoes.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c funcoes.h
	$(CC) $(CFLAGS) -c main.c

funcoes.o: funcoes.c funcoes.h
	$(CC) $(CFLAGS) -c funcoes.c

clean:
	rm -f $(OBJS) $(TARGET)
