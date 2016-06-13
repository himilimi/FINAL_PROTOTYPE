#include <DS3231.h>

#include <Adafruit_NeoPixel.h>;
#define PIN 6
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
// NEO_KHZ800 800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
// NEO_KHZ400 400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
// NEO_GRB Pixels are wired for GRB bitstream (most NeoPixel products)
// NEO_RGB Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, 6, NEO_GRB + NEO_KHZ800);

# define PIN 7
Adafruit_NeoPixel stripDays = Adafruit_NeoPixel(60, 7, NEO_GRB + NEO_KHZ800);


// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);



unsigned long previousMillis = 0;

int colorR = 0;
int colorG = 100;
int colorB = 0;
int resetButton = 8;
int tiltSwitch = 2;
int switchStateRed;
int switchStateTilt;
int teller = 0;
int tellerPrev = 0;
unsigned long lengde = 10000;
boolean mandagBo = false;
boolean tirsdagBo = false;
boolean onsdagBo = false;
boolean torsdagBo = false;
boolean fredagBo = false;
boolean lordagBo = false;
boolean sondagBo = false;

boolean tiltState;
boolean limit = true;
boolean activeShaking = false;
boolean inActive = true;
boolean activeTest = false;
boolean donna = false;
boolean doneDay = false;
boolean day = false;
boolean night = false;
boolean brushedDay = false;
boolean brushedNight = false;
unsigned long millisActive=0;
unsigned long millisActiveStart;
unsigned long millisInActive;
unsigned long millisInActiveStart;
unsigned long millisTellerOpp = 0;
unsigned long millisReset = 0;
unsigned long time;
unsigned long millisFoerInaktiv = 0;
int sekunder;
int sekundertellertid;
const unsigned long millisActiveLimit = 10000;
unsigned long currentMillis;
unsigned long currentMillis2;
unsigned long currentMillis3;
unsigned long wheelMillis; 


void setup() {
  pinMode(mandagB, INPUT_PULLUP);
  pinMode(tirsdagB, INPUT_PULLUP);
  pinMode(onsdagB, INPUT_PULLUP);
  pinMode(torsdagB, INPUT_PULLUP);
  pinMode(fredagB, INPUT_PULLUP);
  pinMode(lordagB, INPUT_PULLUP);
  pinMode(sondagB, INPUT_PULLUP);
  pinMode(alleB, INPUT_PULLUP);
  pinMode(resetButton, INPUT_PULLUP);
  pinMode(tiltSwitch, INPUT_PULLUP);
  Serial.begin(9600);
  strip.begin();
  strip.show();
  stripDays.begin();
  strip.show();
  rtc.begin();
  
  rtc.setDOW(SUNDAY);     // Set Day-of-Week to SUNDAY
  rtc.setTime(23, 59, 00);     // Set the time to 12:00:00 (24hr format)
  rtc.setDate(8, 7, 2016);   // Set the date to January 1st, 2014
  strip.setBrightness(40);
  stripDays.setBrightness(255);
  for(int i = 0; i<12; i++){
  
      stripDays.setPixelColor(i, 0, 0, 0);
      stripDays.show();
 
  }
}

