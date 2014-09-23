ifeq ($(OS),Windows_NT) # If windows
	SDL_LIB_ROOT = C:/dev/SDL2-2.0.3/i686-w64-mingw32/lib
	SDL_LFLAGS = -L$(SDL_LIB_ROOT) -lmingw32 -lSDL2main -lSDL2
else
	SDL_LFLAGS = -lSDL2main -lSDL2
endif

TARGET		= escape
CC			= gcc
CFLAGS		= -std=c99 -Wall -I. -g $(SDL_LFLAGS)

LINKER		= gcc -o
LFLAGS		= -Wall -I. -lm

SRCDIR		= src
OBJDIR		= obj
BINDIR		= .

SOURCES		:= $(wildcard $(SRCDIR)/*.c)
INCLUDES	:= $(wildcard $(SRCDIR)/*.h)
OBJECTS		:= $(SOURCES:$(SRCDIR)%.c=$(OBJDIR)/%.o)

all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS) $(SDL_LFLAGS)

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONEY: clean
clean:
	@$(RM) $(OBJECTS)

.PHONEY: remove
remove: clean
	@$(RM) $(BINDIR)/$(TARGET)
