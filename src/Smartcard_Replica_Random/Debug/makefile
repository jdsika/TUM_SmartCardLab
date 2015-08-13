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
SmartCard_Replica_Random.lss \

FLASH_IMAGE += \
SmartCard_Replica_Random.hex \

SIZEDUMMY += \
sizedummy \


# All Target
all: SmartCard_Replica_Random.elf secondary-outputs

# Tool invocations
SmartCard_Replica_Random.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,SmartCard_Replica_Random.map -mmcu=atmega644 -o "SmartCard_Replica_Random.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

SmartCard_Replica_Random.lss: SmartCard_Replica_Random.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S SmartCard_Replica_Random.elf  >"SmartCard_Replica_Random.lss"
	@echo 'Finished building: $@'
	@echo ' '

SmartCard_Replica_Random.hex: SmartCard_Replica_Random.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex SmartCard_Replica_Random.elf  "SmartCard_Replica_Random.hex"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: SmartCard_Replica_Random.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega644 SmartCard_Replica_Random.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) SmartCard_Replica_Random.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