void loop() {

    
  // For hber dag kl 00.00 skal alt resettes. Booleanen som sier om man har pusset tenner dag eller natt blir satt til false og reset() blir kjoert.  
    if (rtc.getTime().hour == 0 && rtc.getTime().min == 00 && rtc.getTime().sec == 0) {
    brushedNight = false;
    brushedDay = false;
    reset();
  }
 // Hver mandag kl 00.00 blir ukespoengtelleren resatt.  
  
  if(rtc.getTime().dow== 1 && rtc.getTime().hour == 0 && rtc.getTime().min == 00 && rtc.getTime().sec == 00){
    resetWeek();
  }
  
  
 
// periode for dagen er definert som mellom 05:00 og 10:00.
  if(rtc.getTime().hour >=05 && rtc.getTime().hour <= 10){
    day = true;
  
  }
  else{
    day = false;
  
  }
  // perioden for kvelden er definert som mellom 17:00 og 23:00
  if(rtc.getTime().hour >17 && rtc.getTime().hour<=23){
    night = true;
  }
  
  else{
    night = false;
  }
  
  //doneBlink();

  time = millis();
  tellerPrev = teller;
  
 // kjoerer metoden som registrerer om tiltswitchen har blitt ristet. 
  switching();
 // Sjekker hva tiden er, og om noen av lysene til uke-lysene skal lyse opp. 
  checkTime();

  
      currentMillis3 = millis();
     // Dersom tiden er utenfor "dag"-perioden og "kveld"-perioden eller om "donna" (om barnet har pusset ferdig) er true, blir tannboersten satt til inaktiv. 
      
     if((day == false && night == false) || donna  == true){
        inActive = true;
     }
     
    // dersom tannboersten ikke er inaktiv, kjoeres aktiv-metoden
      else if (inActive == false){
      active();
    }
 
    

    


}
//  Metode for registrering av tilt switch-tilstandsendringer

void switching(){
   switchStateRed = digitalRead(resetButton);
  switchStateTilt = digitalRead(tiltSwitch);
  
  if (switchStateTilt == 0) {  
    tiltState = true;
  }
  
  
  else {
    tiltState = false;
    limit = false;
  }
  // dersom den registrer en ny "tilt", kjoerer den tellerTid(); 
   if (tiltState == true && limit == false && donna == false) {
    
    tellerTid();
    limit = true;
    
    
  }
  
  
}
// lagrer tiden naar tiltswitchen endret tilstand
// den er naa aktiv og ikke inaktiv.
void tellerTid(){

  millisTellerOpp = millis();

  inActive = false;
}

