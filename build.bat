arm-none-eabi-gcc -static -mthumb -fno-rtti -fno-exceptions  -g -mcpu=cortex-m3 *.cpp -T linker_script.ld -o main.elf -nostartfiles -std=c++11 -Xlinker --cref -Xlinker -Map -Xlinker main.map
arm-none-eabi-objcopy -g -O binary main.elf main.bin  
