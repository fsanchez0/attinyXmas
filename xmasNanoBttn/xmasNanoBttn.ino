#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define BUTTON_PIN   2    // Digital IO pin connected to the button.  This will be
                          // driven with a pull-up resistor so the switch should
                          // pull the pin to ground momentarily.  On a high -> low
                          // transition the button press logic will execute.

#define PIXEL_PIN    4   // Digital IO pin connected to the NeoPixels.

#define PIXEL_COUNT 145   // number of neopixel (change this accordingly)

#define LED_PIN 3

// Parameter 1 = number of pixels in strip,  neopixel stick has 8
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream, correct for neopixel stick
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip), correct for neopixel stick
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

bool oldState = HIGH;
int showType = 0;
int pixelIterator = 0;
int colorIterator = 0;
unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 50;  //the value is a number of milliseconds

void setup() {
   // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(150);
  //startMillis = millis();

  digitalWrite(LED_PIN, HIGH); 
  delay(1000);
  digitalWrite(LED_PIN, LOW); 
}

void loop() {
  // Get current button state.

  currentMillis = millis();
  //bool buttonIsPressed = digitalRead(BUTTON_PIN);

  if(digitalRead(BUTTON_PIN)){
    while(digitalRead(BUTTON_PIN)){
      digitalWrite(LED_PIN, HIGH);
    }

    // set all variables again 
    pixelIterator = 0;
    colorIterator = 0;
    showType++;
      if (showType > 3)
        showType=0;
    digitalWrite(LED_PIN, LOW);
  }else{
    startShow(showType);
  }
}

void startShow(int i) {
  switch(i){
    case 0: colorWipe(strip.Color(0, 50, 0), 50);    // some green of ready
            break;
    case 1: rainbow(20);
            break;
    case 2: rainbowCycle(20);
            break;
    case 3: turnOff();
            break;
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t color, uint8_t wait) {
  /*for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }*/
  if(pixelIterator < PIXEL_COUNT && currentMillis - startMillis >= wait){
    strip.setPixelColor(pixelIterator, color);
    strip.show();
    // increase iterator
    pixelIterator++;
    // new count
    startMillis = currentMillis;
  }
  if(pixelIterator == PIXEL_COUNT){
    pixelIterator = 0;
    for( int i = 0; i<PIXEL_COUNT; i++){
        strip.setPixelColor(i, strip.Color(0, 0, 0));
    }   
    strip.show();
  }
}

void turnOff() {
  for( int i = 0; i<PIXEL_COUNT; i++){
        strip.setPixelColor(i, strip.Color(0, 0, 0));
    }   
    strip.show();
}

void rainbow(uint8_t wait) {
  if(colorIterator < 256 && currentMillis - startMillis >= wait){
    for(pixelIterator=0; pixelIterator<PIXEL_COUNT; pixelIterator++) {
      strip.setPixelColor(pixelIterator, Wheel((pixelIterator+colorIterator) & 255));
    }
    strip.show();
    // increase iterator
    colorIterator++;
    // new count
    startMillis = currentMillis;
  }
  if(colorIterator == 256){
    colorIterator = 0;
    /*for( int i = 0; i<PIXEL_COUNT; i++){
        strip.setPixelColor(i, strip.Color(0, 0, 0));
    }   
    strip.show();*/
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  /*uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }*/

  if(colorIterator < 256*5 && currentMillis - startMillis >= wait){
    for(pixelIterator=0; pixelIterator<PIXEL_COUNT; pixelIterator++) {
      strip.setPixelColor(pixelIterator, Wheel(((pixelIterator*256/PIXEL_COUNT) + colorIterator) & 255));
    }
    strip.show();
    // increase iterator
    colorIterator++;
    // new count
    startMillis = currentMillis;
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
