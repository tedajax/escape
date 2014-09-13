TARGET		= escape
CC			= gcc
CFLAGS		= -std=c99 -Wall -I. -g

LINKER		= clang -o
LFLAGS		= -Wall -I. -lm

SRCDIR		= src
OBJDIR		= obj
BINDIR		= .

SOURCES		:= $(wildcard $(SRCDIR)/*.c)
INCLUDES	:= $(wildcard $(SRCDIR)/*.h)
OBJECTS		:= $(SOURCES:$(SRCDIR)%.c=$(OBJDIR)/%.o)
rm			= rm -f

all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS)

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONEY: clean
clean:
	@$(rm) $(OBJECTS)

.PHONEY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
