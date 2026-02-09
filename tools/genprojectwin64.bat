@echo off
cd %~dp0..

call tools\gendepswin64.bat

tools\premake5\premake5.exe vs2022