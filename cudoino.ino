#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <MenuBackend.h>
#include <EEPROM.h>

#define THERMO_SCREEN 1
#define SCREEN_2 2
#define SCREEN_3 3
#define SCREEN_4 4
#define SCREEN_5 5
#define BTN_UP 4
#define BTN_DOWN 2 
#define BTN_LEFT 10
#define BTN_RIGHT 3
#define BTN_USE 1


static unsigned char PROGMEM fogLamp[] = {
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B00000000,B00000011,B11111110,B00000011,B11000000,B00000000,
B00000000,B00001111,B11111110,B00000011,B11000000,B00000000,
B00000000,B00011111,B11111110,B11100011,B11000000,B00000000,
B00000000,B01111111,B00001110,B11111011,B10000000,B00000000,
B00000000,B11111100,B00001110,B11111111,B10000000,B00000000,
B00000001,B11110000,B00001110,B00111111,B10000000,B00000000,
B00000011,B11100000,B00001110,B00011111,B11000000,B00000000,
B00000011,B11000000,B00001110,B00001111,B11100000,B00000000,
B00000111,B10000000,B00001110,B00000011,B11110000,B00000000,
B00001111,B00000000,B00001110,B00000001,B11111000,B00000000,
B00001111,B00000000,B00001110,B00000000,B11111110,B00000000,
B00001110,B00000000,B00001110,B00000000,B01111111,B00000000,
B00011110,B00000000,B00001110,B00000000,B01111111,B11000000,
B00011100,B00000000,B00001110,B00000000,B01110111,B11100000,
B00011100,B00000000,B00001110,B00000000,B11110011,B11110000,
B00111100,B00000000,B00001110,B00000001,B11110000,B11111100,
B00111000,B00000000,B00001110,B00000011,B11100000,B01111100,
B00111000,B00000000,B00001110,B11100011,B11100000,B00011100,
B00111000,B00000000,B00001110,B11111011,B11000000,B00000000,
B00111000,B00000000,B00001110,B11111111,B10000000,B00000000,
B00111000,B00000000,B00001110,B00111111,B11000000,B00000000,
B00111000,B00000000,B00001110,B00001111,B11100000,B00000000,
B00111000,B00000000,B00001110,B00000111,B11110000,B00000000,
B00111000,B00000000,B00001110,B00000001,B11111100,B00000000,
B00111000,B00000000,B00001110,B00000000,B11111111,B00000000,
B00111000,B00000000,B00001110,B00000000,B01111111,B10000000,
B00111000,B00000000,B00001110,B00000000,B00111111,B11100000,
B00111000,B00000000,B00001110,B00000000,B00111011,B11111000,
B00111100,B00000000,B00001110,B00000000,B00111000,B11111000,
B00011100,B00000000,B00001110,B00000000,B01111000,B00111000,
B00011100,B00000000,B00001110,B00000000,B01111000,B00000000,
B00011110,B00000000,B00001110,B01110000,B11111000,B00000000,
B00001110,B00000000,B00001110,B01111101,B11110000,B00000000,
B00001111,B00000000,B00001110,B01111111,B11110000,B00000000,
B00001111,B00000000,B00001110,B00011111,B11100000,B00000000,
B00000111,B10000000,B00001110,B00001111,B11100000,B00000000,
B00000011,B11000000,B00001110,B00000011,B11110000,B00000000,
B00000011,B11100000,B00001110,B00000011,B11111100,B00000000,
B00000001,B11110000,B00001110,B00000011,B11111111,B00000000,
B00000000,B11111100,B00001110,B00000011,B11011111,B10000000,
B00000000,B01111111,B00001110,B00000001,B11101111,B11100000,
B00000000,B00011111,B11111110,B00000001,B11100011,B11100000,
B00000000,B00001111,B11111110,B00000001,B11100000,B11100000,
B00000000,B00000011,B11111110,B00000000,B11100000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B0000000
};

static unsigned char PROGMEM degree[] = {
B00111100,
B01111110,
B11100111,
B11000011,
B11000011,
B11100111,
B01111110,
B00111100
};

// ---------NOKIA 5110 LCD----------------
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

// ----------SHIFT REGISTER OUT-------------
//Pin connected to ST_CP of 74HC595
int latchPin = 12;
//Pin connected to SH_CP of 74HC595
int clockPin = 11;
////Pin connected to DS of 74HC595
int dataPin = 13;

int temperaturePin = 1;
int temperature;

int sw = 0;
int btnPress;
String tempMenus[5];



int menuLevel = 0;
int currentScreen = 1;
int totalScreen = 5;
int lastScreen = 1;
boolean isAutoAC = true;
byte expTemperature;
byte relayState = 0;
int isCompressorOn = false;
long previousMillis = 0;

long screenInterval = 1000;

long acInterval = 5000;
long previousACMillis = 0;

long kmInterval = 10000;
long previousKmMillis = 0;

long kmStatInterval = 1000;
long previousKmStatMillis = 0;

long criticalInterval = 10000;
long previousCriticalMillis = 0;

// DIGITAL PIN
int pinDown = 8;
int pinRight = 9;
int pinVss = 2;

int compressorRelay = 1;
int fogYellowRelay = 2;
int fogWhiteRelay = 3;

unsigned int vssClickCount = 0;
unsigned int km = 0;
int vssClickPerKm = 2548;
unsigned int engineKmLimit = 2500;
unsigned int timingBeltKmLimit = 50000;
unsigned int rearAxleKmLimit = 50000;
unsigned int frontAxleKmLimit = 50000;
unsigned int tcKmLimit = 50000;
unsigned int gearBoxKmLimit = 50000;
unsigned int radiatorKmLimit = 20000;

unsigned int engineKm = 0;
unsigned int timingBeltKm = 0;
unsigned int rearAxleKm = 0;
unsigned int frontAxleKm = 0;
unsigned int tcKm = 0;
unsigned int gearBoxKm = 0;
unsigned int radiatorKm = 0;
byte kmStat = 0;


boolean isSelect = false;
boolean isCritical = false;

char keyTemp = 'i';
char keyFrontFogYOn = 'j';
char keyFrontFogYOff = 'k';
char keyFrontFogWOn = 'l';
char keyFrontFogWOff = 'm';
char keyWhiteFog = 'n';
char keyYellowFog = 'o';
char keyOdo = 'p';
char keyEngineKm = 'q';
char keyTcKm = 'r';
char keyGearBoxKm = 's';
char keyRearAxleKm = 't';
char keyFrontAxleKm = 'u';
char keyRadiatorKm = 'v';
char keyTimingBeltKm = 'w';

// EEPROM data 
byte eepromRelayAddr = 0;
byte eepromFlagAddr = 1;
byte eepromTempAddr = 2;
byte eepromKmAddr = 3; 
//4 also used by km
byte eepromVssAddr = 5;
//6 also used by vss click
byte eepromEngineKm = 7;
byte eepromRadiatorKm = 9;
byte eepromTcKm = 11;
byte eepromGearBoxKm = 13;
byte eepromRearAxleKm = 15;
byte eepromFrontAxleKm = 17;
byte eepromTimingBeltKm = 19;

void EEPROMWriteInt(int p_address, unsigned int p_value)
      {
      byte lowByte = ((p_value >> 0) & 0xFF);
      byte highByte = ((p_value >> 8) & 0xFF);

      EEPROM.write(p_address, lowByte);
      EEPROM.write(p_address + 1, highByte);
      }

//This function will read a 2 byte integer from the eeprom at the specified address and address + 1
unsigned int EEPROMReadInt(unsigned int p_address)
      {
      byte lowByte = EEPROM.read(p_address);
      byte highByte = EEPROM.read(p_address + 1);

      return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
      }


MenuBackend menu = MenuBackend(menuUseEvent, menuChangeEvent);
  MenuItem miTemp = MenuItem("Temperature", keyTemp);
  MenuItem miWhiteFog = MenuItem("White Fog", keyWhiteFog);
  MenuItem miYellowFog = MenuItem("Yellow Fog", keyYellowFog);
  MenuItem miOdo = MenuItem("Odometer", keyOdo);
  MenuItem miEngineKm = MenuItem("Engine", keyEngineKm);

  MenuItem miTcKm = MenuItem("Transfer Case", keyTcKm);
  MenuItem miGearBoxKm = MenuItem("Gear Box", keyGearBoxKm);
  MenuItem miRadiatorKm = MenuItem("Radiator", keyRadiatorKm);
  MenuItem miTimingBeltKm = MenuItem("Timing Belt", keyTimingBeltKm);
  MenuItem miRearAxleKm = MenuItem("Rear Axle", keyRearAxleKm);
  MenuItem miFrontAxleKm = MenuItem("Front Axle", keyFrontAxleKm);
    
void menuSetup(){
  
  menu.getRoot().add(miTemp);  
  miTemp.addAfter(miWhiteFog);
  miWhiteFog.addAfter(miYellowFog);
  miYellowFog.addAfter(miOdo);
  miOdo.addAfter(miEngineKm);
  miEngineKm.addAfter(miGearBoxKm);
  miGearBoxKm.addAfter(miTcKm);
  miTcKm.addAfter(miRadiatorKm);
  miRadiatorKm.addAfter(miTimingBeltKm);
  miTimingBeltKm.addAfter(miRearAxleKm);
  miRearAxleKm.addAfter(miFrontAxleKm);
  miFrontAxleKm.addAfter(miTemp);
  
}

void menuUseEvent(MenuUseEvent used){

  Serial.print("menu use:");
  Serial.println(used.item.getName());
  
  if (used.item.getShortkey() == keyWhiteFog){    
    if (getRelayStatByNum(fogWhiteRelay)){
      turnRelayByNum(fogWhiteRelay, false);
    }else{
      turnRelayByNum(fogWhiteRelay, true);
    }
    display.clearDisplay();
    showWhiteFogScreen();
    display.display();
  }
  
  if (used.item.getShortkey() == keyYellowFog){    
    if (getRelayStatByNum(fogYellowRelay)){
      turnRelayByNum(fogYellowRelay, false);      
    }else{
      turnRelayByNum(fogYellowRelay, true);
    }
    display.clearDisplay();
    showYellowFogScreen();
    display.display();
  }
  
  if (used.item.getShortkey() == keyTemp){
    if (expTemperature >= 30){
      expTemperature = 15;
    }
    expTemperature = expTemperature + 1;
    EEPROM.write(eepromTempAddr, (byte)expTemperature);
    display.clearDisplay();
    showTemperatureScreen();
    display.display();
  }
  char key = used.item.getShortkey();
  if (key == keyOdo){
    km = 0;
    vssClickCount = 0;
  }else if (key == keyEngineKm){
    engineKm = 0;
  }else if (key == keyTcKm){
    tcKm = 0;
  }else if (key == keyGearBoxKm){
    gearBoxKm = 0;
  }else if (key == keyRadiatorKm){
    radiatorKm = 0;
  }else if (key == keyTimingBeltKm){
    timingBeltKm = 0;
  }else if (key == keyFrontAxleKm){
    frontAxleKm = 0;
  }else if (key == keyRearAxleKm){
    rearAxleKm = 0;
  }
}

void menuChangeEvent(MenuChangeEvent changed){
  Serial.print("Menu change ");
  Serial.print(changed.from.getName());
  Serial.print(" ");
  Serial.println(changed.to.getName());
  display.clearDisplay();
  display.setCursor(0,10);
  char change = changed.to.getShortkey();
  if (    
    (changed.to.getShortkey() == keyWhiteFog) || (changed.to.getShortkey() == keyYellowFog) ||
    (changed.to.getShortkey() == keyTemp) || changed.to.getShortkey() == keyOdo ||
     (change == keyEngineKm) || (change == keyTcKm) || (change == keyGearBoxKm) || 
     (change == keyRadiatorKm) || (change == keyTimingBeltKm) || (change == keyRearAxleKm) ||
     (change == keyFrontAxleKm)){
    isSelect = true;
    Serial.println("selected");
  }else{
    isSelect = false;
  }

  
  if (changed.to.getShortkey() == keyTemp){
    display.clearDisplay();
    showTemperatureScreen();
    display.display();
  }else if (changed.to.getShortkey() == keyWhiteFog){
    display.clearDisplay();
    showWhiteFogScreen();
    display.display();
  }else if (changed.to.getShortkey() == keyYellowFog){
    display.clearDisplay();
    showYellowFogScreen();
    display.display();
  }else if(changed.to.getShortkey() == keyOdo){
    display.clearDisplay();
    showOdoScreen();
    display.display();  
  }else if (change == keyEngineKm){
    display.clearDisplay();
    showKmScreen("Engine Oil", engineKm);
    display.display();
  }else if (change == keyTcKm){
    display.clearDisplay();
    showKmScreen("Transfer Case Oil", tcKm);
    display.display();
  }else if (change == keyGearBoxKm){
    display.clearDisplay();
    showKmScreen("Gear Box Oil", gearBoxKm);
    display.display();
  }else if (change == keyRadiatorKm){
    display.clearDisplay();
    showKmScreen("Radiator Coolant", radiatorKm);
    display.display();
  }else if (change == keyTimingBeltKm){
    display.clearDisplay();
    showKmScreen("Timing Belt", timingBeltKm);
    display.display();
  }else if (change == keyRearAxleKm){
    display.clearDisplay();
    showKmScreen("Rear Axle Oil", rearAxleKm);
    display.display();
  }else if (change == keyFrontAxleKm){
    display.clearDisplay();
    showKmScreen("Front Axle Oil", frontAxleKm);
    display.display();    
  }else{      
      display.clearDisplay();
      int cursorY = 10;    
      display.setCursor(0, cursorY);
      String text = String(changed.to.getName());
      display.println(text);
      display.display();    
  }
  
}

