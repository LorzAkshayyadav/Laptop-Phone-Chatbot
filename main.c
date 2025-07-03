#include <stdint.h>
#include "stm32f4xx_hal.h"
#include <string.h>
/*Project goal if message sent is Hii/hii blink green and stay on till message received is Bye/bye and glow red till second bye comes then turn off it */
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart2;

uint8_t rx2_byte;
uint8_t rx3_byte;
uint8_t buffer2[100];
uint8_t buffer3[100];
uint8_t idx2 = 0;
uint8_t idx3 = 0;
// Initialize PB10 for green led and PB11 FOR RED
void init_led_pin(void) {
   __HAL_RCC_GPIOB_CLK_ENABLE();
   GPIO_InitTypeDef GPIO_INIT={0};
   GPIO_INIT.Pin=GPIO_PIN_10|GPIO_PIN_11;
   GPIO_INIT.Mode=GPIO_MODE_OUTPUT_PP;
   GPIO_INIT.Pull=GPIO_NOPULL;
   GPIO_INIT.Speed=GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(GPIOB,&GPIO_INIT);

}

// Initialize USART3 on PD8 (TX) and PD9 AS RX
void uart3_init(void)
{
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_USART3_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_INIT={0};
	GPIO_INIT.Pin=GPIO_PIN_8|GPIO_PIN_9;
	GPIO_INIT.Mode=GPIO_MODE_AF_PP;
	GPIO_INIT.Speed=GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_INIT.Pull=GPIO_NOPULL;
	GPIO_INIT.Alternate=GPIO_AF7_USART3;
	HAL_GPIO_Init(GPIOD,&GPIO_INIT);
	huart3.Instance=USART3;
	huart3.Init.BaudRate=9600;
	huart3.Init.HwFlowCtl=UART_HWCONTROL_NONE;
	huart3.Init.Mode=UART_MODE_TX_RX;
	huart3.Init.OverSampling=UART_OVERSAMPLING_16;
	huart3.Init.Parity=UART_PARITY_NONE;
	huart3.Init.StopBits=UART_STOPBITS_1;
	huart3.Init.WordLength=UART_WORDLENGTH_8B;
	HAL_UART_Init(&huart3);
	HAL_NVIC_EnableIRQ(USART3_IRQn);
	HAL_UART_Receive_IT(&huart3,&rx3_byte,1);

}
// Initialize USART2 on PD5 (TX) and PD6 (RX)
void uart2_init(void) {
	__HAL_RCC_GPIOD_CLK_ENABLE();
		__HAL_RCC_USART2_CLK_ENABLE();
		GPIO_InitTypeDef GPIO_INIT={0};
		GPIO_INIT.Pin=GPIO_PIN_5|GPIO_PIN_6;
		GPIO_INIT.Mode=GPIO_MODE_AF_PP;
		GPIO_INIT.Speed=GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_INIT.Pull=GPIO_NOPULL;
		GPIO_INIT.Alternate=GPIO_AF7_USART2;
		HAL_GPIO_Init(GPIOD,&GPIO_INIT);
		huart2.Instance=USART2;
		huart2.Init.BaudRate=9600;
		huart2.Init.HwFlowCtl=UART_HWCONTROL_NONE;
		huart2.Init.Mode=UART_MODE_TX_RX;
		huart2.Init.OverSampling=UART_OVERSAMPLING_16;
		huart2.Init.Parity=UART_PARITY_NONE;
		huart2.Init.StopBits=UART_STOPBITS_1;
		huart2.Init.WordLength=UART_WORDLENGTH_8B;
		HAL_UART_Init(&huart2);
		HAL_NVIC_EnableIRQ(USART2_IRQn);
		HAL_UART_Receive_IT(&huart2,&rx2_byte,1);
}

int main(void) {
    HAL_Init();
    uart3_init();
    init_led_pin();
    uart2_init();


    while (1) {

    }
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    if (huart->Instance == USART3) {
        if (rx3_byte == '\n' || rx3_byte == '\r') {
            buffer3[idx3] = '\0';
            HAL_UART_Transmit(&huart2, buffer3, strlen((char*)buffer3), HAL_MAX_DELAY);
            HAL_UART_Transmit(&huart3, buffer3, strlen((char*)buffer3), HAL_MAX_DELAY);
            HAL_UART_Transmit(&huart3, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
            if (strcmp((char*)buffer3, "hii") == 0 || strcmp((char*)buffer3, "Hii") == 0)
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);

            if (strcmp((char*)buffer3, "bye") == 0 || strcmp((char*)buffer3, "Bye") == 0) {
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);
                HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_11);
            }

            idx3 = 0;
        } else {
            buffer3[idx3++] = rx3_byte;
            if (idx3 >= sizeof(buffer3)) idx3 = 0;
        }

        HAL_UART_Receive_IT(&huart3, &rx3_byte, 1);
    }

    if (huart->Instance == USART2) {
        if (rx2_byte == '\n' || rx2_byte == '\r') {
            buffer2[idx2] = '\0';
            HAL_UART_Transmit(&huart3, buffer2, strlen((char*)buffer2), HAL_MAX_DELAY);
            HAL_UART_Transmit(&huart3, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
            if (strcmp((char*)buffer2, "hii") == 0 || strcmp((char*)buffer2, "Hii") == 0)
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);

            if (strcmp((char*)buffer2, "bye") == 0 || strcmp((char*)buffer2, "Bye") == 0) {
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);
                HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_11);

            }

            idx2 = 0;
        } else {
            buffer2[idx2++] = rx2_byte;
            if (idx2 >= sizeof(buffer2)) idx2 = 0;
        }

        HAL_UART_Receive_IT(&huart2, &rx2_byte, 1);
    }
}


void USART3_IRQHandler(void){
	HAL_UART_IRQHandler(&huart3);

}
void USART2_IRQHandler(void){
	HAL_UART_IRQHandler(&huart2);
}

// SysTick for HAL timing
void SysTick_Handler(void) {
    HAL_IncTick();
}
