# Introduction #

The dshowserver is the only supported way to interface CoreAVC to other Linux applications.  It should run on any platform that Wine works on

# Details #

1) Download the code from svn:
```
svn checkout http://coreavc-for-linux.googlecode.com/svn/trunk/ coreavc-for-linux 
cd coreavc-for-linux
```

2) Install CoreAVC
  * Download CoreAVC installer from CoreCorec website
  * Install using Wine (apply your registration key here)
```
wine "CoreAVC Professional Edition-x.x.x_Setup.exe"
```

3) Install dshowserver
  * Choose one of the following options:
    1. Build winelib version from source using winegcc
```
cd dshowserver && make
```
    1. Build exe version from source using mingw cross-compiler
```
cd dshowserver && make X_COMPILE=1
```
    1. Use included pre-compiled exe in 'precompiled/'
```
cd precompiled
```
  * Install the binaries (they will be installed in /usr/local/bin and /usr/local/share/dshowserver by default, but this can be changed by adding PREFIX=`<path>` to the 'make install' command)
    1. If you are using the precompiled exe or a winelib build:
```
sudo make install
```
    1. If you built a cross-compile build with mingw:
```
sudo make install X_COMPILE=1
```
  * Copy CoreAVCCodec.ax from Wine install to dshowserver dir:
```
sudo cp "$HOME/.wine/drive_c/Program Files/CoreCodec/CoreAVC Professional Edition/CoreAVCDecoder.ax" /usr/local/share/dshowserver
```

4) Verify that dshowserver is working
```
dshowserver -c CoreAVCDecoder.ax
```
Should return:
```
No id specified, assuming test mode
Opening device
len: 992
ProductVersion: 2.0.0
Decoder supports the following YUV formats: YUY2 UYVY YV12 I420 
Decoder is capable of YUV output (flags 0x2b)
Setting fmt
Starting
Initialization is complete
```

5) You can configure CoreAVC by following the instructions on the [ConfigureCodec](ConfigureCodec.md) page