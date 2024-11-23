# Compiler and flags
CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -I../external/cglm/include -I../external/glad -I../external/stb -I/usr/local/include/freetype2 -I/usr/local/include/libpng16 -I/usr/local/include/cimgui -I/usr/local/include/SDL2 -I/usr/local/include/ALmixer
CXXFLAGS = $(CFLAGS)
LDFLAGS = -lGL -lglfw3 -ldl -lm  -lfreetype -lcimgui -lALmixer 

# Directories
SRCDIR = src
OBJDIR = obj
BINDIR = out
EXTDIR = external
GLADDIR = $(EXTDIR)/glad
STBDIR = $(EXTDIR)/stb

# Source files
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# External libraries
EXT_OBJS = $(OBJDIR)/glad.o $(OBJDIR)/stb_image.o

# Output binary
TARGET = $(BINDIR)/game

# Create necessary directories
$(shell mkdir -p $(OBJDIR) $(BINDIR))

# Default target
all: $(TARGET)

# Link the binary
$(TARGET): $(OBJS) $(EXT_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 

# Compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -g -o $@ $<

# Compile external libraries
$(OBJDIR)/glad.o: $(GLADDIR)/glad.c
	$(CC) $(CFLAGS) -c -g -o $@ $<

$(OBJDIR)/stb_image.o: $(STBDIR)/stb_image.c
	$(CXX) $(CXXFLAGS) -c -g -o $@ $<

$(OBJDIR)/main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) -c -g -o $@ $<

# Clean up build files
clean:
	rm -rf $(OBJDIR)/*.o $(TARGET)

.PHONY: all clean


