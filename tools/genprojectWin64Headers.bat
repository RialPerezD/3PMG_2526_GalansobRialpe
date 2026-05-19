@echo off
cd %~dp0..

if exist build rd /s /q build

copy /y tools\premake5Headers.lua premake5.lua

call tools\genDepsWin64.bat

tools\premake5\premake5.exe vs2022