//-------Themperature ----------------
void turnRelayByNum(int number, boolean stat){
  byte start = 0x01;  
  for(int i=1;i<number;i++){
    start = start << 1;
  }
  
  if (stat){
    relayState = relayState | start;
  }else{
    relayState = relayState & ~start;
  }
  turnRelay(relayState);
}

boolean getRelayStatByNum(int number){
  byte start = 0x01;  
  for(int i=1;i<number;i++){
    start = start << 1;
  }
  return ((relayState & start) == start)? true : false;
}

void setCompressor(boolean val){  
  if (val){
    relayState = relayState | 0x01;
  }else{
    relayState = relayState & 0xFE;
  }
  turnRelay(relayState);
}

boolean compressorShouldOn(){
  return (temperature < expTemperature)? false : true;
}

double Thermister(int RawADC) {
 double Temp;
 Temp = log(((10240000/RawADC) - 10000));
 Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
 Temp = Temp - 273.15;            // Convert Kelvin to Celcius
 //Temp = (Temp * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit
 return Temp;
}

void readTemperature(){
  temperature = int(Thermister(analogRead(temperaturePin)));
  //temperature = (5.0 * analogRead(temperaturePin) * 100.0)/1024.0;
  //Serial.println()
}

void controlAC(){
  if (isAutoAC){
    if (temperature < expTemperature){
      setCompressor(false);
    }else{
      setCompressor(true);
    }
  }else{
    setCompressor(false);
  }
}

void showSplashscreen(){
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.print("Cudoino");
  display.display();
  delay(2000);  
}

void showTemperatureScreen(){
  display.setCursor(0,0);
  display.setTextSize(2);
  display.print(temperature);
  display.drawBitmap(25, 0, degree, 8, 8, 1);
  display.setCursor(35, 0);
  display.print("C");
  display.setCursor(60,0);
  display.print(expTemperature);
  display.setCursor(0,30);
  
  display.print((compressorShouldOn())? "ON" : "OFF");
}


void drawBitmapInvert(int16_t x, int16_t y, 
			      const uint8_t *bitmap, int16_t w, int16_t h,
			      uint16_t color) {
  int16_t i, j, byteWidth = (w + 7) / 8;
  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(!(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7)))) {
	display.drawPixel(x+i, y+j, color);
      }
    }
  }
}
void showWhiteFogScreen(){
  if (getRelayStatByNum(fogWhiteRelay)){
    display.drawBitmap(0, 0, fogLamp, 48, 48, 1);
  }else{
    drawBitmapInvert(0, 0, fogLamp, 48, 48,1);
  }
  display.setTextSize(3);
  display.setCursor(55,20);
  display.print("W");  
}



void showYellowFogScreen(){
  if (getRelayStatByNum(fogYellowRelay)){
    display.drawBitmap(0, 0, fogLamp, 48, 48, 1);
  }else{
    drawBitmapInvert(0, 0, fogLamp, 48, 48,1);
  }
  display.setTextSize(3);
  display.setCursor(55,20);
  display.print("Y");  
}

