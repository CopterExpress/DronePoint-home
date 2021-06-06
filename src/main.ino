#include <Arduino.h>
#include <MapleFreeRTOS900.h>
#include <ArduinoJson.h> 
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <HX711.h>

static void vJsonParsingTask(void *pvParameters);
static void vSendTask(void *pvParameters);
static void vKZcheckTask(void *pvParameters);
static void vKYcheckTask(void *pvParameters);
static void vKSpecTask(void *pvParameters);
static void vKStolTask(void *pvParameters);
static void vCmbTask(void *pvParameters);
static void vLaserTask(void *pvParameters);
static void Zjump(int direct);
static void vTest(void *pvParameters); 
static void vSpecOpr(void *pvParameters); 
static void vShagMotTask(void *pvParameters);
static void vVsensTask(void *pvParameters);
static void vStateForm(void *pvParameters);
static void vKCellTask(void *pvParameters);
static void vScaleTask(void *pvParameters);
static void miShag(byte direct,int shagSpeed);
static void gusmotor(byte direct);
static void ovmotor(byte direct);
static void vlpmotor(byte direct);
static void vlzmotor(byte direct);
static void vlps(boolean direct);
static void vlvz(boolean direct);
static void ovsolenoid(boolean direct);
static void ovopcl(boolean direct);
static void servLockCon(boolean direct);
static void encoderZ();
static void CoverInst();
static void CoverUninst();
static void openVL();
static void closeVL();
static void contenerUP();
static void contenerDN();
static void loadSlot();
static void uploadSlot();
static void motKR0(byte direct);
static void lockKR0(boolean direct);
static void rgbLn(boolean direct);
static void posKR0();
static void posOV();
static void posDN();
static void vAxis_Sl(void *pvParameters);
static void vBTcheckTask(void *pvParameters);
static void X0Block();
static void Z0Block();
static void Z1Block();
static void Z2Block();
static void Z3Block();
static void Z4Block();
static void Z5Block();
static void cmbZ0X0();
static void cmbZ1X0();
static void cmbZ2X0();
static void cmbZ3X0();
static void cmbZ4X0();
static void cmbZ5X0();
static void cmnum();
static void Ztest();
static void Ytest();
static void Zcorrect();
static void Y1Block();
static void Y2Block();
static void Y3Block();
static void motorZ(byte direct);
static void motorY(byte direct);
static void centr();
static void uncentr();
static void motorSlX(byte direct);
static void motorSlY(byte direct);
static void vAxis_Z(void *pvParameters);
static void vAxis_Y(void *pvParameters);
static void vygruzkaKP();
static void zagruzkaKP();
static void vygruzkaKZ();
static void zagruzkaKZ();
static void unloadingCELL();
static void loadingCELL();
static void micdelay(int md);
static void debugoutKONS(); 
 


//##################### ОСЬ Z #################
//назначаем пины концевиков оси Z 
#define pKZ0 93  // PF13
#define pKZ1 91  // PF11
#define pKZ2 17  // PB1
#define pKZ3 37  // PC5
#define pKZ4 7   // PA7
#define pKZ5 5   // PA5
//состояние концевиков по Z
//для ячеек этих уровней начинать движение с Z
boolean KZ0=0;   //отсек выдачи здесь Z0X0Y3 перед
boolean KZ1=0;  
boolean KZ2=0;
boolean KZ3=0;   //вставлять крышки отсюда с Z3(в Z4 когда верхний люк закрыт) одна зарядка KR0   
//для ячеек этих уровней начинать движение с X 
boolean KZ4=0;   //зарядка KR0 на внутреней стороне верхнего люка(крышка вставлена)
boolean KZ5=0;   //операции с дроном, контейнер установка снятие(2сервы замок)
boolean ZUN;
//шимы мотора Z
#define zpwm1 24 // PB8
#define zpwm2 25 // PB9
//концевики наличия контейнера в ячейках
#define pKNZ0X0Y3 23    //Z0Y3 PB7 отсек выдачи
#define pKNZ0X0Y1 22    //Z0Y1 PB6
#define pKNZ1X0Y1 21    //Z1Y1 PB5
#define pKNZ2X0Y3 111   //Z2Y3 PG15
#define pKNZ2X0Y1 110   //Z2Y1 PG14
#define pKNZ3X0Y3 109   //Z3Y3 PG13
#define pKNZ3X0Y1 108   //Z3Y1 PG12
//состояние концевиков наличия контейнера
boolean Z0Y3=0,Z0Y1=0,Z1Y1=0,Z2Y3=0,Z2Y1=0,Z3Y3=0,Z3Y1=0;
//флаг отсутствия передней ячейки
boolean Bvz=1;
//флаг отсутствия задней стенки
boolean zSten=1; //1 ее нет
boolean Bpz=1;
//флаги блокировки операций погрузки
//при наличии контейнера в ячейки
boolean BPP=1; //перед
boolean BPZ=1; //зад
//активное состояние оси Z
int8_t AcZ=-1;
//статус оси Z
String StatusZ="unknown";
//предыдущие состояния
String tempmoveZ="vpered";
String meZ="unknown";
String AcSpec="none";
String AcTest="none";
String AcCmb="none";
//статусы операций
String OprSt="none";   

