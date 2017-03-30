#ifndef __IMAGE_PROCESSING_H
#define __IMAGE_PROCESSING_H

#include "stm32f4xx.h"
#include "include.h"

#include "./systick/bsp_SysTick.h"
#include "./camera/bsp_ov5640.h"
#include "./lcd/bsp_lcd.h"


//#define	IMG_WIDTH						800
//#define	IMG_HEIGHT						480
#define	IMG_WIDTH						80
#define	IMG_HEIGHT						48
#define CAMERA_BUFFER_OFFSET			IMG_WIDTH*IMG_HEIGHT*2

extern uint8_t CAMERA_BUFFER_ARRAY[2][IMG_WIDTH*IMG_HEIGHT*2] __EXRAM;

extern uint8_t image_updata_flag;

void Camera_Buffer_To_Lcd_Buffer(void);
void Image_Process(void);

void DMA2_Stream0_Init(void);
void DMA_AtoB_Config(uint32_t DMA_Memory_A_Addr,uint32_t DMA_Memory_B_Addr);

#endif /* __IMAGE_PROCESSING_H */
