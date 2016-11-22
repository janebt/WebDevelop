WEBSOCKETDPATH = $(shell pwd)
CFLAGS = -Wall -g -Wl,-gc-sections -I$(WEBSOCKETDPATH)/. -I$(WEBSOCKETDPATH)/../public/include -I$(WEBSOCKETDPATH)/../../../staging_dir/target-arm-openwrt-linux-uclibcgnueabi/usr/include
LDFLAGS = -L$(WEBSOCKETDPATH)/../public/lib -L$(WEBSOCKETDPATH)/../../../staging_dir/target-arm-openwrt-linux-uclibcgnueabi/usr/lib
LDFLAGS+= -ljson -lblobmsg_json -luci -lubox -lmsglog -lm -lubus

SRC  := $(wildcard *.c)
OBJ  := $(SRC:%.c=%.o)
TARGET := websocketd

.PHONY : clean all FORCE

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

install: $(TARGET)
	cp -f $(TARGET) $(INSTALL_PREFIX)/sbin

clean:
	@rm -f $(TARGET) $(OBJ)

FORCE:
