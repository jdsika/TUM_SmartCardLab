MCU = atmega128

AS = avr-as
ASFLAGS = -mmcu=$(MCU)
CC = avr-gcc
CFLAGS = -I. -O3 -g3 -mmcu=$(MCU) -I../AVRlib -DF_CPU=4000000 #-DDEBUG_FHREEFISH

OBJS = threefish_common.o threefish_enc.o threefish_dec.o skein.o rand.o

HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

all: libfhreefish.a version run-test

test: test.o ../AVRlib/rprintf.o libfhreefish.a
	$(CC) $(CFLAGS) -o $@ $^
	# check for correct alignment of param struct
	@avr-nm $@ | grep "^008...00 d tparam_hack$$"

.PHONY: run-test
run-test: test
	# Size
	@nm -S libfhreefish.a | perl -nE '$$s{$$2} += hex($$1) if m/([0-9a-z]+) t threefish_size_(common|enc|dec)/; $$s{$$2} += hex($$1) if m/([0-9a-z]+) [tT] skein_([^_]+)/; if (m/skein_rand_seed/) { $$s{total} += $$s{$$_} foreach keys %s; $$s{enc} += $$s{common}; $$s{dec} += $$s{common}; $$s{256} += $$s{enc}; $$s{rand} += $$s{256}; say "$$_: ".$$s{$$_} foreach sort keys %s; }'
	# Speed
	@simulavr -d atmega128 -f test -T exit -W 0x22,-

test.o: performance.h fhreefish.h fkein.h

%.hex: %
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.bin: %
	avr-objcopy -O binary $(HEX_FLASH_FLAGS)  $< $@

libfhreefish.a: $(OBJS)
	$(AR) cru $@ $?

clean:
	-$(RM) -r version test *.o *.a *.bin *.hex *.dump default *~

version: fkein.h
	grep SKEIN_VERSION fkein.h | sed -e 's/.* //;s/.$$//' > $@

dist: version
	mkdir fhreefish-`cat version`
	cp *.c *.h *.s *.aps *.aws Makefile *.txt fhreefish-`cat version`
	zip -9rp fhreefish-`cat version`.zip fhreefish-`cat version`
