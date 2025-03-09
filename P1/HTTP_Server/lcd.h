#ifndef __LCD_H
#define __LCD_H


//includes
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h" 
#include "Driver_SPI.h"

////estructura para crear un objeto para la info de LCD usando el write para el get
//typedef struct {
//  char cadena[40];
//  int linea;
//  bool reset;
//} MSGQUEUELCD_OBJ_t;


////funciones
//int Init_LCD(void);
//int Init_testLCD (void);

//osMessageQueueId_t getMsgQueueLCDId(void);

//FUNCIONES LCD
extern void LCD_reset(void);
extern void delay(uint32_t n_microsegundos);
extern void LCD_wr_data(unsigned char data);
extern void LCD_wr_cmd(unsigned char cmd);
extern void startLCD(void);
extern void LCD_init(void);
extern void LCD_updateL1(void);
extern void LCD_updateL2(void);
extern void LCD_update(void);
extern void symbolToLocalBuffer_L1(uint8_t symbol);
extern void symbolToLocalBuffer_L2(uint8_t symbol);
extern void symbolToLocalBuffer(uint8_t line, uint8_t symbol,bool reset);
extern void write_lcd(char cadena[], int linea, bool reset);
extern void cleanBuffer(int linea);
extern void mySPI_callback(uint32_t event);

extern unsigned char buffer[512];

#endif
