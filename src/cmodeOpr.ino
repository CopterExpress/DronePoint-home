 //############## CUSTOM_MODE операция LOAD_DRONE #################
 //vynet iz iacheyki snimet kryshku, zagruzit v dron
 static void cmOpld() 
    {  vTaskDelay(200);
       if ((cmx<1)&&(cmy<4)&&(cmz<4)&&(cmkr<1))
           { //переход на заданную ячейку
             motionCM(cmz,cmx);
             vTaskDelay(250);
             //выгрузка контейнера
             //cmy==3 это перед, cmy==1 это зад
             if      (cmy==3) { vygruzkaKP();  }
             else if (cmy==1) { vygruzkaKZ();  }
             vTaskDelay(250);
             
             //переход на заданную крышку
             posKR0();
             vTaskDelay(250);
             //снятие крышки зарядки
             CoverUninst(); 
             vTaskDelay(250);
                          
             //выход на позицию операций с дроном
             posDN();
             vTaskDelay(250);
             //загружаем контейнер в дрон
             contenerUP();
             vTaskDelay(250);
             
             //закрываем верхний люк
             //раздвигаем механизм позиционирования
             closeVL();
             uncentr();
             vTaskDelay(3000);
           }
       //дрон загружен можно взлетать
       cmx=99;cmy=99;cmz=99;cmkr=99;
       Pld="compl";
    }


//############## CUSTOM_MODE операция UNLOAD_DRONE #################
 //snimet s drona vstavit krishku, zagruzut v iacheyku
 static void cmOpud() 
    {  vTaskDelay(200);
       if ((cmx<1)&&(cmy<4)&&(cmz<4)&&(cmkr<1))
           { //после посадки дрона необходимо
             //выставить механизм центрирования дрона на посадочном столе в центр
             //открыть верхний люк, снять контейнер
             //выход на позицию операций с дроном
             posDN();
             vTaskDelay(250);
             //выгрузка контейнера из дрона
             contenerDN();
             vTaskDelay(250);

             //переход на заданную крышку
             posKR0();
             vTaskDelay(250);
             //установка крышки зарядки
             CoverInst(); 
             vTaskDelay(250);
                          
             //переход на заданную ячейку
             motionCM(cmz,cmx);
             vTaskDelay(250);
             //загрузка контейнера в эту ячейку
             //cmy==3 это перед, cmy==1 это зад
             if      (cmy==3) { zagruzkaKP();  }
             else if (cmy==1) { zagruzkaKZ();  }
             vTaskDelay(250);
           }
      //разгрузка дрона завершена
      cmx=99;cmy=99;cmz=99;cmkr=99;
      Pud="compl";
    }


//############## CUSTOM_MODE операция UNLOAD_TO_USER #################
 //vynet iz iacheyki i vygruzit polzavatelu
 static void cmOpuntu() 
    {  vTaskDelay(200);
       if ((cmx<1)&&(cmy<4)&&(cmz<4))
           {  //переход на заданную ячейку
              motionCM(cmz,cmx);
              vTaskDelay(250);
              //выгрузка контейнера
              //cmy==3 это перед, cmy==1 это зад
              if      (cmy==3) { vygruzkaKP();  }
              else if (cmy==1) { vygruzkaKZ();  }
              vTaskDelay(250);
              
              //выход на позицию отсека выдачи
              posOV();
              vTaskDelay(250);
              //загрузка в отсек выдачи
              zagruzkaKP();
              vTaskDelay(250);
              //открытие люка отсека выдачи
              ovopcl(1);  //OPEN_BOTTOM_HATCH
              vTaskDelay(15000); //время ожидания чтоб забрать посылку
              ovopcl(0);  //CLOSE_BOTTOM_HATCH
              vTaskDelay(250);
           }
      //посылка выдана
      cmx=99;cmy=99;cmz=99;
      Puntu="compl";
    }


//############## CUSTOM_MODE операция GET_FROM_USER #################
 //primet u polzovatelai i vstavit v iacheyku
 static void cmOpgetfu() 
    {  vTaskDelay(200);
       if ((cmx<1)&&(cmy<4)&&(cmz<4))
           {  //выход на позицию отсека выдачи
              posOV();
              vTaskDelay(250);
              //открытие люка отсека выдачи
              ovopcl(1);  //OPEN_BOTTOM_HATCH
              vTaskDelay(15000); //время ожидания чтоб покласть посылку
              ovopcl(0);  //CLOSE_BOTTOM_HATCH
              vTaskDelay(250);
              //выгрузка из отсека выдачи
              vygruzkaKP();
              vTaskDelay(250);
              //переход на заданную ячейку
              motionCM(cmz,cmx);
              vTaskDelay(250);
              //загрузка контейнера в эту ячейку
              //cmy==3 это перед, cmy==1 это зад
              if      (cmy==3) { zagruzkaKP();  }
              else if (cmy==1) { zagruzkaKZ();  }
              vTaskDelay(250); 
           }
      //принял посылку
      cmx=99;cmy=99;cmz=99;
      Pgetfu="compl";
    }

