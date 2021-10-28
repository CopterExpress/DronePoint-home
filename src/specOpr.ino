//выгрузка из ячейки передней или задней в зависимости от флага stY
static void unloadingCELL() 
      { if  (stY=="3") //действия производятся с передней ячейкой
            {   vygruzkaKP();   }
        else if  (stY=="1") //действия производятся с задней ячейкой
            {   vygruzkaKZ();   }
      }

//загрузка в ячейку переднею или заднею в зависимости от флага stY
static void loadingCELL()
      { if  (stY=="3") //действия производятся с передней ячейкой
            {   zagruzkaKP();   }
        else if  (stY=="1") //действия производятся с задней ячейкой
            {   zagruzkaKZ();  }
      }

//выгрузка из передней ячейки
static void vygruzkaKP()   
       {  //{"command":"VKP"}
          vTaskDelay(180);
          if (StatusKkr=="free") { 
          meZ=StatusZ; 
          int Zdw,Zup;
          boolean vyA=0; 
          //параметры спуска и подъема оси Z зависят от уровня
          //операция допускается только на этих уровнях т.е. vyA=1
          if      (meZ=="Z0")   { Zdw=-250; Zup=420; vyA=1; } //отсек выдачи Y3
          else if (meZ=="Z1")   { Zdw=-300; Zup=420; vyA=1; } //нет ячейки
          else if (meZ=="Z2")   { Zdw=-250; Zup=422; vyA=1; }
          else if (meZ=="Z3")   { Zdw=-250; Zup=410; vyA=1; } 
          if ((KZ0)&&(KX0)) { ovopcl(0); } //операция возможна при закрытом люке выдачи
          //операция допускается
          if  ((vyA)&&(XUN==0)&&(VD)&&(Bvz))
             {   OprSt="vygruzka " + meZ + StatusX + "Y3 ... .";
                 //операция выгрузки из ячейки выполняется снизу вверх
                 Zjump(Zdw); //опускаем Z
                 //выдвигаем Y на Y3pered 
                 Y3Block(); 
                 vTaskDelay(120);
                 //Y выдвинут
                 Zjump(Zup); //поднимаемся Z вверх цепляем контейнер
                 //работа гусеницы выгружаем контейнер
                 //пока не сработает концевик на каретке движение не более 9сек.
                 unsigned long gustrack=millis();
                 for(;;)
                       {  gusmotor(1);  
                          //при сработке cnt2  gusmotor(0) и  break;
                          if (cnt2) { gusmotor(0); vTaskDelay(200); if (cnt2) { gusmotor(0); break; }}
                          //при сработке таймера  gusmotor(0) и  break;
                          if ((millis()-gustrack)>10100) { gusmotor(0); break; }
                       }  
                 //задвигаем Y на Y2center
                 Y2Block();
                 vTaskDelay(120);
                 //Y задвинут
                 //возвращаем Z в исходное положение
                 tempmoveZ="nazad";
                 Zcorrect();
                 OprSt="vygruzka " + meZ + StatusX + "Y3 OK";
             }
       }  } 


