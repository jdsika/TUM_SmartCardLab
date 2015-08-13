################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../fhreefish/rand.c \
../fhreefish/skein.c 

S_SRCS += \
../fhreefish/math.s \
../fhreefish/stack_pointer.s \
../fhreefish/temp_storage.s \
../fhreefish/threefish_common.s \
../fhreefish/threefish_dec.s \
../fhreefish/threefish_enc.s \
../fhreefish/threefish_macros.s \
../fhreefish/threefish_registers.s 

OBJS += \
./fhreefish/math.o \
./fhreefish/rand.o \
./fhreefish/skein.o \
./fhreefish/stack_pointer.o \
./fhreefish/temp_storage.o \
./fhreefish/threefish_common.o \
./fhreefish/threefish_dec.o \
./fhreefish/threefish_enc.o \
./fhreefish/threefish_macros.o \
./fhreefish/threefish_registers.o 

C_DEPS += \
./fhreefish/rand.d \
./fhreefish/skein.d 

S_DEPS += \
./fhreefish/math.d \
./fhreefish/stack_pointer.d \
./fhreefish/temp_storage.d \
./fhreefish/threefish_common.d \
./fhreefish/threefish_dec.d \
./fhreefish/threefish_enc.d \
./fhreefish/threefish_macros.d \
./fhreefish/threefish_registers.d 


# Each subdirectory must supply rules for building sources it contributes
fhreefish/%.o: ../fhreefish/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Assembler'
	avr-as -mmcu=atmega644 -c -o "$@" "$<" -I../fhreefish
	@echo 'Finished building: $<'
	@echo ' '

fhreefish/%.o: ../fhreefish/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega644 -DF_CPU=4800000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


