#! bash -e
openocd -f board/stm32f7discovery.cfg -c 'program '$1' '$2';reset;exit'
