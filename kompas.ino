#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include "MPU9255.h"
#include <avr/pgmspace.h>

#define TFT_CS        10
#define TFT_RST        8 
#define TFT_DC         9
#define rdTOdeg        180/PI

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

MPU9255 compass;//obiekt magnetometru
// wykorzystanie tablic trygonometrycznych
const float sin_tab[] PROGMEM = { 0.0000,
0.0175, 0.0349, 0.0523, 0.0698, 0.0872, 0.1045, 0.1219, 0.1392, 0.1564,
0.1736, 0.1908, 0.2079, 0.2249, 0.2419, 0.2588, 0.2756, 0.2924, 0.3090,
0.3256, 0.3420, 0.3584, 0.3746, 0.3907, 0.4067, 0.4226, 0.4384, 0.4540,
0.4695, 0.4848, 0.5000, 0.5150, 0.5299, 0.5446, 0.5592, 0.5736, 0.5878,
0.6018, 0.6156, 0.6293, 0.6428, 0.6560, 0.6691, 0.6820, 0.6946, 0.7071,
0.7193, 0.7313, 0.7431, 0.7547, 0.7660, 0.7771, 0.7880, 0.7986, 0.8090,
0.8191, 0.8290, 0.8387, 0.8480, 0.8572, 0.8660, 0.8746, 0.8829, 0.8910,
0.8988, 0.9063, 0.9135, 0.9205, 0.9272, 0.9336, 0.9397, 0.9455, 0.9510,
0.9563, 0.9613, 0.9659, 0.9703, 0.9744, 0.9781, 0.9816, 0.9848, 0.9877,
0.9903, 0.9925, 0.9945, 0.9962, 0.9976, 0.9986, 0.9994, 0.9998, 1.0000,
0.9998, 0.9994, 0.9986, 0.9976, 0.9962, 0.9945, 0.9926, 0.9903, 0.9877,
0.9848, 0.9816, 0.9782, 0.9744, 0.9703, 0.9659, 0.9613, 0.9563, 0.9511,
0.9455, 0.9397, 0.9336, 0.9272, 0.9205, 0.9136, 0.9063, 0.8988, 0.8910,
0.8830, 0.8746, 0.8661, 0.8572, 0.8481, 0.8387, 0.8291, 0.8192, 0.8091,
0.7987, 0.7881, 0.7772, 0.7661, 0.7548, 0.7432, 0.7314, 0.7194, 0.7072,
0.6947, 0.6820, 0.6692, 0.6561, 0.6428, 0.6294, 0.6157, 0.6019, 0.5878,
0.5736, 0.5593, 0.5447, 0.5300, 0.5151, 0.5001, 0.4849, 0.4695, 0.4541,
0.4384, 0.4227, 0.4068, 0.3908, 0.3747, 0.3584, 0.3421, 0.3256, 0.3091,
0.2925, 0.2757, 0.2589, 0.2420, 0.2250, 0.2080, 0.1909, 0.1737, 0.1565,
0.1393, 0.1220, 0.1046, 0.0872, 0.0698, 0.0524, 0.0350, 0.0175, 0.0001,
-0.0174, -0.0348, -0.0522, -0.0697, -0.0871, -0.1044, -0.1218, -0.1391, -0.1563,
-0.1736, -0.1907, -0.2078, -0.2249, -0.2418, -0.2587, -0.2755, -0.2923, -0.3089,
-0.3255, -0.3419, -0.3583, -0.3745, -0.3906, -0.4066, -0.4225, -0.4383, -0.4539,
-0.4694, -0.4847, -0.4999, -0.5149, -0.5298, -0.5445, -0.5591, -0.5735, -0.5877,
-0.6017, -0.6156, -0.6292, -0.6427, -0.6560, -0.6690, -0.6819, -0.6946, -0.7070,
-0.7193, -0.7313, -0.7431, -0.7546, -0.7660, -0.7771, -0.7879, -0.7986, -0.8089,
-0.8191, -0.8290, -0.8386, -0.8480, -0.8571, -0.8660, -0.8746, -0.8829, -0.8909,
-0.8987, -0.9063, -0.9135, -0.9205, -0.9271, -0.9335, -0.9396, -0.9455, -0.9510,
-0.9563, -0.9612, -0.9659, -0.9703, -0.9743, -0.9781, -0.9816, -0.9848, -0.9877,
-0.9902, -0.9925, -0.9945, -0.9962, -0.9976, -0.9986, -0.9994, -0.9998, -1.0000,
-0.9999, -0.9994, -0.9986, -0.9976, -0.9962, -0.9945, -0.9926, -0.9903, -0.9877,
-0.9848, -0.9817, -0.9782, -0.9744, -0.9703, -0.9660, -0.9613, -0.9563, -0.9511,
-0.9456, -0.9397, -0.9336, -0.9272, -0.9206, -0.9136, -0.9064, -0.8989, -0.8911,
-0.8830, -0.8747, -0.8661, -0.8572, -0.8481, -0.8388, -0.8291, -0.8192, -0.8091,
-0.7987, -0.7881, -0.7772, -0.7661, -0.7548, -0.7433, -0.7315, -0.7195, -0.7072,
-0.6948, -0.6821, -0.6693, -0.6562, -0.6429, -0.6294, -0.6158, -0.6019, -0.5879,
-0.5737, -0.5593, -0.5448, -0.5301, -0.5152, -0.5001, -0.4850, -0.4696, -0.4541,
-0.4385, -0.4228, -0.4069, -0.3909, -0.3748, -0.3585, -0.3422, -0.3257, -0.3092,
-0.2925, -0.2758, -0.2590, -0.2421, -0.2251, -0.2081, -0.1910, -0.1738, -0.1566,
-0.1394, -0.1220, -0.1047, -0.0873, -0.0699, -0.0525, -0.0351, -0.0176 };