int commaKm(unsigned int count){  
  byte stepClick = 255;
  for(int i=0;i<=9;i++){
    if ((count >= i*stepClick) && (count <= (i+1)*stepClick)){
      return i;
    }
  }
  
}

void showOdoScreen(){
  display.setTextSize(0);
  display.setCursor(0,0);
  display.print("ODOMETER");
  display.print("  ");
  display.println(vssClickCount);
  int comma = commaKm(vssClickCount);
  display.setTextSize(3);
  display.print(km);
  display.setTextSize(1);
  display.print(".");
  display.setTextSize(3);
  display.print(comma);
  display.setTextSize(0);
  display.print("km");
}

void showKmScreen(String title, unsigned int kilo){
  display.setTextSize(0);
  display.setCursor(0,0);
  display.println(title);
  display.setTextSize(3);
  display.println(kilo);
}


void turnRelay(int value){
    digitalWrite(latchPin, LOW);
    // shift out the bits:
    shiftOut(dataPin, clockPin, MSBFIRST, value);  

    //take the latch pin high so the LEDs will light up:
    digitalWrite(latchPin, HIGH);
    // pause before next value:
    EEPROM.write(eepromRelayAddr, relayState);
    delay(500);
}

void readKm(){ 
  if ((digitalRead(pinVss) == HIGH)){    
    vssClickCount = vssClickCount + 1;
  }

  if (vssClickCount >= vssClickPerKm){
    km = km + 1;
    vssClickCount = 0;
    engineKm = engineKm + 1;
    tcKm = tcKm + 1;
    gearBoxKm = gearBoxKm + 1;
    rearAxleKm = rearAxleKm + 1;
    frontAxleKm = frontAxleKm + 1;
    timingBeltKm = timingBeltKm + 1;
    radiatorKm = radiatorKm + 1;
  }
}

void verbose(){
  //Serial.print("click:");
  //Serial.println(vssClickCount);
  //Serial.print("km:");
  //Serial.println(km);
  Serial.println(commaKm(2500));
}

void collectKmStat(){
  kmStat = kmStat << 1;
  if (digitalRead(pinVss) == HIGH || (Serial.available() > 0)){
    kmStat = kmStat | 0;    
  }else{
    kmStat = kmStat | 1;
  }
}

int getKmStat(){
  byte count = 0;
  if (kmStat == 0){
    return 0;
  }else if (kmStat == 255){
    return 0;
  }
  byte val = kmStat;
  for (byte i=0;i<=7;i++){
    if (val & 1 == 1){
      count += 1;
    }
    val = val >> 1;
  }
  return count;
}

void setup(){
  attachInterrupt(0, readKm, RISING);
  Serial.begin(9600);
    //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(pinDown, INPUT);
  pinMode(pinRight, INPUT);
  pinMode(pinVss, INPUT);

  menuSetup();
  gotoScreen(keyOdo);
  byte initialized = 0xFE;
  
  if (EEPROM.read(eepromFlagAddr) == initialized){
    relayState = EEPROM.read(eepromRelayAddr);
    expTemperature = EEPROM.read(eepromTempAddr);
    km = EEPROMReadInt(eepromKmAddr);
    vssClickCount = EEPROMReadInt(eepromVssAddr);
    engineKm = EEPROMReadInt(eepromEngineKm);
    radiatorKm = EEPROMReadInt(eepromRadiatorKm);
    timingBeltKm = EEPROMReadInt(eepromTimingBeltKm);
    tcKm = EEPROMReadInt(eepromTcKm);
    gearBoxKm = EEPROMReadInt(eepromGearBoxKm);
    rearAxleKm = EEPROMReadInt(eepromRearAxleKm);
    frontAxleKm = EEPROMReadInt(eepromFrontAxleKm);
  }else{
    EEPROM.write(eepromFlagAddr, initialized);
    relayState = 0x00;
    EEPROM.write(eepromRelayAddr, relayState);
    expTemperature = 22;
    EEPROM.write(eepromTempAddr, expTemperature);
    EEPROMWriteInt(eepromKmAddr, (unsigned int)0);
    EEPROMWriteInt(eepromVssAddr, (unsigned int)0);
    
    EEPROMWriteInt(eepromEngineKm, (unsigned int)0);
    EEPROMWriteInt(eepromRadiatorKm, (unsigned int)0);
    EEPROMWriteInt(eepromTimingBeltKm, (unsigned int)0);
    EEPROMWriteInt(eepromTcKm, (unsigned int)0);
    EEPROMWriteInt(eepromGearBoxKm, (unsigned int)0);
    EEPROMWriteInt(eepromRearAxleKm, (unsigned int)0);
    EEPROMWriteInt(eepromFrontAxleKm, (unsigned int)0);
  }  
  turnRelay(relayState);  
  display.begin();
  display.setContrast(60);
  display.clearDisplay();
  showTemperatureScreen();  
  display.display();
  delay(1000);
  Serial.println("ready");
}

