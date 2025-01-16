/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stepper.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

//모터 회전방향
#define UP   0
#define DOWN 1

uint8_t arrive_msg = 0; //도착 메세지 출력 플레그용 변수
uint8_t move_msg = 0;//각 층에 도착할 때마다 LCD에 층수 표시할 msg변수
uint8_t moving_elv = 0;//엘리베이터 움직이는 중이라고 LCD 출력용 msg변수


extern uint16_t moter_move_i; //모터 회전 횟수

uint8_t moter_turn = 0; //모터 회전 방향 저장용 변수

uint8_t start_floor_num = 1; //현재 및 시작 층수 저장 변수
uint8_t next_floor_num = 0;  //이동할 층수 저장 변수


uint8_t SB_moter_set = 0;//서보모터 문열림 확인용 변수

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

  uint8_t arrive_flag = 0;//도착 확인용 flag
  uint16_t pin_num = GPIO_Pin;//핀 번호 저장할 임시 변수


  //포토인터럽트가 발생 할 때 exti2번이 최상위에 있을 때
  //lcd가 계속 초기화 되는 문제
  //그래서 2층인 exti12번을 3층으로 설정해서 최상위로 두고 exti2을
  //2층으로 내렸는데 문제 해결
  //개인적 뇌피셜로는 인터럽트에서 우선순위가 같은 최상위(0번)일 떄
  //가장 빠른 번호 exti가 실행될때 문제 인듯
  //exti1 을 끌고와서 테스트 해보기



  switch(pin_num)
  {

    case GPIO_PIN_12://3층

      //해당 층에 도착했으면
      if(start_floor_num == 3)
      {
        arrive_flag = 1;

      }
      //해당 층에 거쳐가면 층수 변경
      move_msg = 3;
      break;




    case GPIO_PIN_2://2층

      //해당 층에 도착했으면
      if(start_floor_num == 2)
      {
        arrive_flag = 1;

      }
      //해당 층에 거쳐가면 층수 변경
      move_msg = 2;
      break;




    case GPIO_PIN_7://1층

      //해당 층에 도착했으면
      if(start_floor_num == 1)
      {
        arrive_flag = 1;

      }
      //해당 층에 거쳐가면 층수 변경
      move_msg = 1;
      break;

  }




  if(arrive_flag == 1)
  {
    HAL_NVIC_DisableIRQ(EXTI2_IRQn);//pd2끄기
    HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);//pc12끄기
    HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);//pa7끄기



    moter_move_i=0;
    arrive_msg = 1;
    HAL_TIM_Base_Stop_IT(&htim11);
  }


}



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM11)//모터 회전하라는 타이머 플레그가 세워지면
  {
    Tim11_moter_move(moter_turn);//모터 한번 움직임
  }

  if(htim->Instance == TIM10)
  {

    if(SB_moter_set == 0)
    {

    TIM2->CCR1 = 150; // 서보모터 중앙
    SB_moter_set=1;
    }
    else
    {
    TIM2->CCR1 = 50; // 서보모터 -90

    __HAL_TIM_SET_COUNTER(&htim10, 0);
    HAL_TIM_Base_Stop_IT(&htim10);

    SB_moter_set=0;
    }
  }


}



/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM11_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_TIM10_Init();
  /* USER CODE BEGIN 2 */


  //서보모터 pwm 활성화
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);


  //서보모터 딜레이 계산용 10번 타이머 활성화(시스템 딜레이 방지용 타이머)
  HAL_TIM_PWM_Stop(&htim10, TIM_CHANNEL_1);


  //스탭모터 딜레이 계산용 11번 타이머 활성화(시스템 딜레이 방지용 타이머)
  HAL_TIM_PWM_Start(&htim11, TIM_CHANNEL_1);





  LCD_Init();


  moveCusor(0, 0);
  LCD_String("DH elevator");
  moveCusor(1, 0);
  LCD_String("Press The Button");

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 1);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);


  //문 초기값 닫힘
  TIM2->CCR1 = 50; // 서보모터 -90



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {


    next_floor_num = 0; //이동 층수 초기화



    if(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8))//3층
    {


      next_floor_num = 3; //이동 층수 저장

      if(start_floor_num != next_floor_num)
      {



        //현재 층수 메세지 출력
        //move_msg = 3;

        moter_turn = UP; //시계
        moving_elv = 1;




      }



    }


    if(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6))//2층
    {



      next_floor_num = 2; //이동 층수 저장

      if(start_floor_num != next_floor_num)
      {


        //move_msg = 2;


        if (start_floor_num > 2) // 현재 층이 2층보다 높으면 내려감
        {
          moter_turn = DOWN;
        }
        else if (start_floor_num <= 2) // 현재 층이 2층보다 낮으면 올라감
        {
          moter_turn = UP;
        }

        moving_elv = 1;



      }


    }


    if(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5))//1층
    {

      next_floor_num = 1; //이동 층수 저장

      if(start_floor_num != next_floor_num)
      {

        //move_msg = 1;

        moter_turn = DOWN; //반시계
        moving_elv = 1;




      }


    }





    if(moving_elv==1)
    {
      start_floor_num = next_floor_num; //현재 층수를 이동층수로 갱신

      HAL_NVIC_EnableIRQ(EXTI2_IRQn);//pd2키기
      HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);//pc12키기
      HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);//pa7키기





      moveCusor(1, 0);
      LCD_String("                   "); // 기존 내용 지우기
      moveCusor(1, 0);
      char buffer[20]; // 충분히 큰 크기의 버퍼 (최대 20자)
      sprintf(buffer, "Go to the %dF...", start_floor_num);
      LCD_String(buffer); // 동적으로 생성된 문자열 출력


      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 1);


      SB_moter_set = 1;//서보모터
      //__HAL_TIM_SET_COUNTER(&htim10, 0);
      HAL_TIM_Base_Start_IT(&htim11);
      moving_elv = 0;
    }





    //내가 가고 싶은 층수에 도착 했다면
    if(arrive_msg != 0)
    {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);//인터럽트 체크용 led

      //TIM2->CCR1 = 150; // 서보모터 -90
      SB_moter_set = 0;
      //__HAL_TIM_SET_COUNTER(&htim10, 0);
      HAL_TIM_Base_Start_IT(&htim10);


      //도착 메세지 출력
      moveCusor(1, 0);
      LCD_String("                   ");
      moveCusor(1, 0);
      LCD_String("Arrive !!");
      arrive_msg = 0;



      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 1);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);

    }
    else
    {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);//인터럽트 체크용 led
    }


    //층수가 이동 되었을 때
    if(move_msg != 0)
    {
      //LCD에 층수 표시
      moveCusor(0, 0);
      LCD_String("                   ");
      moveCusor(0, 0);
      LCD_Data('0' + move_msg); // uint8_t 값을 바로 문자로 변환하여 출력
      move_msg = 0;
    }







    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
