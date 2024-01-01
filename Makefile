CC=gcc
SRC=$(wildcard src/*.c)
OBJS=$(SRC:src/%.c=src/%.o)
BIN=fook
P2SBIN=p2so
#CFLAGS = -Wall -Wno-builtin-declaration-mismatch -Wno-logical-not-parentheses -Wno-unused-but-set-variable -Wno-unused-value -g -ggdb -I injector/include/ -I src -I subhook
#LDFLAGS =  -Wl,-E -ldl -I injector/include/ -I src -I subhook  injector/src/linux/libinjector.a
CFLAGS = -Wall -Wbuiltin-declaration-mismatch -Wno-logical-not-parentheses -Wno-unused-but-set-variable -Wno-unused-value -I injector/include/ -I src -I subhook
LDFLAGS = -Wl,-E -ldl  -I injector/include/ -I src -I subhook  injector/src/linux/libinjector.a

ifeq ($(DEBUG),)
	CFLAGS +=  -Os -s -DPROD=1
	LDFLAGS +=  -fvisibility=hidden -Os -s
else
	CFLAGS += -g -ggdb
endif

export CFLAGS
export LDFLAGS

.PHONY: envi

all: envi fook

envi:
	@if [ -n "$${DEBUG}" ]; then \
		printf "\033[1mDEBUG BUILD\033[0m\n";\
	else \
		printf "\033[1mPROD BUILD\033[0m\n";\
	fi;


p2so:
	@echo compiling \& linking $(P2SBIN)
	@$(CC) p2s/pie2so.c -o $(P2SBIN)

fook: p2so $(OBJS)
	@if [ -z "$${FOOK}" ]; then \
		printf "\033[1;31mFOOK variable is missing\033[0m\n \033[1mmake help\033[0m to get help\n";\
		exit 1;\
	fi;
	@make -C injector/src/linux/ 
	@$(CC) $(CFLAGS) $(FOOK) -c  -o src/hooks.o
	@echo -e "linking $(BIN) with \033[1m$(FOOK)\033[0m"
	@$(CC) $(CFLAGS)  $(OBJS) src/hooks.o -pie -o $(BIN) $(LDFLAGS)
	@./$(P2SBIN) ./$(BIN)

%.o : %.c
	@echo CC $@
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo cleaning fook ...
	@make -C injector/src/linux/ clean
	@rm -f src/*.o $(BIN) $(BIN).so $(P2SBIN)

help:
	@printf "Usage:\n\tmake FOOK=myhooks.c (DEBUG=1)\n\t\n\tDEBUG to set debug flag for /tmp/debug file output\n\tFOOK \033[1mmandatory\033[0m parameter indicating hooks file to use for library\n"
