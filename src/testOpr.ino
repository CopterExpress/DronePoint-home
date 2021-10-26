//тест ось Y
static void Ytest() 
        { //{"command":"Ytest"}
          uint8_t i=0; 
          for  (i; i<20; i++)  
               { 
                 
                 Y1Block();
                 Serial.println("Y1zad");
                 vTaskDelay(2000);
                 
                 Y2Block();
                 Serial.println("Y2center");
                 vTaskDelay(1000);
                 gusmotor(1);
                 vTaskDelay(3000);
                 gusmotor(0);
                 vTaskDelay(1000);
                 //X2Block();
                 vTaskDelay(1000);
                 
                 Y3Block();
                 Serial.println("Y3pered");
                 vTaskDelay(2000);

                 Y2Block();
                 Serial.println("Y2center");
                 vTaskDelay(1000);
                 gusmotor(2);
                 vTaskDelay(3000);
                 gusmotor(0);
                 vTaskDelay(1000);
                 //X4Block();
                 vTaskDelay(1000);
               }
        }

//тест ось Z
static void Ztest()  
   { Y2Block();
     uint8_t i=0; 
     for  (i; i<4; i++)
      { vTaskDelay(2000);
        Z0Block();
        Serial.println("___Z0___");
        vTaskDelay(2000);
        Z1Block();
        Serial.println("___Z1___");
        vTaskDelay(2000);
        Z2Block();
        Serial.println("___Z2___");
        vTaskDelay(2000);
        Z3Block();
        Serial.println("___Z3___");
        vTaskDelay(2000);
        Z4Block();
        Serial.println("___Z4___");
        tempmoveZ="nazad";
        vTaskDelay(2000);
        Z3Block();
        Serial.println("___Z3___");
        vTaskDelay(2000);
        Z2Block();
        Serial.println("___Z2___");
        vTaskDelay(2000);
        Z1Block();
        Serial.println("___Z1___");
        vTaskDelay(2000);
        Z0Block();
        Serial.println("___Z0___");
        vTaskDelay(2000);
      }
     /*Z7Block();
     vTaskDelay(2000);
     Z6Block();
     vTaskDelay(2000);
     Z5Block();
     vTaskDelay(2000);
     Z4Block();
     vTaskDelay(2000);
     Z3Block();
     vTaskDelay(2000);
     Z2Block();
     vTaskDelay(2000);
     Z1Block();
     vTaskDelay(2000);
     Z0Block();
     vTaskDelay(2000); */
   }

//тест ZKtest
static void ZKtest()  
   { /*cmbZ3X2();
     vTaskDelay(1000);
     cmbZ3X4();
     vTaskDelay(1000);
     cmbZ1X4();
     vTaskDelay(1000);
     cmbZ1X3();
     vTaskDelay(1000);
     cmbZ2X3();
     vTaskDelay(1000);
     cmbZ2X4();
     zagruzkaKP(); */
   }

//тест PLtest
static void PLtest()  
   { //{"command":"PLtest"}
     uint8_t i=0; 
     for  (i; i<10; i++) 
      { //vTaskDelay(1000);
        //ovopcl(1);  //открыть люк отсек выдачи
        //vTaskDelay(1000);
        //ovopcl(0);  //закрыть люк отсек выдачи
        cmbZ2X0();
        vTaskDelay(1000);
        zagruzkaKP(); //загрузка перед
        vTaskDelay(1000);
        vygruzkaKP(); //выгрузка перед
        vTaskDelay(1000);

      }
   }

//тест соленоида
static void testSol() 
        { static boolean trig;
          ovsolenoid(trig); 
          trig=!trig;
        }

//тест устанока снятие крышкиKR3
static void KR3test()  
   { uint8_t i=0; 
     for  (i; i<10; i++) 
      { /*cmbZ2X4();
        vTaskDelay(1000);
        zagruzkaKP();
        vTaskDelay(2000);
        vygruzkaKP();
        vTaskDelay(2000);
        BBlock();
        vTaskDelay(1000);
        posKR3();
        CoverUninst();
        vTaskDelay(2000);
        CoverInst();
        vTaskDelay(2000);
        CoverUninst();
        vTaskDelay(2000);
        CoverInst();
        vTaskDelay(2000); */
      }
   }

//тест посадочный стол
static void SLtest() 
        { //{"command":"SLtest"}
          uint8_t i=0; 
          for  (i; i<22; i++)  
               { centr();
                 vTaskDelay(4000);
                 uncentr();
                 vTaskDelay(4000);
               }
        }

