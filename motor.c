#include "Motor.h"
#include "tim.h"
#include "Key.h"
#include "SystemFault.h"

STRUCT_MOTOR StructMotor[NUM_OF_MOTOR] = {0};

void vMotor1StateMachine(void);
void vMotor2StateMachine(void);
void vMotor3StateMachine(void);
void vMotorDriver(void);
/*/*************************************************************************************************************
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Return:
 * Created by:
 * Created date:
 * Modified by:
 * Last modified Date:
 * Explain:
 *************************************************************************************************************/
void vMotorInit(void){

    for (uint8_t i = 0; i < NUM_OF_MOTOR; i++) {
        StructMotor[i].Direction = CW;
        StructMotor[i].s16CurrentPwm = 0;
        StructMotor[i].u16TargetPwm = 0;
        StructMotor[i].u8Step = 5;
        StructMotor[i].u8EnableState = DISABLE;
        StructMotor[i].MotionState = IDLE;
    }

    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);

    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
    HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_1);

}

void vEncoderValueSet(uint16_t val) {
    __HAL_TIM_SET_COUNTER(&htim3, val);
}

/*/*************************************************************************************************************
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Return:
 * Created by:
 * Created date:
 * Modified by:
 * Last modified Date:
 * Explain:
 *************************************************************************************************************/
void vMotorControl(void) {
//    if (KeyState.Bit.key_start) {
//        vMotorStart(MOTOR3, CCW);
//    } else if (KeyState.Bit.SW_Handle) {
//        vMotorStart(MOTOR3, CW);
//    }

    vMotor1StateMachine();
    vMotor2StateMachine();
    vMotor3StateMachine();
    vMotorDriver();
}
/*/*************************************************************************************************************
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Return:
 * Created by:
 * Created date:
 * Modified by:
 * Last modified Date:
 * Explain:
 *************************************************************************************************************/
void vMotorDriver(void) {
    for (uint8_t i = 0; i < NUM_OF_MOTOR; i++) {
        if (StructMotor[i].u8EnableState == DISABLE) {
            continue;
        }

        if (StructMotor[i].s16CurrentPwm < StructMotor[i].u16TargetPwm) { //加减速处理
            StructMotor[i].s16CurrentPwm += StructMotor[i].u8Step;
            if (StructMotor[i].s16CurrentPwm >= StructMotor[i].u16TargetPwm) {
                StructMotor[i].s16CurrentPwm = StructMotor[i].u16TargetPwm;
            }
        } else if (StructMotor[i].s16CurrentPwm > StructMotor[i].u16TargetPwm) {
            StructMotor[i].s16CurrentPwm -= StructMotor[i].u8Step;
            if (StructMotor[i].s16CurrentPwm <= StructMotor[i].u16TargetPwm) {
                StructMotor[i].s16CurrentPwm = StructMotor[i].u16TargetPwm;
            }
        } else {
            StructMotor[i].s16CurrentPwm = StructMotor[i].u16TargetPwm;
        }

        switch (i) {
        case MOTOR1:
            if (StructMotor[i].Direction == CW) {
                Motor1_PwmSet_P(MAX_PWM_VALUE - StructMotor[i].s16CurrentPwm);
                Motor1_PwmSet_N(MAX_PWM_VALUE);
            } else if (StructMotor[i].Direction == CCW) {
                Motor1_PwmSet_P(MAX_PWM_VALUE);
                Motor1_PwmSet_N(MAX_PWM_VALUE - StructMotor[i].s16CurrentPwm);
            }
            break;
        case MOTOR2:
            if (StructMotor[i].Direction == CW) {
                Motor2_PwmSet_P(MAX_PWM_VALUE - StructMotor[i].s16CurrentPwm);
                Motor2_PwmSet_N(MAX_PWM_VALUE);
            } else if (StructMotor[i].Direction == CCW) {
                Motor2_PwmSet_P(MAX_PWM_VALUE);
                Motor2_PwmSet_N(MAX_PWM_VALUE - StructMotor[i].s16CurrentPwm);
            }
            break;
        case MOTOR3:
            Motor3_PwmSet_P(StructMotor[i].s16CurrentPwm)
            ;
            break;
        default:
            break;
        }
    }
}
/*/*************************************************************************************************************
 * Function Name:
 * Description:MOTOR1：CW控制矫正机构上升，CCW控制矫正机构下降
 *             MOTOR2：CW控制吸油，CCW控制退油
 *             MOTOR3：单向转动
 * Input:
 * Output:
 * Return:
 * Created by:
 * Created date:
 * Modified by:
 * Last modified Date:
 * Explain:
 *************************************************************************************************************/
