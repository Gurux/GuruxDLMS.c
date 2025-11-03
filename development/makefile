# Generate STATIC library.
TARGET   = libgurux_dlms_c.a

#arm-none-eabi-gcc for embedded ARM systems.
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
CARGO_MANIFEST := dlms_cosem_rs/Cargo.toml

.PHONY: cargo-fmt
cargo-fmt:
	@cargo fmt --manifest-path $(CARGO_MANIFEST)

.PHONY: cargo-clippy
cargo-clippy:
	@cargo clippy --manifest-path $(CARGO_MANIFEST) --all-targets --all-features

.PHONY: cargo-test
cargo-test:
	@cargo test --manifest-path $(CARGO_MANIFEST) --all-features

.PHONY: cargo-doc
cargo-doc:
	@cargo doc --manifest-path $(CARGO_MANIFEST) --all-features --no-deps
