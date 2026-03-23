#ifndef MOTOR_H_
#define MOTOR_H_

#include "main.h"

#define MAX_PWM_VALUE (1199)

#define Motor1_PwmSet_P(x) __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, x);
#define Motor1_PwmSet_N(x) __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, x);
#define Motor2_PwmSet_P(x) __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, x);
#define Motor2_PwmSet_N(x) __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, x);
#define Motor3_PwmSet_P(x) __HAL_TIM_SET_COMPARE(&htim15, TIM_CHANNEL_1, x);


#define ENCODE_ZERO_INIT_COUNT               1000                             //零位编码器位置

typedef enum {
    MOTOR1 = 0, MOTOR2, MOTOR3, NUM_OF_MOTOR,
} ENUM_MOTOR_TYPE;

typedef enum {
    CW = 0, CCW,
} ENUM_MOTOR_DIR;

typedef enum {
    IDLE = 0, RUN, STOP, E_STOP,
} ENUM_MOTIOM_STATE;

typedef struct{
    ENUM_MOTOR_DIR Direction;
    ENUM_MOTIOM_STATE MotionState;
    uint8_t u8EnableState;

    uint16_t u16TargetPwm;
    int16_t s16CurrentPwm;
    uint16_t u16MotionTime;
    uint8_t u8Step;
    int8_t s8MotorStart;
}STRUCT_MOTOR;

extern STRUCT_MOTOR StructMotor[NUM_OF_MOTOR];

void vMotorInit(void);
void vEncoderValueSet(uint16_t val);
void vMotorControl(void);
void vMotorStart(ENUM_MOTOR_TYPE type, ENUM_MOTOR_DIR dir, uint16_t tim);
void vMotorStop(ENUM_MOTOR_TYPE type);


#endif /* MOTOR_H_ */