//тест верхнего люка
static void VLtest() 
        { //{"command":"VLtest"}
          uint8_t i=0; 
          for  (i; i<10; i++)  
               { openVL();
                 vTaskDelay(2000);
                 closeVL();
                 vTaskDelay(2000);
               }
        }



//тест крышек KR0
static void K36test() 
        { //{"command":"K36test"}
          uint8_t i=0; 
          for  (i; i<10; i++)  
               { posKR0();
                 vTaskDelay(1000);
                 CoverInst();
                 vTaskDelay(1000);
                 CoverUninst();
                 vTaskDelay(1000);
                 //cmbZ1X0();
               }  
        }

//тест контейнера
static void tesCont() 
        { //{"command":"tesCont"}
          uint8_t i=0; 
          for  (i; i<10; i++)  
               { posDN();
                 vTaskDelay(1000);
                 contenerUP();
                 vTaskDelay(1000);
                 contenerDN();
                 vTaskDelay(1000);
               }
        }

//сценарий testo1
static void testo1() 
        { //{"command":"testo1"}
          uint8_t i=0; 
          for  (i; i<10; i++)  
               { posKR0();
                 vTaskDelay(1000);
                 CoverInst(); //устанавливаем крышку
                 vTaskDelay(1000);
                
                 cmbZ3X0();
                 vTaskDelay(1000);
                 zagruzkaKP(); //загрузка перед
                 vTaskDelay(1000);
                 vygruzkaKP(); //выгрузка перед
                 vTaskDelay(1000);

                 cmbZ2X0();
                 vTaskDelay(1000);
                 zagruzkaKP(); //загрузка перед
                 vTaskDelay(1000);
                 vygruzkaKP(); //выгрузка перед
                 vTaskDelay(1000); 

                 cmbZ0X0(); //отсек выдачи
                 vTaskDelay(1000);
                 zagruzkaKP(); //загрузка перед
                 vTaskDelay(1000);
                 //vygruzkaKP(); //выгрузка перед
                 //vTaskDelay(1000);

                 ovopcl(1);  //открыть люк отсек выдачи
                 vTaskDelay(6000);
                 ovopcl(0);  //закрыть люк отсек выдачи
                 vTaskDelay(3000);
                 vygruzkaKP(); //выгрузка перед
                 vTaskDelay(1000);

                 posKR0();
                 vTaskDelay(1000);
                 CoverUninst(); //снимаем крышку
                 vTaskDelay(1000);
                 cmbZ0X0();
                 vTaskDelay(1000);
               }
        }


//тест testo2
static void testo2()  
   {  //{"command":"testo2"}
      uint8_t i=0; 
           for  (i; i<10; i++)  
               { //открываем верхний люк снимаем контейнер
                 openVL();
                 vTaskDelay(1000);
                 posDN();
                 vTaskDelay(1000);
                 contenerDN();
                 vTaskDelay(1000);
                 closeVL();  //закрываем верхний люк
                 vTaskDelay(1000);
                 
                 //закрываем верхний люк устанавливаем крышку
                 closeVL();
                 vTaskDelay(1000);
                 posKR0();
                 vTaskDelay(1000);
                 CoverInst(); //устанавливаем крышку
                 vTaskDelay(1000);

                 //переход на Z3 погрузка выгрузка
                 cmbZ3X0();
                 vTaskDelay(1000);
                 zagruzkaKP(); //загрузка перед
                 vTaskDelay(1000);
                 vygruzkaKP(); //выгрузка перед
                 vTaskDelay(1000);

                 //переход на Z2 погрузка выгрузка
                 cmbZ2X0();
                 vTaskDelay(1000);
                 zagruzkaKP(); //загрузка перед
                 vTaskDelay(1000);
                 vygruzkaKP(); //выгрузка перед
                 vTaskDelay(1000);

                 //cmbZ0X0(); //к отсеку выдачи
                 posOV();
                 vTaskDelay(1000);
                 zagruzkaKP(); //загрузка перед
                 vTaskDelay(1000);
                 ovopcl(1);  //открыть люк отсек выдачи
                 vTaskDelay(6000);
                 ovopcl(0);  //закрыть люк отсек выдачи
                 vygruzkaKP(); //выгрузка перед

                 //снимаем крышку открываем люк
                 posKR0();
                 vTaskDelay(1000);
                 CoverUninst(); //снимаем крышку
                 vTaskDelay(1000);
                 openVL();
                 vTaskDelay(1000);

                 //устанавливаем контейнер
                 //закрываем люк
                 //раздвигаем механизм
                 posDN();
                 contenerUP();
                 vTaskDelay(1000);
                 closeVL();  //закрываем верхний люк
                 vTaskDelay(1000);
                 uncentr();
                 vTaskDelay(4000);
                 centr();
                 vTaskDelay(4000);
                 cmnum();
               }
   }


