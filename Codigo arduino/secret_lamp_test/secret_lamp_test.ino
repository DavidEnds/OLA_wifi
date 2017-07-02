// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels


#define PIN_1            11
#define PIN_2            10
#define PIN_3            9
#define PIN_4            6
#define PIN_5            5
#define TX_BT_PIN        4 // van cruzados
#define RX_BT_PIN        2 // van cruzados

// How many NeoPixels are attached to the Arduino
#define NUMPIXELS      16
#define FACES          5
#define DELAY_FACE    50
#define DELAY_FACE_1  75 
#define DELAY_FACE_2  100

#define BAUD_RATE_COMM 9600 


enum STATE{STAND_BY,SECRET,RAND_LED,SWEEP};
STATE state; 
//////////// CODIFICACION DE LOS MODOS DE FUNCIONAMIENTO ///
/*
STATE:   <#><id_state>
    STAND-BY------> #0
    SECRET -------> #1
    RAND_LED------> #2
    SWEEP---------> #3
    
  */
// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.

SoftwareSerial BT(RX_BT_PIN,TX_BT_PIN);

Adafruit_NeoPixel cubo[FACES]={
Adafruit_NeoPixel(NUMPIXELS, PIN_1, NEO_GRB + NEO_KHZ800),
Adafruit_NeoPixel(NUMPIXELS, PIN_2, NEO_GRB + NEO_KHZ800),
Adafruit_NeoPixel(NUMPIXELS, PIN_3, NEO_GRB + NEO_KHZ800),
Adafruit_NeoPixel(NUMPIXELS, PIN_4, NEO_GRB + NEO_KHZ800),
Adafruit_NeoPixel(NUMPIXELS, PIN_5, NEO_GRB + NEO_KHZ800)
};

           int face = 0;
           int led = 0;
           int red = 0;
           int green = 0;
           int blue = 0;

int delayval = 500; // delay for half a second
int rand_face,rand_led,rand_red,rand_green,rand_blue;

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
    // End of trinket special code
  for( int i=0; i<FACES;i++)
    {
      cubo[i].begin();
    }
    
       state = STAND_BY;
       Serial.begin(BAUD_RATE_COMM);
       BT.begin(BAUD_RATE_COMM);
       /* 
       // CONFIGURACION BT 
       Serial.print("AT");
       delay(1000);
       Serial.print("AT+PIN");
       Serial.print("1337");
       delay(1000);
       */
       /*
       Serial.print("AT+NAME");
       Serial.print("LAMPARA_SECRETO");
       delay(1000);
       */
     //Serial.println("Communication enabled:");
     //Serial.println("Establezca el modo de funcionamiento: " );
     Reset_cube();
}// fin void setup

void loop() {
  if ( BT.available())
     if ( BT.find("#"))
       {
         int opc=BT.parseInt();
         //Serial.println(opc);
        
         switch (opc)
           {
               case 0:
                 {
                     state = STAND_BY;
                     Reset_cube();
                     BT.write("Stand-by");
                     debug();
                 }
                 break;
               case 1:
                 {
                     state = SECRET;
                     Reset_cube();
                     BT.write("Secret");
                     debug();
                 }
                 break;
               case 2:
                 {
                     state = RAND_LED;
                     Reset_cube();
                     BT.write("Random_led");
                     debug();
                 }
                 break;
               case 3:
               {
                     state = SWEEP;
                     Reset_cube();
                     BT.write("Sweep");
                     debug();
                 }
                 break;
           
             }
         }
     if ( state == 0)
     {
       fade_up();
       //Serial.println("Awaiting device to connect");
       
      // delay(DELAY_FACE);
      }
     if ( state == 1)
     {
          
           if (BT.available())
           if ( BT.find("*"))
           //if ( BT.read()=='*')
           {
               face  = BT.parseInt();
               led   = BT.parseInt();
               red   = BT.parseInt();
               green = BT.parseInt();
               blue  = BT.parseInt();
               
               //face   = constrain(face  ,0,   4);
              // led    = constrain(led   ,0,   8);
               red    = constrain(red   ,0, 255);
               green  = constrain(green ,0, 255);
               blue   = constrain(blue  ,0, 255);
               
               cubo[face].setPixelColor(led,cubo[face].Color(red,green,blue));
               cubo[face].show(); 
               
               Serial.print("Mensaje cubo led: *");
               Serial.print(face);
               Serial.print(",");
               Serial.print(led);
               Serial.print(",");
               Serial.print(red);
               Serial.print(",");
               Serial.print(green);
               Serial.print(",");
               Serial.println(blue); 
              // delay(DELAY_FACE);     
            
           }
      
     }
     if ( state == 2 ) 
       {  
          // Serial.println("Modo random");
           Random_led_on();
       }  
     if ( state == 3 ) 
       {
           //Serial.println("Modo barrido");
           Cube_random_sweep();
       }
       
       /* para debug por serial */
       /*
     if (Serial.available())
       {  
           String S = GetLine();
           BT.print(S);
           Serial.println("---> " + S);
       }
  */
    
}
void debug()
{

  Serial.print(" Estado : " );
  Serial.println(state);
}
void Reset_cube()
{
  for(int i=0; i<FACES;i++)
        {
             for (int j=0;j<NUMPIXELS;j++)
             cubo[i].setPixelColor(j,cubo[i].Color(0,0,0));
             cubo[i].show();
         }


}
String GetLine()
   {   String S = "" ;
       if (Serial.available())
          {    char c = Serial.read(); ;
                while ( c != '\n')            //Hasta que el caracter sea intro
                  {     S = S + c ;
                        delay(25) ;
                        c = Serial.read();
                  }
                return( S + '\n') ;
          }
   }
