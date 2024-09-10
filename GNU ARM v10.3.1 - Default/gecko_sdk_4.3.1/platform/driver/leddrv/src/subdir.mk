################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../gecko_sdk_4.3.1/platform/driver/leddrv/src/sl_led.c \
../gecko_sdk_4.3.1/platform/driver/leddrv/src/sl_simple_led.c 

OBJS += \
./gecko_sdk_4.3.1/platform/driver/leddrv/src/sl_led.o \
./gecko_sdk_4.3.1/platform/driver/leddrv/src/sl_simple_led.o 

C_DEPS += \
./gecko_sdk_4.3.1/platform/driver/leddrv/src/sl_led.d \
./gecko_sdk_4.3.1/platform/driver/leddrv/src/sl_simple_led.d 


# Each subdirectory must supply rules for building sources it contributes
gecko_sdk_4.3.1/platform/driver/leddrv/src/sl_led.o: ../gecko_sdk_4.3.1/platform/driver/leddrv/src/sl_led.c gecko_sdk_4.3.1/platform/driver/leddrv/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c99 '-DEFR32FG23A020F512GM48=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' '-DSL_RAIL_LIB_MULTIPROTOCOL_SUPPORT=0' '-DSL_RAIL_UTIL_PA_CONFIG_HEADER=<sl_rail_util_pa_config.h>' -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\config" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\config\rail" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\Device\SiliconLabs\EFR32FG23\Include" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\app\common\util\app_assert" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\app\common\util\app_log" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\common\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\driver\button\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\CMSIS\Core\Include" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\service\device_init\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\emdrv\dmadrv\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\emdrv\common\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\emlib\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\emdrv\gpiointerrupt\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\service\hfxo_manager\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\driver\i2cspm\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\service\iostream\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\driver\leddrv\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\service\mpu\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\peripheral\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\service\power_manager\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\util\third_party\printf" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\util\third_party\printf\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\common" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\protocol\ble" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\protocol\ieee802154" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\protocol\wmbus" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\protocol\zwave" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\chip\efr32\efr32xg2x" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\protocol\sidewalk" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\plugin\rail_util_callbacks" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\plugin\pa-conversions" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\plugin\pa-conversions\efr32xg23" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\plugin\rail_util_power_manager_init" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\plugin\rail_util_protocol" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\plugin\rail_util_pti" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\plugin\rail_util_rssi" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\app\flex\component\rail\sl_flex_rail_channel_selector" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\app\flex\component\rail\sl_flex_rail_package_assistant" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\app\flex\component\rail\sl_flex_rail_packet_asm" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\common\toolchain\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\service\system\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\service\sleeptimer\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\service\udelay\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\autogen" -Og -Wall -Wextra -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mcmse -fno-builtin-printf -fno-builtin-sprintf --specs=nano.specs -c -fmessage-length=0 -MMD -MP -MF"gecko_sdk_4.3.1/platform/driver/leddrv/src/sl_led.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

gecko_sdk_4.3.1/platform/driver/leddrv/src/sl_simple_led.o: ../gecko_sdk_4.3.1/platform/driver/leddrv/src/sl_simple_led.c gecko_sdk_4.3.1/platform/driver/leddrv/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c99 '-DEFR32FG23A020F512GM48=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' '-DSL_RAIL_LIB_MULTIPROTOCOL_SUPPORT=0' '-DSL_RAIL_UTIL_PA_CONFIG_HEADER=<sl_rail_util_pa_config.h>' -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\config" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\config\rail" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\Device\SiliconLabs\EFR32FG23\Include" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\app\common\util\app_assert" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\app\common\util\app_log" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\common\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\driver\button\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\CMSIS\Core\Include" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\service\device_init\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\emdrv\dmadrv\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\emdrv\common\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\emlib\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\emdrv\gpiointerrupt\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\service\hfxo_manager\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\driver\i2cspm\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\service\iostream\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\driver\leddrv\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\service\mpu\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\peripheral\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\service\power_manager\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\util\third_party\printf" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\util\third_party\printf\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\common" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\protocol\ble" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\protocol\ieee802154" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\protocol\wmbus" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\protocol\zwave" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\chip\efr32\efr32xg2x" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\protocol\sidewalk" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\plugin\rail_util_callbacks" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\plugin\pa-conversions" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\plugin\pa-conversions\efr32xg23" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\plugin\rail_util_power_manager_init" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\plugin\rail_util_protocol" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\plugin\rail_util_pti" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\radio\rail_lib\plugin\rail_util_rssi" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\app\flex\component\rail\sl_flex_rail_channel_selector" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\app\flex\component\rail\sl_flex_rail_package_assistant" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\app\flex\component\rail\sl_flex_rail_packet_asm" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\common\toolchain\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\service\system\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\service\sleeptimer\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\gecko_sdk_4.3.1\platform\service\udelay\inc" -I"C:\Users\ifat\Dropbox\PC\Documents\GitHub\106_EFR_HUB\autogen" -Og -Wall -Wextra -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mcmse -fno-builtin-printf -fno-builtin-sprintf --specs=nano.specs -c -fmessage-length=0 -MMD -MP -MF"gecko_sdk_4.3.1/platform/driver/leddrv/src/sl_simple_led.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


