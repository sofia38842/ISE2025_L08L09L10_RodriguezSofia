#include "lcd.h"
#include "Arial12x12.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#define TRANSFERDONESPI 0x01U


//osMessageQueueId_t id_MsgQueueLCD;   //id de la cola
extern osThreadId_t TID_Display;

//DE LA PRACTICA 4
extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;


//estructura GPIO para los pines 
GPIO_InitTypeDef GPIO_InitStruct;


//nombramos al timer para el delay y manejo del reset
static TIM_HandleTypeDef tim7;

//buffer para escribir en LCD
unsigned char buffer[512];

static uint16_t positionL1=0; //linea1, pagina 0
static uint16_t positionL2=256;  //donde empieza linea 2, es decir, pagina 2

//FUNCIONES PARA LCD
void LCD_reset(void);
void delay(uint32_t n_microsegundos);
void LCD_wr_data(unsigned char data);
void LCD_wr_cmd(unsigned char cmd);
void startLCD(void);
void LCD_init(void);
void LCD_updateL1(void);
void LCD_updateL2(void);
void LCD_update(void);
void symbolToLocalBuffer_L1(uint8_t symbol);
void symbolToLocalBuffer_L2(uint8_t symbol);
void symbolToLocalBuffer(uint8_t line, uint8_t symbol,bool reset);
void write_lcd(char cadena[], int linea, bool reset);
void cleanBuffer(int linea);
void mySPI_callback(uint32_t event);


/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
//osThreadId_t tid_Lcd;                        // thread id para comunicacion SPI

//void LCD (void *argument);                   // thread function
 
//int Init_LCD (void) {
//  
//  //CREACION COLA 
//   tid_Lcd = osThreadNew(LCD, NULL, NULL);
//  if (tid_Lcd == NULL) {
//    return(-1);
//  }
// 
//  return(0);
//}

///* Hilo de test*/
//int Init_testLCD (void) {
//  
//  //CREACION COLA 
//   id_MsgQueueLCD = osMessageQueueNew(16,sizeof(MSGQUEUELCD_OBJ_t),NULL); //(NUMERO,TAMAÑO, NULL)   tamaño de la funcion de readpins
//   tid_testLCD = osThreadNew(testLCD, NULL, NULL);
//  if (tid_Lcd == NULL) {
//    return(-1);
//  }
// 
//  return(0);
//}


// 
//void LCD (void *argument) {
//  
//	LCD_update();

//  while (1) {
//     
//  }
//}

//osMessageQueueId_t getMsgQueueLCDId(void){
//	return id_MsgQueueLCD;
//}

///* Hilo de test*/
//void testLCD (void *argument) {
//  
//  MSGQUEUELCD_OBJ_t infoLCD;

//  while (1) {}
//}

// FUNCION PARA LIMPIAR LINEA CORRESPONDIENTE
void cleanBuffer(int linea){
 
	int i = 0;
	int j = 0;
	
	if(linea == 1){
		j = 0;
	}else{
		j = 256;
	}
 
	// Vamos a limpiar exclusivamente una linea, por eso recorremos 256 posiciones
	for(i=0; i < 256; i++){
    buffer[i+j] = 0x00;
  }
 
	if(linea == 1){
		positionL1=0;
	}else{
		positionL2=256;
	}
 
	LCD_update();

}




//DE LAS PRACTICAS ANTERIORES DE LCD


//caracteres
void write_lcd(char cadena[], int linea, bool reset){
  int j=0;
  
  if (reset)
    cleanBuffer(linea);
  
  
  for(j=0; j < strlen(cadena); j++){
   symbolToLocalBuffer(linea, cadena[j], reset);
 
  }
   if(linea ==1)
   LCD_updateL1(); //sumamos el valor de offset 
  else
    LCD_updateL2();
}

//PARA AMBAS LINEAS
void symbolToLocalBuffer(uint8_t line, uint8_t symbol,bool reset){
  uint8_t i, value1, value2;
  uint16_t offset=0;
  uint16_t position=0;
  
  position = (line == 1) ? positionL1 : positionL2; //si hay linea, elijo donde toque
  
  //esto significa la linea de arriba
//  if(line ==1)
//   position= positionL1;
//  else
//    position= positionL2;

  offset=25*(symbol - ' ');   //nos da la posicion de la tabla ASCII, tengo 25 columnas
  
  for(i=0; i<12; i++){
    value1=Arial12x12[offset+i*2+1]; //defeine el valor del array Arial12x12
    value2=Arial12x12[offset+i*2+2]; 
    
    buffer[i+position]=value1;  ////define posicion pantalla de pagina 0
    buffer[i+128+position]=value2; //+128 seria siguiente pagina, la 1

  }
  
  //para escribir en la linea correcta 
   if(line ==1)
   positionL1= positionL1+Arial12x12[offset]; //sumamos el valor de offset 
  else
    positionL2= positionL2+Arial12x12[offset];
}