//##################### ОСЬ X #################
//состояние концевиков по X
boolean KX0=0,XUN;   
//статус оси X
String StatusX="unknown";


//##################### ОСЬ Y #################
//назначаем пины концевиков оси Y 
#define pKY1 86 // PF6
#define pKY2 87 // PF7
#define pKY3 88 // PF8
boolean YUN;
//назначаем пины концевиков детект контейнера
#define Kcnt1 49 // PD1
#define Kcnt2 48 // PD0
boolean cnt1=0;
boolean cnt2=0;
//шимы мотора Y
#define ypwm1 54 // PD6
#define ypwm2 55 // PD7
//шимы гусеницы
#define gpwm1 52 // PD4
#define gpwm2 53 // PD5
//состояние концевиков по Y
boolean KY1=0;  //зад каретки  загрузка в ячейку сверху вниз
                  /*     1.   Z+12мм, потом выдвигает Y(вперед или назад)
                         2.   работа гусеницы, короб съехал в ячейку (по таймеру, +конц детект контейнера в ячейки)
                         3.   идем вниз Z-15(короб не цепляет)
                         4.   задвигаем Y(центр) 
                         5.   на Z норм. уровень    Zcorrect()             */
boolean KY2=0;  //центр каретки 
boolean KY3=0;  //перед каретки  выгрузка из ячейки снизу вверх  
                  /*     1.   Z-15, потом выдвигает Y(вперед или назад)
                         2.   потом Z+15 зацепил короб
                         3.   работа гусеницы, выгружаем из ячейки(по таймеру, +конц детект контейнера на коретке)
                         4.   задвигаем Y(центр) 
                         5.   на Z норм. уровень    Zcorrect()             */
//флаг указатель 
//для команд погрузки, выгрузки  {"command":"loadingCELL"}  {"command":"unloadingCELL"}
//если stY="2" просто перемещение
//если stY="3" при поступлении команд погрузки, выгрузки 
//действия производятся с передней ячейкой
//если stY="1" при поступлении команд погрузки, выгрузки 
//действия производятся с задней ячейкой
String stY="2";
//активное состояние оси Y
int8_t AcY=-1;
//статус оси Y
String StatusY="unknown";
//предыдущие состояния
String tempmoveY="vpered";
//статус каретки
String StatusKkr="unknown";
boolean Flzkp=0;

//инициализируем энкодер
#define AenZ 40 // пинA PC8
#define BenZ 39 // пинB PC7
volatile long enzPos=0;
long enZ0=0,enZ1=819,enZ2=1570,enZ3=2360,enZ4=3169,enZ5=3990;

//##################### ПОСАДОЧНЫЙ СТОЛ #################
//назначаем пины pwm для мотора X механизма захвата 
#define zxpwm1 9 // PA9
#define zxpwm2 10 // PA10
//назначаем пины pwm для мотора Y механизма захвата 
#define zypwm1 41 // PC9
#define zypwm2 8 // PA8
//пины концевиков посадочного стола
#define pKXun 82  //PF2 концевик X unцентр
#define pKYun 83  //PF3 концевик Y unцентр
#define pKXce 84  //PF4 концевик X центр
#define pKYce 85  //PF5 концевик Y центр
//состояние концевиков стола 
boolean KXun=0;
boolean KYun=0;
boolean KXce=0;
boolean KYce=0; 
//активное состояние стола
String AcStol="none";
//статус захвата стола
String StatusZuh="unknown";


