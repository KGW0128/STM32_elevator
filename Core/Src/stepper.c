/*
 * STEPPER.c
 *
 *  Created on: Jan 14, 2025
 *      Author: user
 */

#include "STEPPER.h"


uint16_t moter_move_i = 0;



void stepMoter(uint8_t step)
{
  HAL_GPIO_WritePin(IN1_PORT, IN1_PIN, HALF_STEP_SEQ[step][0]);
  HAL_GPIO_WritePin(IN2_PORT, IN2_PIN, HALF_STEP_SEQ[step][1]);
  HAL_GPIO_WritePin(IN3_PORT, IN3_PIN, HALF_STEP_SEQ[step][2]);
  HAL_GPIO_WritePin(IN4_PORT, IN4_PIN, HALF_STEP_SEQ[step][3]);

}


void rotateSteps(uint16_t steps, uint8_t direction)
{
  for(uint16_t i=0; i<steps; i++)
  {

    //회전 방향에 따른 스탭 패턴
    //방향설정
    uint8_t step;

    if(direction == DIR_CW)
    {
      step= i % 8;
    }
    else
    {
      step = 7 - (i % 8);
    }

    stepMoter(step);


  }
}


void rotateDegrees(uint16_t degrees, uint8_t direction)
{
  uint16_t steps = (uint16_t)((uint32_t)(degrees * STEP_PER_RESOLUTION) / 360);
  rotateSteps(steps, direction);
}



void Tim11_moter_move(uint8_t direction)
{

  uint8_t step;

     if(direction == DIR_CW)
     {
       step= moter_move_i % 8;
     }
     else
     {
       step = 7 - (moter_move_i % 8);
     }

     moter_move_i++;
     stepMoter(step);
}


