CFLAGS := -Wall -O3 -fomit-frame-pointer
LDFLAGS := storm.lib

MOD := util

SOURCES := $(MOD).c ini.c
OBJS := $(subst .c,.o,$(SOURCES))

all: $(MOD).lib

$(MOD).lib: $(OBJS) storm.lib fog.lib
	rm -f $@
	TEMP="$(shell mktemp -d .tmp.XXXXXX)" && \
	(cd "$$TEMP" && \
		ar x ../storm.lib && \
		ar x ../fog.lib && \
		ar cru ../$@ *.o) && \
	rm -r "$$TEMP"
	ar cru $@ $(OBJS)

storm.lib: storm.def
	dlltool -k -d $< -l $@

fog.lib: fog.def
	dlltool -k -d $< -l $@

clean:
	rm -f $(MOD).lib storm.lib fog.lib *.o
	rm -rf .tmp.*

-include $(addprefix .,$(subst .c,.d,$(SOURCES)))
.%.d: %.c
	$(CC) $(CFLAGS) -o $@ -MM $<