const float cos_tab[] PROGMEM = { 1.0000,
0.9998, 0.9994, 0.9986, 0.9976, 0.9962, 0.9945, 0.9925, 0.9903, 0.9877,
0.9848, 0.9816, 0.9781, 0.9744, 0.9703, 0.9659, 0.9613, 0.9563, 0.9511,
0.9455, 0.9397, 0.9336, 0.9272, 0.9205, 0.9136, 0.9063, 0.8988, 0.8910,
0.8830, 0.8746, 0.8660, 0.8572, 0.8481, 0.8387, 0.8290, 0.8192, 0.8090,
0.7986, 0.7880, 0.7772, 0.7661, 0.7547, 0.7432, 0.7314, 0.7194, 0.7071,
0.6947, 0.6820, 0.6691, 0.6561, 0.6428, 0.6293, 0.6157, 0.6018, 0.5878,
0.5736, 0.5592, 0.5447, 0.5299, 0.5151, 0.5000, 0.4848, 0.4695, 0.4540,
0.4384, 0.4226, 0.4068, 0.3908, 0.3746, 0.3584, 0.3421, 0.3256, 0.3091,
0.2924, 0.2757, 0.2589, 0.2420, 0.2250, 0.2080, 0.1908, 0.1737, 0.1565,
0.1392, 0.1219, 0.1046, 0.0872, 0.0698, 0.0524, 0.0349, 0.0175, 0.0000,
-0.0174, -0.0349, -0.0523, -0.0697, -0.0871, -0.1045, -0.1218, -0.1391, -0.1564,
-0.1736, -0.1908, -0.2079, -0.2249, -0.2419, -0.2588, -0.2756, -0.2923, -0.3090,
-0.3255, -0.3420, -0.3583, -0.3746, -0.3907, -0.4067, -0.4226, -0.4383, -0.4539,
-0.4694, -0.4848, -0.4999, -0.5150, -0.5299, -0.5446, -0.5591, -0.5735, -0.5877,
-0.6018, -0.6156, -0.6293, -0.6427, -0.6560, -0.6691, -0.6819, -0.6946, -0.7071,
-0.7193, -0.7313, -0.7431, -0.7547, -0.7660, -0.7771, -0.7880, -0.7986, -0.8090,
-0.8191, -0.8290, -0.8386, -0.8480, -0.8571, -0.8660, -0.8746, -0.8829, -0.8910,
-0.8988, -0.9063, -0.9135, -0.9205, -0.9272, -0.9336, -0.9397, -0.9455, -0.9510,
-0.9563, -0.9612, -0.9659, -0.9703, -0.9744, -0.9781, -0.9816, -0.9848, -0.9877,
-0.9903, -0.9925, -0.9945, -0.9962, -0.9976, -0.9986, -0.9994, -0.9998, -1.0000,
-0.9998, -0.9994, -0.9986, -0.9976, -0.9962, -0.9945, -0.9926, -0.9903, -0.9877,
-0.9848, -0.9816, -0.9782, -0.9744, -0.9703, -0.9660, -0.9613, -0.9563, -0.9511,
-0.9456, -0.9397, -0.9336, -0.9272, -0.9205, -0.9136, -0.9064, -0.8988, -0.8911,
-0.8830, -0.8747, -0.8661, -0.8572, -0.8481, -0.8387, -0.8291, -0.8192, -0.8091,
-0.7987, -0.7881, -0.7772, -0.7661, -0.7548, -0.7432, -0.7314, -0.7194, -0.7072,
-0.6947, -0.6821, -0.6692, -0.6561, -0.6429, -0.6294, -0.6158, -0.6019, -0.5879,
-0.5737, -0.5593, -0.5447, -0.5300, -0.5151, -0.5001, -0.4849, -0.4696, -0.4541,
-0.4385, -0.4227, -0.4069, -0.3908, -0.3747, -0.3585, -0.3421, -0.3257, -0.3091,
-0.2925, -0.2758, -0.2589, -0.2420, -0.2251, -0.2080, -0.1909, -0.1738, -0.1566,
-0.1393, -0.1220, -0.1047, -0.0873, -0.0699, -0.0525, -0.0350, -0.0176, -0.0001,
0.0173, 0.0348, 0.0522, 0.0696, 0.0870, 0.1044, 0.1217, 0.1390, 0.1563,
0.1735, 0.1907, 0.2078, 0.2248, 0.2418, 0.2587, 0.2755, 0.2922, 0.3089,
0.3254, 0.3419, 0.3582, 0.3745, 0.3906, 0.4066, 0.4225, 0.4382, 0.4539,
0.4693, 0.4847, 0.4999, 0.5149, 0.5298, 0.5445, 0.5591, 0.5734, 0.5877,
0.6017, 0.6155, 0.6292, 0.6427, 0.6559, 0.6690, 0.6819, 0.6945, 0.7070,
0.7192, 0.7312, 0.7430, 0.7546, 0.7659, 0.7770, 0.7879, 0.7985, 0.8089,
0.8191, 0.8289, 0.8386, 0.8480, 0.8571, 0.8659, 0.8745, 0.8829, 0.8909,
0.8987, 0.9062, 0.9135, 0.9204, 0.9271, 0.9335, 0.9396, 0.9455, 0.9510,
0.9563, 0.9612, 0.9659, 0.9703, 0.9743, 0.9781, 0.9816, 0.9848, 0.9877,
0.9902, 0.9925, 0.9945, 0.9962, 0.9976, 0.9986, 0.9994, 0.9998 }; 

