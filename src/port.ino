// 1. движение и защиты с учетом данных энкодера
// 2. интеграция custom mode операций cmodeOpr.ino
// 3. обработка входных параметров custom mode операций port.ino

//прием данных json с уарт паралленый таск
static void vJsonParsingTask(void *pvParameters) {  
 for (;;) { String input;  
            if (Serial.available()) 
                { memset(json,0,sizeof(json));  //чистим глобальный массив char json[160]
                  input = Serial.readStringUntil('\n'); //помещаем содержимое уарт в переменную стринг input
                  input.toCharArray(json,160); //помещаем input в массив json[160]
                }

             StaticJsonBuffer<160> jsonBuffer;
             JsonObject& root = jsonBuffer.parseObject(json);
             //если парсинг прошел успешно
             if (root.success())
                  {  //помещаем поступившую комманду в переменную String
                     const char* line = root["command"]; 
                     String command = String(line);
                     
                     if (command=="goto_cell")   
                         {   String str_x, str_xkr, str_z, str_y;
                             if ((input.lastIndexOf("params"))!=-1)
                                     {      //выстивить каретку на концевики позиции крышек
                                            //{"command": "goto_cell", "params": {"Xkr": 0, "y": 2, "z": 7}}
                                            //{"command": "goto_cell", "params": {"Xkr": 1, "y": 2, "z": 7}}
                                            if ((input.lastIndexOf("Xkr"))!=-1)
                                              { const char* lz=root["params"]["z"]; str_z=String(lz); 
                                                const char* lx=root["params"]["Xkr"]; str_xkr=String(lx); 
                                                const char* ly=root["params"]["y"]; str_y=String(ly);
                                                stY=str_y;
                                                if      (str_xkr=="0") { command="posKR0"; }
                                                else if (str_xkr=="1") { command="posKR1"; }
                                                else if (str_xkr=="2") { command="posKR2"; }
                                                else if (str_xkr=="3") { command="posKR3"; }
                                                else if (str_xkr=="4") { command="posKR4"; }
                                                else if (str_xkr=="5") { command="posKR5"; }
                                                else if (str_xkr=="6") { command="posKR6"; }
                                                else if (str_xkr=="7") { command="posKR7"; }
                                              } 
                                            else   
                                              {//КОМПЛЕКСНАЯ КОМАНДА 
                                               //{"command": "goto_cell", "params": {"x": 2, "y": 2, "z": 1}}
                                               //{"command": "goto_cell", "params": {"x": 0, "y": 2, "z": 0}}
                                               //{"command": "goto_cell", "params": {"x": 1, "y": 2, "z": 0}}
                                               const char* lz=root["params"]["z"]; str_z=String(lz); 
                                               const char* lx=root["params"]["x"]; str_x=String(lx); 
                                               const char* ly=root["params"]["y"]; str_y=String(ly);
                                               command = "Z" + str_z + "X" + str_x;
                                               //флаг указатель 
                                               //для команд погрузки, выгрузки {"command":"loadingCELL"} {"command":"unloadingCELL"}
                                               //если stY="2" просто перемещение
                                               //если stY="3" при поступлении команд погрузки, выгрузки 
                                               //действия производятся с передней ячейкой
                                               //если stY="1" при поступлении команд погрузки, выгрузки 
                                               //действия производятся с задней ячейкой
                                               stY=str_y;
                                              }
                                            
                                     }
                         } 
                     
                     //не блокирующие комманды перемещения ось Z (не рекомендуется) 
                     if      (command=="Z0")  { AcZ=0; } //{"command":"Z0"}
                     else if (command=="Z1")  { AcZ=1; } //{"command":"Z1"}
                     else if (command=="Z2")  { AcZ=2; } //{"command":"Z2"}
                     else if (command=="Z3")  { AcZ=3; } //{"command":"Z3"}
                     else if (command=="Z4")  { AcZ=4; } //{"command":"Z4"}
                     else if (command=="Z5")  { AcZ=5; } //{"command":"Z5"}
                     //не блокирующие комманды перемещения ось Y (не рекомендуется)
                     else if (command=="Y1zad")     { AcY=1; } //{"command":"Y1zad"}
                     else if (command=="Y2center")  { AcY=2; } //{"command":"Y2center"}
                     else if (command=="Y3pered")   { AcY=3; } //{"command":"Y3pered"}
                    
                     //комбинированные комманды перемещения по осям (рекомендуется)  
                     //контроль последовательности выполнения, контроль столкновения
                     else if (command=="Z0X0")  { AcCmb="z0x0"; } //  {"command":"Z0X0"}
                     else if (command=="Z1X0")  { AcCmb="z1x0"; } //  {"command":"Z1X0"}
                     else if (command=="Z2X0")  { AcCmb="z2x0"; } //  {"command":"Z2X0"}
                     else if (command=="Z3X0")  { AcCmb="z3x0"; } //  {"command":"Z3X0"}
                     else if (command=="Z4X0")  { AcCmb="z4x0"; } //  {"command":"Z4X0"}
                     else if (command=="Z5X0")  { AcCmb="z5x0"; } //  {"command":"Z5X0"}
                                          
                     //позиция крышки
                     else if (command=="posKR0")  { AcCmb="posKR0"; } //  {"command":"posKR0"}
                     //позиция отсека выдачи
                     else if (command=="posOV")   { AcCmb="posOV"; }  //  {"command":"posOV"}
                     //позиция операций с дроном
                     else if (command=="posDN")   { AcCmb="posDN"; }  //  {"command":"posDN"}
                     
                     //################### команды специальных операций #############################################
                     //{"command":"loadingCELL"} загрузка в ячейку переднею или заднею зависит от флага stY
                     //команды выполняется если StatusKkr=="loaded" 
                     else if (command=="loadingCELL")  { AcSpec="zkf"; }  
                     //{"command":"unloadingCELL"} выгрузка  из ячейки передней или задней зависит от флага stY
                     //команды выполняется если StatusKkr=="free" 
                     else if (command=="unloadingCELL")  { AcSpec="vkf"; }
                     else if (command=="ZKP")  { AcSpec="zkp"; } //{"command":"ZKP"} загрузка в переднею ячейку 
                     else if (command=="VKP")  { AcSpec="vkp"; } //{"command":"VKP"} выгрузка из передней ячейки
                     else if (command=="ZKZ")  { AcSpec="zkz"; } //{"command":"ZKZ"} загрузка в заднею ячейку 
                     else if (command=="VKZ")  { AcSpec="vkz"; } //{"command":"VKZ"} выгрузка из задней ячейки
                     else if (command=="top_hatch_state_install")  { AcSpec="closeVL"; } //{"command":"top_hatch_state_install"} установка верхнего люка команда выполняется если ((!KZ7)||(!KZ8)) 
                     else if (command=="top_hatch_state_removing") { AcSpec="openVL";  } //{"command":"top_hatch_state_removing"} снятие верхнего люка команда выполняется если ((!KZ7)||(!KZ8))
                     else if (command=="lock_top_hatch_lock")    { AcSpec="closeVL"; } //{"command":"lock_top_hatch_lock"} установка верхнего люка команда выполняется если ((!KZ7)||(!KZ8)) 
                     else if (command=="release_top_hatch_lock") { AcSpec="openVL";  } //{"command":"release_top_hatch_lock"} снятие верхнего люка команда выполняется если ((!KZ7)||(!KZ8))
                     else if (command=="hatch_delivery_open")  { AcSpec="ovo"; }  //{"command":"hatch_delivery_open"} открыть люк отсека выдачи
                     else if (command=="hatch_delivery_close") { AcSpec="ovc"; }  //{"command":"hatch_delivery_close"} закрыть люк отсека выдачи 
                     else if (command=="charging_cover_removing") { AcSpec="CUninst"; } //{"command":"charging_cover_removing"} 
                     else if (command=="charging_cover_install")  { AcSpec="CInst"; }   //{"command":"charging_cover_install"}
                     else if (command=="unloading_charging_cell") { AcSpec="CUninst"; } //{"command":"unloading_charging_cell"} 
                     else if (command=="loading_charging_cell")   { AcSpec="CInst";   } //{"command":"loading_charging_cell"}
                     else if (command=="chargingKR1_cover_removing") { AcSpec="CUninst"; }  //{"command":"chargingKR1_cover_removing"} 
                     else if (command=="chargingKR1_cover_install")  { AcSpec="CInst";   }  //{"command":"chargingKR1_cover_install"}
                     else if (command=="container_insert")    { AcSpec="CUP"; } //{"command":"container_insert"}
                     else if (command=="container_removing")  { AcSpec="CDN"; } //{"command":"container_removing"}
                     else if (command=="load_slot")    { AcSpec="loadSlot"; }   //{"command":"load_slot"} загрузка в отсек выдачи
                     else if (command=="upload_slot")  { AcSpec="uploadSlot"; } //{"command":"upload_slot"} выгрузка из отсека выдачи
                     //############## CUSTOM_MODE операции ##################################
                     else if (command=="cmOpld")     { AcSpec="cmOpld";     }  //{"command":"cmOpld"}
                     else if (command=="cmOpud")     { AcSpec="cmOpud";     }  //{"command":"cmOpud"}
                     else if (command=="cmOpuntu")   { AcSpec="cmOpuntu";   }  //{"command":"cmOpuntu"}
                     else if (command=="cmOpgetfu")  { AcSpec="cmOpgetfu";  }  //{"command":"cmOpgetfu"}
                     else if (command=="cmOcls")     { AcSpec="cmOcls";     }  //{"command":"cmOcls"}
                     else if (command=="cmOopn")     { AcSpec="cmOopn";     }  //{"command":"cmOopn"}
                     else if (command=="cmOchan")    { AcSpec="cmOchan";    }  //{"command":"cmOchan"}
                  
                     //########### комманды тестирования отдельных элементов и сценариев ########################
                     else if (command=="Ztest")    { AcTest="Ztest"; } //{"command":"Ztest"} тест оси Z
                     else if (command=="Ytest")    { AcTest="Ytest"; } //{"command":"Ytest"} тест оси Y
                     else if (command=="ABlock")   { AcTest="ABlock"; } //{"command":"ABlock"} к платформе А
                     else if (command=="BBlock")   { AcTest="BBlock"; } //{"command":"BBlock"} к платформе B
                     else if (command=="ZKtest")   { AcTest="ZKtest"; } //{"command":"ZKtest"} ZKtest
                     else if (command=="PLtest")   { AcTest="PLtest"; } //{"command":"PLtest"} тест платформы крышек
                     else if (command=="KR3test")  { AcTest="KR3test"; } //{"command":"KR3test"} тест крышки KR3
                     else if (command=="VLtest")   { AcTest="VLtest";  } //{"command":"VLtest"} тест верхнего люка
                     else if (command=="lockKR0O") { AcTest="lockKR0O"; } //{"command":"lockKR0O"} открыть замок KR0
                     else if (command=="lockKR0C") { AcTest="lockKR0C"; } //{"command":"lockKR0C"} закрыть замок KR0
                     else if (command=="LockConO") { AcTest="LockConO"; } //{"command":"LockConO"} открыть замок контейнера
                     else if (command=="LockConC") { AcTest="LockConC"; } //{"command":"LockConC"} закрыть замок контейнера
                     else if (command=="testSol")  { AcTest="testSol"; }  //{"command":"testSol"} тест соленоида отсека выдачи
                     else if (command=="K36test")  { AcTest="K36test"; }  //{"command":"K36test"} тест крышек KR3 и KR6
                     else if (command=="tesCont")  { AcTest="tesCont"; }  //{"command":"tesCont"} тест контейнера
                     else if (command=="testo1")   { AcTest="testo1"; }   //{"command":"testo1"} тест testo1
                     else if (command=="testo2")   { AcTest="testo2"; }   //{"command":"testo2"} тест testo2
                     else if (command=="inPacts")    { AcTest="inPacts";  }  // {"command":"inPacts"}  тест приема
                     else if (command=="outPacts")   { AcTest="outPacts"; }  // {"command":"outPacts"} тест получения
                     else if (command=="compTest")   { AcTest="compTest"; }  // {"command":"compTest"} комплексный тест
                     else if (command=="SLtest")     { AcTest="SLtest";   }  // {"command":"SLtest"} тест посадочного стола
                     else if (command=="NKcon")      { AcTest="NKcon";    }  // {"command":"NKcon"}
                     else if (command=="levcon")     { AcTest="levcon";   }  // {"command":"levcon"}
                     else if (command=="Xtest")      { AcTest="Xtest";    }  // {"command":"Xtest"}
                     else if (command=="OVTest")     { AcTest="OVTest";   }  // {"command":"OVTest"}
                     else if (command=="getFU")      { AcTest="getFU";    }  // {"command":"getFU"} 
                     else if (command=="unloadTU")   { AcTest="unloadTU"; }  // {"command":"unloadTU"} 

                     //команды посадочному столу
                     //{"command":"centr"} {"command":"uncentr"} 
                     else if (command=="centr")     { AcStol="centr"; }
                     else if (command=="uncentr")   { AcStol="uncentr"; } 
                     
                     //{"MOT":"on"} вкл или выключить мотор
                     //if ((input.lastIndexOf("MOT"))!=-1) { const char* lmot=root["MOT"]; MOT=String(lmot); }
                     //{"SERV":"on"} вкл или выключить сервы
                     //if ((input.lastIndexOf("SERV"))!=-1) { const char* lserv=root["SERV"]; SERV=String(lserv); }
                     //интервал мотора {"Mval":4000}
                     //if ((input.lastIndexOf("Mval"))!=-1) { Mval=root["Mval"]; }
                   }
          } 
}   


