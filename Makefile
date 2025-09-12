# Makefile for a C project using SDL2

# 1. Compiler and Flags
# ---------------------
# CC: The C compiler to use (gcc is standard on Linux)
# CFLAGS: Flags passed to the compiler for each file.
#   -std=c11: Use the C11 standard.
#   -Wall -Wextra: Enable all major warnings (good practice).
#   -Iinclude: Tell the compiler to look for header files in the 'include' directory.
# LDFLAGS: Flags passed to the linker when creating the final executable.
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Iinclude -lpthread
LDFLAGS =

# Use sdl2-config to automatically get the required flags.
# The 'shell' command runs the command in parentheses and captures its output.
SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LIBS = $(shell sdl2-config --libs)

# Add the SDL flags to our main compiler and linker flags.
CFLAGS += $(SDL_CFLAGS)
LDFLAGS += $(SDL_LIBS)


# 2. Project Structure
# --------------------
# EXECUTABLE: The name of the final program.
# SRCDIR: The directory containing your source (.c) files.
# INCDIR: The directory containing your header (.h) files.
# OBJDIR: A directory to store intermediate compiled files (.o files).
EXECUTABLE = G29
SRCDIR = src
INCDIR = include
OBJDIR = obj


# 3. Automatic File Discovery
# ---------------------------
# wild-card: Find all files in SRCDIR ending with .c and store them in SOURCES.
# patsubst: Take the list of source files, replace 'src/' with 'obj/' and '.c' with '.o'.
#           This creates the list of object files we need to build.
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))


# 4. Build Rules
# --------------
# These are the instructions for 'make'. The first rule is the default.

# The 'all' rule is the default. It depends on the EXECUTABLE.
# .PHONY means 'all' is just a name for a command, not an actual file.
.PHONY: all
all: $(EXECUTABLE)

# Rule to create the final executable.
# It depends on all the object files being created first.
# The command links all the object files together using the C compiler.
# $@ is an automatic variable in Make that means "the target name" (here, 'game').
# $^ means "all the prerequisites" (here, all the .o files).
$(EXECUTABLE): $(OBJECTS)
	@echo "Linking..."
	$(CC) $^ -o $@ $(LDFLAGS)
	@echo "Build finished: Run with ./$(EXECUTABLE)"

# Rule to compile a source file (.c) into an object file (.o).
# This is a pattern rule. It says: to make a file like 'obj/something.o',
# you need a file named 'src/something.c'.
# $< means "the first prerequisite" (the .c file).
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@# Create the object directory if it doesn't exist
	@mkdir -p $(OBJDIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# 5. Clean Rule
# -------------
# A rule to clean up the project by removing compiled files.
.PHONY: clean
clean:
	@echo "Cleaning up..."
	rm -f $(EXECUTABLE)
	rm -rf $(OBJDIR)