void Random_led_on()
{
  randomSeed(millis());
  rand_face = random(5);
  rand_led  = random(16);
  rand_red  = random(255);
  rand_green  = random(255);
  rand_blue  = random(255);
  
  cubo[rand_face].setPixelColor(rand_led,cubo[rand_face].Color(rand_red,rand_green,rand_blue));
  cubo[rand_face].show();
  delay(DELAY_FACE);

}
void Cube_random_sweep()
{
  
  
  for ( int j=0;j<FACES;j++)
    {
      randomSeed(millis());
      rand_red    = random(255);
      rand_green  = random(255);
      rand_blue   = random(255);
      for ( int i=0; i<NUMPIXELS;i++)
      {
        cubo[j].setPixelColor(i,cubo[j].Color(rand_red,rand_green,rand_blue));
        cubo[j].show();
        delay(DELAY_FACE);
      }
    }
    

}
void Cube_rgb_sweep()
{
  for ( int j=0;j<FACES;j++)
    {
      for ( int i=0; i<NUMPIXELS;i++)
      {
        cubo[j].setPixelColor(i,cubo[j].Color(255,0,0));
        cubo[j].show();
        delay(DELAY_FACE);
      }
    }
    for ( int j=0;j<FACES;j++)
    {
      for ( int i=0; i<NUMPIXELS;i++)
      {
        cubo[j].setPixelColor(i,cubo[j].Color(0,255,0));
        cubo[j].show();
        delay(DELAY_FACE);
      }
    }
    for ( int j=0;j<FACES;j++)
    {
      for ( int i=0; i<NUMPIXELS;i++)
      {
        cubo[j].setPixelColor(i,cubo[j].Color(0,0,255));
        cubo[j].show();
        delay(DELAY_FACE);
      }
    }

}
void fade_up()
{
    for(int i = 0 ; i<FACES-1;i++)
      for( int j = 0 ;j<4;j++)
        cubo[i].setPixelColor(j,85,0,85);
    for(int i = 0; i< FACES-1;i++)
        cubo[i].show();
    delay(DELAY_FACE_2);
    
    for(int i = 0 ; i<FACES-1;i++)
      for( int j = 3 ;j<8;j++)
        cubo[i].setPixelColor(j,170,0,170);
    for(int i = 0; i< FACES-1;i++)
        cubo[i].show();
    delay(DELAY_FACE_2);
    
    for(int i = 0 ; i<FACES-1;i++)
      for( int j = 6 ;j<12;j++)
        cubo[i].setPixelColor(j,255,0,255);
    for(int i = 0; i< FACES-1;i++)
        cubo[i].show();
    delay(100);
    
    for ( int j = 0; j<NUMPIXELS;j++)
      cubo[4].setPixelColor(j,100,0,255);
    cubo[4].show();
    delay(200);
    Reset_cube();
    delay(100);

}
