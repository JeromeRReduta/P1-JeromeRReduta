# Output binary name
bin=inspector

# Set the following to '0' to disable log messages:
LOGGER ?= 1

# Compiler/linker flags
CFLAGS += -g -Wall -fPIC -DLOGGER=$(LOGGER)
LDLIBS += -lm -lncurses
LDFLAGS +=

# Source C files
src=inspector.c procfs.c display.c util.c os_searcher.c string_extractor.c perc_bar_drawer.c uptime_formatter.c cpu_usage.c mem_usage.c
obj=$(src:.c=.o)

# Makefile recipes --
all: $(bin) libinspector.so

$(bin): $(obj)
	$(CC) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(obj) -o $@

libinspector.so: $(obj)
	$(CC) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(obj) -shared -o $@

docs: Doxyfile
	doxygen

clean:
	rm -f $(bin) $(obj) libinspector.so
	rm -rf docs

# Individual dependencies --
inspector.o: inspector.c logger.h hostname_reader.c hostname_reader.h
procfs.o: procfs.c procfs.h logger.h
display.o: display.c display.h procfs.h util.h logger.h
util.o: util.c util.h logger.h
os_searcher.o: os_searcher.c os_searcher.h
string_extractor.o: string_extractor.c string_extractor.h
perc_bar_drawer.o: perc_bar_drawer.c perc_bar_drawer.h
uptime_formatter.o: uptime_formatter.c uptime_formatter.h
cpu_usage.o: cpu_usage.c cpu_usage.h
mem_usage.o: mem_usage.c mem_usage.h

# Tests --
test: $(bin) libinspector.so ./tests/run_tests
	@DEBUG="$(debug)" ./tests/run_tests $(run)

testupdate: testclean test

./tests/run_tests:
	rm -rf tests
	git clone https://github.com/USF-OS/Inspector-Tests.git tests

testclean:
	rm -rf tests
