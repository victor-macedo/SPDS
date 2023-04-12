#define AIC3204_I2C_ADDR 0x18
#include "usbstk5515.h"
#include "usbstk5515_gpio.h"
#include "usbstk5515_i2c.h"
#include "stdio.h"
#include "usbstk5515.h"
extern Int16 AIC3204_rset( Uint16 regnum, Uint16 regval);
#define Rcv 0x08
#define Xmit 0x20


/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  main( )                                                                 *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void main( void )


{
    Int16 r = 0,s, rmax, rmin,delta, delta0, deltamin, deltamax,gain = 32767,SDD,t,k = 27853,y2,inter,f,phase, alfa = 31523,//fc = 100 -> alfa = 0.962
    look_sen[33]= {0,3212,6393 ,9512 ,12539,15446,18204,20787,23170,25329,27245,28898,30273,31356,32137,32609,
    32767,32609,32137,31356,30273,28898,27245,25329,23170,20787,18204,15446,12539, 9512,6393,3212,0},
    NCO, e,ec, xband[3] = {0,0,0}, yband[3] = {0,0,0},Kf,ef,a1,fd,dec = 0,yd,senkf,senkf2,Kf2,I_Kf;
    // Definicao do Delta = 4,096 * Fo
    delta0 = 16384;                             //Variavel Delta a ser incrementada
    deltamin = 8192;                            //4.096*2000
    deltamax = 24576;                           //4.096*6000

    /* Initialize BSL */
    USBSTK5515_init( );

    /* Configure AIC3204 */

       AIC3204_rset( 0, 0 );          // Select page 0
       AIC3204_rset( 1, 1 );          // Reset codec
       AIC3204_rset( 0, 1 );          // Select page 1
       AIC3204_rset( 1, 8 );          // Disable crude AVDD generation from DVDD
       AIC3204_rset( 2, 1 );          // Enable Analog Blocks, use LDO power
       AIC3204_rset( 0, 0 );          // Select page 0
       /* PLL and Clocks config and Power Up  */
       AIC3204_rset( 27, 0x0d );      // BCLK and WCLK is set as o/p to AIC3204(Master)
       AIC3204_rset( 28, 0x00 );      // Data ofset = 0
       AIC3204_rset( 4, 3 );          // PLL setting: PLLCLK <- MCLK, CODEC_CLKIN <-PLL CLK
       AIC3204_rset( 6, 7 );          // PLL setting: J=7
       AIC3204_rset( 7, 0x06 );       // PLL setting: HI_BYTE(D=1680)
       AIC3204_rset( 8, 0x90 );       // PLL setting: LO_BYTE(D=1680)
       AIC3204_rset( 30, 0x88 );      // For 32 bit clocks per frame in Master mode ONLY
                                      // BCLK=DAC_CLK/N =(12288000/8) = 1.536MHz = 32*fs
       AIC3204_rset( 5, 0x91 );       // PLL setting: Power up PLL, P=1 and R=1
       AIC3204_rset( 13, 0 );         // Hi_Byte(DOSR) for DOSR = 128 decimal or 0x0080 DAC oversamppling
       AIC3204_rset( 14, 0x80 );      // Lo_Byte(DOSR) for DOSR = 128 decimal or 0x0080
       AIC3204_rset( 20, 0x80 );      // AOSR for AOSR = 128 decimal or 0x0080 for decimation filters 1 to 6
       AIC3204_rset( 11, 0x86 );      // Power up NDAC and set NDAC value to 6 (fs=16kHz)
       AIC3204_rset( 12, 0x87 );      // Power up MDAC and set MDAC value to 7
       AIC3204_rset( 18, 0x87 );      // Power up NADC and set NADC value to 7
       AIC3204_rset( 19, 0x86 );      // Power up MADC and set MADC value to 6 (fs=16kHz)
       /* DAC ROUTING and Power Up */
       AIC3204_rset(  0, 0x01 );      // Select page 1
       AIC3204_rset( 12, 0x08 );      // LDAC AFIR routed to HPL
       AIC3204_rset( 13, 0x08 );      // RDAC AFIR routed to HPR
       AIC3204_rset(  0, 0x00 );      // Select page 0
       AIC3204_rset( 64, 0x02 );      // Left vol=right vol
       AIC3204_rset( 65, 0x00 );      // Left DAC gain to 0dB VOL; Right tracks Left
       AIC3204_rset( 63, 0xd4 );      // Power up left,right data paths and set channel
       AIC3204_rset(  0, 0x01 );      // Select page 1
       AIC3204_rset( 16, 0x00 );      // Unmute HPL , 0dB gain
       AIC3204_rset( 17, 0x00 );      // Unmute HPR , 0dB gain
       AIC3204_rset(  9, 0x30 );      // Power up HPL,HPR
       AIC3204_rset(  0, 0x00 );      // Select page 0
       USBSTK5515_wait( 500 );        // Wait

       /* ADC ROUTING and Power Up */
       AIC3204_rset( 0, 1 );          // Select page 1
       AIC3204_rset( 0x34, 0x30 );    // STEREO 1 Jack
                                   // IN2_L to LADC_P through 40 kohm
       AIC3204_rset( 0x37, 0x30 );    // IN2_R to RADC_P through 40 kohmm
       AIC3204_rset( 0x36, 3 );       // CM_1 (common mode) to LADC_M through 40 kohm
       AIC3204_rset( 0x39, 0xc0 );    // CM_1 (common mode) to RADC_M through 40 kohm
       AIC3204_rset( 0x3b, 0x18 );       // MIC_PGA_L unmute
       AIC3204_rset( 0x3c, 0x18 );       // MIC_PGA_R unmute
       AIC3204_rset( 0, 0 );          // Select page 0
       AIC3204_rset( 0x51, 0xc0 );    // Powerup Left and Right ADC
       AIC3204_rset( 0x52, 0 );       // Unmute Left and Right ADC

       AIC3204_rset( 0, 0 );
       USBSTK5515_wait( 200 );        // Wait
       /* I2S settings */
       I2S0_SRGR = 0x0;
       I2S0_CR = 0x8010;    // 16-bit word, slave, enable I2C
       I2S0_ICMR = 0x3f;    // Enable interrupts



       Int16 DataInLeft, DataInRight, DataOutLeft, DataOutRight;
       while(1) {
                    /* Read Digital audio */
                    while((Rcv & I2S0_IR) == 0);  // Wait for interrupt pending flag
                    DataInRight = I2S0_W0_MSW_R;  // 16 bit right channel received audio data
                    DataInLeft = I2S0_W1_MSW_R;  // 16 bit left channel received audio data
                    /* Write Digital audio */
                    while((Xmit & I2S0_IR) == 0);  // Wait for interrupt pending flag
                    I2S0_W0_MSW_W = DataOutRight;  // 16 bit right channel transmit audio data
                    I2S0_W1_MSW_W = DataOutLeft;  // 16 bit left channel transmit audio data

//--------------------------------------------------------------------------------------------------------------------
 // Your program here!!!
//--------------------------------------------------------------------------------------------------------------------
//
//            NCO
//
//-------------------------------------------------------------------------------------
delta = delta0 + ((((long)k * e)<<1)>>16);
//Soma dos sinais
r = r + delta;
//rmin = rmin + deltamin;  Nao necessario no projeto?
//rmax = rmax +deltamax;    Nao necessario no projeto?

s = r & 31744;           //0b1111110000000000
s =s >> 10;              //Utiliza os 5 MSB + bit de sinal e converte para Q15


t = look_sen[s];        //Converte o valor da rampa em uma senoide
y2 = look_sen[s+1];
if(r<0){                 // if para inverter o valor do sen
    t = -t;
    y2= -y2;
}
f = (r & 1023) << 5;           //0b0000 0011 1111 1111 Utiliza os outros bits da rampa para interpolacao
inter = t+((((y2-t)*(long)f)<<1)>>16); //Interpolacao do seno
NCO = (((long)gain * inter)<<1) >> 16;//Calcula a nova saida do NCO

// Phase detector
//a simple multiplier which multiplies the sample of the input signal by the sample generated on the previously developed NCO
//---------------------------------------------------------
phase = ((((long)DataInLeft * NCO)<< 1)>>16);
//--------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------
//
// Loop Filter
//
//---------------------------------------------------------
e = ((((long)alfa*e + (32767-alfa)*(long)phase))<<1)>>16; //Resultado em Q15
ec = ((((long)e*24932) <<1 ) << 1 ) >> 16;   //Valor de e corrigido e*1.76 (Q15)
//------------------------------------------------------------
//
// Decimation
//
//---------------------------------------------------------
//Filtro passa baixa IIR Fc=250, alpha = 0,91
fd = ((((long)29839*fd + (32767-29839)*(long)ec))<<1)>>16; //Resultado em Q15
//fd = ((((long)29839*fd + (32767-29839)*(long)DataInLeft))<<1)>>16; //Resultado em Q15
dec++;
//Salva o valor de fd a cada 32 amostras para realizar a decimacao
if (dec == 32)
    {
        dec = 0 ;
        yd = fd;
    }

//yd=16384>>1;
//-------------------------------------------------------
//      Frequency Control
//-------------------------------------------------------
senkf = (((16384*(long)yd)) << 1 ) >> 16; //ef/2 resultado final em Q15
senkf2 = (senkf & 31744) >> 10;           //0b1111 1100 0000 000  e Utiliza os 5 MSB + bit de sinal e converte para Q10


Kf = (-look_sen[senkf2] >> 1);   //Converte o valor do sen de Q15 para Q14
Kf2 = (-look_sen[senkf2+1] >> 1);
if(senkf<0){
    Kf = -Kf;
}
I_Kf = (senkf & 1023) << 5;           //0b0000 0011 1111 1111 Utiliza os outros bits da rampa para interpolacao
inter = Kf+((((Kf2-Kf)*(long)I_Kf)<<1)>>16); //Interpolacao do seno
//Kf = (((-25736*(long)yd)) << 1 ) >> 16; //pi/2*ef resultado final em Q14

//---------------------------------------------------------
//      Band Pass Filter
//------------------------------------------------------------
xband[0] = DataInLeft;
a1 = ((2*(long)Kf*(32767-2555)) << 1 ) >> 16; //a1 = 2*Kf*(1-Kb) Valor varia entre -1.3 e 1.3  Q14
yband[0] = (((((long)a1*yband[1])<< 1) -(long)27986*yband[2]+(long)2391*xband[0]-(long)2391*xband[2]) << 1) >> 16;


yband[2]=yband[1];
yband[1]=yband[0];

xband[2]=xband[1];
xband[1]=xband[0];
//
// Saida
//--------------------------------------------------------------------------------------------------------------------
DataOutLeft = yband[0] ;       // loop left channel samples
DataOutRight = NCO;         // loop right channel samples
  }

 } // main()
