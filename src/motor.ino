//мотор ось Z 
static void motorZ(byte direct) 
   { uint16_t shm1,shm2;
     if      (direct==0)    { shm1=0; shm2=0;     } //стоп
     else if (direct==1)    { shm1=65000; shm2=0; } //вперет 46000
     else if (direct==2)    { shm1=0; shm2=63700; } //назат 20100
   /*  if ((StatusZ=="ZmoveZ2")&&(direct==1)) { shm1=44500; shm2=0;  } 
     if ((StatusZ=="ZmoveZ2")&&(direct==2)) { shm1=19800; shm2=0;  }
     if ((StatusZ=="ZmoveZ3")&&(direct==1)) { shm1=44500; shm2=0;  } 
     //if ((StatusZ=="ZmoveZ3")&&(direct==2)) { shm1=19800; shm2=0;  }
     if ((StatusZ=="ZmoveZ4")&&(direct==1)) { shm1=44500; shm2=0;  } //крышка
     //if ((StatusZ=="ZmoveZ4")&&(direct==2)) { shm1=19800; shm2=0;  }
     if ((StatusZ=="ZmoveZ5")&&(direct==1)) { shm1=44500; shm2=0;  } //контейнер*/
     pwmWrite(zpwm1,shm1);
     pwmWrite(zpwm2,shm2);
   }    

//мотор ось Y 
static void motorY(byte direct) 
   { uint16_t shm1,shm2;
     if      (direct==0)    { shm1=0; shm2=0;     } //стоп
     else if (direct==1)    { shm1=36000; shm2=0; } //вперет к Y3pered
     else if (direct==2)    { shm1=0; shm2=36000; } //назат к Y1zad
     pwmWrite(ypwm1,shm1);
     pwmWrite(ypwm2,shm2);
   } 

//мотор гусеницы 
static void gusmotor(byte direct) 
      {  boolean shm1,shm2;
         if      (direct==0)  {  shm1=0; shm2=0;  }
         else if (direct==1)  {  shm1=1; shm2=0;  } //выгрузка
         else if (direct==2)  {  shm1=0; shm2=1;  } //загрузка
         digitalWrite(gpwm1, shm1);
         digitalWrite(gpwm2, shm2);
      } 

//мотор отсека выдачи 
static void ovmotor(byte direct) 
      {  boolean shm1,shm2;
         if      (direct==0)  {  shm1=0; shm2=0;  }
         else if (direct==1)  {  shm1=1; shm2=0;  } //открыть
         else if (direct==2)  {  shm1=0; shm2=1;  } //закрыть
         digitalWrite(ovpwm1, shm1);
         digitalWrite(ovpwm2, shm2);
      }

//соленоид отсека выдачи 
static void ovsolenoid(boolean direct) 
      {  digitalWrite(povsl, direct); }

//мотор подъема спуска верхнего люка
static void vlpmotor(byte direct) 
      {  boolean shm1,shm2;
         if      (direct==0)  {  shm1=0; shm2=0;  }
         else if (direct==1)  {  shm1=1; shm2=0;  } //подъем
         else if (direct==2)  {  shm1=0; shm2=1;  } //спуск
         digitalWrite(vlppwm1, shm1);
         digitalWrite(vlppwm2, shm2);
      }

//мотор выдвинуть задвинуть верхнего люка
static void vlzmotor(byte direct) 
      {  boolean shm1,shm2;
         if      (direct==0)  {  shm1=0; shm2=0;  }
         else if (direct==1)  {  shm1=1; shm2=0;  } //выдвинуть
         else if (direct==2)  {  shm1=0; shm2=1;  } //задвинуть
         digitalWrite(vlvpwm1, shm1);
         digitalWrite(vlvpwm2, shm2);
      }

//мотор X механизма захвата
static void motorSlX(byte direct) 
   { uint16_t shm1,shm2;
     if      (direct==0)    { shm1=0; shm2=0; }
     else if (direct==1)    { shm1=65000; shm2=0; } //вперет
     else if (direct==2)    { shm1=0; shm2=65000; } //назат
     pwmWrite(zxpwm1,shm1);
     pwmWrite(zxpwm2,shm2);
   } 