//############## CUSTOM_MODE операция COVER_REMOVAL(CLOSE) #################
//после выполнения CONTAINER_UNLOADING
//контейнер без крышки находится на каретки
//эта команда производит СНЯТИЕ крышки с зарядки.
//далее возвращает контейнер С КРЫШКОЙ в заданную ячейку
//т.е. по внешним параметрам команды
 static void cmOcls() 
    {  vTaskDelay(200);
       if ((!ZUN)&&(!XUN)&&(!YUN)&&(cmx<1)&&(cmy<4)&&(cmz<4)&&(cmkr<1))
           { 
             //переход на заданную крышку
             posKR0();
             vTaskDelay(250);
             //снятие крышки зарядки
             CoverUninst(); 
             vTaskDelay(250);

             //переход на заданную ячейку
             motionCM(cmz,cmx);
             vTaskDelay(250);
             
             //загрузка контейнера в эту ячейку
             //cmy==3 это перед, cmy==1 это зад
             if      (cmy==3) { zagruzkaKP();  }
             else if (cmy==1) { zagruzkaKZ();  }
             vTaskDelay(250);
           }
      //контейнер с крышкой вставлен в ячейку
      cmx=99;cmy=99;cmz=99;cmkr=99;
      Pcls="compl";
    }


//############## CUSTOM_MODE операция COVER_INSTALLATION(OPEN) #################
//после выполнения CONTAINER_UNLOADING
//контейнер с крышкой находится на каретки
//эта команда производит УСТАНОВКУ крышки на зарядку.
//делее возвращает контейнер БЕЗ КРЫШКИ в заданную ячейку
//т.е. по внешним параметрам команды
 static void cmOopn() 
    {  vTaskDelay(200);
       if ((!ZUN)&&(!KR0n)&&(!YUN)&&(cmx<1)&&(cmy<4)&&(cmz<4)&&(cmkr<1))
           { 
             //переход на заданную крышку
             posKR0();
             vTaskDelay(250);
             //установка крышки зарядки
             CoverInst(); 
             vTaskDelay(250);

             //переход на заданную ячейку
             motionCM(cmz,cmx);
             vTaskDelay(250);
             
             //загрузка контейнера в эту ячейку
             //cmy==3 это перед, cmy==1 это зад
             if      (cmy==3) { zagruzkaKP();  }
             else if (cmy==1) { zagruzkaKZ();  }
             vTaskDelay(250);
            }
      //контейнер без крышки вставлен в ячейку
      cmx=99;cmy=99;cmz=99;cmkr=99;
      Popn="compl";
    } 

 //############## CUSTOM_MODE операция CONTAINER_UNLOADING(CHANGING_BATTERY) #################
 //контролируемое перемещение каретки для операций замены АКБ
 //и выгрузка контейнера(контейнер может быть как с крышкой так и без нее)
 static void cmOchan() 
    {  vTaskDelay(200);
       if ((cmx<1)&&(cmy<4)&&(cmz<4)&&(cmkr<1))
           { if       (cmy==1) { stY="1"; }
             else if  (cmy==3) { stY="3"; }
             else  { stY="2"; }
             //переход на заданную ячейку
             motionCM(cmz,cmx);

             //выгрузка из ячейки передней или задней в зависимости от флага stY
             unloadingCELL();  //stY=="3" перед, stY=="1" зад
             vTaskDelay(250);
           }
      //перемещение и выгрузка завершены
      cmx=99;cmy=99;cmz=99;cmkr=99;
      Pchan="compl";
    }   


//движение каретки для операций CUSTOM_MODE
static void motionCM(uint8_t mz,uint8_t mx) 
      {  if       ((mz==0)&&(mx==0))  { cmbZ0X0(); } 
         else if  ((mz==1)&&(mx==0))  { cmbZ1X0(); } 
         else if  ((mz==2)&&(mx==0))  { cmbZ2X0(); } 
         else if  ((mz==3)&&(mx==0))  { cmbZ3X0(); } 
      }

