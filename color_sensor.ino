#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h> // This assumes you are using the ILI9341-based TFT display

// Define the pin connections for the TFT display
#define TFT_CS 10      // Chip select pin
#define TFT_RST 9      // Reset pin
#define TFT_DC 8       // Data/Command pin
#define TFT_MOSI 11    // SPI MOSI (Master Out Slave In) pin
#define TFT_SCK 13     // SPI SCK (Clock) pin
#define TFT_MISO 12    // SPI MISO (Master In Slave Out) pin

// Create an instance of the Adafruit_ILI9341 class with specified pin connections
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST, TFT_MISO);

//push button variables
int push_button_pin = 4;
bool button_released = true;

// Define pin numbers for the color sensor and output
const int out = 7; 
const int s0 = 3;
const int s1 = 2;  
const int s2 = 6;  
const int s3 = 5;  

// Variables to store the frequency / pulse duration for Red, Blue, and Green color
int pulse_duration_red, pulse_duration_blue, pulse_duration_green;

//Variables to store value of Red, Blue, and Green color 
int red_color, blue_color, green_color;

//Variables to store the lowest and highest pulse duration for Red, Blue, and Green color.
int min_duration_red, min_duration_blue, min_duration_green;
int max_duration_red, max_duration_blue, max_duration_green;

    
void setup()   
{  
   // Initialize the TFT display
  tft.begin();
  // Set display rotation (1 for 90 degrees clockwise)
  tft.setRotation(1);
    // Set text size
  tft.setTextSize(2);



  //Setting up push button pin
  pinMode(push_button_pin, INPUT);
  digitalWrite(push_button_pin, HIGH);


  //Setting lowest and highest pulse duration for Red, Blue, and Green color as noted during the test run
  min_duration_red = 9;
  max_duration_red = 53;
  min_duration_blue = 5;
  max_duration_blue = 40;
  min_duration_green = 6;
  max_duration_green = 50;

  // Start serial communication at 9600 bps
  Serial.begin(9600); 

  // Set pin modes for the color sensor and output
  pinMode(s0, OUTPUT);  
  pinMode(s1, OUTPUT);  
  pinMode(s2, OUTPUT);  
  pinMode(s3, OUTPUT);  
  pinMode(out, INPUT);  

  // Set initial conditions for frequency scaling
  digitalWrite(s0, HIGH);  
  digitalWrite(s1, HIGH);  
}  


void loop() 
{  
   //Reading push button state
   int PinState = digitalRead(push_button_pin);
   //Detecting the color when push button is pressed. Avoid recalling the color function until push button
   //released and pressed again.
   if((PinState == 0) && (button_released == true)){
     button_released = false;
     color();
   }
   if((button_released == false) && (PinState == 1)){
     button_released = true;
   }
   
}


// Function to detect and print the dominant color
void color()  
{    
  
  // Set the sensor to detect the red color
  digitalWrite(s2, LOW);  
  digitalWrite(s3, LOW);   
  //small pause
  delay(50); 
  // Measure the pulse duration (frequency) for the red color
  pulse_duration_red = pulseIn(out, LOW);
  //updating the min pulse duration for red if smaller than previous min pulse duration
  if(pulse_duration_red < min_duration_red){
    min_duration_red = pulse_duration_red;
  }
  //updating the max pulse duration for red if smaller than previous max pulse duration
  if(pulse_duration_red > max_duration_red){
    max_duration_red = pulse_duration_red; 
  }


  // Set the sensor to detect the blue color
  digitalWrite(s2, LOW);
  digitalWrite(s3, HIGH); 
  //small pause
  delay(50);  
  // Measure the pulse duration (frequency) for the blue color
  pulse_duration_blue = pulseIn(out, LOW);
  //updating the min pulse duration for blue if smaller than previous min pulse duration
  if(pulse_duration_blue < min_duration_blue){
    min_duration_blue = pulse_duration_blue;
  }
  //updating the max pulse duration for blue if smaller than previous max pulse duration
  if(pulse_duration_blue > max_duration_blue){
    max_duration_blue = pulse_duration_blue; 
  }


  // Set the sensor to detect the Green color
  digitalWrite(s2, HIGH);
  digitalWrite(s3, HIGH);  
  //small pause
  delay(50);  
  // Measure the pulse duration (frequency) for the green color
  pulse_duration_green = pulseIn(out, LOW);
  //updating the min pulse duration for green if smaller than previous min pulse duration
  if(pulse_duration_green < min_duration_green){
   min_duration_green = pulse_duration_green;
  }
  //updating the max pulse duration for green if smaller than previous max pulse duration
  if(pulse_duration_green > max_duration_green){
   max_duration_green = pulse_duration_green; 
  }

 //using the map function to get Red, Blue, Green color values, between 0 and 255, from pulse duration
  red_color = map(pulse_duration_red, min_duration_red, max_duration_red, 255, 0);
  green_color = map(pulse_duration_green, min_duration_green, max_duration_green, 255, 0);
  blue_color = map(pulse_duration_blue, min_duration_blue, max_duration_blue, 255, 0);

 //Using the RGB value as a background color of the TFT screen  
  uint16_t rgbColor = tft.color565(red_color, green_color, blue_color);
  tft.fillScreen(rgbColor);

  //printing RGB value on tft screen.
  // Set the cursor position for text drawing (coordinates 50, 50)
  tft.setCursor(30, 50);
  //If background is dark, using light font for the text. 
  //If background is light, using dark font for the text.
  //using luminance formula, R, G, and B are the RGB values normalized to the range [0, 1]
  if(get_luminance(float(red_color)/255.0, float(green_color)/255.0, float(blue_color)/255.0) > 0.5){ //background is light color
    //Dark font color for light background
    // Set text color to black
    tft.setTextColor(ILI9341_BLACK);
  }
  else{//background is dark color
    //Light font color for dark background
    // Set text color to white
    tft.setTextColor(ILI9341_WHITE);
  }
  //printing on the TFT
  tft.print("RGB ( "+String(red_color)+" , "+String(green_color)+" , "+String(blue_color)+" ) ");

}



float get_luminance(float R, float G, float B){
    float luminance = 0.2126 * R + 0.7152 * G + 0.0722 * B;
    return luminance;
 }