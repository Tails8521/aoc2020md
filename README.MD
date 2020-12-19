# Advent of Code 2020 on a Sega Megadrive

### How to build the ROM, with your own input

Install [SGDK 1.6](https://github.com/Stephane-D/SGDK)  
Put your inputs like so: res/inputs/dayX.input where X is the day number  
Run this command to build the ROM:  
`%GDK%\bin\make -f %GDK%\makefile.gen clean && %GDK%\bin\make -f %GDK%\makefile.gen`
