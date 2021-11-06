//###################### функции комбинированных перемещений ####################
static void cmbZ0X0() {  Y2Block(); Z0Block(); X0Block();  }
static void cmbZ1X0() {  Y2Block(); Z1Block(); X0Block();  }
static void cmbZ2X0() {  Y2Block(); Z2Block(); X0Block();  }
static void cmbZ3X0() {  Y2Block(); Z3Block(); X0Block();  }
static void cmbZ4X0() {  Y2Block(); X0Block(); Z4Block();  }
static void cmbZ5X0() {  Y2Block(); X0Block(); Z5Block();  } 

//########### выход на позиции операций с крышкой ################
static void posKR0()  {  Y2Block(); Z3Block(); X0Block(); closeVL(); centr(); }
//есть две функции CoverUninst() и CoverInst() снятие и установка крышкек 
//номер крышки определяется по концевикам позиции (переменная pKR)

//########### выход на позицию отсека выдачи ######################
static void posOV()  {  Y2Block(); Z0Block(); X0Block(); stY="3";  }
//########### выход на позицию операций с коптером ################
static void posDN()  {  Y2Block(); Z3Block(); X0Block(); openVL(); centr(); }


//мониторинг активного состояния AcCmb и запуск функций 
static void vCmbTask(void *pvParameters) {
   for (;;) {  
                     if      ((AcCmb=="z0x0")&&(gSem==0))   { gSem=1; cmbZ0X0(); AcCmb="none"; gSem=0; cmnum(); }
                     else if ((AcCmb=="z1x0")&&(gSem==0))   { gSem=1; cmbZ1X0(); AcCmb="none"; gSem=0; cmnum(); }
                     else if ((AcCmb=="z2x0")&&(gSem==0))   { gSem=1; cmbZ2X0(); AcCmb="none"; gSem=0; cmnum(); }
                     else if ((AcCmb=="z3x0")&&(gSem==0))   { gSem=1; cmbZ3X0(); AcCmb="none"; gSem=0; cmnum(); }
                     else if ((AcCmb=="z4x0")&&(gSem==0))   { gSem=1; cmbZ4X0(); AcCmb="none"; gSem=0; cmnum(); }
                     else if ((AcCmb=="z5x0")&&(gSem==0))   { gSem=1; cmbZ5X0(); AcCmb="none"; gSem=0; cmnum(); }
                     else if ((AcCmb=="posKR0")&&(gSem==0)) { gSem=1; posKR0();  AcCmb="none"; gSem=0; cmnum(); }
                     else if ((AcCmb=="posOV")&&(gSem==0))  { gSem=1; posOV();   AcCmb="none"; gSem=0; cmnum(); }
                     else if ((AcCmb=="posDN")&&(gSem==0))  { gSem=1; posDN();   AcCmb="none"; gSem=0; cmnum(); }
                     vTaskDelay(120);
            }
}