//загрузка в переднею ячейку
static void zagruzkaKP() 
       {  //{"command":"ZKP"}
          vTaskDelay(180);
          if (StatusKkr=="loaded") { 
          meZ=StatusZ; 
          int Zdw,Zup;
          boolean vaA=0;
          //параметры спуска и подъема оси Z зависят от уровня
          //операция допускается только на этих уровнях т.е. vaA=1
          if      (meZ=="Z0")   { Zup=185; Zdw=-415; vaA=1;  } //отсек выдачи Y3
          else if (meZ=="Z1")   { Zup=185; Zdw=-415; vaA=1;  } //нет ячейки
          else if (meZ=="Z2")   { Zup=200; Zdw=-420; vaA=1;  }
          else if (meZ=="Z3")   { Zup=185; Zdw=-380; vaA=1;  } 
          if ((KZ0)&&(KX0)) { ovopcl(0); } //операция возможна при закрытом люке выдачи
          //операция допускается 
          if  ((vaA)&&(XUN==0)&&(VD)&&(Bvz)&&(BPP))
               {  OprSt="zagruzka " + meZ + StatusX + "Y3 ... .";
                  //операция загрузки в ячейку выполняется сверху вниз
                  Zjump(Zup); //поднимаем Z
                  //выдвигаем Y на Y3pered
                  Y3Block();
                  vTaskDelay(120);
                  //Y выдвинут
                  //работа гусеницы загружаем контейнер
                  //движение не более 5сек.
                  unsigned long gustrack=millis();
                  for(;;)
                       {  //при сработки концевиков в передних ячейках gusmotor(0) и  break;
                          if      ((meZ=="Z0")&&(Z0Y3)) { gusmotor(0); vTaskDelay(200); if (Z0Y3) { gusmotor(0); break; }}
                          else if ((meZ=="Z2")&&(Z2Y3)) { gusmotor(0); vTaskDelay(200); if (Z2Y3) { gusmotor(0); break; }}
                          else if ((meZ=="Z3")&&(Z3Y3)) { gusmotor(0); vTaskDelay(200); if (Z3Y3) { gusmotor(0); break; }}
                          //при сработке таймера  gusmotor(0) и  break;
                          if ((millis()-gustrack)>7200) { gusmotor(0); break; }
                          gusmotor(2);
                       } 
                  //контейнер загружен    
                  Zjump(Zdw); //опускаем Z контейнер не цепляет
                  //задвигаем Y на Y2center
                  Y2Block();
                  vTaskDelay(120);
                  //Y задвинут
                  //возвращаем Z в исходное положение
                  tempmoveZ="vpered";
                  if  (meZ=="Z0") { Flzkp=1; } 
                  Zcorrect();
                  Flzkp=0;
                  OprSt="zagruzka " + meZ + StatusX + "Y3 OK";
               }
       }  }



//выгрузка из задней ячейки
static void vygruzkaKZ()   
       {  //{"command":"VKZ"}
          vTaskDelay(180);
          if (StatusKkr=="free") { 
          meZ=StatusZ; 
          int Zdw,Zup;
          boolean vyA=0; 
          //параметры спуска и подъема оси Z зависят от уровня
          //операция допускается только на этих уровнях т.е. vyA=1
          if      (meZ=="Z0")   { Zdw=-370; Zup=260; vyA=1; } //отсек выдачи
          else if (meZ=="Z1")   { Zdw=-300; Zup=240; vyA=1; } 
          else if (meZ=="Z2")   { Zdw=-290; Zup=314; vyA=1; }
          else if (meZ=="Z3")   { Zdw=-280; Zup=344; vyA=1; } 
          //операция допускается 
          if  ((vyA)&&(XUN==0)&&(VD)&&(Bpz))
             {   OprSt="vygruzka " + meZ + StatusX + "Y1 ... .";
                 //операция выгрузки из ячейки выполняется снизу вверх
                 Zjump(Zdw); //опускаем Z
                 //выдвигаем Y на Y1zad 
                 Y1Block(); 
                 vTaskDelay(120);
                 //Y выдвинут
                 Zjump(Zup); //поднимаемся Z вверх цепляем контейнер
                 //работа гусеницы выгружаем контейнер
                 //пока не сработает концевик на каретке движение не более 9сек.
                 unsigned long gustrack=millis();
                 for(;;)
                       {  gusmotor(2);  
                          //при сработке cnt2  gusmotor(0) и  break;
                          if (cnt2) { gusmotor(0); vTaskDelay(200); if (cnt2) { gusmotor(0); break; }}
                          //при сработке таймера  gusmotor(0) и  break;
                          if ((millis()-gustrack)>9000) { gusmotor(0); break; }
                       }  
                 //задвигаем Y на Y2center
                 Y2Block();
                 vTaskDelay(120);
                 //Y задвинут
                 //возвращаем Z в исходное положение
                 tempmoveZ="nazad";
                 Zcorrect();
                 OprSt="vygruzka " + meZ + StatusX + "Y1 OK";
             }
       }  } 


