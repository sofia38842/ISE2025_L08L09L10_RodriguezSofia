/*-----------------------------------------------------------------------------
 * Copyright (c) 2018 Arm Limited (or its affiliates). All
 * rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   1.Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   2.Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   3.Neither the name of Arm nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *-----------------------------------------------------------------------------
 * Name:    LED_32F469IDISCOVERY.c
 * Purpose: LED interface for 32F469IDISCOVERY Kit
 * Rev.:    1.0.0
 *----------------------------------------------------------------------------*/

#include "stm32f4xx_hal.h"
#include "LEDS.h"

/* GPIO Pin identifier */
typedef struct _GPIO_PIN {
  GPIO_TypeDef *port;
  uint16_t      pin;
  uint16_t      reserved;
} GPIO_PIN;

/* LED GPIO Pins */
/*static*/const GPIO_PIN LED_PIN[] = { //Primeramente se definen los LEDs de la tarjeta.
  { GPIOB, GPIO_PIN_0, 0},
  { GPIOB, GPIO_PIN_7, 0},
  { GPIOB, GPIO_PIN_14,0},
};

#define LED_COUNT (sizeof(LED_PIN)/sizeof(GPIO_PIN))


/**
  \fn          int32_t LED_Initialize (void)
  \brief       Initialize LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/

//Funcion que inicializa los LEDs para que puedan encenderse:
int32_t LED_Initialize_stm (void) 
{
  GPIO_InitTypeDef GPIO_InitStruct;
  uint32_t i;

  __GPIOG_CLK_ENABLE(); //GPIO Ports Clock Enable.
  __GPIOB_CLK_ENABLE();

  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  for (i = 0; i < LED_COUNT; i++) 
	{
    //HAL_GPIO_WritePin(LED_PIN[i].port, LED_PIN[i].pin, 0);
    GPIO_InitStruct.Pin = LED_PIN[i].pin;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }

  return 0;
}

/**
  \fn          int32_t LED_Uninitialize (void)
  \brief       De-initialize LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/

int32_t LED_Uninitialize_stm (void) {
  uint32_t i;

  for (i = 0; i < LED_COUNT; i++) {
    HAL_GPIO_DeInit(LED_PIN[i].port, LED_PIN[i].pin);
  }

  return 0;
}

/**
  \fn          int32_t LED_On (uint32_t num)
  \brief       Turn on requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/

//Funcion que realiza el encendido de un LED:
int32_t LED_On_stm (uint32_t num) 
{
  int32_t retCode = 0;

  if (num < LED_COUNT) //Recibe como parametro el numero del LED, y si es menor del total...
	{
    HAL_GPIO_WritePin(LED_PIN[num].port, LED_PIN[num].pin, 1); //...se realizara el encendido.
  }
  else 
	{
    retCode = -1;
  }

  return retCode;
}

/**
  \fn          int32_t LED_Off (uint32_t num)
  \brief       Turn off requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/

//Funcion que realiza el apagado de un LED:
int32_t LED_Off_stm (uint32_t num) 
{
  int32_t retCode = 0;

  if (num < LED_COUNT) // ---> El razonamiento es equivalente al de la funcion de encendido.
	{
    HAL_GPIO_WritePin(LED_PIN[num].port, LED_PIN[num].pin, 0);
  }
  else 
	{
    retCode = -1;
  }

  return retCode;
}

/**
  \fn          int32_t LED_SetOut (uint32_t val)
  \brief       Write value to LEDs
  \param[in]   val  value to be displayed on LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_SetOut_stm (uint32_t val) {
  uint32_t n;

  for (n = 0; n < LED_COUNT; n++) {
    if (val & (1 << n)) LED_On_stm (n);
    else                LED_Off_stm(n);
  }

  return 0;
}

/**
  \fn          uint32_t LED_GetCount (void)
  \brief       Get number of LEDs
  \return      Number of available LEDs
*/
uint32_t LED_GetCount_stm (void) {

  return LED_COUNT;
}

