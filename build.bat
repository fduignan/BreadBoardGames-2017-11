@echo off
@echo Go to http://ioprog.com/bbg for instructions
..\arm_sdk\bin\arm-none-eabi-gcc -static -mthumb -fno-rtti -fno-exceptions  -g -mcpu=cortex-m3 *.cpp -T linker_script.ld -o main.elf -nostartfiles -std=c++11 -Xlinker --cref -Xlinker -Map -Xlinker main.map
..\arm_sdk\bin\arm-none-eabi-objcopy -g -O ihex main.elf main.hex  
..\arm_sdk\bin\arm-none-eabi-objcopy -g -O binary main.elf main.bin  
echo Make sure the ST-Link device is plugged in and the CPU is wired to it.
pause
..\arm_sdk\flasher\bin\st-flash.exe write main.bin 0x8000000
echo Hopefully that worked! :-)
echo (you may have to press the reset button on the CPU board)
