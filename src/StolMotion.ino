static void centr() //движение механизма захвата к центру, пока не сработают
  {                 //концевики KXce и KYce движение не более 75сек.
      unsigned long timerCE=millis();
      servLockCon(1); //закрыть замок контейнера
      vTaskDelay(910);
      for(;;)
        {   StatusZuh="movecenter";
            //при сработке (KXce)&&(KYce) stop и break;
            if ((KXce)&&(KYce)) { motorSlX(0); motorSlY(0); vTaskDelay(200); if ((KXce)&&(KYce)) { motorSlX(0); motorSlY(0); break; }}
            //при сработке таймера stop и break;
            if ((millis()-timerCE)>75000) { motorSlX(0); motorSlY(0); break; }
            //первой начинает движение ось X захвата
            if (!KXce) { motorSlX(1); motorSlY(0); } //X вперет Y стоп
            if (KXce)  { motorSlX(0); motorSlY(1); } //X стоп Y вперет
        } 
  } 


static void uncentr() //движение механизма захвата от центра, пока не сработают
  {                   //концевики KXun и KYun движение не более 55сек.
      unsigned long timerUN=millis();
      servLockCon(1); //закрыть замок контейнера
      vTaskDelay(910);
      for(;;)
        {   StatusZuh="moveuncenter";
            if (!KXun) { motorSlX(2); } //X назат
            if (KXun)  { motorSlX(0); } //X стоп
            if (!KYun) { motorSlY(2); } //Y назат
            if (KYun)  { motorSlY(0); } //Y стоп
            //при сработке (KXun)&&(KYun) stop и break;
            if ((KXun)&&(KYun)) { motorSlX(0); motorSlY(0); vTaskDelay(200); if ((KXun)&&(KYun)) { motorSlX(0); motorSlY(0); break; }}
            //при сработке таймера stop и break;
            if ((millis()-timerUN)>55000) { motorSlX(0); motorSlY(0); break; }
        } 
  } 

//мониторинг активного состояния AcStol и запуск функций   
static void vAxis_Sl(void *pvParameters) {
   for (;;) {     if      ((AcStol=="centr")&&(gSem==0))    { gSem=1; centr();   AcStol="none"; gSem=0; cmnum(); }
                  else if ((AcStol=="uncentr")&&(gSem==0))  { gSem=1; uncentr(); AcStol="none"; gSem=0; cmnum(); }
                  vTaskDelay(120);
            }
}