// sjekker tiden som er naa og aktiverer ulike lys paa ukes-lys-stripen dersom de oppfyller de riktige kravene.
void checkTime(){

  // ***************** MONDAY*********************
  if(rtc.getTime().dow == 1 && brushedDay == true){
      stripDays.setPixelColor(0, 100, 100, 0);
      stripDays.show();
  }
  if(rtc.getTime().dow == 1 && brushedNight == true){
    if (brushedDay == true) {
      stripDays.setPixelColor(0, 0, 100, 0);
      stripDays.show();
      mandagBo = true;
    }
    else {
      stripDays.setPixelColor(0, 100, 100, 0);
      stripDays.show();
    }
    
  }
  if(rtc.getTime().dow == 1 && rtc.getTime().hour == 23 && rtc.getTime().min == 59 && rtc.getTime().sec == 59 && brushedDay == false && brushedNight == false){
    stripDays.setPixelColor(0, 0, 0, 0);
    stripDays.show();
    
    
  }
  
  // **********************TUESDAY**************************
    if(rtc.getTime().dow == 2 && rtc.getTime().hour == 0 && rtc.getTime().min == 0 && rtc.getTime().sec == 0){
     brushedNight == false;
     brushedDay == false;
    
    
  }
  if(rtc.getTime().dow == 2 && brushedDay == true){
      stripDays.setPixelColor(1, 100, 100, 0);
      stripDays.show();
   
  }
  if(rtc.getTime().dow == 2 && brushedNight == true){
    if (brushedDay == true) {
      stripDays.setPixelColor(1, 0, 100, 0);
      stripDays.show();
      tirsdagBo = true;
    }
    else {
      stripDays.setPixelColor(1, 100, 100, 0);
      stripDays.show();
    }
    
  }
  if(rtc.getTime().dow == 2 && rtc.getTime().hour == 23 && rtc.getTime().min == 59 && rtc.getTime().sec == 59 && brushedDay == false && brushedNight == false){
    stripDays.setPixelColor(1, 0, 0, 0);
    stripDays.show();
    
    
  }
  
    if(rtc.getTime().dow == 3 && rtc.getTime().hour == 0 && rtc.getTime().min == 0 && rtc.getTime().sec == 0){
     brushedNight == false;
     brushedDay == false;
    
    
  }
  
// *********************WEDNESDAY***********************  
  if(rtc.getTime().dow == 3 && brushedDay == true){
      stripDays.setPixelColor(2, 100, 100, 0);
      stripDays.show();

     
  }
  if(rtc.getTime().dow == 3 && brushedNight == true){
    if (brushedDay == true) {
      stripDays.setPixelColor(2, 0, 100, 0);
      stripDays.show();
      onsdagBo = true;

    }
    else {
      stripDays.setPixelColor(2, 100, 100, 0);
      stripDays.show();

    }
    
  }
  if(rtc.getTime().dow == 3 && rtc.getTime().hour == 23 && rtc.getTime().min == 59 && rtc.getTime().sec == 57 && brushedDay == false && brushedNight == false){
    stripDays.setPixelColor(2, 0, 0, 0);
    stripDays.show();
 
  }
  
    if(rtc.getTime().dow == 3 && rtc.getTime().hour == 23 && rtc.getTime().min == 59 && rtc.getTime().sec == 59){
     brushedNight = false;
     brushedDay = false;
     

    
    
  }
  
 //********************* THURSDAY ******************* 
  if(rtc.getTime().dow == 4 && brushedDay == true){
      stripDays.setPixelColor(3, 100, 100, 0);
      stripDays.show();

  }
  if(rtc.getTime().dow == 4 && brushedNight == true){
    if (brushedDay == true) {
      stripDays.setPixelColor(3, 0, 100, 0);
      stripDays.show();
      torsdagBo = true;

    }
    else {
      stripDays.setPixelColor(3, 100, 100, 0);
      stripDays.show();

      
    }
    
  }
  if(rtc.getTime().dow == 4 && rtc.getTime().hour == 23 && rtc.getTime().min == 59 && rtc.getTime().sec == 57 && brushedDay == false && brushedNight == false){
    stripDays.setPixelColor(3, 0, 0, 0);
    stripDays.show();
    
 
    
    
  }
  
    if(rtc.getTime().dow == 5 && rtc.getTime().hour == 0 && rtc.getTime().min == 0 && rtc.getTime().sec == 0){
     brushedNight == false;
     brushedDay == false;

    
  }
  
  
 // ******************* FRIDAY ***********************
 
   if(rtc.getTime().dow == 5 && brushedDay == true){
      stripDays.setPixelColor(4, 100, 100, 0);
      stripDays.show();
  }
  if(rtc.getTime().dow == 5 && brushedNight == true){
    if (brushedDay == true) {
      stripDays.setPixelColor(4, 0, 100, 0);
      stripDays.show();
      fredagBo = true;
    }
    else {
      stripDays.setPixelColor(4, 100, 100, 0);
      stripDays.show();
    }
    
  }
  if(rtc.getTime().dow == 5 && rtc.getTime().hour == 23 && rtc.getTime().min == 59 && rtc.getTime().sec == 59 && brushedDay == false && brushedNight == false){
    stripDays.setPixelColor(4, 0, 0, 0);
    stripDays.show();
    
    
  }
  
    if(rtc.getTime().dow == 6 && rtc.getTime().hour == 0 && rtc.getTime().min == 0 && rtc.getTime().sec == 0){
     brushedNight == false;
     brushedDay == false;
    
    
  }
  
// ********************* SATURDAY **********************
  if(rtc.getTime().dow == 6 && brushedDay == true){
      stripDays.setPixelColor(5, 100, 100, 0);
      stripDays.show();

  }
  if(rtc.getTime().dow == 6 && brushedNight == true){
    if (brushedDay == true) {
      stripDays.setPixelColor(5, 0, 100, 0);
      stripDays.show();
      lordagBo = true;
    }
    else {
      stripDays.setPixelColor(5, 100, 100, 0);
      stripDays.show();
    }
    
  }
  if(rtc.getTime().dow == 6 && rtc.getTime().hour == 23 && rtc.getTime().min == 59 && rtc.getTime().sec == 59 && brushedDay == false && brushedNight == false){
    stripDays.setPixelColor(5, 0, 0, 0);
    stripDays.show();
    
    
  }
  
    if(rtc.getTime().dow == 7 && rtc.getTime().hour == 0 && rtc.getTime().min == 0 && rtc.getTime().sec == 0){
     brushedNight == false;
     brushedDay == false;
    
    
  }
  
  
// **************** SUNDAY ****************

  if(rtc.getTime().dow == 7 && brushedDay == true){
      stripDays.setPixelColor(6, 100, 100, 0);
      stripDays.show();
  }
  if(rtc.getTime().dow == 7 && brushedNight == true){
    if (brushedDay == true) {
      stripDays.setPixelColor(6, 0, 100, 0);
      stripDays.show();
      sondagBo = true;
    }
    else {
      stripDays.setPixelColor(6, 100, 100, 0);
      stripDays.show();
    }
    
  }
  if(rtc.getTime().dow == 7 && rtc.getTime().hour == 23 && rtc.getTime().min == 59 && rtc.getTime().sec == 59 && brushedDay == false && brushedNight == false){
    stripDays.setPixelColor(6, 0, 0, 0);
    stripDays.show();
    
    
  }
  
    if(rtc.getTime().dow == 1 && rtc.getTime().hour == 0 && rtc.getTime().min == 0 && rtc.getTime().sec == 0){
     brushedNight == false;
     brushedDay == false;
    
    
  }
  
  //********ENDCHECK***********
 
     if (mandagBo == true && tirsdagBo == true && onsdagBo == true && torsdagBo == true && fredagBo == true && lordagBo == true && sondagBo == true) {
        stripDays.setPixelColor(7, 0, 255, 0);
        stripDays.show(); 
     }
     else {
        stripDays.setPixelColor(7, 0, 0, 0);
        stripDays.show(); 
     }
  

}
// Denne metoden blir kjoert hver gang tannboersten blir satt som aktiv..
void active(){
 // aktivitetene i aktiv faar kun skje dersom tiden er innenfor den definerte "dag"-perioden eller "kveld"-perioden 
  if (night == true || day == true) {
  
   
    millisActiveStart = millis(); // lagrer tiden paa naar aktiv starter aa kjoere
    
    while(inActive == false && donna == false && brushedDay == false){ // kjoerer saa lenge tannboersten er aktiv, barnet ikke er ferdig med aa pusse.
// sjekker om tiltswitchen har endret tilstand
      switching();
        

      switching();
    
     currentMillis2 = millis(); // lagrer tiden som er naa
// millisActive (den tiden tannboersten har vaert aktiv) er forholdet mellom tiden som ble lagret naa og tiden som ble lagret da tannboersten foerst ble aktiv
// i tillegg til den tiden tannboersten hadde vaert aktiv foer den ble inaktiv. (dette er hvis man har tatt en pause fra boerstingen mens "pussesesjonen" fortsatt er i gang.
      millisActive = millisFoerInaktiv +(currentMillis2 - millisActiveStart) - millisReset;

      currentMillis = millis(); // lagrer tiden som er naa
       // dersom forholdet mellom tiden som er naa og tiden som ble lagret da tilt switchen sist endret tilstand er over tre sekunder settes tannboersten som inaktiv.
       // dette er for naar barnet tar en pause fra pussingen for enten aa sjekke tennene, gaa paa do eller andre ting som kommer opp
     if((currentMillis - millisTellerOpp)>=3000 ){
       inActive = true;
       millisFoerInaktiv = millisActive; // lagrer tiden for hvor lenge tannboersten hadde vaert aktiv foer den ble inaktiv
       

     }
     
   // dersom boersten har vaert aktiv i en tolvtedel av tiden som er satt for optimalpussetid, skal det foerste lyset lyse
  // progresjonen av pussetid oeker med ett og ett lys
        if ( millisActive < (lengde/12)) {

     for(int i=0; i<12; i++){
       strip.setPixelColor(i, 0,0,0);
     }
     
     
     
     strip.show();

     }
     

     if ( millisActive >= (lengde/12) && millisActive< ((lengde/12) * 2)) {

     
       
  
       strip.setPixelColor(0, colorR,colorG,colorB);
  
     
     strip.show();
   
     }
     if ( millisActive >= ((lengde/12) * 2) && millisActive<((lengde/12) * 3)) {

  
         strip.setPixelColor(1, colorR,colorG,colorB);
  
       strip.show();

        
     }
        if ( millisActive >= ((lengde/12) * 3) && millisActive<((lengde/12) * 4)) {

       strip.setPixelColor(2, colorR,colorG,colorB);
  
        }
        
              if ( millisActive >= ((lengde/12) * 4) && millisActive<((lengde/12) * 5)) {

       strip.setPixelColor(3, colorR,colorG,colorB);
       strip.show();
  
        }
              if ( millisActive >= ((lengde/12) * 5) && millisActive<((lengde/12) * 6)) {
;
       strip.setPixelColor(4, colorR,colorG,colorB);
       strip.show();
  
        }
              if ( millisActive >= ((lengde/12) * 6) && millisActive<((lengde/12) * 7)) {

       strip.setPixelColor(5, colorR,colorG,colorB);
       strip.show();
  
        }
        
              if ( millisActive >= ((lengde/12) * 7) && millisActive<((lengde/12) * 8)) {

       strip.setPixelColor(6, colorR,colorG,colorB);
       strip.show();
  
        }
              if ( millisActive >= ((lengde/12) * 8) && millisActive<((lengde/12) * 9)) {

       strip.setPixelColor(7, colorR,colorG,colorB);
       strip.show();
  
        }
        
              if ( millisActive >= ((lengde/12) * 9) && millisActive<((lengde/12) * 10)) {
  
       strip.setPixelColor(8, colorR,colorG,colorB);
  
        }
              if ( millisActive >= ((lengde/12) * 10) && millisActive<((lengde/12) * 11)) {

       strip.setPixelColor(9, colorR,colorG,colorB);
       strip.show();
  
        }
              if ( millisActive >= ((lengde/12) * 11) && millisActive<lengde) {
 
       strip.setPixelColor(10, colorR,colorG,colorB);
       strip.show();
  
        }
      // dersom tannboersten har vaert aktiv for hele pussetiden og det er paa dagtid, vil den sette booleanen for dagen til true og donna (den er ferdig) til true. millisActive blir resatt til aa vaere 0
      else if ( millisActive >= lengde && day == true) {

       
       brushedDay = true;

           strip.setPixelColor(11, colorR,colorG,colorB);
  
        strip.show();
       donna  = true;
       inActive = true;
       
       millisActive = 0;
    
      }
      
      // dersom tannboersten har vaert aktiv for hele pussetiden og det er paa kveldstid, vil den sette booleanen for natten til true og donna til true. 
      else if ( millisActive >= lengde && night == true) {
  
       brushedNight = true;
      
         strip.setPixelColor(11, colorR,colorG,colorB);
  
         strip.show();

       donna  = true;
       inActive = true;
        millisActive = 0;
      }
      
      

      
  
    }
  }
  
}
// denne metoden resetter alle verdiene til aa vaere 0. Den skrur ogsaa av alle lysene i progresjonsringen
void reset(){
  
  millisReset = currentMillis2;
  brushedNight =false;
  brushedDay =false;
  donna = false;
  millisActive = 0; 
  for(int i = 0; i<12; i++){
    strip.setPixelColor(i, 0, 0, 0);
    strip.show();
  }
  
}
// denne metoden skrur av alle lysene for ukesdag-lysstripen. 
// den resetter ogsaa millisActive og setter donna til false.
void resetWeek(){
 // inActive = true;
     
boolean mandagBo = false;
boolean tirsdagBo = false;
boolean onsdagBo = false;
boolean torsdagBo = false;
boolean fredagBo = false;
boolean lordagBo = false;
boolean sondagBo = false;
 // inActive = false;
  donna = false;
  millisActive = 0; 
  for(int i = 0; i<8; i++){
    stripDays.setPixelColor(i, 0, 0, 0);
    stripDays.show();
  }

}






