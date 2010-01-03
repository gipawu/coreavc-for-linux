include ../config.mak

install::
	mkdir -p $(PREFIX_SHARE)
	mkdir -p $(PREFIX_EXE)
	cp dshowserver.exe $(PREFIX_SHARE)/
	echo "#!/bin/sh" > $(PREFIX_EXE)/dshowserver
	echo "echo 'Starting wine dshowserver.exe'" >> $(PREFIX_EXE)/dshowserver
	echo 'wine $(PREFIX_SHARE)/dshowserver.exe $$*' >> $(PREFIX_EXE)/dshowserver
	chmod +x $(PREFIX_EXE)/dshowserver
