################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include fhreefish/subdir.mk
-include aes/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
SmartCard_Boolean_Masking.lss \

FLASH_IMAGE += \
SmartCard_Boolean_Masking.hex \

SIZEDUMMY += \
sizedummy \


# All Target
all: SmartCard_Boolean_Masking.elf secondary-outputs

# Tool invocations
SmartCard_Boolean_Masking.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,SmartCard_Boolean_Masking.map -mmcu=atmega644 -o "SmartCard_Boolean_Masking.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

SmartCard_Boolean_Masking.lss: SmartCard_Boolean_Masking.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S SmartCard_Boolean_Masking.elf  >"SmartCard_Boolean_Masking.lss"
	@echo 'Finished building: $@'
	@echo ' '

SmartCard_Boolean_Masking.hex: SmartCard_Boolean_Masking.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex SmartCard_Boolean_Masking.elf  "SmartCard_Boolean_Masking.hex"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: SmartCard_Boolean_Masking.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega644 SmartCard_Boolean_Masking.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) SmartCard_Boolean_Masking.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
