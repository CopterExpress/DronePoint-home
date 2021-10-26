// залить все
void fillAll(CRGB newcolor) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = newcolor;
  }
}

// функция получения цвета пикселя по его номеру
uint32_t getPixColor(int thisPixel) {
  return (((uint32_t)leds[thisPixel].r << 16) | ((long)leds[thisPixel].g << 8 ) | (long)leds[thisPixel].b);
}


//процесс приема команд на ленту
static void vLedTask(void *pvParameters) { 
   vTaskDelay(550);
   unsigned long timerspd=millis();
   unsigned long ledact=millis();
   unsigned long smf=millis();
   uint8_t bc1=1;
   for (;;) {  
     
              //выбор и скорость движения эффекта
              if  ((millis()-timerspd)>25)
               {   if      (bc1==0)  { FastLED.clear();  } 
                   else if (bc1==1)  { fillAll(CRGB::White);}
                   else if (bc1==2)  { fillAll(CRGB::Red);}
                   else if (bc1==3)  { fillAll(CRGB::Yellow);}
                   else if (bc1==4)  { fillAll(CRGB::Aqua);}
                   else if (bc1==5)  { fillAll(CRGB::Green);}
                   else if (bc1==6)  { fillAll(CRGB::Blue);}
                   else if (bc1==7)  { lighter();        }
                   else if (bc1==8)  { lightBugs();      }
                   else if (bc1==9)  { colors();         }
                   else if (bc1==10) { rainbow();        }
                   else if (bc1==11) { sparkles();       }
                   else if (bc1==12) { fire();           }
                   timerspd=millis();
               }

              //шлем в ленту
              if  ((millis()-ledact)>10) 
               {   FastLED.setBrightness(180);
                   FastLED.show();
                   ledact=millis(); 
               }

              //смена
              if  ((millis()-smf)>5000) 
               {   bc1=bc1+1;
                   if (bc1>12) { bc1=0; }
                   smf=millis(); 
               }


            }
}






           