void vMotorStart(ENUM_MOTOR_TYPE type, ENUM_MOTOR_DIR dir, uint16_t tim) {
    StructMotor[type].s8MotorStart = 1;
    StructMotor[type].Direction = dir;
    StructMotor[type].u16MotionTime = tim;
}
/*/*************************************************************************************************************
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Return:
 * Created by:
 * Created date:
 * Modified by:
 * Last modified Date:
 * Explain:
 *************************************************************************************************************/
void vMotorStop(ENUM_MOTOR_TYPE type) {
    StructMotor[type].s8MotorStart = 0;
    StructMotor[type].u16TargetPwm = 0;
}
/*/*************************************************************************************************************
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Return:
 * Created by:
 * Created date:
 * Modified by:
 * Last modified Date:
 * Explain:
 *************************************************************************************************************/
void vMotor1StateMachine(void) {
    switch (StructMotor[MOTOR1].MotionState) {
    case IDLE:
        if (StructMotor[MOTOR1].s8MotorStart == 0) {
            break;
        }

        StructMotor[MOTOR1].s16CurrentPwm = 0;
        StructMotor[MOTOR1].u16TargetPwm = 800;
        StructMotor[MOTOR1].u8Step = 5;
        StructMotor[MOTOR1].u8EnableState = ENABLE;
        StructMotor[MOTOR1].MotionState = RUN;
        break;
    case RUN:
        /*限位信号控制电机停止*/
        if (StructMotor[MOTOR1].Direction == CW && KeyState.Bit.Limit_Up1) {/*初始位置停机*/
            StructMotor[MOTOR1].u16TargetPwm = 0;
            StructMotor[MOTOR1].u8Step = 50;
            StructMotor[MOTOR1].MotionState = STOP;
        } else if (StructMotor[MOTOR1].Direction == CW
                && KeyState.Bit.Limit_Up2) {/*上限位停机*/
            StructMotor[MOTOR1].u16TargetPwm = 0;
            StructMotor[MOTOR1].u8Step = 50;
            SystemFaultState.Bit.button_state_fault = 1;
            StructMotor[MOTOR1].MotionState = E_STOP;
        } else if (StructMotor[MOTOR1].Direction == CCW
                && KeyState.Bit.Limit_Down) {/*下限位停机*/
            StructMotor[MOTOR1].u16TargetPwm = 0;
            StructMotor[MOTOR1].u8Step = 50;
            StructMotor[MOTOR1].MotionState = STOP;
        }

        if (StructMotor[MOTOR1].s8MotorStart == 0) {
            StructMotor[MOTOR1].u16TargetPwm = 0;
            StructMotor[MOTOR1].u8Step = 50;
            StructMotor[MOTOR1].MotionState = STOP;
        }

        /*异常保护控制电机停机*/
        if (SystemFaultState.Bit.motor1_overcurrent
                || SystemFaultState.Bit.button_state_fault) {/*过流保护、矫正过程中按键异常*/
            StructMotor[MOTOR1].u16TargetPwm = 0;
            StructMotor[MOTOR1].u8Step = 100;
            StructMotor[MOTOR1].MotionState = E_STOP;
        }

        break;
    case STOP:
        if (StructMotor[MOTOR1].s16CurrentPwm == 0) {
            StructMotor[MOTOR1].u16TargetPwm = 0;
            StructMotor[MOTOR1].s8MotorStart = 0;
            StructMotor[MOTOR1].u8Step = 0;
            StructMotor[MOTOR1].u8EnableState = DISABLE;
            StructMotor[MOTOR1].MotionState = IDLE;
        }
        break;
    case E_STOP:
        if (StructMotor[MOTOR1].s16CurrentPwm == 0) {
            StructMotor[MOTOR1].u16TargetPwm = 0;
            StructMotor[MOTOR1].s8MotorStart = 0;
            StructMotor[MOTOR1].u8Step = 0;
            StructMotor[MOTOR1].u8EnableState = DISABLE;
            StructMotor[MOTOR1].MotionState = IDLE;
        }
        break;
    default:
        break;
    }
}
/*/*************************************************************************************************************
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Return:
 * Created by:
 * Created date:
 * Modified by:
 * Last modified Date:
 * Explain:
 *************************************************************************************************************/
