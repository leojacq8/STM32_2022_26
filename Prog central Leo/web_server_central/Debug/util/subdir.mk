################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../util/DWT_Delay.c 

OBJS += \
./util/DWT_Delay.o 

C_DEPS += \
./util/DWT_Delay.d 


# Each subdirectory must supply rules for building sources it contributes
util/%.o: ../util/%.c util/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xE -c -I../Core/Inc -I".. /ioLibrary_Driver/Ethernet" -I".. /ioLibrary_Driver/Ethernet/W5100" -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I"G:/ECOLE/MASTER/Realisation systeme/STM32/Programme/STM32_2022_26/Prog central/web_server_central/ioLibrary_Driver/Ethernet" -I"G:/ECOLE/MASTER/Realisation systeme/STM32/Programme/STM32_2022_26/Prog central/web_server_central/ioLibrary_Driver/Ethernet/W5100" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-util

clean-util:
	-$(RM) ./util/DWT_Delay.d ./util/DWT_Delay.o

.PHONY: clean-util

