################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../aes/aes128_dec.c \
../aes/aes_dec.c \
../aes/aes_dummy_operations.c \
../aes/aes_invsbox.c \
../aes/aes_keyschedule.c \
../aes/aes_sbox.c 

S_UPPER_SRCS += \
../aes/gf256mul.S 

OBJS += \
./aes/aes128_dec.o \
./aes/aes_dec.o \
./aes/aes_dummy_operations.o \
./aes/aes_invsbox.o \
./aes/aes_keyschedule.o \
./aes/aes_sbox.o \
./aes/gf256mul.o 

C_DEPS += \
./aes/aes128_dec.d \
./aes/aes_dec.d \
./aes/aes_dummy_operations.d \
./aes/aes_invsbox.d \
./aes/aes_keyschedule.d \
./aes/aes_sbox.d 

S_UPPER_DEPS += \
./aes/gf256mul.d 


# Each subdirectory must supply rules for building sources it contributes
aes/%.o: ../aes/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega644 -DF_CPU=4800000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

aes/%.o: ../aes/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Assembler'
	avr-as -mmcu=atmega644 -c -o "$@" "$<" -I../fhreefish
	@echo 'Finished building: $<'
	@echo ' '