void vMotor2StateMachine(void) {
    switch (StructMotor[MOTOR2].MotionState) {
    case IDLE:
        if (StructMotor[MOTOR2].s8MotorStart == 0) {
            break;
        }

        StructMotor[MOTOR2].s16CurrentPwm = 0;
        StructMotor[MOTOR2].u16TargetPwm = MAX_PWM_VALUE;
//        StructMotor[MOTOR2].u16MotionTime = 2000;/*运行2S*/
        StructMotor[MOTOR2].u8Step = 5;
        StructMotor[MOTOR2].u8EnableState = ENABLE;
        StructMotor[MOTOR2].MotionState = RUN;

        break;
    case RUN:
        /*控制电机停止*/
        if (StructMotor[MOTOR2].u16MotionTime > 0) {
            StructMotor[MOTOR2].u16MotionTime--;
            if (StructMotor[MOTOR2].u16MotionTime == 0
                    || StructMotor[MOTOR2].s8MotorStart == 0) {/*定时时间到，停止电机*/
                StructMotor[MOTOR2].u16TargetPwm = 0;
                StructMotor[MOTOR2].u8Step = 10;
                StructMotor[MOTOR2].MotionState = STOP;
            }
        }
        
        /*异常保护控制电机停机*/
        if (SystemFaultState.Bit.motor2_overcurrent) {/*过流保护*/
            StructMotor[MOTOR2].u16TargetPwm = 0;
            StructMotor[MOTOR2].u8Step = 10;
            StructMotor[MOTOR2].MotionState = E_STOP;
        }
        break;
    case STOP:
        if (StructMotor[MOTOR2].s16CurrentPwm == 0) {
            StructMotor[MOTOR2].u16TargetPwm = 0;
            StructMotor[MOTOR2].s8MotorStart = 0;
            StructMotor[MOTOR2].u16MotionTime = 0;
            StructMotor[MOTOR2].u8Step = 0;
            StructMotor[MOTOR2].u8EnableState = DISABLE;
            StructMotor[MOTOR2].MotionState = IDLE;
        }
        break;
    case E_STOP:
        if (StructMotor[MOTOR2].s16CurrentPwm == 0) {
            StructMotor[MOTOR2].u16TargetPwm = 0;
            StructMotor[MOTOR2].s8MotorStart = 0;
            StructMotor[MOTOR2].u16MotionTime = 0;
            StructMotor[MOTOR2].u8Step = 0;
            StructMotor[MOTOR2].u8EnableState = DISABLE;
            StructMotor[MOTOR2].MotionState = IDLE;
        }
        break;
    default:
        break;
    }
}
/*/*************************************************************************************************************
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Return:
 * Created by:
 * Created date:
 * Modified by:
 * Last modified Date:
 * Explain:
 *************************************************************************************************************/
void vMotor3StateMachine(void) {
    switch (StructMotor[MOTOR3].MotionState) {
    case IDLE:
        if (StructMotor[MOTOR3].s8MotorStart == 0) {
            break;
        }

        StructMotor[MOTOR3].s16CurrentPwm = 0;
        StructMotor[MOTOR3].u16TargetPwm = 600;
//        StructMotor[MOTOR3].u16MotionTime = 5000;
        StructMotor[MOTOR3].u8Step = 5;
        StructMotor[MOTOR3].u8EnableState = ENABLE;
        StructMotor[MOTOR3].MotionState = RUN;
        break;
    case RUN:
        /*控制电机停止*/
        if (StructMotor[MOTOR3].u16MotionTime > 0) {
            StructMotor[MOTOR3].u16MotionTime--;
            if (StructMotor[MOTOR3].u16MotionTime == 0
                    || StructMotor[MOTOR3].s8MotorStart == 0) {/*定时时间到，停止电机*/
                StructMotor[MOTOR3].u16TargetPwm = 0;
                StructMotor[MOTOR3].u8Step = 10;
                StructMotor[MOTOR3].MotionState = STOP;
            }
        }
        /*异常保护控制电机停机*/
        if (SystemFaultState.Bit.motor3_overcurrent) {/*过流保护*/
            StructMotor[MOTOR3].u16TargetPwm = 0;
            StructMotor[MOTOR3].u8Step = 10;
            StructMotor[MOTOR3].MotionState = E_STOP;
        }
        break;
    case STOP:
        if (StructMotor[MOTOR3].s16CurrentPwm == 0) {
            StructMotor[MOTOR3].u16TargetPwm = 0;
            StructMotor[MOTOR3].s8MotorStart = 0;
            StructMotor[MOTOR3].u16MotionTime = 0;
            StructMotor[MOTOR3].u8Step = 0;
            StructMotor[MOTOR3].u8EnableState = DISABLE;
            StructMotor[MOTOR3].MotionState = IDLE;
        }
        break;
    case E_STOP:
        if (StructMotor[MOTOR3].s16CurrentPwm == 0) {
            StructMotor[MOTOR3].u16TargetPwm = 0;
            StructMotor[MOTOR3].s8MotorStart = 0;
            StructMotor[MOTOR3].u16MotionTime = 0;
            StructMotor[MOTOR3].u8Step = 0;
            StructMotor[MOTOR3].u8EnableState = DISABLE;
            StructMotor[MOTOR3].MotionState = IDLE;
        }
        break;
    default:
        break;
    }
}



