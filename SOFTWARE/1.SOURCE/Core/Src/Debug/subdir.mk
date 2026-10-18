################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../i2c_forLCD.c \
../main.c \
../spi_forRFID.c \
../stm32f1xx_hal_msp.c \
../stm32f1xx_it.c \
../syscalls.c \
../sysmem.c \
../system_stm32f1xx.c 

C_DEPS += \
./i2c_forLCD.d \
./main.d \
./spi_forRFID.d \
./stm32f1xx_hal_msp.d \
./stm32f1xx_it.d \
./syscalls.d \
./sysmem.d \
./system_stm32f1xx.d 

OBJS += \
./i2c_forLCD.o \
./main.o \
./spi_forRFID.o \
./stm32f1xx_hal_msp.o \
./stm32f1xx_it.o \
./syscalls.o \
./sysmem.o \
./system_stm32f1xx.o 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean--2e-

clean--2e-:
	-$(RM) ./i2c_forLCD.d ./i2c_forLCD.o ./main.d ./main.o ./spi_forRFID.d ./spi_forRFID.o ./stm32f1xx_hal_msp.d ./stm32f1xx_hal_msp.o ./stm32f1xx_it.d ./stm32f1xx_it.o ./syscalls.d ./syscalls.o ./sysmem.d ./sysmem.o ./system_stm32f1xx.d ./system_stm32f1xx.o

.PHONY: clean--2e-

