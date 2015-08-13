################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../comm.c \
../entropy.c \
../hiding.c \
../main.c \
../protocol.c \
../rng.c \
../uart.c 

OBJS += \
./comm.o \
./entropy.o \
./hiding.o \
./main.o \
./protocol.o \
./rng.o \
./uart.o 

C_DEPS += \
./comm.d \
./entropy.d \
./hiding.d \
./main.d \
./protocol.d \
./rng.d \
./uart.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega644 -DF_CPU=4800000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