//мониторинг активного состояния AcTest и запуск функций   
static void vTest(void *pvParameters) {
   for (;;) {  
                     if      ((AcTest=="Ztest")&&(gSem==0)) { gSem=1; Ztest(); AcTest="none"; gSem=0; cmnum(); }
                     else if ((AcTest=="Ytest")&&(gSem==0)) { gSem=1; Ytest(); AcTest="none"; gSem=0; cmnum(); }
                     else if ((AcTest=="ZKtest")&&(gSem==0)) { gSem=1; ZKtest(); AcTest="none"; gSem=0; cmnum(); }
                     else if ((AcTest=="SLtest")&&(gSem==0)) { gSem=1; SLtest(); AcTest="none"; gSem=0; cmnum(); }
                     else if ((AcTest=="PLtest")&&(gSem==0)) { gSem=1; PLtest(); AcTest="none"; gSem=0; cmnum(); }
                     else if ((AcTest=="KR3test")&&(gSem==0))  { gSem=1; KR3test(); AcTest="none"; gSem=0; cmnum();  }
                     else if ((AcTest=="lockKR0O")&&(gSem==0)) { gSem=1; lockKR0(1); AcTest="none"; gSem=0; cmnum(); }
                     else if ((AcTest=="lockKR0C")&&(gSem==0)) { gSem=1; lockKR0(0); AcTest="none"; gSem=0; cmnum(); }
                     else if ((AcTest=="VLtest")&&(gSem==0))   { gSem=1; VLtest(); AcTest="none"; gSem=0; cmnum(); }
                     else if ((AcTest=="LockConO")&&(gSem==0)) { gSem=1; servLockCon(0); AcTest="none"; gSem=0; cmnum(); }
                     else if ((AcTest=="LockConC")&&(gSem==0)) { gSem=1; servLockCon(1); AcTest="none"; gSem=0; cmnum(); }
                     else if ((AcTest=="K36test")&&(gSem==0)) { gSem=1; K36test(); AcTest="none"; gSem=0; cmnum(); }
                     else if ((AcTest=="tesCont")&&(gSem==0)) { gSem=1; tesCont(); AcTest="none"; gSem=0; cmnum(); }
                     else if ((AcTest=="testo1")&&(gSem==0))  { gSem=1; testo1(); AcTest="none"; gSem=0; cmnum();  }
                     else if ((AcTest=="testSol")&&(gSem==0)) { gSem=1; testSol(); AcTest="none"; gSem=0; cmnum(); }
                     else if ((AcTest=="testo2")&&(gSem==0))  { gSem=1; testo2(); AcTest="none"; gSem=0; cmnum();  }
                     vTaskDelay(120);
            }
}


