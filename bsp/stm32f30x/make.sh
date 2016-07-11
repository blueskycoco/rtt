#! /bin/bash
scons.bat
openocd -f openocd.cfg -c "flash_image"
#jlink.exe burn.txt
