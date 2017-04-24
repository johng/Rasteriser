# project name (generate executable with this name)
TARGET   = Main

CC       = g++
# compiling flags here
CFLAGS   = -pipe -Wall -Wno-switch -ggdb -g3 -Ofast -std=c++11


SDL_CFLAGS := $(shell sdl-config --cflags)
GLM_CFLAGS := -I$(GLMDIR)
SDL_LDFLAGS := $(shell sdl-config --libs)

LINKER   = g++ -o
# linking flags here
LFLAGS   = -Wall

# change these to proper directories where each file should be
BINDIR   = .
SRCDIR   = Source
OBJDIR   = Objects

SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
rm       = rm -f

print-%  : ; @echo $* = $($*)

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(LINKER) $@ $(LFLAGS) $(OBJECTS) $(SDL_LDFLAGS)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) $(SDL_CFLAGS) $(GLM_CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
	$(rm) $(OBJECTS) Main
	@echo "Cleanup complete!"


.PHONY: remove
remove: clean
	$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"
