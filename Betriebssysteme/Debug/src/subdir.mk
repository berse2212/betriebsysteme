################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/osmpLib.c \
../src/osmprun.c \
../src/starter.c 

OBJS += \
./src/osmpLib.o \
./src/osmprun.o \
./src/starter.o 

C_DEPS += \
./src/osmpLib.d \
./src/osmprun.d \
./src/starter.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -Wextra -Wconversion -c -fmessage-length=0 -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


