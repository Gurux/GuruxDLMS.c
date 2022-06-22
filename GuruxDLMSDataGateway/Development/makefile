# project name (generate executable with this name)
TARGET   = gurux.dlms.datagateway.bin

CC       = gcc

# compiling flags here
CFLAGS   = -std=gnu99 -Wall -I. -DDLMS_USE_EPOCH_TIME

LINKER   = gcc -o

DLMS_SRCDIR = dlms/src
SRCDIR      = src
OBJDIR      = obj
BINDIR      = bin

DLMS_SOURCES  := $(wildcard $(DLMS_SRCDIR)/*.c)
SOURCES  := $(wildcard $(SRCDIR)/*.c)
$(info $(SOURCES))
$(info $(DLMS_SOURCES))

OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
DLMS_OBJECTS  := $(DLMS_SOURCES:$(DLMS_SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f

$(BINDIR)/$(TARGET): $(OBJECTS) $(DLMS_OBJECTS)
	@$(LINKER) $@ $(OBJECTS) $(DLMS_OBJECTS) -lpthread -lm
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

$(DLMS_OBJECTS): $(OBJDIR)/%.o : $(DLMS_SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONEY: clean
clean:
	@$(rm) $(OBJECTS)
	@$(rm) $(DLMS_OBJECTS)
	@echo "Cleanup complete!"
	@echo $(OBJECTS)

.PHONEY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"