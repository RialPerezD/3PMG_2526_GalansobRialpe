@echo off
cd %~dp0..
tools\gendeps.bat
tools\premake5.exe vs2022