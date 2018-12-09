@echo off
echo 开始编译安装包程序...................
"%VS100COMNTOOLS%../IDE/devenv.com" ../Setup/Setup/Setup.vcxproj /rebuild "Release|Win32"
pause