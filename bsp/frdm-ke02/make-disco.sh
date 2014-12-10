#! /bin/bash
#export RTT_EXEC_PATH="d:/Program Files/CodeSourcery/Sourcery_CodeBench_Lite_for_ARM_EABI/bin"
scons.bat
#openocd -f openocd.cfg -c "mt_flash_bin rtthread.bin 0"
'c:\Program Files (x86)\SEGGER\JLinkARM_V484f\jlink.exe' burn.txt
