################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/osmprun.c \
../src/starter.c 

OBJS += \
./src/osmprun.o \
./src/starter.o 

C_DEPS += \
./src/osmprun.d \
./src/starter.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/tobias/git/betriebsysteme/osmpLib/src" -O0 -g3 -Wall -Wextra -Wconversion -c -fmessage-length=0 -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


