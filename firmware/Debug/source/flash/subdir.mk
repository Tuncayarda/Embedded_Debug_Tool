################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/flash/check_flash.c \
../source/flash/flash_init.c 

C_DEPS += \
./source/flash/check_flash.d \
./source/flash/flash_init.d 

OBJS += \
./source/flash/check_flash.o \
./source/flash/flash_init.o 


# Each subdirectory must supply rules for building sources it contributes
source/flash/%.o: ../source/flash/%.c source/flash/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MK02FN128VFM10 -DCPU_MK02FN128VFM10_cm4 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"/Users/tuncay/Documents/MCUXpressoIDE_25.6.136/workspace/Debug_Tool/drivers" -I"/Users/tuncay/Documents/MCUXpressoIDE_25.6.136/workspace/Debug_Tool/device" -I"/Users/tuncay/Documents/MCUXpressoIDE_25.6.136/workspace/Debug_Tool/CMSIS" -I"/Users/tuncay/Documents/MCUXpressoIDE_25.6.136/workspace/Debug_Tool/CMSIS/m-profile" -I"/Users/tuncay/Documents/MCUXpressoIDE_25.6.136/workspace/Debug_Tool/utilities" -I"/Users/tuncay/Documents/MCUXpressoIDE_25.6.136/workspace/Debug_Tool/device/periph" -I"/Users/tuncay/Documents/MCUXpressoIDE_25.6.136/workspace/Debug_Tool/component/lists" -I"/Users/tuncay/Documents/MCUXpressoIDE_25.6.136/workspace/Debug_Tool/component/serial_manager" -I"/Users/tuncay/Documents/MCUXpressoIDE_25.6.136/workspace/Debug_Tool/utilities/str" -I"/Users/tuncay/Documents/MCUXpressoIDE_25.6.136/workspace/Debug_Tool/utilities/debug_console/config" -I"/Users/tuncay/Documents/MCUXpressoIDE_25.6.136/workspace/Debug_Tool/component/uart" -I"/Users/tuncay/Documents/MCUXpressoIDE_25.6.136/workspace/Debug_Tool/utilities/debug_console" -I"/Users/tuncay/Documents/MCUXpressoIDE_25.6.136/workspace/Debug_Tool/board" -I"/Users/tuncay/Documents/MCUXpressoIDE_25.6.136/workspace/Debug_Tool/source" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-flash

clean-source-2f-flash:
	-$(RM) ./source/flash/check_flash.d ./source/flash/check_flash.o ./source/flash/flash_init.d ./source/flash/flash_init.o

.PHONY: clean-source-2f-flash