//загрузка в заднею ячейку
static void zagruzkaKZ() 
       {  //{"command":"ZKZ"}
          vTaskDelay(180);
          if (StatusKkr=="loaded") { 
          meZ=StatusZ; 
          int Zdw,Zup;
          boolean vaA=0;
          //параметры спуска и подъема оси Z зависят от уровня
          //операция допускается только на этих уровнях т.е. vaA=1
          if      (meZ=="Z0")   { Zup=100; Zdw=-180; vaA=1;  } 
          else if (meZ=="Z1")   { Zup=80;  Zdw=-620; vaA=1;  }
          else if (meZ=="Z2")   { Zup=110; Zdw=-620; vaA=1;  }
          else if (meZ=="Z3")   { Zup=100; Zdw=-655; vaA=1;  } 
          //операция допускается 
          if  ((vaA)&&(XUN==0)&&(VD)&&(Bpz)&&(BPZ))
               {  OprSt="zagruzka " + meZ + StatusX + "Y1 ... .";
                  //операция загрузки в ячейку выполняется сверху вниз
                  Zjump(Zup); //поднимаем Z
                  //выдвигаем Y на Y1zad
                  Y1Block();
                  vTaskDelay(120);
                  //Y выдвинут
                  //работа гусеницы загружаем контейнер
                  //движение не более 5сек.
                  unsigned long gustrack=millis();
                  for(;;)
                       {  //при сработки концевиков в задних ячейках gusmotor(0) и  break;
                          if      ((meZ=="Z0")&&(Z0Y1)) { gusmotor(0); vTaskDelay(200); if (Z0Y1) { gusmotor(0); break; }}
                          else if ((meZ=="Z1")&&(Z1Y1)) { gusmotor(0); vTaskDelay(200); if (Z1Y1) { gusmotor(0); break; }}
                          else if ((meZ=="Z2")&&(Z2Y1)) { gusmotor(0); vTaskDelay(200); if (Z2Y1) { gusmotor(0); break; }}
                          else if ((meZ=="Z3")&&(Z3Y1)) { gusmotor(0); vTaskDelay(200); if (Z3Y1) { gusmotor(0); break; }}
                          //при сработке таймера  gusmotor(0) и  break;
                          if ((millis()-gustrack)>6200) { gusmotor(0); break; }
                          gusmotor(1);
                       } 
                  //контейнер загружен    
                  Zjump(Zdw); //опускаем Z контейнер не цепляет
                  //задвигаем Y на Y2center
                  Y2Block();
                  vTaskDelay(120);
                  //Y задвинут
                  //возвращаем Z в исходное положение
                  tempmoveZ="vpered";
                  if  (meZ=="Z0") { Flzkp=1; } 
                  Zcorrect();
                  Flzkp=0;
                  OprSt="zagruzka " + meZ + StatusX + "Y1 OK";
               }
       }  }



//открыть верхний люк 
static void openVL() 
   {//операция не допускается при нахождении каретки на уровнях Z4 или Z5
      if ((!KZ4)||(!KZ5))
         { OprSt="top hatch opening.. .";
           vlps(0);
           vlvz(0);
           OprSt="top hatch open OK";
         }
   } 

//закрыть верхний люк 
static void closeVL() 
   {//операция не допускается при нахождении каретки на уровнях Z4 или Z5
        if ((!KZ4)||(!KZ5))
         { OprSt="top hatch closing.. .";
           vlvz(1);
           vlps(1);
           OprSt="top hatch close OK";           
         }
   }