//отсылки данных json по уарт паралленый таск
static void vSendTask(void *pvParameters) {
    vTaskDelay(1200);
    for (;;) {      StaticJsonBuffer<250> jsonBuffer1;
                    JsonObject& root1 = jsonBuffer1.createObject(); 
                    if (nSen==0)
                        { root1["StatusX"] = StatusX;
                          root1["StatusZ"] = StatusZ;
                          root1["StatusY"] = StatusY;
                          root1["statelock"] = StatusZuh; //состояние механизма захвата
                          root1["top_hatch_state"] = StatusVL; //верхний люк
                          root1["bottom_hatch_state"] = StatusOV; //люк осека выдачи
                          root1["count"] = cnum; //счетчик комманд
                          nSen+=1;
                        }
                     else if (nSen==1)
                        { root1["stateOperation"] = OprSt; //статусы операций
                          root1["cover_position"] = pKR; //позиция крышки
                          root1["YFlag"] = stY; //флаг указатель YFlag для команд погрузки, выгрузки
                                                // 3=передняя ячейка 1=задняя
                          root1["platform_payload_state"] = StatusKkr; //концевики каретки
                          root1["count"] = cnum; //счетчик комманд
                          root1["scales_state"] = outkg; // данные с весов 
                          root1["chargingKR0_availability"] = VBkr0; // наличие и напряжение крышки KR0                     
                          nSen+=1;
                        }
                     else if (nSen==2)
                        { root1["pOV"] = sOV; // позиция отсека выдачи sOV="none" sOV="ov"
                          root1["pDN"] = sDN; // поция операций с дроном sDN="none" sDN="dn"
                          root1["Pld"] = Pld; // завершение операции LOAD_DRONE
                          root1["Pud"] = Pud; // завершение операции UNLOAD_DRONE
                          root1["Puntu"]  = Puntu; // завершение операции UNLOAD_TO_USER
                          root1["Pgetfu"] = Pgetfu; // завершение операции GET_FROM_USER
                          root1["Pcls"]  = Pcls; // завершение операции CLOSE
                          root1["Popn"]  = Popn; // завершение операции OPEN
                          root1["Pchan"]  = Pchan; // завершение операции CHANGING_BATTERY
                          if (Pld=="compl")    { Pld="none";    }
                          if (Pud=="compl")    { Pud="none";    }
                          if (Puntu=="compl")  { Puntu="none";  }
                          if (Pgetfu=="compl") { Pgetfu="none"; }
                          if (Pcls=="compl")   { Pcls="none";   }
                          if (Popn=="compl")   { Popn="none";   }
                          if (Pchan=="compl")  { Pchan="none";  }
                          nSen+=1;
                        }
                     else if (nSen==3)
                        { //vTaskDelay(130);
                          //debugout4021();
                          //vTaskDelay(130);
                          debugoutKONS();
                          //dlMeter();
                          //debugoutDIR();
                          nSen+=1; 
                        }                     
                     
                     //if (nSen<3)   
                     root1.printTo(Serial);
                     Serial.print('\n');
                       
                                          
                     vTaskDelay(750);
                     if (nSen>3) { nSen=0; }
             }
}

