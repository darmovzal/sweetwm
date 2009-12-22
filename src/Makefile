PROJECT = sweetwm
CFLAGS := -Wall $(shell pkg-config --cflags lua5.1 x11)
LDFLAGS := $(shell pkg-config --libs lua5.1 x11)
OBJECTS = script.o x11.o func.o main.o

all: $(PROJECT)

$(PROJECT): $(OBJECTS)
	gcc $^ $(LDFLAGS) -o $@

clean:
	rm -f $(PROJECT) $(OBJECTS)

archive: clean
	mkdir -p ../archive
	cd ..; tar c src | gzip > archive/$(PROJECT)_`date +%Y-%m-%d_%H-%M-%S`.tgz

run: build
	./$(PROJECT) sweetwm.lua

server: build
	Xephyr -screen 1280x780 :1 &
	sleep 1
	DISPLAY=:1 xterm -geometry 105x29+0+0 &
	DISPLAY=:1 xterm -geometry 105x59+640+0 -e make run
	pkill Xephyr