//подъем и спуск верхнего люка
static void vlps(boolean direct) 
      {  vTaskDelay(180);
         unsigned long vltrack;
         if ((direct)&&(Vld)) //подъем допускается при выдвинутом положении
             { vltrack=millis();
               for(;;)
                  {  //при сработке Vlv vlpmotor(0) и  break;
                     if (Vlv) { vlpmotor(0); vTaskDelay(200); if (Vlv) { vlpmotor(0); break; }}
                     //при сработке таймера  vlpmotor(0) и  break;
                     if ((millis()-vltrack)>26000) { vlpmotor(0); break; }
                     vlpmotor(1);
                  } 
             }
         else if (!direct) //спуск
             { vltrack=millis(); 
               for(;;)
                  {  //при сработке Vln vlpmotor(0) и  break;
                     if (Vln) { vlpmotor(0); vTaskDelay(200); if (Vln) { vlpmotor(0); break; }}
                     //при сработке таймера  vlpmotor(0) и  break;
                     if ((millis()-vltrack)>26000) { vlpmotor(0); break; }
                     vlpmotor(2);
                  } 
             }
      }

//выдвинуть и задвинуть верхний люк
static void vlvz(boolean direct) 
      {  vTaskDelay(180);
         unsigned long vltrack;
         if ((direct)&&(Vln)) //выдвинуть 
             { vltrack=millis(); 
               for(;;)
                  {  //при сработке Vld vlzmotor(0) и  break;
                     if (Vld) { vlzmotor(0); vTaskDelay(200); if (Vld) { vlzmotor(0); break; }}
                     //при сработке таймера  vlzmotor(0) и  break;
                     if ((millis()-vltrack)>30000) { vlzmotor(0); break; }
                     vlzmotor(1);
                  }
             }
         else if ((!direct)&&(Vln)) //задвинуть 
             { vltrack=millis();
               for(;;)
                  {  //при сработке Vlz vlzmotor(0) и  break;
                     if (Vlz) { vlzmotor(0); vTaskDelay(200); if (Vlz) { vlzmotor(0); break; }}
                     //при сработке таймера  vlzmotor(0) и  break;
                     if ((millis()-vltrack)>30000) { vlzmotor(0); break; }
                     vlzmotor(2);
                  }  
             }
      }

//открыть и закрыть замок крышки KR0
static void lockKR0(boolean direct) 
      {  vTaskDelay(180);
         unsigned long zmtrack;
         if (direct) //открыть
             { zmtrack=millis(); 
               for(;;)
                  {  if (KR0o) { motKR0(0); vTaskDelay(200); if (KR0o) { motKR0(0); break; }}
                     if ((millis()-zmtrack)>3500) { motKR0(0); break; }
                     motKR0(1);
                  }
             }
         else //закрыть
             { zmtrack=millis();
               for(;;)
                  {  if (KR0c) { motKR0(0); vTaskDelay(200); if (KR0c) { motKR0(0); break; }}
                     if ((millis()-zmtrack)>3500) { motKR0(0); break; }
                     motKR0(2);
                  }  
             }
      }

