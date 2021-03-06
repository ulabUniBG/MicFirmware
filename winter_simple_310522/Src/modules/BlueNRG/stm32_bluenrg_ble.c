/**
 ******************************************************************************
 * @file    stm32_bluenrg_ble.c
 * @author  CL
 * @version V1.0.0
 * @date    04-July-2014
 * @brief
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32_bluenrg_ble.h"
#include "gp_timer.h"

#define HEADER_SIZE 5
#define MAX_BUFFER_SIZE 255
#define TIMEOUT_DURATION 15

extern SPI_HandleTypeDef hspi1;

/* Private function prototypes -----------------------------------------------*/
static void us150Delay(void);
void set_irq_as_output(void);
void set_irq_as_input(void);

/**
 * @brief  Writes data to a serial interface.
 * @param  data1   :  1st buffer
 * @param  data2   :  2nd buffer
 * @param  n_bytes1: number of bytes in 1st buffer
 * @param  n_bytes2: number of bytes in 2nd buffer
 * @retval None
 */
void Hal_Write_Serial(const void* data1, const void* data2, int32_t n_bytes1,
		int32_t n_bytes2) {
	struct timer t;
	int ret;
	uint8_t data2_offset = 0;

	Timer_Set(&t, CLOCK_SECOND / 10);

	Disable_SPI_IRQ();

#if ENABLE_SPI_FIX
	BlueNRG_IRQ_High();
#endif

	while (1) {
		ret = BlueNRG_SPI_Write((uint8_t *) data1,
				(uint8_t *) data2 + data2_offset, n_bytes1, n_bytes2);

		if (ret >= 0) {
			n_bytes1 = 0;
			n_bytes2 -= ret;
			data2_offset += ret;
			if (n_bytes2 == 0)
				break;
		}

		if (Timer_Expired(&t)) {
			break;
		}
	}
#if ENABLE_SPI_FIX
	BlueNRG_Release_IRQ();
#endif

	Enable_SPI_IRQ();
}

/**
 * @brief  Resets the BlueNRG.
 * @param  None
 * @retval None
 */
void BlueNRG_RST(void) {
	HAL_GPIO_WritePin(BNRG_SPI_RESET_PORT, BNRG_SPI_RESET_PIN, GPIO_PIN_RESET);
	HAL_Delay(5);
	HAL_GPIO_WritePin(BNRG_SPI_RESET_PORT, BNRG_SPI_RESET_PIN, GPIO_PIN_SET);
	HAL_Delay(5);
}

/**
 * @brief  Activate internal bootloader using pin.
 * @param  None
 * @retval None
 */
void BlueNRG_HW_Bootloader(void) {
	Disable_SPI_IRQ();
	set_irq_as_output();
	BlueNRG_RST();
	set_irq_as_input();
	Enable_SPI_IRQ();
}

/**
 * @brief  Reads from BlueNRG SPI buffer and store data into local buffer.
 * @param  hspi     : Handle of the STM32Cube HAL SPI interface
 * @param  buffer   : Buffer where data from SPI are stored
 * @param  buff_size: Buffer size
 * @retval int32_t  : Number of read bytes
 */
int32_t BlueNRG_SPI_Read_All(uint8_t *buffer, uint8_t buff_size) {
	uint16_t byte_count;
	uint8_t len = 0;

	const uint8_t header_master[5] = { 0x0b, 0x00, 0x00, 0x00, 0x00 };
	uint8_t header_slave[5];

	__disable_irq();

	/* CS reset */
	HAL_GPIO_WritePin(BNRG_SPI_CS_PORT, BNRG_SPI_CS_PIN, GPIO_PIN_RESET);

	/* Read the header */
	HAL_SPI_TransmitReceive(&hspi1, header_master, header_slave, HEADER_SIZE,
			TIMEOUT_DURATION);

	if (header_slave[0] == 0x02) {
		// device is ready

		byte_count = (header_slave[4] << 8) | header_slave[3];

		if (byte_count > 0) {

			// avoid to read more data that size of the buffer
			if (byte_count > buff_size)
				byte_count = buff_size;

			HAL_SPI_Receive(&hspi1, buffer, byte_count, TIMEOUT_DURATION);

			len = byte_count;
		}
	}
	/* Release CS line */
	HAL_GPIO_WritePin(BNRG_SPI_CS_PORT, BNRG_SPI_CS_PIN, GPIO_PIN_SET);

	__enable_irq();

	return len;
}