//инициализируем шаговик с внешним драйвером
#define Xstep 106 // пульс пин PG10
#define Xdir 105 // директион пин PG9
String MOT="off";
int Mval=15000;
unsigned long timertrack;

//############### инициализируем сервоприводы #################
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40); // Установка адреса I2C 0x40
// от 544мкс до 2400мкс.
#define SERVOMIN  100     // Минимальная длительность импульса для сервопривода 150
#define SERVOMAX  450     // Максимальная длина импульса для сервопривода 600
//#define SERVO_1 4       // Номер порта (0 - 15)
//#define SERVO_2 5       // Номер порта (0 - 15)
#define conLock 0         // Серва замок коптера
String SERV="off";
boolean s_trig=0;


//##################### ОТСЕК ВЫДАЧИ #################
//назначаем пины pwm для мотора отсека выдачи 
#define ovpwm1 50 // PD2
#define ovpwm2 51 // PD3
//пин соленоида отсека выдачи
#define povsl 42 // PС10
//пины концевиков отсека выдачи
#define pKovv 57 // PD9
#define pKovn 56 // PD8
//состояние концевиков 
boolean ovv=0,ovn=0;
//статус отсека выдачи
String StatusOV="unknown";
boolean pOV=0;
boolean pDN=0;
//инициализация весы HX711 
#define DOUT_PIN  46 //PC14
#define SCK_PIN   47 //PC15
HX711 scale;
long  units; //вес
float calibration_factor = 14.4;
//валидность массы и объема
boolean VD=1; //разрешенно
boolean Vm=1; //масса
boolean Vo=1; //объем
//лазеры система контроля объема 
#define pADClz1  32 // PC0
#define pADClz2  34 // PC2
#define pADClz3  0  // PA0
#define pADClz4  2  // PA2
#define pADClz5  4  // PA4
#define pADClz6  6  // PA6
#define pADClz7  36 // PC4
#define pADClz8  16 // PB0
//пин Laser Diode Module KY-008
#define pLaz  99 // PG3
//пины RGB ленты
#define pLg  63 // PD15
#define pLr  62 // PD14


//##############__ЗРЯДКА KR0__##########################
//############# вольт сенсор крышки KR0 ###############
#define pADCkr0 33 // PC1
float vsensc = 0.00797;
float ADCkr0;
char VBkr0[20];
//назначаем пины мотора замка крышки KR0
#define kr0m1 81 // PF1
#define kr0m2 80 // PF0
//назначаем пины концевиков крышки KR0
#define kr0O 70 // PE6
#define kr0C 45 // PC13
#define kr0N 68 // PE4
//состояние концевиков 
boolean KR0n=0,KR0c=0,KR0o=0;
//статус позиции крышки 
String pKR="unknown";

//инициализируем кнопки
#define pBT0 78 //PE14
#define pBT1 76 //PE12
#define pBT2 74 //PE10
#define pBT3 72 //PE8
#define pBT4 95 //PF15
#define pBT5 79 //PE15
#define pBT6 77 //PE13
#define pBT7 75 //PE11
#define pBT8 73 //PE9
#define pBT9 96 //PG0

//############## МЕХАНИЗМ ВЕРХНЕГО ЛЮКА #################
//назначаем пины мотора подъема спуска люка 
#define vlppwm1 28 // PB12
#define vlppwm2 29 // PB13
//назначаем пины мотора выдвинуть задвинуть люк
#define vlvpwm1 30 // PB14
#define vlvpwm2 31 // PB15
//назначаем пины концевиков подъема спуска люка
#define pKVlv 61 // PD13
#define pKVln 60 // PD12
//назначаем пины концевиков выдвинуть задвинуть люк
#define pKVld 59 // PD11
#define pKVlz 58 // PD10
//состояние концевиков 
boolean Vlv=0,Vln=0,Vld=0,Vlz=0;
//статус верхнего люка
String StatusVL="unknown";


//глобальный массив хранения json данных
char json[160] =""; 
//глобальный семафор
boolean gSem=0;
uint8_t nSen=0;
//счетчик команд
uint8_t cnum=0;


