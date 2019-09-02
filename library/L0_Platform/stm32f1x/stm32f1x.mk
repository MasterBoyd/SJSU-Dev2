INCLUDES += $(LIBRARY_DIR)/L0_Platform/freertos

SYSTEM_INCLUDES +=

LIBRARY_LPC17XX += $(LIBRARY_DIR)/L0_Platform/stm32f10x/diskio.cpp
LIBRARY_LPC17XX += $(LIBRARY_DIR)/L0_Platform/stm32f10x/startup.cpp
LIBRARY_LPC17XX += $(LIBRARY_DIR)/L0_Platform/arm_cortex/m3/ARM_CM3/port.c
LIBRARY_LPC17XX += $(LIBRARY_DIR)/L0_Platform/arm_cortex/exceptions.cpp

OPENOCD_CONFIG = $(LIBRARY_DIR)/L0_Platform/stm32f10x/stm32f10x.cfg

$(eval $(call BUILD_LIRBARY,libstm32f10x,LIBRARY_STM32F10X))

include $(LIBRARY_DIR)/L0_Platform/arm_cortex/m3/m3.mk

platform-flash:
	@bash -c "\
	source $(TOOLS_DIR)/nxpprog/modules/bin/activate && \
	python3 $(TOOLS_DIR)/nxpprog/nxpprog.py \
	--oscfreq=12000000 --baud=230400 --control \
	\"$(SJDEV)\" \"$(BINARY)\""
