****The Xine patch is still experimental, and only works with the xinelib 1.1.** branch 

# Introduction #
> Only the dshowserver method of xine will be supported from now on.  Any other patching methods are now deprecated**

# Details #

1) Download the code from svn:
```
svn checkout http://coreavc-for-linux.googlecode.com/svn/trunk/ coreavc-for-linux 
cd coreavc-for-linux
```

2) Install dshowserver forrlowing these instructions: [DshowserverInstall](DshowserverInstall.md)

3) Install the xine patch
```
cd <path to xine-lib src>
patch -p1 < <path to coreavc-for-linux>/xine/dshowserver.patch
make
make install
```