//открыть или закрыть люк отсека выдачи
static void ovopcl(boolean direct) 
      {  vTaskDelay(180);
         unsigned long ovtrack;
         if ((direct)&&(!ovv)) //открыть 
             { OprSt="bottom hatch opening.. .";
               ovsolenoid(1);
               vTaskDelay(1200);
               ovsolenoid(0);
               vTaskDelay(1000);
               ovsolenoid(1);
               vTaskDelay(1200);
               ovsolenoid(0);
               vTaskDelay(1000);
               ovsolenoid(1);
               vTaskDelay(1900);
               ovtrack=millis(); 
               for(;;)
                  {  //при сработке ovv ovmotor(0) и  break;
                     if (ovv) { ovmotor(0); vTaskDelay(200); if (ovv) { ovmotor(0); break; }}
                     //при сработке таймера  ovmotor(0) и  break;
                     if ((millis()-ovtrack)>22000) { ovmotor(0); break; }
                     ovmotor(1);
                  }
               vTaskDelay(1000);
               ovsolenoid(0);
               vTaskDelay(1000);
               OprSt="bottom hatch open OK";
             }
         else if ((!direct)&&(!ovn)) //закрыть
             { OprSt="bottom hatch closing.. .";
               ovsolenoid(1);
               vTaskDelay(1200);
               ovsolenoid(0);
               vTaskDelay(1000);
               ovsolenoid(1);
               vTaskDelay(1200);
               ovsolenoid(0);
               vTaskDelay(1000);
               ovsolenoid(1);
               vTaskDelay(1900);
               ovtrack=millis();
               for(;;)
                  {  //при сработке ovn ovmotor(0) и  break;
                     if (ovn) { ovmotor(0); vTaskDelay(200); if (ovn) { ovmotor(0); break; }}
                     //при сработке таймера  ovmotor(0) и  break;
                     if ((millis()-ovtrack)>22000) { ovmotor(0); break; }
                     ovmotor(2);
                  }
               vTaskDelay(1000);
               ovsolenoid(0);
               vTaskDelay(1000);
               OprSt="bottom hatch close OK";  
             }
      }

//установка крышки в зарядку 
static void CoverInst()   
  {     vTaskDelay(180);
        if ((!KR0n)&&(pKR=="KR0"))
            { OprSt="coverKR0 installed.. .";
              vTaskDelay(120);
              lockKR0(1);//открываем замок 
              Z4Block(); //Z4 уровень крышек
              lockKR0(0);//закрываем замок
              tempmoveZ="nazad";
              vTaskDelay(120);
              Z3Block(); //Z3 уровень
              Y2Block();
              OprSt="coverKR0 installed OK"; 
            }
  } 

//снятие крышки c зарядки   
static void CoverUninst()   
  {     vTaskDelay(180);
        if ((KR0n)&&(pKR=="KR0")) 
             { OprSt="coverKR0 uninstalled.. .";
               vTaskDelay(120);
               Z4Block();  //Z4 уровень крышек
               vTaskDelay(120);
               lockKR0(1); //открываем замок
               vTaskDelay(120);
               tempmoveZ="nazad";
               Z3Block();  //Z3 уровень
               lockKR0(0); //закрываем замок
               Y2Block();
               OprSt="coverKR0 uninstalled OK";
             }
  }

 //вставка контейнера в коптер
 static void contenerUP() 
    {  vTaskDelay(180);
       if ((Vln)&&(Vlz)&&(pDN)&&((cnt1)||(cnt2)))  
         { OprSt="insert container.. .";
           servLockCon(1); 
           vTaskDelay(300);
           servLockCon(0); //открыть замок контейнера
           vTaskDelay(910);
           Y2Block();
           vTaskDelay(200);
           Z5Block();  //Z5 уровень коптера
           vTaskDelay(200);
           servLockCon(1); //закрыть замок контейнера
           vTaskDelay(1500);
           tempmoveZ="nazad";
           Z3Block();  //Z3 уровень
           Y2Block();
           OprSt="insert container OK";
         }
    }

