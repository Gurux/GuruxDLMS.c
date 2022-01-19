# project name (generate executable with this name)
TARGET   = gurux.dlms.server.bin

CC       = gcc

# compiling flags here
CFLAGS   = -std=gnu99 -Wall -I. -DGX_DLMS_MICROCONTROLLER -DDLMS_USE_EPOCH_TIME -DDLMS_IGNORE_CLIENT -DDLMS_IGNORE_MALLOC

LINKER   = gcc -o

DLMS_SRCDIR = ../development/src
SRCDIR      = src
OBJDIR      = obj
BINDIR      = bin

DLMS_SOURCES  := $(wildcard $(DLMS_SRCDIR)/*.c)
SOURCES  := $(wildcard $(SRCDIR)/*.c)
$(info $(SOURCES))
$(info $(DLMS_SOURCES))
$(info $(DLMS_SRCDIR))

OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
DLMS_OBJECTS  := $(DLMS_SOURCES:$(DLMS_SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f

$(BINDIR)/$(TARGET): $(OBJECTS) $(DLMS_OBJECTS)
	@$(LINKER) $@ $(OBJECTS) $(DLMS_OBJECTS) -lpthread -lm
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(info $(OBJECTS))
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

$(DLMS_OBJECTS): $(OBJDIR)/%.o : $(DLMS_SRCDIR)/%.c
	$(info $(DLMS_OBJECTS))
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