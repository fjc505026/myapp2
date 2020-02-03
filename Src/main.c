
#include <math.h>
#include "main.h"
#include "board.h"
#include "interpolation.h"


#define MAXTEMP 28           //threshold of LCD buff
#define MINTEMP 20

#define upperdegree  24      //threshold of INT
#define lowerdegree  15

#define  RM_TEMP 25    // high -> increase color, set the room temperature

float pixdata[64];          // the pixel data read from AMG8833
float pixel_buff[32*32];    // pixel buff after interpolation
uint16_t colorbuff[32*32];
volatile uint16_t LCD_buff[128*128];
uint8_t pixelInts[8];
char    drawInts[64];

const uint16_t color_red=0x1F<<3 ;
//uint16_t color_green=0x07&0x07<<13;
const uint16_t color_blue=0x1F<<8;
const uint16_t color_black=0x0000;

volatile uint16_t therm =0;
volatile bool intReceived = false;
volatile float offset=0;
volatile float ThermValue=0;

const uint16_t camColors[] = {0x0F48,
0x0F40,0x0F40,0x0F40,0x1040,0x1038,0x1038,0x1038,0x1038,0x1030,0x1030,
0x1030,0x1028,0x1028,0x1028,0x1028,0x1020,0x1020,0x1020,0x1018,0x1018,
0x1118,0x1118,0x1110,0x1110,0x1110,0x1108,0x1108,0x1108,0x1100,0x1100,
0x1100,0x1100,0x1100,0x3100,0x3100,0x5100,0x7200,0x7200,0x9200,0xB200,
0xB200,0xD200,0xF200,0xF200,0x1201,0x3201,0x5201,0x5201,0x7201,0x9201,
0x9201,0xB201,0xD201,0xF301,0xF301,0x1302,0x3302,0x5302,0x5302,0x7302,
0x9302,0xB302,0xD302,0xD302,0xF302,0x1303,0x3303,0x3303,0x5303,0x7303,
0x9403,0xB403,0xD403,0xD403,0xF403,0x1404,0x3404,0x5404,0x7404,0x7404,
0x9404,0xB404,0xD404,0xF404,0x1405,0x3405,0x3405,0x5405,0x5405,0x7405,
0x7405,0x7305,0x7305,0x7305,0x7205,0x7205,0x7205,0x7105,0x9105,0x9105,
0x9005,0x9005,0x8F05,0x8F05,0x8F05,0x8E05,0xAE05,0xAE05,0xAD05,0xAD05,
0xAD05,0xAC05,0xAC05,0xAB05,0xCB05,0xCB05,0xCA05,0xCA05,0xCA05,0xC905,
0xC905,0xC805,0xE805,0xE805,0xE705,0xE705,0xE605,0xE605,0xE605,0xE505,
0xE505,0x0406,0x0406,0x0406,0x0306,0x0306,0x0206,0x0206,0x0106,0x2106,
0x2106,0x2006,0x2006,0x2006,0x2006,0x200E,0x200E,0x400E,0x4016,0x4016,
0x401E,0x401E,0x4026,0x4026,0x402E,0x602E,0x6036,0x6036,0x603E,0x603E,
0x603E,0x6046,0x6046,0x604E,0x804E,0x8056,0x8056,0x805E,0x805E,0x8066,
0x8066,0x806E,0xA06E,0xA076,0xA076,0xA07E,0xA07E,0xA086,0xA086,0xA08E,
0xC08E,0xC096,0xC096,0xC09E,0xC09E,0xC0A6,0xC0AE,0xC0AE,0xE0B6,0xE0B6,
0xE0BE,0xE0BE,0xE0C6,0xE0C6,0xE0CE,0xE0CE,0xE0D6,0x00D7,0x00DF,0xE0DE,
0xC0DE,0xA0DE,0x80DE,0x80DE,0x60E6,0x40E6,0x20E6,0x00E6,0xE0E5,0xC0E5,
0xA0E5,0x80E5,0x60E5,0x40E5,0x20E5,0x00E5,0xE0E4,0xC0E4,0xA0E4,0x80E4,
0x60E4,0x40EC,0x20EC,0x00EC,0xE0EB,0xC0EB,0xA0EB,0x80EB,0x60EB,0x40EB,
0x20EB,0x00EB,0xE0EA,0xC0EA,0xA0EA,0x80EA,0x60EA,0x40EA,0x20F2,0x00F2,
0xE0F1,0xC0F1,0xA0F1,0x80F1,0x60F1,0x40F1,0x00F1,0xE0F0,0xC0F0,0xA0F0,
0x80F0,0x60F0,0x40F0,0x20F0,0x00F8,};


void show_buffer(float* buff);

static void Show_image(float tmp_offset);
static void pixdeldata_adjust(float* pixdata,int size, float offset);
static void DrawInts(uint8_t* InfBuff);
static void Handling_AMG8833Int(void);
//static void buffer_convert(uint8_t* buff);
int _write(int file, char *ptr, int len)               //replace the printf func in systemcall.c
{
	int DataIdx;
	for (DataIdx = 0; DataIdx < len; DataIdx++)
	{
		ITM_SendChar(*ptr++);
	}
	return len;
}