//снятие контейнера из коптера
 static void contenerDN() 
    {  vTaskDelay(180);
       if ((Vln)&&(Vlz)&&(pDN)&&(!cnt1)&&(!cnt2))  
         { OprSt="removing container.. .";
           Y2Block();
           vTaskDelay(200);
           Z5Block();  //Z5 уровень коптера
           servLockCon(0); //открыть замок контейнера
           vTaskDelay(1500);
           tempmoveZ="nazad";
           Z3Block();  //Z3 уровень
           Y2Block();
           vTaskDelay(350);
           servLockCon(1);
           OprSt="removing container OK";
         }
    }


 //проверка вольт сенсоров крышек
 static void vVsensTask(void *pvParameters) { 
   vTaskDelay(402);
   for (;;) { ADCkr0=analogRead(pADCkr0);
              
              uint8_t i=0;
                    for  (i; i<9; i++)
                      {  ADCkr0+=(analogRead(pADCkr0));  
                         vTaskDelay(44);
                      }

              ADCkr0=ADCkr0/10;
                                         
              //переводим в вольты
              ADCkr0=ADCkr0*vsensc;
                            
              if (KR0n) { sprintf(VBkr0, "VBAT=%.2f", ADCkr0);  } 
              else      { sprintf(VBkr0, "none");   }
              
              //sprintf(vdKr3, "VBATKr3=%.2f", ADCkr3);
              vTaskDelay(700);
            }
}


 //проверка состояния весов
 static void vScaleTask(void *pvParameters) { 
   vTaskDelay(311);
   for (;;) {   scale.set_scale(calibration_factor);
                units=scale.get_units(), 10;
                if (units<0) { units=0.00; }
                
                units=units*0.035274; //унции в граммы
                units=units-360;      //не учитываем вес контейнера
                if (units<0) { units=0.00; }

                if ((KZ0)&&(KX0)) //отсек выдачи
                   { if (units>2000) { Vm=0; } //запрещенно
                     else  { Vm=1; } //разрешенно  
                   }
                else  { Vm=1; } //разрешенно

                outkg=units;          
                
                //if   ((Vo)&&(Vm)) { VD=1; rgbLn(0); } //разрешенно
                //else { VD=0; rgbLn(1); } //запрещенно
                                
                vTaskDelay(500);
            }
} 


//лазерная система контроля объема
static void vLaserTask(void *pvParameters) { 
   vTaskDelay(315);
   unsigned long lzDt1,lzDt2,lzDt3,lzDt4,lzDt5,lzDt6,lzDt7,lzDt8;
   for (;;) {  if ((KZ0)&&(KX0)) //отсек выдачи
                  { digitalWrite(pLaz, HIGH); //вкл лазер
                    vTaskDelay(400);
                    lzDt1=analogRead(pADClz1);
                    lzDt2=analogRead(pADClz2);
                    lzDt3=analogRead(pADClz3);
                    lzDt4=analogRead(pADClz4);
                    lzDt5=analogRead(pADClz5);
                    lzDt6=analogRead(pADClz6);
                    lzDt7=analogRead(pADClz7);
                    lzDt8=analogRead(pADClz8);
                
                    uint8_t i=0;
                    for  (i; i<9; i++)
                      {  lzDt1+=(analogRead(pADClz1));  
                         lzDt2+=(analogRead(pADClz2));
                         lzDt3+=(analogRead(pADClz3));
                         lzDt4+=(analogRead(pADClz4));
                         lzDt5+=(analogRead(pADClz5));
                         lzDt6+=(analogRead(pADClz6));
                         lzDt7+=(analogRead(pADClz7));
                         lzDt8+=(analogRead(pADClz8));
                         vTaskDelay(44);
                      }

                    lzDt1=lzDt1/10; lzDt2=lzDt2/10; lzDt3=lzDt3/10; lzDt4=lzDt4/10;
                    lzDt5=lzDt5/10; lzDt6=lzDt6/10; lzDt7=lzDt7/10; lzDt8=lzDt8/10;

                    //if  ( (lzDt1>2800)||(lzDt2>2800)||(lzDt3>2800)||(lzDt4>2800)|| 
                    //      (lzDt5>2800)||(lzDt6>2800)||(lzDt7>2800)||(lzDt8>2800)  ) 
                    //         { Vo=0; } //запрещенно
                    if ((lzDt1>2800)||(lzDt2>2800)) { Vo=1; } //разрешенно
                    else  { Vo=1; } //разрешенно
                                       
                    digitalWrite(pLaz, LOW); //выкл лазер
                    vTaskDelay(200);
                  }
               else
                 { digitalWrite(pLaz, LOW); //выкл лазер
                   Vo=1; //разрешенно
                   
                   //lzDt1=analogRead(pADClz1);
                   //lzDt2=analogRead(pADClz2);
                   //vTaskDelay(200);
                   //Serial.println(lzDt1);
                   //Serial.println(lzDt2);
                   vTaskDelay(800);
                 } 
                   
               
               //if   ((Vo)&&(Vm)) { VD=1; rgbLn(0); } //разрешенно
               //else { VD=0; rgbLn(1); } //запрещенно
           }
}