uint16_t alfa = 0;
float heading = 0;
float x_min = 0;
float x_max = 0;
float y_min = 0;
float y_max = 0;
float Xsf = 1;//soft filter
float Ysf = 1;
float Xoff=0;//offset
float Yoff=0;
float x_value=0;//value after compensation
float y_value=0;
float previousHeading=0;
uint8_t counter = 0;
void drawCompass(int pozx, int pozy, int r);

void setup(void) 
{ 
    Serial.begin(115200);
    tft.initR(INITR_BLACKTAB);   // inicjalizacja chipu graficznego ST7735S, czarne tlo       
    tft.setTextWrap(false); // zawijanie tekstu
    tft.fillScreen(ST77XX_BLACK);
    tft.setRotation(3);
  //test polaczenia
    if(compass.init())
    {
        tft.setCursor(5,50);
        tft.setTextColor(ST77XX_RED);
        tft.setTextSize(1);
        tft.print("BAD MPU, check wiring!");
        delay(3000);
        tft.print("test init():");
        tft.println(compass.init());
        tft.print("testMag");
        tft.println(compass.testMag());
        tft.print("testIMU");
        tft.println(compass.testIMU());
        delay(3000);
    }
    else 
    {
        tft.println("good MPU, go!"); delay(500);
        tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
        tft.setCursor(10,10);
        tft.print("1. PWR_MGMT_REG info: ");
        tft.println(compass.read(104, 0x6B));
        tft.setCursor(10,30);
        tft.print("2.USR_STRL_REG info: ");
        tft.println(compass.read(104, 0x6A));
        tft.setCursor(10,50);
        tft.print("3.GYRO_CONFIG_REG info: ");
        tft.println(compass.read(104, 27));
        tft.setCursor(10,70);
        tft.print("4.INT_PIN_CFG_REG info: ");
        tft.println(compass.read(104, 55));
        tft.setCursor(10,90);
        tft.print("5.MAG_CNTL_REG info: ");
        tft.print(compass.read(12, 10));
        delay(2000);
        tft.fillScreen(ST77XX_BLACK);
    }
//calibration   
  for(int i=0; i<100;i++)
  {
    compass.read_mag();
    if(compass.mxs>x_max) x_max=compass.mxs;
    if(compass.mxs<x_min) x_min=compass.mxs;
    if(compass.mys>y_max) y_max=compass.mys;
    if(compass.mys<y_min) y_min=compass.mys;
    tft.setCursor(5,10);
    tft.print("GETTING MAX/MIN...");
    tft.setCursor(5,30);
    tft.print(x_max);tft.print(" x_max | x_min ");tft.print(x_min);
    tft.setCursor(5,50);
    tft.print(y_max);tft.print(" y_max | y_min ");tft.print(y_min);
    delay(50);
    if(i%20==0)
    {
      counter++;
      tft.setCursor(5, 70);
      tft.print("timer: ");tft.print(counter);tft.print(" s");
    }
  }
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(5,10);
    tft.print(" END OF SCANNING, VALUES: ");
    tft.setCursor(5,30);
    tft.print(x_max);tft.print(" x_max | x_min ");tft.print(x_min);
    tft.setCursor(5,50);
    tft.print(y_max);tft.print(" y_max | y_min ");tft.print(y_min);
    delay(3000);
    tft.print(" Zaczynamy pomiar...");
    tft.fillScreen(ST77XX_BLACK);

    Xsf = 1;
    float Xsf_temp = (y_max-y_min)/(x_max-x_min);
    Ysf = 1;
    float Ysf_temp = (x_max-x_min)/(y_max-y_min);
    if(Xsf < Xsf_temp)
       Xsf = Xsf_temp;
    if(Ysf < Ysf_temp)
      Ysf = Ysf_temp;

    Xoff = ((x_max - x_min) /2 - x_max)*Xsf;
    Yoff = ((y_max - y_min) /2 - y_max)*Ysf;

}
void loop(void) 
{
tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
compass.read_mag();

x_value = Xsf * compass.mxs + Xoff;
y_value = Ysf * compass.mys + Yoff;
heading = (atan2f(-y_value, x_value))+PI+0.08;

if(heading < 0)
   heading = 0;
if(heading > TWO_PI)
   heading = TWO_PI;
if(heading < (previousHeading + 0.03) && heading > (previousHeading - 0.03))
  {
    heading = previousHeading;
  }
  
  previousHeading = heading;
alfa=(uint16_t)((heading)*RAD_TO_DEG);
if(alfa == 360)
   alfa = 0;
  tft.setCursor(5,100);
  tft.print("HD:");
  tft.print(heading);
  tft.setCursor(5,110);
  tft.print("ALFA:");
  tft.print(alfa);
  tft.setCursor(5,120);
  tft.print("TMG:");
  tft.print(compass.total_mag_vector(compass.mxs, compass.mys, compass.mzs));
  //Serial.print("Direction:");
  Serial.println(alfa);
  // We need delay ~28ms for allow data rate 30Hz (~33ms)
  delay(30);

  drawCompass( 80, 64, 20);//drawing compass
}

