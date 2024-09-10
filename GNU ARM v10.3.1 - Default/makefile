################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include libraries/tools/subdir.mk
-include libraries/tcxo_handler/subdir.mk
-include libraries/system_mode/subdir.mk
-include libraries/sensor_processing/subdir.mk
-include libraries/phytech_protocol/subdir.mk
-include libraries/flash_storage/subdir.mk
-include libraries/UART/subdir.mk
-include libraries/SleepModeTimestampManager/subdir.mk
-include libraries/Sensors_List/subdir.mk
-include libraries/RADIO/subdir.mk
-include libraries/NonBlockingDelay/subdir.mk
-include libraries/I2C/subdir.mk
-include libraries/Hub_Definition/subdir.mk
-include libraries/ABP2_Sensor/subdir.mk
-include libraries/106_ButtonHandler/subdir.mk
-include libraries/106_BlinkLED/subdir.mk
-include libraries/106_ADC/subdir.mk
-include gecko_sdk_4.3.1/util/third_party/printf/src/subdir.mk
-include gecko_sdk_4.3.1/util/third_party/printf/subdir.mk
-include gecko_sdk_4.3.1/platform/service/udelay/src/subdir.mk
-include gecko_sdk_4.3.1/platform/service/system/src/subdir.mk
-include gecko_sdk_4.3.1/platform/service/sleeptimer/src/subdir.mk
-include gecko_sdk_4.3.1/platform/service/power_manager/src/subdir.mk
-include gecko_sdk_4.3.1/platform/service/mpu/src/subdir.mk
-include gecko_sdk_4.3.1/platform/service/iostream/src/subdir.mk
-include gecko_sdk_4.3.1/platform/service/hfxo_manager/src/subdir.mk
-include gecko_sdk_4.3.1/platform/service/device_init/src/subdir.mk
-include gecko_sdk_4.3.1/platform/radio/rail_lib/plugin/rail_util_rssi/subdir.mk
-include gecko_sdk_4.3.1/platform/radio/rail_lib/plugin/rail_util_pti/subdir.mk
-include gecko_sdk_4.3.1/platform/radio/rail_lib/plugin/rail_util_protocol/subdir.mk
-include gecko_sdk_4.3.1/platform/radio/rail_lib/plugin/rail_util_power_manager_init/subdir.mk
-include gecko_sdk_4.3.1/platform/radio/rail_lib/plugin/pa-conversions/subdir.mk
-include gecko_sdk_4.3.1/platform/peripheral/src/subdir.mk
-include gecko_sdk_4.3.1/platform/emlib/src/subdir.mk
-include gecko_sdk_4.3.1/platform/emdrv/gpiointerrupt/src/subdir.mk
-include gecko_sdk_4.3.1/platform/emdrv/dmadrv/src/subdir.mk
-include gecko_sdk_4.3.1/platform/driver/leddrv/src/subdir.mk
-include gecko_sdk_4.3.1/platform/driver/i2cspm/src/subdir.mk
-include gecko_sdk_4.3.1/platform/driver/button/src/subdir.mk
-include gecko_sdk_4.3.1/platform/common/toolchain/src/subdir.mk
-include gecko_sdk_4.3.1/platform/common/src/subdir.mk
-include gecko_sdk_4.3.1/platform/Device/SiliconLabs/EFR32FG23/Source/subdir.mk
-include gecko_sdk_4.3.1/app/flex/component/rail/sl_flex_rail_packet_asm/subdir.mk
-include gecko_sdk_4.3.1/app/flex/component/rail/sl_flex_rail_package_assistant/subdir.mk
-include gecko_sdk_4.3.1/app/flex/component/rail/sl_flex_rail_channel_selector/subdir.mk
-include gecko_sdk_4.3.1/app/common/util/app_log/subdir.mk
-include autogen/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := 106_EFR_HUB
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: main-build

# Main-build Target
main-build: 106_EFR_HUB.axf

