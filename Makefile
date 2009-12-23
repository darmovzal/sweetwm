PROJECT = sweetwm

all: run

build:
	cd src; make

arch: clean
	mkdir -p archive
	tar c include src Makefile $(PROJECT).lua README | gzip > archive/$(PROJECT)_`date +%Y-%m-%d_%H-%M-%S`.tgz

clean:
	cd src; make clean

run: build
	Xephyr -screen 1280x780 :1 &
	sleep 1
	DISPLAY=:1 xterm -geometry 105x29+0+0 &
	DISPLAY=:1 xterm -geometry 105x59+640+0 -e "src/$(PROJECT) $(PROJECT).lua || sleep 1000"
	pkill Xephyr