MenuItem gotoScreen(char key){
  for (int i=0;i<20;i++){
    if (menu.getCurrent().getShortkey() == key){
      return menu.getCurrent();
    }else{
      menu.moveDown();
    }
  }
}

boolean catchCritical(){
  boolean ret = false;
  char key;
  if (engineKm >= engineKmLimit){
    key = keyOdo;
    ret = true;
  }else if (tcKm >= tcKmLimit){
    ret = true;
    key = keyTcKm;
  }else if (gearBoxKm >= gearBoxKmLimit){
    ret = true;
    key = keyGearBoxKm;
  }else if (timingBeltKm >= timingBeltKmLimit){
    ret = true;
    key = keyTimingBeltKm;
  }else if (radiatorKm >= radiatorKmLimit){
    ret = true;
    key = keyRadiatorKm;
  }else if (rearAxleKm >= rearAxleKmLimit){
    ret = true;
    key = keyRearAxleKm;
  }else if (frontAxleKm >= frontAxleKmLimit){
    ret = true;
    key = keyFrontAxleKm;
  }
  if(ret){
    gotoScreen(key);
  }
  return ret;
}

void loop(){  
  readTemperature();     
  controlAC();
  if (digitalRead(pinDown) == HIGH){
    menu.moveDown();
  }
  if (digitalRead(pinRight) == HIGH){
    if (isSelect){
      menu.use();
    }else{
      menu.moveRight();
    }
  }
  
  
  unsigned long currentCriticalMillis = millis();  
  if (currentCriticalMillis - previousCriticalMillis > criticalInterval){   
    previousCriticalMillis = currentCriticalMillis;
    isCritical = catchCritical();
  }
  unsigned long currentMillis = millis();  
  if (currentMillis - previousMillis > screenInterval){   
    previousMillis = currentMillis;
    
    if (!isCritical){
      if (menu.getCurrent().getShortkey() == keyTemp){
        display.clearDisplay();
        showTemperatureScreen();
        display.display();        
      }else if (menu.getCurrent().getShortkey() == keyOdo){        
        display.clearDisplay();
        showOdoScreen();  
        display.display();        
      }
    }    
  } 
  
  unsigned long currentACMillis = millis();
  if (currentACMillis - previousACMillis > acInterval){
    previousACMillis = currentACMillis;    
  }    

  unsigned long currentKmStatMillis = millis();
  if (currentKmStatMillis - previousKmStatMillis > kmStatInterval){
    previousKmStatMillis = currentKmStatMillis;
    collectKmStat();    
  }    
  
  unsigned long currentKmMillis = millis();
  if (currentKmMillis - previousKmMillis > kmInterval){
    previousKmMillis = currentKmMillis;
    // save vss info
    EEPROMWriteInt(eepromKmAddr, km);
    EEPROMWriteInt(eepromEngineKm, engineKm);
    EEPROMWriteInt(eepromTcKm, tcKm);
    EEPROMWriteInt(eepromGearBoxKm, gearBoxKm);
    EEPROMWriteInt(eepromTimingBeltKm, timingBeltKm);
    EEPROMWriteInt(eepromRearAxleKm, rearAxleKm);
    EEPROMWriteInt(eepromFrontAxleKm, frontAxleKm);
    EEPROMWriteInt(eepromRadiatorKm, radiatorKm);

    
    EEPROMWriteInt(eepromVssAddr, vssClickCount);    
  }  
  verbose();
}
