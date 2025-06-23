# Configuring mingw64 on Windows:
## Downloading the mingw64
Go to [winlibs site](https://winlibs.com/) and select a latest version of UCRT Runtime (64bit version) (or download directly the latest version with this [link](https://github.com/brechtsanders/winlibs_mingw/releases/download/15.1.0posix-13.0.0-ucrt-r2/winlibs-i686-posix-dwarf-gcc-15.1.0-mingw-w64ucrt-13.0.0-r2.zip))
## Installing the mingw64
Extract the zip that you downloaded to the root of system (example: C:\ (C is the disk letter))
Now, we will be set the path C:\mingw64\bin to the PATH of windows.
# To set the dir path to Windows PATH with CMD, do this:
## Setting paths with CMD:
Set the dir path to PATH for local users with: 
```
setx PATH "%PATH%;C:\mingw64\bin" /M
```
Set the dir path to PATH for ALL Users with: 
```
setx PATH "%PATH%;C:\mingw64\bin"
```
# To set the dir path to Windows PATH with PowerShell, do this:
## Setting paths with PowerShell:
Set the dir path to PATH for local users with: 
```
[System.Environment]::SetEnvironmentVariable("PATH", $env:PATH + ";C:\ming64\bin", "User")
```
Set the dir path to PATH for ALL User with: 
``` 
[System.Environment]::SetEnvironmentVariable("PATH", $env:PATH + ";C:\ming64\bin", "Machine") 
```
# Note (important)
The path **C:\ming64\bin** is the path where you should extract the zip file containing mingw64. Even if it's not the same path or different drive letter, include the location where you extracted it to avoid errors!
