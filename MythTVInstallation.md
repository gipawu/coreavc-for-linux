# Details #

1) Download the code from svn:
```
svn checkout http://coreavc-for-linux.googlecode.com/svn/trunk/ coreavc-for-linux 
cd coreavc-for-linux
```

2) Install dshowserver following these instructions: [DshowserverInstall](DshowserverInstall.md)

3) Install the mythtv patch
```
cd <path to mythsrc>
patch -p0 < <path to coreavc-for-linux>/mythtv/dshowserver-0.22.patch
make
make install
cp <path to coreavc-for-linux>/mythtv/dshowcodecs ~/.mythtv 
```