#! /bin/bash
scons.bat
openocd-0.7.0 -f openocd.cfg -c "flash_image"
