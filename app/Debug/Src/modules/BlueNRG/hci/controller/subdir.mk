################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/modules/BlueNRG/hci/controller/bluenrg_IFR.c \
../Src/modules/BlueNRG/hci/controller/bluenrg_gap_aci.c \
../Src/modules/BlueNRG/hci/controller/bluenrg_gatt_aci.c \
../Src/modules/BlueNRG/hci/controller/bluenrg_hal_aci.c \
../Src/modules/BlueNRG/hci/controller/bluenrg_l2cap_aci.c \
../Src/modules/BlueNRG/hci/controller/bluenrg_updater_aci.c \
../Src/modules/BlueNRG/hci/controller/bluenrg_utils.c 

OBJS += \
./Src/modules/BlueNRG/hci/controller/bluenrg_IFR.o \
./Src/modules/BlueNRG/hci/controller/bluenrg_gap_aci.o \
./Src/modules/BlueNRG/hci/controller/bluenrg_gatt_aci.o \
./Src/modules/BlueNRG/hci/controller/bluenrg_hal_aci.o \
./Src/modules/BlueNRG/hci/controller/bluenrg_l2cap_aci.o \
./Src/modules/BlueNRG/hci/controller/bluenrg_updater_aci.o \
./Src/modules/BlueNRG/hci/controller/bluenrg_utils.o 

C_DEPS += \
./Src/modules/BlueNRG/hci/controller/bluenrg_IFR.d \
./Src/modules/BlueNRG/hci/controller/bluenrg_gap_aci.d \
./Src/modules/BlueNRG/hci/controller/bluenrg_gatt_aci.d \
./Src/modules/BlueNRG/hci/controller/bluenrg_hal_aci.d \
./Src/modules/BlueNRG/hci/controller/bluenrg_l2cap_aci.d \
./Src/modules/BlueNRG/hci/controller/bluenrg_updater_aci.d \
./Src/modules/BlueNRG/hci/controller/bluenrg_utils.d 


# Each subdirectory must supply rules for building sources it contributes
Src/modules/BlueNRG/hci/controller/%.o: ../Src/modules/BlueNRG/hci/controller/%.c Src/modules/BlueNRG/hci/controller/subdir.mk
	arm-none-eabi-gcc -c "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DBLUENRG_MS=1 -DSTM32L475xx -c -I../Inc -I../Src/modules/BlueNRG/includes -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-modules-2f-BlueNRG-2f-hci-2f-controller

clean-Src-2f-modules-2f-BlueNRG-2f-hci-2f-controller:
	-$(RM) ./Src/modules/BlueNRG/hci/controller/bluenrg_IFR.d ./Src/modules/BlueNRG/hci/controller/bluenrg_IFR.o ./Src/modules/BlueNRG/hci/controller/bluenrg_gap_aci.d ./Src/modules/BlueNRG/hci/controller/bluenrg_gap_aci.o ./Src/modules/BlueNRG/hci/controller/bluenrg_gatt_aci.d ./Src/modules/BlueNRG/hci/controller/bluenrg_gatt_aci.o ./Src/modules/BlueNRG/hci/controller/bluenrg_hal_aci.d ./Src/modules/BlueNRG/hci/controller/bluenrg_hal_aci.o ./Src/modules/BlueNRG/hci/controller/bluenrg_l2cap_aci.d ./Src/modules/BlueNRG/hci/controller/bluenrg_l2cap_aci.o ./Src/modules/BlueNRG/hci/controller/bluenrg_updater_aci.d ./Src/modules/BlueNRG/hci/controller/bluenrg_updater_aci.o ./Src/modules/BlueNRG/hci/controller/bluenrg_utils.d ./Src/modules/BlueNRG/hci/controller/bluenrg_utils.o

.PHONY: clean-Src-2f-modules-2f-BlueNRG-2f-hci-2f-controller

