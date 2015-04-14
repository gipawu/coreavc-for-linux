
---

<font color='#FF0000'>
<h1>THIS PAGE IS OUT OF DATE!</h1>
<h1>Wine is now the only supported way to configure CoreAVC</h1>
</font>

---


# Introduction #

Before you can use CoreAVC you must add your license information to the registry.


As of Version 1.7.0, the Installation should complete within Wine (unverified).  However, if it doesn't work, you'll need to do the install on Linux.

# Details #
Once CoreAVC has been installed you can likely find the CoreAVCDecoder.ax file in 'C:\Program Files\Core Codec\CoreAVC Professional\CoreAVCDecoder.ax'  You will need to copy this to your linux box in /usr/lib/win32

```
export REGISTRY=$HOME/.mplayer/registry32
```
Next you'll need to open Regedit and find the following keys:

For CoreAVC 1.7.0:
  * CoreAVC serial number (used to register product):
```
registercodec -r $REGISTRY -k
"HKLM\\Software\\CoreCodec\\CoreAVC Pro\\Serial" -v "55555-55555-CORE-55555-55555"
```
For CoreAVC 1.2, 1.3, 1.5:
  * Windows ProductID which is formatted like 55555-333-7777777-55555
    1. From the Control Panel go to 'System' and find the Id under 'Registered To'
    1. In the registry here: `HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\ProductID`
    1. In the registry here: `HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\ProductID`
  * CoreAVC serial number here: `HKEY_CURRENT_USER\Software\CoreCodec\CoreAVC Pro\Serial`
  * Install in linux with (**Note that we always use 'Windows' regardless how you got the ProductID**):
```
registercodec -r $REGISTRY -k "HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\ProductID" -v "55555-333-7777777-55555"
registercodec -r $REGISTRY -k "HKCU\\Software\\CoreCodec\\CoreAVC Pro\\Serial" -v "111111-222222-333333-444444-55555"
```
For CoreAVC 1.0 and 1.1:
  * Get these registry keys:
```
HKEY_CURRENT_USER\Software\Licenturion GmbH\0000032D\Product Key
HKEY_CURRENT_USER\Software\Licenturion GmbH\0000032D\User ID
```
  * Install in linux with:
```
registercodec -r $REGISTRY -k "HKCU\\Software\\Licenturion GmbH\\0000032D\\Product Key" -v "AAAA-BBBB-CCCC-DDDD-EEEE-FFFF-GGGG-HHH"
registercodec -r $REGISTRY -k "HKCU\\Software\\Licenturion GmbH\\0000032D\\User ID" -v "John Doe"
```

There are other options you can play with too.  The following are all untested as to how they behave in Linux.
For 1.7.0, use the following options.  They are aplied in a single string as follows
```
registercodec -r $REGISTRY -k "HKLM\\Software\\IniFileMapping\\CoreAVC\\Settings\\C:\\coreavc.ini" -v "ilevels=2 olevels=2 di=3 deblock=7 ai=0 crop1088=0 vmr_ar=0" 
```


Deinterlacing:
```
key: di
3 : None
4 : Blend
5 : Bob
6 : Hardware
```
Aggressive Deinterlacing:
```
key: ai
0 : Off
1 : On
```
Crop 1088 to 1080:
```
key: crop1088
0 : Off
1 : On
```
Deblocking:
```
key: deblock
7 : Standard
8 : Skip when safe
9 : Skip always
```
Force VMR AR Correction:
```
key: vmr_ar
0 : Off
1 : On
```

For 1.5.0 and previous:

Deinterlacing:
```
0 : Software deinterlacing
1 : Software double frame rate
2 : DirectShow deinterlacing
3 : No deinterlacing
registercodec -r $REGISTRY -k "HKEY_CURRENT_USER\\Software\\CoreCodec\\CoreAVC Pro\\Deinterlace" -v <value> -t  dword
```
Aggressive Deinterlacing:
```
0 : Off
1 : On
registercodec -r $REGISTRY -k "HKEY_CURRENT_USER\\Software\\CoreCodec\\CoreAVC Pro\\AggressiveDeinterlace" -v <value> -t  dword
```
Crop 1088 to 1080:
```
0 : Off
1 : On
registercodec -r $REGISTRY -k "HKEY_CURRENT_USER\\Software\\CoreCodec\\CoreAVC Pro\\Crop1080" -v <value> -t  dword
```
Deblocking:
```
0 : Standard deblocking
1 : Skip b-frame deblocking
2 : Skip all deblocking
registercodec -r $REGISTRY -k "HKEY_CURRENT_USER\\Software\\CoreCodec\\CoreAVC Pro\\Deblocking" -v <value> -t  dword
```
Force VMR AR Correction:
```
0 : Off
1 : On
registercodec -r $REGISTRY -k "HKEY_CURRENT_USER\\Software\\CoreCodec\\CoreAVC Pro\\ForceVMRAR" -v <value> -t  dword
```


---

<font color='#FF0000'>
<h1>THIS PAGE IS OUT OF DATE!</h1>
<h1>Wine is now the only supported way to configure CoreAVC</h1>
</font>

---
