#!/bin/bash


prog="${1:-../build-stm32/stm32.elf}"
scripts="-s /usr/share/openocd/scripts"
interface="-f interface/stlink-v2.cfg"
board="-f board/stm32f3discovery.cfg"
new_board='-c "hla_vid_pid 0x0483 0x374b"'

echo "prog $prog"

openocd -f interface/stlink-v2.cfg -f board/stm32f3discovery.cfg -c "hla_vid_pid 0x0483 0x374b" -c "program $prog"


#openocd $scripts $interface $board $new_board -c init -c "reset init" -c "flash write_image erase $prog" -c reset -c shutdown

#openocd $scripts $interface $board  -c "program $prog  verify reset exit" -c "shutdown"


# program and verify using elf/hex/s19. verify and reset  are optional parameters
#openocd $scripts $interface $board -c "program filename.elf verify reset exit"

# binary files need the flash address passing
#openocd -f board/stm32f3discovery.cfg -c "program filename.bin exit 0x08000000"
