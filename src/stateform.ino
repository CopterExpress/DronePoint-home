//формируем статусы
static void vStateForm(void *pvParameters) {
   for (;;) {  //проверка состояния концевиков оси X, устанавливаем соответствующие статусы
               if      (KX0) { StatusX="X0";  }  
               //проверка состояния концевиков оси Z, устанавливаем соответствующие статусы
               if      (KZ0) { StatusZ="Z0";  }  
               else if (KZ1) { StatusZ="Z1";  }
               else if (KZ2) { StatusZ="Z2";  }
               else if (KZ3) { StatusZ="Z3";  }
               else if (KZ4) { StatusZ="Z4";  }
               else if (KZ5) { StatusZ="Z5";  } 
               if ((ZUN)&&(AcZ==-1)) { StatusZ="unknown";  }
               //проверка состояние концевиков оси Y, устанавливаем соответствующие статусы
               if      (KY1) { StatusY="Y1zad";    }
               else if (KY2) { StatusY="Y2center"; }
               else if (KY3) { StatusY="Y3pered";  } 
               if ((YUN)&&(AcY==-1)) { StatusY="unknown";  } 
               //проверка состояние концевиков посадочного стола, устанавливаем соответствующие статусы
               if ((KXun)&&(KYun)) { StatusZuh="uncenter"; }
               else if ((KXce)&&(KYce)) { StatusZuh="center"; }
               else { if (AcStol=="none")  { StatusZuh="unknown"; }}
                //проверяем состояние концевиков каретки, устанавливаем соостветствующие статусы 
               if ((cnt1)||(cnt2)) { StatusKkr="loaded";  }
               else  { StatusKkr="free"; } 
               //проверяем состояние концевиков верхнего люка, устанавливаем соостветствующие статусы 
               if      ((Vln)&&(Vlz))  { StatusVL="open";   }
               else if ((Vlv)&&(Vld))  { StatusVL="close";  } 
               else  { StatusVL="unknown"; } 
               //проверяем состояние концевиков люка отсека выдачи, устанавливаем соостветствующие статусы 
               if      (ovv) { StatusOV="open";   }
               else if (ovn) { StatusOV="close";  } 
               else  { StatusOV="unknown"; }
               //проверяем состояние концевиков, верхнего люка и осей, устанавливаем статус позиции крышки 
               if      ((KY2)&&(KZ3)&&(KX0)&&(Vlv)&&(Vld))  { pKR="KR0"; }
               else    { pKR="unknown"; }
               //######### БЛОКИРОВКА ОПЕРАЦИЙ ПОГРУЗКИ ##################
               //по умолчанию разрешенно для передних BPP=1 и задних BPZ=1
               BPP=1; BPZ=1;
               //если есть контейнер в передних
               if       ((KZ0)&&(KX0)&&(Z0Y3)) { BPP=0; }
               else if  ((KZ2)&&(KX0)&&(Z2Y3)) { BPP=0; }
               else if  ((KZ3)&&(KX0)&&(Z3Y3)) { BPP=0; }
               //если есть контейнер в задних
               if       ((KZ0)&&(KX0)&&(Z0Y1)) { BPZ=0; }
               else if  ((KZ1)&&(KX0)&&(Z1Y1)) { BPZ=0; }
               else if  ((KZ2)&&(KX0)&&(Z2Y1)) { BPZ=0; }
               else if  ((KZ3)&&(KX0)&&(Z3Y1)) { BPZ=0; }
               //проверяем позиции отсутствующих передних ячеек
               if  ((KZ1)&&(KX0)) { Bvz=0; }
               else    { Bvz=1; }
               //если флаг отсутствия задней стенки true
               if (zSten)  //блокируем операции погрузки-выгрузки для 
                  {        //следующих задних ячеек
                     if       ((KZ0)&&(KX0)) { Bpz=0; }
                     else if  ((KZ1)&&(KX0)) { Bpz=0; }
                     else if  ((KZ2)&&(KX0)) { Bpz=0; }
                     else if  ((KZ3)&&(KX0)) { Bpz=0; }
                  }
               else { Bpz=1; }
               //позиция отсека выдачи
               if ((KY2)&&(KZ0)&&(KX0))  { pOV=1; }
               else  { pOV=0; }
               //позиция операций с дроном
               if ((KY2)&&(KZ3)&&(KX0))  { pDN=1; }
               else  { pDN=0; }
            }  
}  
