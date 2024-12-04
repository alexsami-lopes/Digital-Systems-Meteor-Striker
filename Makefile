# Variables
CC = gcc
CFLAGS = -Wall -std=gnu99 -Wextra -pthread
LIBS = -lintelfpgaup -lm 
SRCS = src/meteor_striker.c src/meteor.c src/mouse_module.c src/gen_sprites.c src/graphics_fpga.c src/gen_cover.c src/gen_background_animation.c src/keys.c src/input.c src/colision_module.c src/draw_trophy_to_bg.c # Arquivos .c
ASM_SRCS = src/mem.s #src/graphics_fpga.s  # Arquivo assembly

OBJS = $(SRCS:.c=.o) $(ASM_SRCS:.s=.o)

# Executable
TARGET = meteor_striker

# Compilation rule
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Compile C source files
src/%.o: src/%.c include/%.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compile assembly source file
src/%.o: src/%.s
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS) $(TARGET)

excecute:
	sudo ./meteor_striker 