//funcion para caracteres ASCII  La primera columna de arial12x12 dice el espacio que ocupa ese caracter
void symbolToLocalBuffer_L1(uint8_t symbol){
 
  uint8_t i, value1, value2;
  uint16_t offset=0;
  
  offset=25*(symbol - ' ');   //nos da la posicion de la tabla ASCII, tengo 25 columnas
  
  for(i=0; i<12; i++){
    value1=Arial12x12[offset+i*2+1]; //defeine el valor del array Arial12x12
    value2=Arial12x12[offset+i*2+2]; 
    
    buffer[i+positionL1]=value1;  ////define posicion pantalla de pagina 0
    buffer[i+128+positionL1]=value2; //+128 seria siguiente pagina, la 1
    
    //para pintar por la mitad tendriamos que meter el +64, mas o menos a la mitad
//    buffer[i+64/*posicionL1*/]=value1; 
//    buffer[i+128+64/*posicionL1*/]=value1; 
    
  }
 
  positionL1= positionL1+Arial12x12[offset]; //guardara lo que ya hemos escrito para no escribir encima 
}
//funcion anterior pero para la linea 2
void symbolToLocalBuffer_L2(uint8_t symbol){
 
  uint8_t i, value1, value2;
  uint16_t offset=0;
  
  offset=25*(symbol - ' ');   //nos da la posicion de la tabla ASCII, tengo 25 columnas
  
  for(i=0; i<12; i++){
    value1=Arial12x12[offset+i*2+1]; //defeine el valor del array Arial12x12
    value2=Arial12x12[offset+i*2+2]; 
    
    buffer[i+256+positionL2]=value1;  ////define posicion pantalla de pagina 0
    buffer[i+384+positionL2]=value2; //+128 seria siguiente pagina, la 1
    
    //para pintar por la mitad tendriamos que meter el +64, mas o menos a la mitad
//    buffer[i+64/*posicionL1*/]=value1; 
//    buffer[i+128+64/*posicionL1*/]=value1; 
    
  }
 
  positionL2= positionL2+Arial12x12[offset]; //guardara lo que ya hemos escrito para no escribir encima 
}
//funcion que llamara a las dos funciones de reset e init

void startLCD(void){
  LCD_reset();
  LCD_init();
}



//pasa por todas las paginas escribiendo lo que necesitamos donde necesitamos
void LCD_update(void){
  int i;
   LCD_wr_cmd(0x00); //4 bits de la parte baja de la direccion a 0
   LCD_wr_cmd(0x10); //4 bits de la parte alta de la direccion a 0
   LCD_wr_cmd(0xB0); //pagina 0
  
  for(i=0; i<128;i++){
    LCD_wr_data(buffer[i]);     //recorrer el buffer de la pagina 0 para escribir lo que queremos
  }
  
  
   LCD_wr_cmd(0x00); //4 bits de la parte baja de la direccion a 0
   LCD_wr_cmd(0x10); //4 bits de la parte alta de la direccion a 0
   LCD_wr_cmd(0xB1); //pagina 1
  
  for(i=128; i<256;i++){
    LCD_wr_data(buffer[i]);     //recorrer el buffer de la pagina 0 para escribir lo que queremos
  }
  LCD_wr_cmd(0x00); //4 bits de la parte baja de la direccion a 0
   LCD_wr_cmd(0x10); //4 bits de la parte alta de la direccion a 0
   LCD_wr_cmd(0xB2); //pagina 2
  
  for(i=256; i<384;i++){
    LCD_wr_data(buffer[i]);     //recorrer el buffer de la pagina 0 para escribir lo que queremos
  }
   
   LCD_wr_cmd(0x00); //4 bits de la parte baja de la direccion a 0
   LCD_wr_cmd(0x10); //4 bits de la parte alta de la direccion a 0
   LCD_wr_cmd(0xB3); //pagina 3
  
  for(i=384; i<512;i++){
    LCD_wr_data(buffer[i]);     //recorrer el buffer de la pagina 0 para escribir lo que queremos
  }

}



void LCD_updateL1(void){
   int i;
   LCD_wr_cmd(0x00); //4 bits de la parte baja de la direccion a 0
   LCD_wr_cmd(0x10); //4 bits de la parte alta de la direccion a 0
   LCD_wr_cmd(0xB0); //pagina 0
  
  for(i=0; i<128;i++){
    LCD_wr_data(buffer[i]);     //recorrer el buffer de la pagina 0 para escribir lo que queremos
  }
  
  
   LCD_wr_cmd(0x00); //4 bits de la parte baja de la direccion a 0
   LCD_wr_cmd(0x10); //4 bits de la parte alta de la direccion a 0
   LCD_wr_cmd(0xB1); //pagina 1
  
  for(i=128; i<256;i++){
    LCD_wr_data(buffer[i]);     //recorrer el buffer de la pagina 0 para escribir lo que queremos
  }
  

}