void drawCompass(int pozx, int pozy, int r)
{  
    tft.fillCircle( 80, 64, r+2, ST77XX_BLACK);
    tft.drawLine( pozx, pozy, (pozx + r*pgm_read_float_near(cos_tab+alfa)), (pozy + r*pgm_read_float_near(sin_tab+alfa)), ST77XX_RED);
    tft.drawLine( pozx, pozy, (pozx - r*pgm_read_float_near(cos_tab+alfa)), (pozy - r*pgm_read_float_near(sin_tab+alfa)), ST77XX_WHITE);
    tft.drawLine( pozx, pozy, (pozx - r*pgm_read_float_near(sin_tab+alfa)), (pozy + r*pgm_read_float_near(cos_tab+alfa)), ST77XX_WHITE);
    tft.drawLine( pozx, pozy, (pozx + r*pgm_read_float_near(sin_tab+alfa)), (pozy - r*pgm_read_float_near(cos_tab+alfa)), ST77XX_WHITE);
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setCursor(2,2);
    tft.print(compass.mxs);tft.print(" mT/xa | cmp: ");tft.print(x_value);
    tft.setCursor(2,12);
    tft.print(compass.mys);tft.print(" mT/ya | cmp: ");tft.print(y_value);
    tft.setCursor(2,22);
    tft.print(pgm_read_float_near(sin_tab+alfa));
    tft.setCursor(2,32);
    tft.print(pgm_read_float_near(cos_tab+alfa));
    tft.drawChar(78,102, 'E', ST77XX_WHITE, ST77XX_BLACK, 1),
    tft.drawChar(38,62, 'S', ST77XX_WHITE, ST77XX_BLACK, 1),
    tft.drawChar(78,21, 'W', ST77XX_WHITE, ST77XX_BLACK, 1),
    tft.drawChar(118,62, 'N', ST77XX_WHITE, ST77XX_BLACK, 1),
    
    tft.drawCircle( 80, 64, 35, ST77XX_WHITE);
    tft.fillTriangle(140, 59, 140, 69, 150, 64, ST77XX_RED);
}
