@echo off
cd %~dp0..

call tools\gendeps.bat

tools\premake5\premake5.exe vs2022