void LCD_updateL2(void){
   
   int i;
   LCD_wr_cmd(0x00); //4 bits de la parte baja de la direccion a 0
   LCD_wr_cmd(0x10); //4 bits de la parte alta de la direccion a 0
   LCD_wr_cmd(0xB2); //pagina 2
  
  for(i=256; i<384;i++){
    LCD_wr_data(buffer[i]);     //recorrer el buffer de la pagina 0 para escribir lo que queremos
  }
   
   LCD_wr_cmd(0x00); //4 bits de la parte baja de la direccion a 0
   LCD_wr_cmd(0x10); //4 bits de la parte alta de la direccion a 0
   LCD_wr_cmd(0xB3); //pagina 3
  
  for(i=384; i<512;i++){
    LCD_wr_data(buffer[i]);     //recorrer el buffer de la pagina 0 para escribir lo que queremos
  }
}



//numeros hezadecimales que correcponden a los 8 bits de las tablas 
// que dicen que funcion debe hacer el LCD
void LCD_init(void){
  LCD_wr_cmd(0xAE); //1010 1110 en la tabla dicce que es display off
  LCD_wr_cmd(0xA2); 
  LCD_wr_cmd(0xA0); 
  LCD_wr_cmd(0xC8);
  LCD_wr_cmd(0x22);
  LCD_wr_cmd(0x2F);
  LCD_wr_cmd(0x40);
  LCD_wr_cmd(0xAF);
  LCD_wr_cmd(0x81);
  LCD_wr_cmd(0x15);
  LCD_wr_cmd(0xA4);
  LCD_wr_cmd(0xA6);
 
}

void LCD_reset(void){
  //INICIALIZACION Y CONFIG DE UN DRIVER SPI PARA LCD
  SPIdrv -> Initialize(mySPI_callback);
  SPIdrv -> PowerControl(ARM_POWER_FULL);
  SPIdrv -> Control(ARM_SPI_MODE_MASTER |ARM_SPI_CPOL1_CPHA1| ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8),20000000); //frecuencia SCLK, 20MHz y 8 bits
  
  //inicializar los pines que necesitamos
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  
  GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pin=GPIO_PIN_6;
  GPIO_InitStruct.Pull=GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin=GPIO_PIN_13;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  
  
  GPIO_InitStruct.Pin=GPIO_PIN_14;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  
  //GENERACION DE LA SEÑAL RESET SEGUN SU GRAFICA
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6, GPIO_PIN_RESET);
  delay(1);
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6, GPIO_PIN_SET);
  delay(1000);
}


void delay(uint32_t n_microsegundos){
  //inciializacion delay timer 7, reloj de 84MHz
  __HAL_RCC_TIM7_CLK_ENABLE();
  tim7.Instance= TIM7;
  tim7.Init.Prescaler=83; //1us por cuenta 
  tim7.Init.Period=(n_microsegundos-1); //el timer salta cada n cuentas 
  HAL_TIM_Base_Init(&tim7); //inicializo
  HAL_TIM_Base_Start(&tim7); 
  
  
  
  while(! __HAL_TIM_GET_FLAG(&tim7, TIM_FLAG_UPDATE)){}; //comprueba que el timer salta y slae. si no sale es que no va bien
  
  __HAL_TIM_CLEAR_FLAG(&tim7, TIM_FLAG_UPDATE); //limpiamos el flag del timer 
  
  HAL_TIM_Base_Stop(&tim7); //paramos el timer
  HAL_TIM_Base_DeInit(&tim7); 


}

//funcion escribe dato en el LCD
void LCD_wr_data(unsigned char data){
  
  //SELECCIONA CS=0
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
  
  //SELECCIONA A0 =1
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
  
  //Escribir un dato (data) usando la función SPIDrv->Send(…);
  SPIdrv->Send(&data,sizeof(data));
  
  // Esperar a que se libere el bus SPI;
  osThreadFlagsWait(TRANSFERDONESPI,osFlagsWaitAny,osWaitForever);
  
  // Seleccionar CS = 1;
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);

}


 //Función que escribe un comando en el LCD.
void LCD_wr_cmd(unsigned char cmd){
  
  //SELECCIONA CS=0
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
  
  //SELECCIONA A0 =0
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);
  
  //Escribir un dato (data) usando la función SPIDrv->Send(…);
  SPIdrv->Send(&cmd,sizeof(cmd));
  
  // Esperar a que se libere el bus SPI;
	osThreadFlagsWait(TRANSFERDONESPI,osFlagsWaitAny,osWaitForever);
  
  // Seleccionar CS = 1;
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);

}

void mySPI_callback(uint32_t event)
{
    switch (event)
    {
    case ARM_SPI_EVENT_TRANSFER_COMPLETE:
        /* Success: Wakeup Thread */
         osThreadFlagsSet(TID_Display, TRANSFERDONESPI);
        break;
    case ARM_SPI_EVENT_DATA_LOST:
        
        break;
    case ARM_SPI_EVENT_MODE_FAULT:
        
        break;
		default :
			break;
    }
}
