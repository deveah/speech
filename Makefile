
CC=clang
CFLAGS=-Wall -Wextra
LDFLAGS=-lsndfile -lm
TARGET=speech

all: $(TARGET)
    
$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(TARGET).c
