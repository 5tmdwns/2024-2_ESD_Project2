################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/AlmondWithCavior.c \
../Core/Src/BurdockTarteTatinWithSkinJuice.c \
../Core/Src/EmberToastedAcornNoodle.c \
../Core/Src/HearthOvenGrilledHanwoo.c \
../Core/Src/SmallBites.c \
../Core/Src/SmallSweet1.c \
../Core/Src/SmallSweet2.c \
../Core/Src/SnowCrabAndPickledChrysanthemum.c \
../Core/Src/TilefishMustardBrassica.c \
../Core/Src/anSungJae.c \
../Core/Src/dongHyun.c \
../Core/Src/fonts.c \
../Core/Src/main.c \
../Core/Src/st7789.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c 

OBJS += \
./Core/Src/AlmondWithCavior.o \
./Core/Src/BurdockTarteTatinWithSkinJuice.o \
./Core/Src/EmberToastedAcornNoodle.o \
./Core/Src/HearthOvenGrilledHanwoo.o \
./Core/Src/SmallBites.o \
./Core/Src/SmallSweet1.o \
./Core/Src/SmallSweet2.o \
./Core/Src/SnowCrabAndPickledChrysanthemum.o \
./Core/Src/TilefishMustardBrassica.o \
./Core/Src/anSungJae.o \
./Core/Src/dongHyun.o \
./Core/Src/fonts.o \
./Core/Src/main.o \
./Core/Src/st7789.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o 

C_DEPS += \
./Core/Src/AlmondWithCavior.d \
./Core/Src/BurdockTarteTatinWithSkinJuice.d \
./Core/Src/EmberToastedAcornNoodle.d \
./Core/Src/HearthOvenGrilledHanwoo.d \
./Core/Src/SmallBites.d \
./Core/Src/SmallSweet1.d \
./Core/Src/SmallSweet2.d \
./Core/Src/SnowCrabAndPickledChrysanthemum.d \
./Core/Src/TilefishMustardBrassica.d \
./Core/Src/anSungJae.d \
./Core/Src/dongHyun.d \
./Core/Src/fonts.d \
./Core/Src/main.d \
./Core/Src/st7789.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/AlmondWithCavior.cyclo ./Core/Src/AlmondWithCavior.d ./Core/Src/AlmondWithCavior.o ./Core/Src/AlmondWithCavior.su ./Core/Src/BurdockTarteTatinWithSkinJuice.cyclo ./Core/Src/BurdockTarteTatinWithSkinJuice.d ./Core/Src/BurdockTarteTatinWithSkinJuice.o ./Core/Src/BurdockTarteTatinWithSkinJuice.su ./Core/Src/EmberToastedAcornNoodle.cyclo ./Core/Src/EmberToastedAcornNoodle.d ./Core/Src/EmberToastedAcornNoodle.o ./Core/Src/EmberToastedAcornNoodle.su ./Core/Src/HearthOvenGrilledHanwoo.cyclo ./Core/Src/HearthOvenGrilledHanwoo.d ./Core/Src/HearthOvenGrilledHanwoo.o ./Core/Src/HearthOvenGrilledHanwoo.su ./Core/Src/SmallBites.cyclo ./Core/Src/SmallBites.d ./Core/Src/SmallBites.o ./Core/Src/SmallBites.su ./Core/Src/SmallSweet1.cyclo ./Core/Src/SmallSweet1.d ./Core/Src/SmallSweet1.o ./Core/Src/SmallSweet1.su ./Core/Src/SmallSweet2.cyclo ./Core/Src/SmallSweet2.d ./Core/Src/SmallSweet2.o ./Core/Src/SmallSweet2.su ./Core/Src/SnowCrabAndPickledChrysanthemum.cyclo ./Core/Src/SnowCrabAndPickledChrysanthemum.d ./Core/Src/SnowCrabAndPickledChrysanthemum.o ./Core/Src/SnowCrabAndPickledChrysanthemum.su ./Core/Src/TilefishMustardBrassica.cyclo ./Core/Src/TilefishMustardBrassica.d ./Core/Src/TilefishMustardBrassica.o ./Core/Src/TilefishMustardBrassica.su ./Core/Src/anSungJae.cyclo ./Core/Src/anSungJae.d ./Core/Src/anSungJae.o ./Core/Src/anSungJae.su ./Core/Src/dongHyun.cyclo ./Core/Src/dongHyun.d ./Core/Src/dongHyun.o ./Core/Src/dongHyun.su ./Core/Src/fonts.cyclo ./Core/Src/fonts.d ./Core/Src/fonts.o ./Core/Src/fonts.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/st7789.cyclo ./Core/Src/st7789.d ./Core/Src/st7789.o ./Core/Src/st7789.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su

.PHONY: clean-Core-2f-Src

