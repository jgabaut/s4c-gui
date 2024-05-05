AUTOMAKE_OPTIONS = foreign
CFLAGS = @CFLAGS@
SHELL := /bin/bash
.ONESHELL:
MACHINE := $$(uname -m)
PACK_NAME = $(TARGET)-$(VERSION)-$(OS)-$(MACHINE)
s4c_gui_SOURCES = src/s4c_gui.c src/main.c
LDADD = $(S4C_GUI_LDFLAGS)
AM_LDFLAGS = -O2
AM_CFLAGS = $(S4C_GUI_CFLAGS) -O2 -Werror -Wpedantic -Wall
if DEBUG_BUILD
AM_LDFLAGS += -ggdb -O0
AM_CFLAGS +=
else
AM_LDFLAGS += -s
endif
%.o: %.c
	$(CCOMP) -c $(CFLAGS) $(AM_CFLAGS) $< -o $@
$(TARGET): $(s4c_gui_SOURCES:.c=.o)
	@echo -e "    AM_CFLAGS: [ $(AM_CFLAGS) ]"
	@echo -e "    LDADD: [ $(LDADD) ]"
	$(CCOMP) $(CFLAGS) $(AM_CFLAGS) $(s4c_gui_SOURCES:.c=.o) -o $@ $(LDADD) $(AM_LDFLAGS)
clean:
	@echo -en "Cleaning build artifacts:  "
	-rm $(TARGET)
	-rm src/*.o
	@echo -e "Done."
cleanob:
	@echo -en "Cleaning object build artifacts:  "
	-rm src/*.o
	@echo -e "Done."
anviltest:
	@echo -en "Running anvil tests."
	./anvil -tX
	@echo -e "Done."
all: $(TARGET)
rebuild: clean all
.DEFAULT_GOAL := all
