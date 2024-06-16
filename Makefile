CFLAGS = -I. -Werror
LDFLAGS = -L.
CFLAGS = -O3 -DDEBUG=0

ifdef DEBUG
CFLAGS = -g -O0 -DDEBUG=1
prefix ?= .
bindir ?= $(prefix)
libdir ?= $(prefix)
endif

prefix ?= /usr/local
bindir ?= $(prefix)/bin
libdir ?= $(prefix)/lib64

SOVERSION=0.1.0
LDFLAGS += -Wl,-rpath=$(libdir) 

LDFLAGSLIB = $(LDFLAGS) -Wl,-init,init -Wl,-fini,fini
CFLAGSLIB = -fPIC $(CFLAGS)

LIBS = libstore.so.$(SOVERSION)
LIBS += libcomm.so.$(SOVERSION)

TARGETS = server teststore

all: $(TARGETS)

# LD_LIBRARY_PATH=$PWD ./main
server: server.o $(LIBS)
	$(CC) $(LDFLAGS) $< -lstore -lcomm -o $@

teststore: teststore.o $(LIBS)
	$(CC) $(LDFLAGS) $< -lstore -o $@

%.o: %.c config.h
	$(CC) -c $(CFLAGSLIB) $< -o $@

%.so.$(SOVERSION): %.so
	mv -f $< $@
	ln -sf $@ $< 

%.so: %.o
	$(CC) -shared $(LDFLAGSLIB) -Wl,-soname,$@.$(SOVERSION) $< -o $@

server.o: libstore.h

libstore.o: libstore.h

libcomm.o: libcomm.h

.PHONY: install
install: server libstore.so libcomm.so myserver.service
	cp -f server $(bindir)/server
	cp -f libstore.so.$(SOVERSION) $(libdir)/libstore.so.$(SOVERSION)
	cp -f libcomm.so.$(SOVERSION) $(libdir)/libcomm.so.$(SOVERSION)
	cp -f myserver.service /etc/systemd/system/myserver.service

.PHONY: distclean
distclean:
	systemctl stop myserver || true
	systemctl disable myserver || true
	rm -f $(bindir)/main $(libdir)/libstore.so.* $(libdir)/libcomm.so.* /etc/systemd/system/myserver.service

.PHONY: clean
clean:
	@rm -f *.o *.a $(LIBS) *.so $(TARGETS)
