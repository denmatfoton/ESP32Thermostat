@echo off

set PATH=%PATH%;C:\SysGCC\esp32\bin

make -j 8 %*