//опциональные кнопки
static void vBTcheckTask(void *pvParameters) {
   static boolean tg;
   vTaskDelay(305); 
   for (;;) {  if  (digitalRead(pBT0)==LOW)  
                      { motorZ(1); vTaskDelay(44); motorZ(0); //вперет
                        //servLockCon(0); //открыть замок контейнера
                        //Serial.println("BT0");
                      }
                      
               if  (digitalRead(pBT1)==LOW)  
                      { motorZ(2); vTaskDelay(44); motorZ(0); //назат
                        //servLockCon(1); //закрыть замок контейнера
                        //Serial.println("BT1");
                      }
               if  (digitalRead(pBT2)==LOW)  
                      { //motKR0(1); vTaskDelay(44); motKR0(0); //открыть
                        //motorY(1); vTaskDelay(44); motorY(0);  //вперет к Y3pered
                        //servLockCon(0); //открыть замок контейнера
                        testo2();
                        Serial.println("BT2");
                      }
                      
               if  (digitalRead(pBT3)==LOW)
                      { //motKR0(2); vTaskDelay(44); motKR0(0);   //закрыть
                        //motorY(2); vTaskDelay(44); motorY(0); //назат
                        servLockCon(1); //закрыть замок контейнера
                        Serial.println("BT3");
                      }

               if  (digitalRead(pBT4)==LOW)
                      { //tg=!tg; ovsolenoid(tg); vTaskDelay(320); //соленоид
                        //openVL();
                        //centr();
                        //ovmotor(1); vTaskDelay(44); ovmotor(0); //открыть
                        //gusmotor(1); vTaskDelay(44); gusmotor(0); //выгрузка
                        motorSlY(1); vTaskDelay(44); motorSlY(0); //вперед
                        //vlpmotor(1); vTaskDelay(44); vlpmotor(0); //подъем
                        //vlzmotor(1); vTaskDelay(44); vlzmotor(0); //выдвинуть
                        //motKR0(1); vTaskDelay(44); motKR0(0);   //открыть
                        //servLockCon(0); //открыть замок контейнера
                        Serial.println("BT4");
                      }
               if  (digitalRead(pBT5)==LOW)  
                      { //ovmotor(1); vTaskDelay(44); ovmotor(0); //открыть
                        //closeVL();
                        //uncentr();
                        //ovmotor(2); vTaskDelay(44); ovmotor(0); //закрыть
                        //gusmotor(2); vTaskDelay(44); gusmotor(0); //загрузка
                        //tg=!tg; ovsolenoid(tg); vTaskDelay(320); //соленоид
                        motorSlY(2); vTaskDelay(44); motorSlY(0); //назад
                        //vlpmotor(2); vTaskDelay(44); vlpmotor(0); //спуск
                        //vlzmotor(2); vTaskDelay(44); vlzmotor(0); //задвинуть
                        //motKR0(2); vTaskDelay(44); motKR0(0);   //закрыть
                        //servLockCon(1); //закрыть замок контейнера
                        //VLtest();
                        Serial.println("BT5");
                      }
               
               if  (digitalRead(pBT6)==LOW)  
                      { //ovmotor(2); vTaskDelay(44); ovmotor(0); //закрыть
                        //vlzmotor(1); vTaskDelay(44); vlzmotor(0); //выдвинуть
                        //vlpmotor(1); vTaskDelay(44); vlpmotor(0); //подъем
                        motorSlX(1); vTaskDelay(44); motorSlX(0); //вперет
                        //servLockCon(0); //открыть замок контейнера
                        //zagruzkaKP(); //загрузка перед
                        //lockKR0(0); //закрываем замок
                        //motorSlY(1); vTaskDelay(44); motorSlY(0); //вперет
                        Serial.println("BT6");
                      } 

               if  (digitalRead(pBT7)==LOW)  
                      { //vlpmotor(2); vTaskDelay(44); vlpmotor(0); //спуск
                        //ovmotor(1); vTaskDelay(44); ovmotor(0); //открыть
                        motorSlX(2); vTaskDelay(44); motorSlX(0); //назат
                        //tesCont();
                        //servLockCon(1); //закрыть замок контейнера
                        //vygruzkaKP(); //выгрузка перед
                        //lockKR0(1); //открываем замок
                        //vlzmotor(2); vTaskDelay(44); vlzmotor(0); //задвинуть
                        //motorSlY(2); vTaskDelay(44); motorSlY(0); //назат
                        Serial.println("BT7");
                      }

               if  (digitalRead(pBT8)==LOW)  
                      { //gusmotor(1); vTaskDelay(44); gusmotor(0); //выгрузка
                        //motorSlY(1); vTaskDelay(44); motorSlY(0); //вперет
                        //servLockCon(0); //открыть замок контейнера
                        //motKR0(1); vTaskDelay(44); motKR0(0);   //открыть
                        //zagruzkaKP(); //загрузка перед
                        centr();
                        //vlpmotor(1); vTaskDelay(44); vlpmotor(0); //подъем
                        //ovopcl(1); //открыть
                        Serial.println("BT8");
                      }
               if  (digitalRead(pBT9)==LOW)  
                      { //gusmotor(2); vTaskDelay(44); gusmotor(0); //загрузка
                        //motorSlY(2); vTaskDelay(44); motorSlY(0); //назат
                        //servLockCon(1); //закрыть замок контейнера
                        //motKR0(2); vTaskDelay(44); motKR0(0);   //закрыть
                        //vygruzkaKP(); //выгрузка перед
                        uncentr();
                        //vlpmotor(2); vTaskDelay(44); vlpmotor(0); //спуск
                        //ovopcl(0); //закрыть
                        Serial.println("BT9");
                      } 
            }
}   