void setup()
{ delay(40);
  Serial.begin(57600);
  //ось Z концевики
  pinMode(pKZ0, INPUT);
  pinMode(pKZ1, INPUT);
  pinMode(pKZ2, INPUT);
  pinMode(pKZ3, INPUT);
  pinMode(pKZ4, INPUT);
  pinMode(pKZ5, INPUT);
  //концевики контейнера
  pinMode(pKNZ0X0Y3, INPUT);
  pinMode(pKNZ0X0Y1, INPUT);
  pinMode(pKNZ1X0Y1, INPUT);
  pinMode(pKNZ2X0Y3, INPUT);
  pinMode(pKNZ2X0Y1, INPUT);
  pinMode(pKNZ3X0Y3, INPUT);
  pinMode(pKNZ3X0Y1, INPUT);
  //ось Y концевики
  pinMode(pKY1, INPUT);
  pinMode(pKY2, INPUT);
  pinMode(pKY3, INPUT);
  pinMode(Kcnt1, INPUT);
  pinMode(Kcnt2, INPUT);
  //концевики посадочного стола
  pinMode(pKXun, INPUT);
  pinMode(pKYun, INPUT);
  pinMode(pKXce, INPUT);
  pinMode(pKYce, INPUT);
  //тестовые кнопки
  pinMode(pBT0, INPUT);
  pinMode(pBT1, INPUT);
  pinMode(pBT2, INPUT);
  pinMode(pBT3, INPUT);
  pinMode(pBT4, INPUT);
  pinMode(pBT5, INPUT);
  pinMode(pBT6, INPUT);
  pinMode(pBT7, INPUT);
  pinMode(pBT8, INPUT);
  pinMode(pBT9, INPUT);
  //####__ЗРЯДКА KR0__#####
  //ADC крышек
  pinMode(pADCkr0,INPUT_ANALOG);
  //шимы мотора KR0
  pinMode(kr0m1, OUTPUT);
  pinMode(kr0m2, OUTPUT);
  digitalWrite(kr0m1, LOW);
  digitalWrite(kr0m2, LOW);
  //концевики KR0
  pinMode(kr0O, INPUT);
  pinMode(kr0C, INPUT);
  pinMode(kr0N, INPUT);
  //ADC лазера
  pinMode(pADClz1,INPUT_ANALOG);
  pinMode(pADClz2,INPUT_ANALOG);
  pinMode(pADClz3,INPUT_ANALOG);
  pinMode(pADClz4,INPUT_ANALOG);
  pinMode(pADClz5,INPUT_ANALOG);
  pinMode(pADClz6,INPUT_ANALOG);
  pinMode(pADClz7,INPUT_ANALOG);
  pinMode(pADClz8,INPUT_ANALOG);
  //энкодер Z
  pinMode(AenZ,INPUT);
  attachInterrupt(BenZ,encoderZ,FALLING);
  //шимы мотора Z
  pinMode(zpwm1, PWM);
  pinMode(zpwm2, PWM);
  pwmWrite(zpwm1,0);
  pwmWrite(zpwm2,0);
  //шимы мотора Y
  pinMode(ypwm1, OUTPUT);
  pinMode(ypwm2, OUTPUT);
  digitalWrite(ypwm1, LOW);
  digitalWrite(ypwm2, LOW);
  //шимы гусеницы
  pinMode(gpwm1, OUTPUT);
  pinMode(gpwm2, OUTPUT);
  digitalWrite(gpwm1, LOW);
  digitalWrite(gpwm2, LOW);
  //шимы отсека выдачи
  pinMode(ovpwm1, OUTPUT);
  pinMode(ovpwm2, OUTPUT);
  digitalWrite(ovpwm1, LOW);
  digitalWrite(ovpwm2, LOW);
  pinMode(povsl, OUTPUT);
  digitalWrite(povsl, LOW);
  pinMode(pKovv, INPUT);
  pinMode(pKovn, INPUT);
  //KY-008
  pinMode(pLaz, OUTPUT);
  digitalWrite(pLaz, LOW);
  //RGB лента
  pinMode(pLg, OUTPUT);
  pinMode(pLr, OUTPUT);
  digitalWrite(pLg, LOW);
  digitalWrite(pLr, LOW);
  //весы HX711
  scale.begin(DOUT_PIN,SCK_PIN);
  delay(100);
  scale.set_scale();
  delay(100); 
  scale.tare();//обнуление
  //шимы моторов посадочного стола
  pinMode(zxpwm1, PWM);
  pinMode(zxpwm2, PWM);
  pinMode(zypwm1, PWM);
  pinMode(zypwm2, PWM);
  pwmWrite(zxpwm1,0);
  pwmWrite(zxpwm2,0);
  pwmWrite(zypwm1,0);
  pwmWrite(zypwm2,0);
  //верхний люк
  pinMode(vlppwm1, OUTPUT);
  pinMode(vlppwm2, OUTPUT);
  digitalWrite(vlppwm1, LOW);
  digitalWrite(vlppwm2, LOW);
  pinMode(vlvpwm1, OUTPUT);
  pinMode(vlvpwm2, OUTPUT);
  digitalWrite(vlvpwm1, LOW);
  digitalWrite(vlvpwm2, LOW);
  pinMode(pKVlv, INPUT);
  pinMode(pKVln, INPUT);
  pinMode(pKVld, INPUT);
  pinMode(pKVlz, INPUT);
  //шаговик
  pinMode(Xstep, OUTPUT);
  pinMode(Xdir, OUTPUT);
  //сервы
  delay(300);
  pwm.begin();  // Инициализация
  delay(300);
  pwm.setPWMFreq(56); // Частота следования импульсов 60 Гц
  delay(120);
  pwm.setPWM(conLock, 0, SERVOMIN);

  //######### ПРОЦЕССЫ ############## 
  xTaskCreate(vSendTask,       "Task01",410,NULL,1,NULL);
  xTaskCreate(vLaserTask,      "Task02",220,NULL,1,NULL);
  xTaskCreate(vKZcheckTask,    "Task03",configMINIMAL_STACK_SIZE,NULL,1,NULL);
  xTaskCreate(vKYcheckTask,    "Task04",configMINIMAL_STACK_SIZE,NULL,1,NULL);
  xTaskCreate(vKCellTask,      "Task05",configMINIMAL_STACK_SIZE,NULL,1,NULL);
  xTaskCreate(vKSpecTask,      "Task06",configMINIMAL_STACK_SIZE,NULL,1,NULL);
  xTaskCreate(vKStolTask,      "Task07",configMINIMAL_STACK_SIZE,NULL,1,NULL);
  xTaskCreate(vShagMotTask,    "Task08",220,NULL,1,NULL);
  xTaskCreate(vJsonParsingTask,"Task09",400,NULL,1,NULL); 
  xTaskCreate(vSpecOpr,        "Task10",500,NULL,1,NULL);
  xTaskCreate(vTest,           "Task11",360,NULL,1,NULL);
  xTaskCreate(vCmbTask,        "Task12",400,NULL,1,NULL);
  xTaskCreate(vStateForm,      "Task13",220,NULL,1,NULL);
  xTaskCreate(vScaleTask,      "Task14",220,NULL,1,NULL);
  xTaskCreate(vAxis_Z,         "Task15",310,NULL,1,NULL);
  xTaskCreate(vAxis_Y,         "Task16",310,NULL,1,NULL);
  xTaskCreate(vAxis_Sl,        "Task17",310,NULL,1,NULL);
  xTaskCreate(vBTcheckTask,    "Task18",310,NULL,1,NULL);
  xTaskCreate(vVsensTask,      "Task19",310,NULL,1,NULL);
  vTaskStartScheduler();
}
 
void loop()
   {  }   


/*
enum {
PA0,PA1,PA2,PA3,PA4,PA5,PA6,PA7,PA8,PA9,PA10,PA11,PA12,PA13,PA14,PA15,
PB0,PB1,PB2,PB3,PB4,PB5,PB6,PB7,PB8,PB9,PB10,PB11,PB12,PB13,PB14,PB15,
PC0,PC1,PC2,PC3,PC4,PC5,PC6,PC7,PC8,PC9,PC10,PC11,PC12,PC13,PC14,PC15,
PD0,PD1,PD2,PD3,PD4,PD5,PD6,PD7,PD8,PD9,PD10,PD11,PD12,PD13,PD14,PD15,
PE0,PE1,PE2,PE3,PE4,PE5,PE6,PE7,PE8,PE9,PE10,PE11,PE12,PE13,PE14,PE15,
PF0,PF1,PF2,PF3,PF4,PF5,PF6,PF7,PF8,PF9,PF10,PF11,PF12,PF13,PF14,PF15,
PG0,PG1,PG2,PG3,PG4,PG5,PG6,PG7,PG8,PG9,PG10,PG11,PG12,PG13,PG14,PG15
};    */










           
