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
             vTaskDelay(6000);
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
              vTaskDelay(12000); //время ожидания чтоб забрать посылку
              ovopcl(0);  //CLOSE_BOTTOM_HATCH
              vTaskDelay(250);

     /*       //далее вынимаем пустой контейнер и возвращаем в туже ячейку
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
              vTaskDelay(250);         */
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
              vTaskDelay(12000); //время ожидания чтоб покласть посылку
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

//############## CUSTOM_MODE операция CLOSE #################
//snimet s drona vstavit krishku, vydast polsovatelu
 static void cmOcls() 
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

             //выход на позицию отсека выдачи
             posOV();
             vTaskDelay(250);
             //загрузка в отсек выдачи
             zagruzkaKP();
             vTaskDelay(250);
             //открытие люка отсека выдачи
             ovopcl(1);  //OPEN_BOTTOM_HATCH
             vTaskDelay(12000); //время ожидания чтоб забрать посылку
             ovopcl(0);  //CLOSE_BOTTOM_HATCH
             vTaskDelay(250);
           }
      //разгрузка дрона завершена
      cmx=99;cmy=99;cmz=99;cmkr=99;
      Pcls="compl";
    }


//############## CUSTOM_MODE операция OPEN #################
 //vozmet u polsovatelia, snimet kryshku zagruzit v dron
 static void cmOopn() 
    {  vTaskDelay(200);
       if ((cmx<1)&&(cmy<4)&&(cmz<4)&&(cmkr<1))
           { //выход на позицию отсека выдачи
              posOV();
              vTaskDelay(250);
              //открытие люка отсека выдачи
              ovopcl(1);  //OPEN_BOTTOM_HATCH
              vTaskDelay(12000); //время ожидания чтоб покласть посылку
              ovopcl(0);  //CLOSE_BOTTOM_HATCH
              vTaskDelay(250);
              //выгрузка из отсека выдачи
              vygruzkaKP();
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
              vTaskDelay(6000);
           }
      //дрон загружен можно взлетать
      cmx=99;cmy=99;cmz=99;cmkr=99;
      Popn="compl";
    } 

//############## CUSTOM_MODE операция CHANGING_BATTERY #################
 //zamena AKB drona
 static void cmOchan() 
    {  vTaskDelay(200);
       if ((cmx<1)&&(cmy<4)&&(cmz<4)&&(cmkr<1))
           { 
             motionCM(cmz,cmx);
           }
      //замена АКБ завершена
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

/*//таск открытие верхнего люка для операций CUSTOM_MODE
static void vCmOpenVL(void *pvParameters) {
   vTaskDelay(295);
   for (;;) { if (TOVL==true)
                   { centr();
                     openVL();
                     TOVL=false;   }
              vTaskDelay(200);
            }
}

//таск закрытие верхнего люка для операций CUSTOM_MODE
static void vCmCloseVL(void *pvParameters) {
   vTaskDelay(329);
   for (;;) { if (TCVL==true)
               { closeVL();
                 TCVL=false;  }
              vTaskDelay(200);
            }
} */