@echo off
set DEVKITPRO=C:\Users\TuUsuario\devkitPro
set DEVKITARM=%DEVKITPRO%\devkitARM
set PATH=%PATH%;%DEVKITPRO%\tools\bin;%DEVKITPRO%\devkitARM\bin
[Environment]::SetEnvironmentVariable("Path", [Environment]::GetEnvironmentVariable("Path", "User") + ";C:\devkitPro\tools\bin;C:\devkitPro\msys2\usr\bin", "User")
echo Entorno de Nintendo Switch configurado.
cmd /k