/* LedStripGradient: Example Arduino sketch that shows
 * how to control an Addressable RGB LED Strip from Pololu.
 *
 * To use this, you will need to plug an Addressable RGB LED
 * strip from Pololu into pin 12.  After uploading the sketch,
 * you should see a pattern on the LED strip that fades from
 * green to pink and also moves along the strip.
 */

#include <PololuLedStrip.h>
#include <SoftwareSerial.h>


#define BUFFER_SIZE 3
#define LED_COUNT 60

#define HC12RxPin 9
#define HC12TxPin 10
#define HC12SetPin 11

SoftwareSerial hc12(HC12TxPin, HC12RxPin);
#define HC12 hc12

PololuLedStrip<4> ledStrip;

rgb_color colors[LED_COUNT];

void (*draw_func) () = nullptr;

bool changed = true;

struct rainbow_color
{
  uint8_t index;
  uint8_t value;
};

void rainbow_step(rainbow_color &color)
{
  rainbow_color next_color;
  if(color.value == 1)
  {
    color.index = (color.index+1)%3;
    color.value = 255;
  }
  else
  {
    color.value = color.value-1;
  }
}

rgb_color toRGB(rainbow_color color)
{
  uint8_t rgb_arr[3] = {0,0,0};
  rgb_arr[color.index] = color.value;
  rgb_arr[(color.index+1)%3] = 255 - color.value;
  
  return rgb_color(rgb_arr[0],rgb_arr[1],rgb_arr[2]);
}

rainbow_color rainbow;

rgb_color interpolate(rgb_color c1, rgb_color c2, float percent)
{
  return rgb_color(
      static_cast<byte>((int)c1.red + percent*((int)c2.red-(int)c1.red)),
      static_cast<byte>((int)c1.green + percent*((int)c2.green-(int)c1.green)),
      static_cast<byte>((int)c1.blue + percent*((int)c2.blue-(int)c1.blue))
    );
}

void draw_rainbow()
{
  rainbow_step(rainbow);
  rainbow_color copy = rainbow;
  rgb_color c = toRGB(copy);
  colors[0] = c;
  for(uint8_t i = 1; i < LED_COUNT/2; ++i)
  {
    for(uint8_t j = 0; j < 255*3/(LED_COUNT/2-1); ++j)
    {
      rainbow_step(copy);
    } 
    colors[LED_COUNT/2-1-i] = colors[LED_COUNT/2-1+i] = toRGB(copy);
  }
  ledStrip.write(colors, LED_COUNT);
  delay(10);
}

void draw_pulse(rgb_color base, rgb_color pulse, int length)
{
  static int currentIndex = 0;
  currentIndex = changed ? 0 : currentIndex;
  changed = false;
  for (int i = 0; i < LED_COUNT; i++)
  {
    if(i > currentIndex)
    {
      colors[i] = base;
    }
    else if ( abs(i-currentIndex) < length)
    {
      colors[i] = interpolate(pulse, base, abs(i-currentIndex)/(float)length);
    }
    else
    {
      colors[i] = base;
    }
  }

  currentIndex = (currentIndex+1)%(LED_COUNT+length);

  // Write the colors to the LED strip.
  ledStrip.write(colors, LED_COUNT);
  delay(5);
}
  
void draw_double_pulse(rgb_color base, rgb_color pulse, int length)
{
  static int currentIndex = 0;
  currentIndex = changed ? 0 : currentIndex;
  changed = false;
  for (int i = 0; i < LED_COUNT/2; i++)
  {
    if(i > currentIndex)
    {
      colors[i] = base;
      colors[LED_COUNT-1-i] = base;
    }
    else if ( abs(i-currentIndex) < length)
    {
      colors[i] = interpolate(pulse, base, abs(i-currentIndex)/(float)length);
      colors[LED_COUNT-1-i] = interpolate(pulse, base, abs(i-currentIndex)/(float)length);
    }
    else
    {
      colors[i] = base;
      colors[LED_COUNT-1-i] = base;
    }
  }

  currentIndex = (currentIndex+1)%(LED_COUNT+length);

  // Write the colors to the LED strip.
  ledStrip.write(colors, LED_COUNT);
  delay(10);
}

void draw_white()
{
  static int counter = 0;
  counter = changed ? 0 : counter;
  changed = false;
  bool all_lights = true;
  for (int i = 0; i < LED_COUNT; i++)
  {
    colors[i] = counter ? interpolate(rgb_color(0,0,0), rgb_color(255,255,255), counter / 1000.0) : rgb_color(0,0,0);
    all_lights = all_lights && colors[i].red == 255 && colors[i].green == 255 && colors[i].blue == 255;
  }
  ++counter;
  ledStrip.write(colors, LED_COUNT);
  if(all_lights)
  {
    draw_func = nullptr;
    counter = 0;
  }
  else
    delay(5);
}

