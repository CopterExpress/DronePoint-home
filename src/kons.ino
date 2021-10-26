//проверка состояния концевиков Z паралленый таск
static void vKZcheckTask(void *pvParameters) {
   vTaskDelay(300);   
   for (;;) {  if      (digitalRead(pKZ0)==HIGH)  { KZ0=0;  }
               else if (digitalRead(pKZ0)==LOW)   { KZ0=1; ZUN=0; enzPos=enZ0; }
               if      (digitalRead(pKZ1)==HIGH)  { KZ1=0;  }
               else if (digitalRead(pKZ1)==LOW)   { KZ1=1; ZUN=0; enzPos=enZ1; }
               if      (digitalRead(pKZ2)==HIGH)  { KZ2=0;  }
               else if (digitalRead(pKZ2)==LOW)   { KZ2=1; ZUN=0; enzPos=enZ2; }
               if      (digitalRead(pKZ3)==HIGH)  { KZ3=0;  }
               else if (digitalRead(pKZ3)==LOW)   { KZ3=1; ZUN=0; enzPos=enZ3; }
               if      (digitalRead(pKZ4)==HIGH)  { KZ4=0;  }
               else if (digitalRead(pKZ4)==LOW)   { KZ4=1; ZUN=0; enzPos=enZ4; }
               if      (digitalRead(pKZ5)==HIGH)  { KZ5=0;  }
               else if (digitalRead(pKZ5)==LOW)   { KZ5=1; ZUN=0; enzPos=enZ5; }
               if ((!KZ0)&&(!KZ1)&&(!KZ2)&&(!KZ3)&&(!KZ4)&&(!KZ5)) { ZUN=1; }
               KX0=1; XUN=0;
            }
} 



//проверка состояния концевиков Y паралленый таск
static void vKYcheckTask(void *pvParameters) {
   vTaskDelay(330);
   for (;;) {  if      (digitalRead(pKY1)==HIGH)  { KY1=0;   }
               else if (digitalRead(pKY1)==LOW)   { KY1=1; YUN=0; }
               if      (digitalRead(pKY2)==HIGH)  { KY2=0;   }
               else if (digitalRead(pKY2)==LOW)   { KY2=1; YUN=0; }
               if      (digitalRead(pKY3)==HIGH)  { KY3=0;   }
               else if (digitalRead(pKY3)==LOW)   { KY3=1; YUN=0; }
               if      (digitalRead(Kcnt1)==HIGH) { cnt1=0;  }
               else if (digitalRead(Kcnt1)==LOW)  { cnt1=1;  }
               if      (digitalRead(Kcnt2)==HIGH) { cnt2=0;  }
               else if (digitalRead(Kcnt2)==LOW)  { cnt2=1;  }
               if ((!KY1)&&(!KY2)&&(!KY3)) { YUN=1; }
            } 
} 

//проверка состояния концевиков посадочного стола паралленый таск
static void vKStolTask(void *pvParameters) { 
   vTaskDelay(290);
   for (;;) {  if      (digitalRead(pKXun)==HIGH)  { KXun=0;  }
               else if (digitalRead(pKXun)==LOW)   { KXun=1;  }
               if      (digitalRead(pKYun)==HIGH)  { KYun=0;  }
               else if (digitalRead(pKYun)==LOW)   { KYun=1;  }
               if      (digitalRead(pKXce)==HIGH)  { KXce=0;  }
               else if (digitalRead(pKXce)==LOW)   { KXce=1;  }
               if      (digitalRead(pKYce)==HIGH)  { KYce=0;  } 
               else if (digitalRead(pKYce)==LOW)   { KYce=1;  }
            }
}

//проверка состояния концевиков ячеек паралленый таск
static void vKCellTask(void *pvParameters) { 
   vTaskDelay(290);
   for (;;) {  if      (digitalRead(pKNZ0X0Y3)==HIGH)  { Z0Y3=0;  }
               else if (digitalRead(pKNZ0X0Y3)==LOW)   { Z0Y3=1;  }
               if      (digitalRead(pKNZ0X0Y1)==HIGH)  { Z0Y1=0;  }
               else if (digitalRead(pKNZ0X0Y1)==LOW)   { Z0Y1=1;  }
               if      (digitalRead(pKNZ1X0Y1)==HIGH)  { Z1Y1=0;  }
               else if (digitalRead(pKNZ1X0Y1)==LOW)   { Z1Y1=1;  }
               if      (digitalRead(pKNZ2X0Y3)==HIGH)  { Z2Y3=0;  } 
               else if (digitalRead(pKNZ2X0Y3)==LOW)   { Z2Y3=1;  }
               if      (digitalRead(pKNZ2X0Y1)==HIGH)  { Z2Y1=0;  } 
               else if (digitalRead(pKNZ2X0Y1)==LOW)   { Z2Y1=1;  }
               if      (digitalRead(pKNZ3X0Y3)==HIGH)  { Z3Y3=0;  } 
               else if (digitalRead(pKNZ3X0Y3)==LOW)   { Z3Y3=1;  }
               if      (digitalRead(pKNZ3X0Y1)==HIGH)  { Z3Y1=0;  } 
               else if (digitalRead(pKNZ3X0Y1)==LOW)   { Z3Y1=1;  }
            }
}


//энкодер Z 
static void encoderZ() 
   { if  (digitalRead(AenZ)==LOW)
          { enzPos++; }
     else { enzPos--; }
     //Serial.println(enzPos);
   }


//проверка состояния концевиков специальных операций
static void vKSpecTask(void *pvParameters) { 
   vTaskDelay(295);
   for (;;) {  //верхний люк
               if      (digitalRead(pKVlv)==HIGH)  { Vlv=0;  }
               else if (digitalRead(pKVlv)==LOW)   { Vlv=1;  }
               if      (digitalRead(pKVln)==HIGH)  { Vln=0;  }
               else if (digitalRead(pKVln)==LOW)   { Vln=1;  }
               if      (digitalRead(pKVld)==HIGH)  { Vld=0;  }
               else if (digitalRead(pKVld)==LOW)   { Vld=1;  }
               if      (digitalRead(pKVlz)==HIGH)  { Vlz=0;  }
               else if (digitalRead(pKVlz)==LOW)   { Vlz=1;  }
               //отсек выдачи
               if      (digitalRead(pKovv)==HIGH)  { ovv=0;  }
               else if (digitalRead(pKovv)==LOW)   { ovv=1;  }
               if      (digitalRead(pKovn)==HIGH)  { ovn=0;  }
               else if (digitalRead(pKovn)==LOW)   { ovn=1;  }
               //крышка KR0
               if      (digitalRead(kr0O)==HIGH)  { KR0o=0;  }
               else if (digitalRead(kr0O)==LOW)   { KR0o=1;  }
               if      (digitalRead(kr0C)==HIGH)  { KR0c=0;  }
               else if (digitalRead(kr0C)==LOW)   { KR0c=1;  }
               if      (digitalRead(kr0N)==HIGH)  { KR0n=0;  }
               else if (digitalRead(kr0N)==LOW)   { KR0n=1;  }
            }
}