# Tool invocations
106_EFR_HUB.axf: $(OBJS) $(USER_OBJS) makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m33 -mthumb -T "C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\autogen\linkerfile.ld" -Xlinker --gc-sections -Xlinker -Map="106_EFR_HUB.map" -mfpu=fpv5-sp-d16 -mfloat-abi=hard --specs=nano.specs -o 106_EFR_HUB.axf -Wl,--start-group "./autogen/rail_config.o" "./autogen/sl_device_init_clocks.o" "./autogen/sl_event_handler.o" "./autogen/sl_i2cspm_init.o" "./autogen/sl_iostream_handles.o" "./autogen/sl_iostream_init_eusart_instances.o" "./autogen/sl_power_manager_handler.o" "./autogen/sl_rail_util_callbacks.o" "./autogen/sl_rail_util_init.o" "./autogen/sl_simple_button_instances.o" "./autogen/sl_simple_led_instances.o" "./gecko_sdk_4.3.1/app/common/util/app_log/app_log.o" "./gecko_sdk_4.3.1/app/flex/component/rail/sl_flex_rail_channel_selector/sl_flex_rail_channel_selector.o" "./gecko_sdk_4.3.1/app/flex/component/rail/sl_flex_rail_package_assistant/sl_flex_rail_package_assistant.o" "./gecko_sdk_4.3.1/app/flex/component/rail/sl_flex_rail_packet_asm/sl_flex_packet_asm.o" "./gecko_sdk_4.3.1/platform/Device/SiliconLabs/EFR32FG23/Source/startup_efr32fg23.o" "./gecko_sdk_4.3.1/platform/Device/SiliconLabs/EFR32FG23/Source/system_efr32fg23.o" "./gecko_sdk_4.3.1/platform/common/src/sl_assert.o" "./gecko_sdk_4.3.1/platform/common/src/sl_slist.o" "./gecko_sdk_4.3.1/platform/common/src/sl_string.o" "./gecko_sdk_4.3.1/platform/common/toolchain/src/sl_memory.o" "./gecko_sdk_4.3.1/platform/driver/button/src/sl_button.o" "./gecko_sdk_4.3.1/platform/driver/button/src/sl_simple_button.o" "./gecko_sdk_4.3.1/platform/driver/i2cspm/src/sl_i2cspm.o" "./gecko_sdk_4.3.1/platform/driver/leddrv/src/sl_led.o" "./gecko_sdk_4.3.1/platform/driver/leddrv/src/sl_simple_led.o" "./gecko_sdk_4.3.1/platform/emdrv/dmadrv/src/dmadrv.o" "./gecko_sdk_4.3.1/platform/emdrv/gpiointerrupt/src/gpiointerrupt.o" "./gecko_sdk_4.3.1/platform/emlib/src/em_burtc.o" "./gecko_sdk_4.3.1/platform/emlib/src/em_cmu.o" "./gecko_sdk_4.3.1/platform/emlib/src/em_core.o" "./gecko_sdk_4.3.1/platform/emlib/src/em_emu.o" "./gecko_sdk_4.3.1/platform/emlib/src/em_eusart.o" "./gecko_sdk_4.3.1/platform/emlib/src/em_gpio.o" "./gecko_sdk_4.3.1/platform/emlib/src/em_i2c.o" "./gecko_sdk_4.3.1/platform/emlib/src/em_iadc.o" "./gecko_sdk_4.3.1/platform/emlib/src/em_ldma.o" "./gecko_sdk_4.3.1/platform/emlib/src/em_msc.o" "./gecko_sdk_4.3.1/platform/emlib/src/em_prs.o" "./gecko_sdk_4.3.1/platform/emlib/src/em_rmu.o" "./gecko_sdk_4.3.1/platform/emlib/src/em_system.o" "./gecko_sdk_4.3.1/platform/emlib/src/em_timer.o" "./gecko_sdk_4.3.1/platform/emlib/src/em_usart.o" "./gecko_sdk_4.3.1/platform/peripheral/src/peripheral_sysrtc.o" "./gecko_sdk_4.3.1/platform/radio/rail_lib/plugin/pa-conversions/pa_conversions_efr32.o" "./gecko_sdk_4.3.1/platform/radio/rail_lib/plugin/pa-conversions/pa_curves_efr32.o" "./gecko_sdk_4.3.1/platform/radio/rail_lib/plugin/rail_util_power_manager_init/sl_rail_util_power_manager_init.o" "./gecko_sdk_4.3.1/platform/radio/rail_lib/plugin/rail_util_protocol/sl_rail_util_protocol.o" "./gecko_sdk_4.3.1/platform/radio/rail_lib/plugin/rail_util_pti/sl_rail_util_pti.o" "./gecko_sdk_4.3.1/platform/radio/rail_lib/plugin/rail_util_rssi/sl_rail_util_rssi.o" "./gecko_sdk_4.3.1/platform/service/device_init/src/sl_device_init_dcdc_s2.o" "./gecko_sdk_4.3.1/platform/service/device_init/src/sl_device_init_emu_s2.o" "./gecko_sdk_4.3.1/platform/service/device_init/src/sl_device_init_hfxo_s2.o" "./gecko_sdk_4.3.1/platform/service/device_init/src/sl_device_init_nvic.o" "./gecko_sdk_4.3.1/platform/service/hfxo_manager/src/sl_hfxo_manager.o" "./gecko_sdk_4.3.1/platform/service/hfxo_manager/src/sl_hfxo_manager_hal_s2.o" "./gecko_sdk_4.3.1/platform/service/iostream/src/sl_iostream.o" "./gecko_sdk_4.3.1/platform/service/iostream/src/sl_iostream_eusart.o" "./gecko_sdk_4.3.1/platform/service/iostream/src/sl_iostream_retarget_stdio.o" "./gecko_sdk_4.3.1/platform/service/iostream/src/sl_iostream_stdlib_config.o" "./gecko_sdk_4.3.1/platform/service/iostream/src/sl_iostream_uart.o" "./gecko_sdk_4.3.1/platform/service/iostream/src/sl_iostream_usart.o" "./gecko_sdk_4.3.1/platform/service/mpu/src/sl_mpu.o" "./gecko_sdk_4.3.1/platform/service/power_manager/src/sl_power_manager.o" "./gecko_sdk_4.3.1/platform/service/power_manager/src/sl_power_manager_debug.o" "./gecko_sdk_4.3.1/platform/service/power_manager/src/sl_power_manager_hal_s2.o" "./gecko_sdk_4.3.1/platform/service/sleeptimer/src/sl_sleeptimer.o" "./gecko_sdk_4.3.1/platform/service/sleeptimer/src/sl_sleeptimer_hal_burtc.o" "./gecko_sdk_4.3.1/platform/service/sleeptimer/src/sl_sleeptimer_hal_sysrtc.o" "./gecko_sdk_4.3.1/platform/service/sleeptimer/src/sl_sleeptimer_hal_timer.o" "./gecko_sdk_4.3.1/platform/service/system/src/sl_system_init.o" "./gecko_sdk_4.3.1/platform/service/system/src/sl_system_process_action.o" "./gecko_sdk_4.3.1/platform/service/udelay/src/sl_udelay.o" "./gecko_sdk_4.3.1/platform/service/udelay/src/sl_udelay_armv6m_gcc.o" "./gecko_sdk_4.3.1/util/third_party/printf/printf.o" "./gecko_sdk_4.3.1/util/third_party/printf/src/iostream_printf.o" "./libraries/106_ADC/106_adc_reader.o" "./libraries/106_BlinkLED/106_BlinkLED.o" "./libraries/106_ButtonHandler/106_ButtonHandler.o" "./libraries/ABP2_Sensor/ABP2_Sensor.o" "./libraries/Hub_Definition/Hub_Protocols.o" "./libraries/Hub_Definition/rf_parser.o" "./libraries/Hub_Definition/rf_rx_handle.o" "./libraries/I2C/I2C.o" "./libraries/NonBlockingDelay/NonBlockingDelay.o" "./libraries/RADIO/radio_handler.o" "./libraries/RADIO/radio_init.o" "./libraries/RADIO/rf_decoder.o" "./libraries/RADIO/rf_monitor_decoder.o" "./libraries/RADIO/rf_monitor_state_machine.o" "./libraries/RADIO/rf_power.o" "./libraries/RADIO/rf_protocol.o" "./libraries/RADIO/rf_state_machine.o" "./libraries/Sensors_List/SenorsListHandle.o" "./libraries/Sensors_List/slot_handle.o" "./libraries/SleepModeTimestampManager/SleepModeTimestampManager.o" "./libraries/UART/UARTComm.o" "./libraries/UART/uart_decoder.o" "./libraries/flash_storage/flash_storage.o" "./libraries/phytech_protocol/phytech_protocol.o" "./libraries/sensor_processing/sensor_processing.o" "./libraries/system_mode/system_mode.o" "./libraries/tcxo_handler/tcxo_handler.o" "./libraries/tools/tools.o" "./main.o" "C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\autogen\librail_release\librail_efr32xg23_gcc_release.a" -lgcc -lc -lm -lnosys -Wl,--end-group -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: 106_EFR_HUB.hex'
	arm-none-eabi-objcopy -O ihex "106_EFR_HUB.axf" "106_EFR_HUB.hex"
	@echo ' '

	@echo 'Building bin file: 106_EFR_HUB.bin'
	arm-none-eabi-objcopy -O binary "106_EFR_HUB.axf" "106_EFR_HUB.bin"
	@echo ' '

	@echo 'Building s37 file: 106_EFR_HUB.s37'
	arm-none-eabi-objcopy -O srec "106_EFR_HUB.axf" "106_EFR_HUB.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "106_EFR_HUB.axf" -A
	@echo ' '

# Other Targets
clean:
	-@$(foreach FILE, $(EXECUTABLES), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(OBJS), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(C_DEPS), $(shell $(RM) $(FILE)) )
	-$(RM) 106_EFR_HUB.axf
	-@echo ' '

.PHONY: all clean dependents main-build

-include ../makefile.targets