//дебаг формируем строки вывода концевиков 
static void debugoutKONS() 
      {  char skzout[65];
         char skxout[40];
         char skyout[44];
         char skstol[35];
         //char skntout[35];
         char sVlout[40];
         //char sPlout[35];
         char sOVout[35];
         char sdata2[40];
         //char sZ0[30];
         //char sZ1[30];
         //char sZ2[30];
         //char sZ3[30];
                 
         sprintf(skzout, "KZout: KZ0=%i KZ1=%i KZ2=%i KZ3=%i KZ4=%i KZ5=%i KX0=%i enzPos=%i" , KZ0,KZ1,KZ2,KZ3,KZ4,KZ5,KX0,enzPos);
         Serial.println(skzout);

         //sprintf(sZ0, "Z0level: Z0Y3=%i Z0Y1=%i", Z0Y3,Z0Y1);
         //Serial.println(sZ0);
         //sprintf(sZ1, "Z1level: Z1Y1=%i", Z1Y1);
         //Serial.println(sZ1);
         //sprintf(sZ2, "Z0level: Z2Y3=%i Z2Y1=%i", Z2Y3,Z2Y1);
         //Serial.println(sZ2);
         //sprintf(sZ3, "Z0level: Z3Y3=%i Z3Y1=%i", Z3Y3,Z3Y1);
         //Serial.println(sZ3);
        
         sprintf(skyout, "KYout: KY1=%i KY2=%i KY3=%i cnt1=%i cnt2=%i" , KY1,KY2,KY3,cnt1,cnt2);
         Serial.println(skyout);
         //vTaskDelay(130);
         sprintf(skstol, "KStol: KXun=%i KYun=%i KXce=%i KYce=%i" , KXun,KYun,KXce,KYce);
         Serial.println(skstol);
         // vTaskDelay(130);

         //sprintf(sPlout, "Plout: plA=%i plB=%i" , plA,plB);
         //Serial.println(sPlout);
         //vTaskDelay(130);

         sprintf(sOVout, "OVout: ovv=%i ovn=%i" , ovv,ovn);
         Serial.println(sOVout);
         //vTaskDelay(130);

         sprintf(sdata2, "KRysh: KR0n=%i KR0c=%i KR0o=%i" , KR0n,KR0c,KR0o);
         Serial.println(sdata2);
         //vTaskDelay(130);


         //sprintf(vdKr3, "VBAT=%.2f", ADCkr3);
         //Serial.println(vdKr3);
         //vTaskDelay(130);


         
         //sprintf(skntout, "kntout: cnt1=%i cnt2=%i" , cnt1,cnt2);
         //Serial.println(skntout);
         //vTaskDelay(130);
         
         sprintf(sVlout, "Vlout: Vlv=%i Vln=%i Vld=%i Vlz=%i" , Vlv,Vln,Vld,Vlz);
         Serial.println(sVlout);
         //vTaskDelay(130); 

         Serial.print('\n'); 
      }

/*//дебаг формируем строки вывода моторных директив 
static void debugoutDIR() 
      {  char sdirxout[65];
         const char *StepMX2=StepMX.c_str();
                  
         sprintf(sdirxout, "DIRXout: AcX=%i StepMX=%s" , AcX,StepMX2);
         Serial.println(sdirxout);
         vTaskDelay(130);
                  
         Serial.print('\n');
      } */

