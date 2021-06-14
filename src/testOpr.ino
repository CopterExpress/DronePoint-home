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
   { /*cmbZ0X2();
     vTaskDelay(2000);
     Z0Block();
     vTaskDelay(2000);
     Z1Block();
     vTaskDelay(2000);
     Z2Block();
     vTaskDelay(2000);
     Z3Block();
     vTaskDelay(2000);
     Z4Block();
     vTaskDelay(2000);
     Z5Block();
     vTaskDelay(2000);
     Z6Block();
     vTaskDelay(2000);
     Z7Block();
     vTaskDelay(2000);
     Z8Block();
     vTaskDelay(2000);
     Z7Block();
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
   { uint8_t i=0; 
     for  (i; i<20; i++) 
      { //X2Block();
        vTaskDelay(1000);
        //ABlock();
        vTaskDelay(2000);
        //X4Block();
        vTaskDelay(1000);
        //X2Block();
        vTaskDelay(1000);
        //BBlock();
        vTaskDelay(2000);
        //X4Block();
        vTaskDelay(1000); }
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

//тест крышек KR3 и KR6
static void K36test() 
        { /*//{"command":"K36test"}
          uint8_t i=0; 
          for  (i; i<10; i++)  
               { posKR3();
                 vTaskDelay(1000);
                 CoverInst();
                 vTaskDelay(1000);
                 CoverUninst();
                 vTaskDelay(1000);
                 posKR6();
                 vTaskDelay(1000);
                 CoverInst();
                 vTaskDelay(1000);
                 CoverUninst();
                 vTaskDelay(1000);
               }  */
        }

//тест контейнера
static void tesCont() 
        { uint8_t i=0; 
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
        { uint8_t i=0; 
          for  (i; i<10; i++)  
               { /*posDN();
                 vTaskDelay(1000);
                 openVL();
                 vTaskDelay(1000);
                 contenerDN();
                 vTaskDelay(1000);
                 posKR3();
                 vTaskDelay(1000);
                 CoverInst();
                 vTaskDelay(1000);
                 cmbZ6X4();
                 vTaskDelay(1000);
                 zagruzkaKP();
                 vTaskDelay(3000);
                 vygruzkaKP();
                 vTaskDelay(1000);
                 cmbZ5X4();
                 vTaskDelay(1000);
                 zagruzkaKP();
                 vTaskDelay(3000);
                 vygruzkaKP();
                 vTaskDelay(1000);
                 posOV();
                 zagruzkaKP();
                 vTaskDelay(1000);
                 ovopcl(1);
                 vTaskDelay(10000);
                 ovopcl(0);
                 vTaskDelay(1000);
                 vygruzkaKP();
                 vTaskDelay(1000);
                 posKR3();
                 vTaskDelay(1000);
                 CoverUninst();
                 vTaskDelay(1000);
                 posDN();
                 contenerUP();
                 vTaskDelay(1000);
                 closeVL();
                 vTaskDelay(1000);  */
               }
        }

//тест testo2
static void testo2()  
   { uint8_t i=0; 
     for  (i; i<10; i++) 
      { /* openVL();
         vTaskDelay(1000);
         contenerDN();
         vTaskDelay(1000);
         cmbZ6X0();
         vTaskDelay(1000);
         cmbZ6X2();
         vTaskDelay(1000);
         contenerUP();
         vTaskDelay(2000);
         contenerDN();
         vTaskDelay(1000);
         cmbZ6X4();
         vTaskDelay(1000);
         cmbZ6X2();
         vTaskDelay(1000);
         contenerUP();
         vTaskDelay(1000);  */
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
   vTaskDelay(305); 
   for (;;) {  if  (digitalRead(pBT0)==LOW)  
                      { motorZ(1);//вперет
                        vTaskDelay(44);
                        Serial.println("BT0");
                        motorZ(0);
                      }
               if  (digitalRead(pBT1)==LOW)  
                      { motorZ(2);//назат
                        vTaskDelay(44);
                        Serial.println("BT1");
                        motorZ(0);
                      }
               if  (digitalRead(pBT2)==LOW)  
                      { motorY(1);//вперет к Y3pered
                        vTaskDelay(44);
                        //Serial.println("BT2");
                        motorY(0);
                      }
                      
               if  (digitalRead(pBT3)==LOW)
                      { motorY(2);//назат к Y1zad
                        vTaskDelay(44);
                        //Serial.println("BT3");
                        motorY(0);
                      }

               if  (digitalRead(pBT4)==LOW)
                      { ovmotor(1); vTaskDelay(44); ovmotor(0); //открыть
                        //gusmotor(1); vTaskDelay(44); gusmotor(0); //выгрузка
                        //vlpmotor(1); vTaskDelay(44); vlpmotor(0); //подъем
                        //vlzmotor(1); vTaskDelay(44); vlzmotor(0); //выдвинуть
                        //motKR0(1); vTaskDelay(44); motKR0(0);   //открыть
                        //servLockCon(0); //открыть замок контейнера
                      }
               if  (digitalRead(pBT5)==LOW)  
                      { ovmotor(2); vTaskDelay(44); ovmotor(0); //закрыть
                        //gusmotor(2); vTaskDelay(44); gusmotor(0); //загрузка
                        //vlpmotor(2); vTaskDelay(44); vlpmotor(0); //спуск
                        //vlzmotor(2); vTaskDelay(44); vlzmotor(0); //задвинуть
                        //motKR0(2); vTaskDelay(44); motKR0(0);   //закрыть
                        //servLockCon(1); //закрыть замок контейнера
                      }
               
               if  (digitalRead(pBT6)==LOW)  
                      { motorSlX(1); vTaskDelay(44); motorSlX(0); //вперет
                        //motorSlY(1); vTaskDelay(44); motorSlY(0); //вперет
                      } 

               if  (digitalRead(pBT7)==LOW)  
                      { motorSlX(2); vTaskDelay(44); motorSlX(0); //назат
                        //motorSlY(2); vTaskDelay(44); motorSlY(0); //назат
                      }

               if  (digitalRead(pBT8)==LOW)  
                      { //Serial.println("BT8");

                      }
               if  (digitalRead(pBT9)==LOW)  
                      { //Serial.println("BT9");

                      } 
            }
}   