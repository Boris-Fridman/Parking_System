################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/boris/Documents/RealTimeColedge/Embedded_Linux/Parking_System/Common_Libs/CommonData.c 

OBJS += \
./Common_Libs/CommonData.o 

C_DEPS += \
./Common_Libs/CommonData.d 


# Each subdirectory must supply rules for building sources it contributes
Common_Libs/CommonData.o: /home/boris/Documents/RealTimeColedge/Embedded_Linux/Parking_System/Common_Libs/CommonData.c Common_Libs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F756xx -c -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../UserLibs -I../../Common_Libs -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Common_Libs

clean-Common_Libs:
	-$(RM) ./Common_Libs/CommonData.cyclo ./Common_Libs/CommonData.d ./Common_Libs/CommonData.o ./Common_Libs/CommonData.su

.PHONY: clean-Common_Libs

