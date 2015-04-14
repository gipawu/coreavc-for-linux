# Introduction #

Here are a few benchmarks I collected using dshowserver with mplayer

Benchmarking is done via:
```
mplayer -nosound -vo null -benchmark -demuxer <mkv|mov> -vc <codec> <file>
```

# Details #
The following were done on an Intel Core-2-Quad (Penryn):

NASA 720p shuttle launch from Apple Quicktime site:
```
dshowserver using DivxH264.ax
BENCHMARKs: VC:  12.946s VO:   0.003s A:   0.000s Sys:   0.220s =   13.169s
BENCHMARK%: VC: 98.3067% VO:  0.0212% A:  0.0000% Sys:  1.6721% = 100.0000%

mplayer FFh264 (from SVN: Feb 6, 2010):
BENCHMARKs: VC:  12.553s VO:   0.001s A:   0.000s Sys:   0.110s =   12.664s
BENCHMARK%: VC: 99.1252% VO:  0.0098% A:  0.0000% Sys:  0.8650% = 100.0000%

dshowserver using CoreAVC 2.0:
BENCHMARKs: VC:   8.171s VO:   0.001s A:   0.000s Sys:   0.792s =    8.965s
BENCHMARK%: VC: 91.1506% VO:  0.0135% A:  0.0000% Sys:  8.8358% = 100.0000%
```

A DVD rip to MKV H264 AVC1 via x264 (480p)
```
dshowserver using DivxH264.ax
BENCHMARKs: VC:  24.265s VO:   0.006s A:   0.000s Sys:   0.316s =   24.586s
BENCHMARK%: VC: 98.6933% VO:  0.0226% A:  0.0000% Sys:  1.2841% = 100.0000%

mplayer FFh264 (from SVN: Feb 6, 2010):
BENCHMARKs: VC:  20.788s VO:   0.003s A:   0.000s Sys:   0.205s =   20.995s
BENCHMARK%: VC: 99.0112% VO:  0.0137% A:  0.0000% Sys:  0.9751% = 100.0000%

dshowserver using CoreAVC 2.0:
BENCHMARKs: VC:  18.948s VO:   0.004s A:   0.000s Sys:   0.244s =   19.196s
BENCHMARK%: VC: 98.7088% VO:  0.0189% A:  0.0000% Sys:  1.2724% = 100.0000%
```