void draw_night()
{
  static int counter = 0;
  counter = changed ? 0 : counter;
  changed = false;
  bool all_lights = true;
  for (int i = 0; i < LED_COUNT; i++)
  {
    colors[i] = counter ? interpolate(rgb_color(0,0,0), rgb_color(255,80,0), counter / 1000.0) : rgb_color(0,0,0);
    all_lights = all_lights && colors[i].red == 255 && colors[i].green == 80 && colors[i].blue == 0;
  }
  ++counter;
  ledStrip.write(colors, LED_COUNT);
  if(all_lights)
  {
    draw_func = nullptr;
    counter = 0;
  }
  else
    delay(5);
}

void draw_green()
{
  static int counter = 0;
  counter = changed ? 0 : counter;
  changed = false;
  bool all_lights = true;
  for (int i = 0; i < LED_COUNT; i++)
  {
    colors[i] = counter ? interpolate(rgb_color(0,0,0), rgb_color(0,255,0), counter / 1000.0) : rgb_color(0,0,0);
    all_lights = all_lights && colors[i].red == 0 && colors[i].green == 255 && colors[i].blue == 0;
  }
  ++counter;
  ledStrip.write(colors, LED_COUNT);
  if(all_lights)
  {
    draw_func = nullptr;
    counter = 0;
  }
  else
    delay(5);
}

void draw_green_pulse()
{
  //draw_pulse(rgb_color(0,0,0), rgb_color(0,50,0), 30);
  // Update the colors.
  byte time = millis() >> 0;
  for (uint16_t i = 0; i < LED_COUNT; i++)
  {
    byte x = time - 16*i;
    colors[i] = rgb_color(0, 255 - x/10, 0);
  }

  // Write the colors to the LED strip.
  ledStrip.write(colors, LED_COUNT);
}

void draw_green_double_pulse()
{
  //draw_double_pulse(rgb_color(0,0,0), rgb_color(0,50,0), 50);
  // Update the colors.
  byte time = millis() >> 1;
  for (uint16_t i = 0; i < LED_COUNT/2; i++)
  {
    byte x = time - 12*i;
    colors[i] = rgb_color(0, 255 - x/8, 0);
    colors[LED_COUNT-i-1] = rgb_color(0, 255 - x/8, 0);
  }

  // Write the colors to the LED strip.
  ledStrip.write(colors, LED_COUNT);
}

void draw_fenix_pulse()
{
  draw_pulse(rgb_color(0,255,0), rgb_color(255,255,255), 30);
}

void draw_fenix_double_pulse()
{
  draw_double_pulse(rgb_color(0,255,0), rgb_color(255,255,255), 50);
}

void disable()
{
  bool all_black = true;
  for(byte i =0; i<LED_COUNT;++i)
  {
    colors[i] = rgb_color(
      colors[i].red * 0.999,
      colors[i].green * 0.999,
      colors[i].blue * 0.999
      );
    all_black = all_black && colors[i].red == 0 && colors[i].green == 0 && colors[i].blue == 0;
  }
  ledStrip.write(colors, LED_COUNT);
  if(all_black)
  {
    draw_func = nullptr;
  }
  delay(10);
}

void handle_HC12_command()
{
  if(!HC12.available())
    return;
  byte code = HC12.read();
  Serial.print("Read ");
  Serial.println((char)code);
  switch(code)
  {
    case '0':
    draw_func = &draw_green;
    changed = true;
    break;
    case '1':
    draw_func = &draw_white;
    changed = true;
    break;
    case '2':
    draw_func = &draw_night;
    changed = true;
    break;
    case '3':
    draw_func = &draw_green_pulse;
    changed = true;
    break;
    case '4':
    draw_func = &draw_green_double_pulse;
    changed = true;
    break;
    case '5':
    draw_func = &draw_fenix_pulse;
    changed = true;
    break;
    case '6':
    draw_func = &draw_fenix_double_pulse;
    changed = true;
    break;
    case '7':
    draw_func = &draw_rainbow;
    changed = true;
    break;
    case '9':
    draw_func = &disable;
    changed = true;
    break;
    default:
    Serial.print("Invalid code ");
    Serial.println((char) code);
  }
}

void setup()
{
  PololuLedStripBase::interruptFriendly = true;
  draw_func = &disable;
  HC12.begin(9600);
  Serial.begin(9600);
  Serial.setTimeout(1000);
  Serial.println("Jacked up and ready to go");  
  delay(100);
  rainbow.index = 1;
  rainbow.value = 255;
}

void loop()
{
  handle_HC12_command();
  if(draw_func != nullptr)
    draw_func();
}