//счетчик комманд 
static void cmnum()  
  {  cnum+=1;
     if (cnum>250) { cnum=0; }
  }

 //загрузка в отсек выдачи
 static void loadSlot() 
    {  if (pOV)  
         { OprSt="load slot issue.. .";
                      
           zagruzkaKP();
           vTaskDelay(1500);
           ovopcl(1);  //открыть отсек выдачи
           
           OprSt="load slot issue OK";
         }
    }

 //выгрузка из отсека выдачи
 static void uploadSlot() 
    {  if (pOV)  
         { OprSt="upload slot issue.. .";
                     
           vygruzkaKP();
           vTaskDelay(1500);
           
           OprSt="upload slot issue OK";
         }
    }


//мониторинг активного состояния AcSpec и запуск функций   
static void vSpecOpr(void *pvParameters) {
   for (;;) {  
                     if      ((AcSpec=="vkp")&&(gSem==0)) { gSem=1; vygruzkaKP(); AcSpec="none"; gSem=0; cmnum();  }
                     else if ((AcSpec=="vkz")&&(gSem==0)) { gSem=1; vygruzkaKZ(); AcSpec="none"; gSem=0; cmnum();  }
                     else if ((AcSpec=="zkp")&&(gSem==0)) { gSem=1; zagruzkaKP(); AcSpec="none"; gSem=0; cmnum();  }
                     else if ((AcSpec=="zkz")&&(gSem==0)) { gSem=1; zagruzkaKZ(); AcSpec="none"; gSem=0; cmnum();  }
                     else if ((AcSpec=="openVL")&&(gSem==0))  { gSem=1; openVL();  AcSpec="none"; gSem=0; cmnum();  }
                     else if ((AcSpec=="closeVL")&&(gSem==0)) { gSem=1; closeVL(); AcSpec="none"; gSem=0; cmnum();  }
                     else if ((AcSpec=="vkf")&&(gSem==0)) { gSem=1; unloadingCELL(); AcSpec="none"; gSem=0; cmnum();  }
                     else if ((AcSpec=="zkf")&&(gSem==0)) { gSem=1; loadingCELL();  AcSpec="none";  gSem=0; cmnum();  }
                     else if ((AcSpec=="ovo")&&(gSem==0)) { gSem=1; ovopcl(1); AcSpec="none"; gSem=0; cmnum();   }
                     else if ((AcSpec=="ovc")&&(gSem==0)) { gSem=1; ovopcl(0); AcSpec="none";  gSem=0; cmnum();  }
                     else if ((AcSpec=="CUninst")&&(gSem==0)) { gSem=1; CoverUninst(); AcSpec="none";  gSem=0; cmnum(); }
                     else if ((AcSpec=="CInst")&&(gSem==0))   { gSem=1; CoverInst();   AcSpec="none";  gSem=0; cmnum(); }
                     else if ((AcSpec=="CUP")&&(gSem==0))   { gSem=1; contenerUP();   AcSpec="none";  gSem=0; cmnum(); }
                     else if ((AcSpec=="CDN")&&(gSem==0))   { gSem=1; contenerDN();   AcSpec="none";  gSem=0; cmnum(); }
                     else if ((AcSpec=="loadSlot")&&(gSem==0))   { gSem=1; loadSlot();   AcSpec="none";  gSem=0; cmnum(); }
                     else if ((AcSpec=="uploadSlot")&&(gSem==0)) { gSem=1; uploadSlot(); AcSpec="none";  gSem=0; cmnum(); }
                     vTaskDelay(120);
            }
}