int main(void)
 {

	InitBoard();
	AMG8833_Init();
	LCD_Init();
	fillScreen(color_blue);
	memset(pixdata,0, sizeof(pixdata));
	memset(colorbuff,0, sizeof(colorbuff));
	Set_interrupt(upperdegree, lowerdegree);
	Enable_interrupt();

	while (1)
	{
	  if(intReceived)
	  {
		  Handling_AMG8833Int();
	  }
	  AMG8833_Read_pixdels(pixdata);                  //output temperature
	  printf("\n**********Real Temp is:\n");
	  show_buffer(pixdata);
	  ThermValue=0.0625*AMG8833_Read_thermistor();
	  offset=ThermValue-RM_TEMP;         // Calibration  ****increase: color-> blue  decrease: color->red*******
	  Set_interrupt(upperdegree+offset, lowerdegree+offset);        //reset the threshold

	//  pixdeldata_adjust(pixdata,64, offset/2);
	  //show_buffer(pixdata);
	  //printf("\n**********ThermValue is %d***********\n\n",(int)(ThermValue*100));
    //Show_image(offset);
	  fillScreen(0x0000);
	}
}



void show_buffer(float* buff)
{
	float sum=0;
	for(int line=0; line<8;line++)
	{

		for (int col=0;col<8;col++)
		{
		sum+=buff[col+line*8];
		printf("No.(%d.%d)-%d\t",line,col, (int)(buff[(col+line*8)]*100));  //float can not work

		}
	puts("\n");
	}
}

static void Handling_AMG8833Int(void)
{
	getInterrupt(pixelInts);
	printf("\n*********** interrupt received! ***************\n");
	DrawInts(pixelInts);
	Clear_interrupt();
	//HAL_Delay(200);
	intReceived = false;
}


static void DrawInts(uint8_t* InfBuff)
{
	volatile int j=0;
	volatile int shift=0;
	volatile int temp_intpos[8][8]={0};
	//int kk=0;

	for(int i=0;i<64;i++)
	{
		j=i/8;
		shift=(7-i%8);
		if( ((InfBuff[j]) & (0x01<<shift))== (0x01<<shift))  drawInts[(8*((63-i)/8))+((63-i)%8)]='*';
		else  drawInts[(8*((63-i)/8))+((63-i)%8)]='0';
	}


	for(int i=0;i<8;i++)       //line
	{
	   for(int j=0;j<8;j++)   //col
	   {
		  printf("%c   ",drawInts[j+i*8]);

		  if (drawInts[j+i*8]=='*')  temp_intpos[i][j]=1;

		  /*
		  if((drawInts[j+i*8]=='*') &&((j+1+i*8)<64)&&((j+(i+1)*8)<64)&&((j-1+i*8)>0)&&((j+(i-1)*8)>0))
		  {
				if ( ((drawInts[j+1+i*8]=='*')||(drawInts[j-1+i*8]=='*')) && ((drawInts[j+(i+1)*8]=='*')||(drawInts[j+(i-1)*8]=='*')) )
		         {
				   temp_intpos[i][j]=1;
		          kk++;
		         }
		  }*/
	   }
	   puts("\n");
	}

	int ii=0,jj=0;
	for(int i =0;i<128;i++)          //line
	{
		ii=i/16;
		for(int j =0;j<128;j++)       //column
		{
			jj=j/16;
			if(temp_intpos[ii][jj]==1)  LCD_buff[(j+128*i)]=0x00F8;
			else LCD_buff[(j+128*i)]=0x0000;

		}

	}
	char  cur_Temp[15]="Rm temp:";
	char  cur_value[5]={0};
	itoa((int)(ThermValue*100),cur_value, 10);
	strcat(cur_Temp, cur_value);
	showText(cur_Temp);
	setCursorAddr(0, 0, 128, 128);
	writeCmd(0x2c);
	writeDatBytes((uint8_t*)LCD_buff, sizeof(LCD_buff));

}




static void pixdeldata_adjust(float* pixdata,int size, float offset)
{
	float temp=0;
	for(int i=0;i<size;i++)
	{
		temp=pixdata[i]-offset;
		pixdata[i]=temp;
	}
}



static void Show_image(float tmp_offset)
{

    interpolate_image(pixdata,8,8,pixel_buff,32,32);  //buf
    for (int i=0;i<32*32;i++)
   	{
   		int colorTemp;
   		if(pixel_buff[i] >= (MAXTEMP) ) colorTemp = MAXTEMP;
   		else if(pixel_buff[i] <= (MINTEMP)) colorTemp = MINTEMP;
   		else colorTemp = pixel_buff[i];
   		uint8_t colorIndex =ceil((colorTemp-(MINTEMP))*256/(MAXTEMP-MINTEMP));
   		colorbuff[i]=camColors[colorIndex];
   	}

	volatile int col=0;
    volatile int line=0;
    for(int li=0;li<=127;li++)                  //line
    {
   	     line=li/4;
		 for(int cl=0;cl<=127;cl++)            //column
		 {
			col=cl/4;
			LCD_buff[(cl+128*li)]=colorbuff[(col+line*32)];
		 }
    }



	char  cur_Temp[15]="Rm temp:";
	char  cur_value[5]={0};
	itoa((int)(ThermValue*100),cur_value, 10);
	strcat(cur_Temp, cur_value);
	showText(cur_Temp);

	setCursorAddr(0, 0, 128, 128);
	writeCmd(0x2c);
	writeDatBytes((uint8_t*)LCD_buff, sizeof(LCD_buff));

}

/*
static void buffer_convert(uint8_t* buff)
{

	uint8_t  temp_buff[64]={0};
	for(int li=0;li<=7;li++)                  //line
	{
		 for(int cl=0;cl<=7;cl++)            //column
		 {
			 temp_buff[(cl+li*8)]=buff[(7-cl)+(7-li)*8];
		 }
	}

	for(int i=0;i<64;i++)
	{
		buff[i]=temp_buff[i];
	}

}*/


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