/**
 * @brief  Writes data from local buffer to SPI.
 * @param  hspi     : Handle of the STM32Cube HAL SPI interface
 * @param  data1    : First data buffer to be written
 * @param  data2    : Second data buffer to be written
 * @param  Nb_bytes1: Size of first data buffer to be written
 * @param  Nb_bytes2: Size of second data buffer to be written
 * @retval Number of read bytes
 */
int32_t BlueNRG_SPI_Write(uint8_t* data1, uint8_t* data2, uint8_t Nb_bytes1,
		uint8_t Nb_bytes2) {
	int16_t result = 0;
	uint16_t tx_bytes;
	uint8_t rx_bytes;

	const uint8_t header_master[5] = { 0x0a, 0x00, 0x00, 0x00, 0x00 };
	uint8_t header_slave[5] = { 0x00 };

	/* CS reset */
	HAL_GPIO_WritePin(BNRG_SPI_CS_PORT, BNRG_SPI_CS_PIN, GPIO_PIN_RESET);

	/* Exchange header */
	HAL_SPI_TransmitReceive(&hspi1, header_master, header_slave, HEADER_SIZE,
			TIMEOUT_DURATION);

	if (header_slave[0] != 0x02) {
		result = -1;
		goto failed;
		// BlueNRG not awake.
	}

	rx_bytes = header_slave[1];

	if (rx_bytes < Nb_bytes1) {
		result = -2;
		goto failed;
		// BlueNRG .
	}

	HAL_SPI_Transmit(&hspi1, data1, Nb_bytes1, TIMEOUT_DURATION);

	rx_bytes -= Nb_bytes1;

	if (Nb_bytes2 > rx_bytes) {
		tx_bytes = rx_bytes;
	} else {
		tx_bytes = Nb_bytes2;
	}

	HAL_SPI_Transmit(&hspi1, data2, tx_bytes, TIMEOUT_DURATION);

	result = tx_bytes;

	failed:

	/* Release CS line */
	HAL_GPIO_WritePin(BNRG_SPI_CS_PORT, BNRG_SPI_CS_PIN, GPIO_PIN_SET);

	return result;
}

/**
 * @brief  Set in Output mode the IRQ.
 * @param  None
 * @retval None
 */
void set_irq_as_output() {
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Pull IRQ high */
	GPIO_InitStructure.Pin = BNRG_SPI_IRQ_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = BNRG_SPI_IRQ_SPEED;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(BNRG_SPI_IRQ_PORT, &GPIO_InitStructure);
	HAL_GPIO_WritePin(BNRG_SPI_IRQ_PORT, BNRG_SPI_IRQ_PIN, GPIO_PIN_SET);
}

/**
 * @brief  Set the IRQ in input mode.
 * @param  None
 * @retval None
 */
void set_irq_as_input() {
	GPIO_InitTypeDef GPIO_InitStructure;

	/* IRQ input */
	GPIO_InitStructure.Pin = BNRG_SPI_IRQ_PIN;
	GPIO_InitStructure.Mode = BNRG_SPI_IRQ_MODE;
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	GPIO_InitStructure.Speed = BNRG_SPI_IRQ_SPEED;
	GPIO_InitStructure.Alternate = BNRG_SPI_IRQ_ALTERNATE;
	HAL_GPIO_Init(BNRG_SPI_IRQ_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pull = BNRG_SPI_IRQ_PULL;
	HAL_GPIO_Init(BNRG_SPI_IRQ_PORT, &GPIO_InitStructure);
}

/**
 * @brief  Enable SPI IRQ.
 * @param  None
 * @retval None
 */
void Enable_SPI_IRQ(void) {
	HAL_NVIC_EnableIRQ(BNRG_SPI_EXTI_IRQn);
}

/**
 * @brief  Disable SPI IRQ.
 * @param  None
 * @retval None
 */
void Disable_SPI_IRQ(void) {
	HAL_NVIC_DisableIRQ(BNRG_SPI_EXTI_IRQn);
}

/**
 * @brief  Clear Pending SPI IRQ.
 * @param  None
 * @retval None
 */
void Clear_SPI_IRQ(void) {
	HAL_NVIC_ClearPendingIRQ(BNRG_SPI_EXTI_IRQn);
}

/**
 * @brief  Clear EXTI (External Interrupt) line for SPI IRQ.
 * @param  None
 * @retval None
 */
void Clear_SPI_EXTI_Flag(void) {
	__HAL_GPIO_EXTI_CLEAR_IT(BNRG_SPI_EXTI_PIN);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
