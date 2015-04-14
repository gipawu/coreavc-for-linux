# Introduction #

The mplayer build does not support interlaced playback (it will fail rather than just look bad)

# Details #

Before beginning, ensure that you have a recent mplayer build (only SVN is supported, but recent snapshots should work as well)

1) Download the code from svn:
```
svn checkout http://coreavc-for-linux.googlecode.com/svn/trunk/ coreavc-for-linux 
cd coreavc-for-linux
```

2) Install dshowserver forrlowing these instructions: [DshowserverInstall](DshowserverInstall.md)

3) Install patch into mplayer:
```
cd <path to mplayer source code>
patch -p0 < <path to coreavc-for-linux>/mplayer/dshowserver.patch
make
make install
```

4) Update codecs.conf
  * If you don't have ~/.mplayer/codecs.conf
```
mkdir ~/.mplayer
cp <path to mplayer source>/etc/codecs.conf ~/.mplayer/
```
  * Add the following to the end of ~/.mplayer/codecs.conf
```
videocodec coreserve
  info "CoreAVC DShow H264 decoder x.x for x86 - http://corecodec.org/"
  status working
  format 0x10000005
  fourcc H264,h264 H264
  fourcc X264,x264
  fourcc avc1,AVC1 AVC1
  fourcc davc,DAVC
  fourcc VSSH
  driver dshowserver
  dll "CoreAVCDecoder.ax"
  guid 0x09571a4b, 0xf1fe, 0x4c60, 0x97, 0x60, 0xde, 0x6d, 0x31, 0x0c, 0x7c, 0x31
  out YV12,IYUV,I420,YUY2
```

# Playing tips #

to use coreavc, you may need to add '-vc coreserve' (assuming you use the above codecs.conf options)

you may also need to use '-demuxer lavf' for some formats (TS streams for example)

with current mplayer you may also need -no-correct-pts to prevent jerky playback.