//мотор Y механизма захвата
static void motorSlY(byte direct)  
   { /*char sMslYout[65];
     sprintf(sMslYout, "SlYout: MstolY=%s" , (const char *)MstolY.c_str());*/
     uint16_t shm1,shm2;
     if      (direct==0)    { shm1=0; shm2=0; }
     else if (direct==1)    { shm1=65000; shm2=0; } //вперет
     else if (direct==2)    { shm1=0; shm2=65000; } //назат
     pwmWrite(zypwm1,shm1);
     pwmWrite(zypwm2,shm2);
   }

//мотор крышки KR0
static void motKR0(byte direct) 
      {  boolean shm1,shm2;
         if      (direct==0)  {  shm1=0; shm2=0;  }
         else if (direct==1)  {  shm1=1; shm2=0;  } //открыть
         else if (direct==2)  {  shm1=0; shm2=1;  } //закрыть
         digitalWrite(kr0m1, shm1);
         digitalWrite(kr0m2, shm2);
      }

 //серва замок контейнера 
 static void servLockCon(boolean direct)  
  {  vTaskDelay(330);
     if ((direct)&&(s_trig))//закрыть
            {  for (int pulselen=SERVOMAX; pulselen>SERVOMIN; pulselen-=22) 
                    {  pwm.setPWM(conLock, 0, pulselen); vTaskDelay(6); }
               s_trig=false;
            }
     else if ((!direct)&&(!s_trig))//открыть
            {  for (int pulselen=SERVOMIN; pulselen<SERVOMAX; pulselen+=22) 
                     {  pwm.setPWM(conLock, 0, pulselen); vTaskDelay(6);  }
               s_trig=true;
            }
  }

//управляем шаговым двигателем глобальная функция
static void miShag(byte direct,int shagSpeed) 
      {  static boolean trig;
         if      (direct==1)  {  digitalWrite(Xdir,HIGH); }
         else if (direct==2)  {  digitalWrite(Xdir,LOW);  }
                         //shagSpeed зависит от делителя
         if (direct!=0)  //shagSpeed от 50 до 3000
                 {  micdelay(shagSpeed);
                    trig=!trig;
                    digitalWrite(Xstep,trig);
                 }
      } 

//управляем шаговым двигателем паралленый таск {"MOT":"on"}
static void vShagMotTask(void *pvParameters) {
    vTaskDelay(1200); 
    for (;;) {  if (MOT=="on")  
                   { timertrack=millis(); 
                     while ((millis()-timertrack)<Mval) 
                        {  miShag(1,350); } 
                     miShag(0,50);
                     vTaskDelay(3000);
                     timertrack=millis(); 
                     while ((millis()-timertrack)<Mval) 
                        {  miShag(2,350); }
                     miShag(0,50);
                     vTaskDelay(3000); 
                   }
             }
}  

//микрос делай
static void micdelay(int md) 
      {  unsigned long mdtimer=micros();
         while ((micros()-mdtimer)<md) 
               {  } 
      }

static void Zjump(int direct)    
  {  long pzStr;
     pzStr=enzPos;
     //meZ=StatusZ; глобальная

     if (direct>0)  {  vTaskDelay(200);
                       direct=abs(direct);
                       motorZ(1); //вверх
                       vTaskDelay(direct);
                       motorZ(0);
                       vTaskDelay(200);
                       if (pzStr<enzPos)
                          { Serial.println("___UP__OK___"); }
                    }
     else if (direct<0) { vTaskDelay(200);
                          direct=abs(direct);
                          motorZ(2); //вниз
                          vTaskDelay(direct);
                          motorZ(0);
                          vTaskDelay(200);
                          if (pzStr>enzPos)
                             { Serial.println("___DW__OK___"); }
                        }
  }

