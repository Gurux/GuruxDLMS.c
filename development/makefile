# Generate STATIC library.
TARGET   = libgurux_dlms_c.a

CC       = gcc
# compiling flags here
CFLAGS   = -g -std=c99 -Wall -fPIC
#Add this if used on microcontroller. -DGX_DLMS_MICROCONTROLLER
#Add this if you don't want to use malloc. -DDLMS_IGNORE_MALLOC
#Add this if you don't want to use object pointers. -DDLMS_IGNORE_OBJECT_POINTERS
#Add this if you want to use EPOC time. -DDLMS_USE_EPOCH_TIME
#Example: make CFLAGS='-DDLMS_IGNORE_OBJECT_POINTERS'

LINKER   = ar rvs
# linking flags here
LFLAGS   = 

# change these to set the proper directories where each files shoould be
SRCDIR   = src
OBJDIR   = obj
BINDIR   = lib

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f


$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONEY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!" 
	@echo $(OBJECTS)

.PHONEY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"