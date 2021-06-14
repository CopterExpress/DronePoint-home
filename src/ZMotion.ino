static void Z0Block() //движение к Z0(кстати отсек выдачи)
  {   //пока не сработает концевик KZ0 движение не более 90сек.
      unsigned long timerZ=millis();
      for(;;)
        {   StatusZ="ZmoveZ0";
            tempmoveZ="vpered"; //если проедет конечный концевик то следующее движение вперет
            //при сработке KZ0  motorZ(0) и  break;
            if (KZ0) { motorZ(0); vTaskDelay(200); if (KZ0) { motorZ(0); break; }}
            if ((KZ1)||(KZ2)||(KZ3)) { Flzkp=0; }
            //при сработке таймера  motorZ(0) и  break;
            if ((millis()-timerZ)>90000) { motorZ(0); break; }
            if      (Flzkp)   { motorZ(1);  } //вперед
            else if (!Flzkp)  { motorZ(2);  } //назат
        } 
  } 

static void Z1Block() //движение к Z1
  {   //пока не сработает концевик KZ1 движение не более 90сек.
      unsigned long timerZ=millis(); 
      for(;;)
        {   if (KZ0) { StatusZ="ZmoveZ1"; motorZ(1); tempmoveZ="vpered"; }
            else if ((ZUN)&&(tempmoveZ=="vpered")) { StatusZ="ZmoveZ1"; motorZ(1); }
            else if ((KZ2)||(KZ3)||(KZ4)||(KZ5)) { StatusZ="ZmoveZ1"; motorZ(2); tempmoveZ="nazad"; }
            else if ((ZUN)&&(tempmoveZ=="nazad")) { StatusZ="ZmoveZ1"; motorZ(2); }
            //при сработке KZ1  motorZ(0) и  break;
            if (KZ1) { motorZ(0); vTaskDelay(200); if (KZ1) { motorZ(0); break; }}
            //при сработке таймера  motorZ(0) и  break;
            if ((millis()-timerZ)>90000) { motorZ(0); break; }
        }  
  } 

  static void Z2Block() //движение к Z2
  {   //пока не сработает концевик KZ2 движение не более 90сек.
      unsigned long timerZ=millis();
      for(;;)
        {   if ((KZ0)||(KZ1)) { StatusZ="ZmoveZ2"; motorZ(1); tempmoveZ="vpered"; }
            else if ((ZUN)&&(tempmoveZ=="vpered")) { StatusZ="ZmoveZ2"; motorZ(1); }
            else if ((KZ3)||(KZ4)||(KZ5)) { StatusZ="ZmoveZ2"; motorZ(2); tempmoveZ="nazad"; }
            else if ((ZUN)&&(tempmoveZ=="nazad")) { StatusZ="ZmoveZ2"; motorZ(2); }
            //при сработке KZ2  motorZ(0) и  break;
            if (KZ2) { motorZ(0); vTaskDelay(200); if (KZ2) { motorZ(0); break; }}
            //при сработке таймера  motorZ(0) и  break;
            if ((millis()-timerZ)>90000) { motorZ(0); break; }
        } 
  } 

  static void Z3Block() //движение к Z3
  {   //пока не сработает концевик KZ3 движение не более 90сек.
      unsigned long timerZ=millis(); 
      for(;;)
        {   if ((KZ0)||(KZ1)||(KZ2)) { StatusZ="ZmoveZ3"; motorZ(1); tempmoveZ="vpered"; }
            else if ((ZUN)&&(tempmoveZ=="vpered")) { StatusZ="ZmoveZ3"; motorZ(1); }
            else if ((KZ4)||(KZ5)) { StatusZ="ZmoveZ3"; motorZ(2); tempmoveZ="nazad"; }
            else if ((ZUN)&&(tempmoveZ=="nazad")) { StatusZ="ZmoveZ3"; motorZ(2); }
            //при сработке KZ3  motorZ(0) и  break;
            if (KZ3) { motorZ(0); vTaskDelay(200); if (KZ3) { motorZ(0); break; }}
            //при сработке таймера  motorZ(0) и  break;
            if ((millis()-timerZ)>90000) { motorZ(0); break; }
        } 
  } 

  static void Z4Block() //движение к Z4
  {   //пока не сработает концевик KZ4 движение не более 90сек.
      unsigned long timerZ=millis();
      for(;;)
        {   if ((KZ0)||(KZ1)||(KZ2)||(KZ3)) { StatusZ="ZmoveZ4"; motorZ(1); tempmoveZ="vpered"; }
            else if ((ZUN)&&(tempmoveZ=="vpered")) { StatusZ="ZmoveZ4"; motorZ(1); }
            else if ((KZ5)) { StatusZ="ZmoveZ4"; motorZ(2); tempmoveZ="nazad"; }
            else if ((ZUN)&&(tempmoveZ=="nazad")) { StatusZ="ZmoveZ4"; motorZ(2); }
            //при сработке KZ4  motorZ(0) и  break;
            if (KZ4) { motorZ(0); vTaskDelay(200); if (KZ4) { motorZ(0); break; }}
            //при сработке таймера  motorZ(0) и  break;
            if ((millis()-timerZ)>90000) { motorZ(0); break; }
        } 
  } 

  static void Z5Block() //движение к Z5
  {   //пока не сработает концевик KZ5 движение не более 90сек.
      unsigned long timerZ=millis();
      for(;;)
        {   StatusZ="ZmoveZ5";
            tempmoveZ="nazad"; //если проедет конечный концевик то следующее движение назат
            //при сработке KZ5  motorZ(0) и  break;
            if (KZ5) { motorZ(0); vTaskDelay(200); if (KZ5) { motorZ(0); break; }}
            //при сработке таймера  motorZ(0) и  break;
            if ((millis()-timerZ)>90000) { motorZ(0); break; }
            //если верхний люк закрыт motorZ(0) и  break;
            if (StatusVL!="open") { motorZ(0); break; }
            motorZ(1); //вперед
        } 
  }

  //всегда X0
  static void X0Block() { KX0=1; } 

  //Z корректор 
  static void Zcorrect()  
     {  if      (meZ=="Z0")   { Z0Block();  }
        else if (meZ=="Z1")   { Z1Block();  }
        else if (meZ=="Z2")   { Z2Block();  }
        else if (meZ=="Z3")   { Z3Block();  }
     }

//мониторинг активного состояния AcZ и запуск функций 
static void vAxis_Z(void *pvParameters) {
   for (;;) {  
                    if      ((AcZ==0)&&(gSem==0))  { gSem=1; Z0Block(); AcZ=-1; gSem=0; cmnum(); }
                    else if ((AcZ==1)&&(gSem==0))  { gSem=1; Z1Block(); AcZ=-1; gSem=0; cmnum(); }
                    else if ((AcZ==2)&&(gSem==0))  { gSem=1; Z2Block(); AcZ=-1; gSem=0; cmnum(); }
                    else if ((AcZ==3)&&(gSem==0))  { gSem=1; Z3Block(); AcZ=-1; gSem=0; cmnum(); }
                    else if ((AcZ==4)&&(gSem==0))  { gSem=1; Z4Block(); AcZ=-1; gSem=0; cmnum(); }
                    else if ((AcZ==5)&&(gSem==0))  { gSem=1; Z5Block(); AcZ=-1; gSem=0; cmnum(); }
                    vTaskDelay(120);
            }
}

