all: dshowserver/dshowserver

dshowserver/dshowserver:
	#cd loader && $(MAKE)
	cd dshowserver && $(MAKE)

clean:
	cd loader && $(MAKE) clean
	cd dshowserver && $(MAKE) clean
