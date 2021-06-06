 static void Y1Block() //движение к Y1zad
  {   //пока не сработает концевик KY1 движение не более 24сек.
      unsigned long timerY=millis();
      for(;;)
        {   StatusY="YmoveY1";
            tempmoveY="vpered"; //если проедет конечный концевик то следующее движение вперет
            //при сработке KY1  motorY(0) и  break;
            if (KY1) { motorY(0); vTaskDelay(200); if (KY1) { motorY(0); break; }}
            //при сработке таймера  motorY(0) и  break;
            if ((millis()-timerY)>24000) { motorY(0); break; }
            motorY(2); //назат
        } 
  } 

static void Y2Block() //движение к Y2center
  {   //пока не сработает концевик KY2 движение не более 24сек.
      unsigned long timerY=millis(); 
      for(;;)
        {   if (KY1) { StatusY="YmoveY2"; motorY(1); tempmoveY="vpered"; }
            else if ((YUN)&&(tempmoveY=="vpered")) { StatusY="YmoveY2"; motorY(1); }
            else if (KY3) { StatusY="YmoveY2"; motorY(2); tempmoveY="nazad"; }
            else if ((YUN)&&(tempmoveY=="nazad")) { StatusY="YmoveY2"; motorY(2); }
            //при сработке KY2  motorY(0) и  break;
            if (KY2) { motorY(0); vTaskDelay(200); if (KY2) { motorY(0); break; }}
            //при сработке таймера  motorY(0) и  break;
            if ((millis()-timerY)>24000) { motorY(0); break; }
        } 
  }  

  static void Y3Block() //движение к Y3pered
  {   //пока не сработает концевик KY3 движение не более 24сек.
      unsigned long timerY=millis();
      for(;;)
        {   StatusY="YmoveY3";
            tempmoveY="nazad"; //если проедет конечный концевик то следующее движение назат
            //при сработке KY3  motorY(0) и  break;
            if (KY3) { motorY(0); vTaskDelay(200); if (KY3) { motorY(0); break; }}
            //при сработке таймера  motorY(0) и  break;
            if ((millis()-timerY)>24000) { motorY(0); break; }
            motorY(1); //вперет
        }  
  } 

//мониторинг активного состояния AcY и запуск функций 
static void vAxis_Y(void *pvParameters) {
   for (;;) {  
                  if      ((AcY==1)&&(gSem==0))  { gSem=1; Y1Block(); AcY=-1; gSem=0; cmnum(); }
                  else if ((AcY==2)&&(gSem==0))  { gSem=1; Y2Block(); AcY=-1; gSem=0; cmnum(); }
                  else if ((AcY==3)&&(gSem==0))  { gSem=1; Y3Block(); AcY=-1; gSem=0; cmnum(); }
                  vTaskDelay(120);
            }
}