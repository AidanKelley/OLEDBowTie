#include "book.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "URW_Gothic_L_Book_8.h"
#include "URW_Gothic_L_Book_15.h"
#include "URW_Gothic_L_Book_20.h"
#include <ESP8266WiFi.h>

Adafruit_SSD1306 display(4);
String currentWord = "";
int ind = 0;
char current = ' ';

String getMaxString(String w, int s, uint16_t* width);

volatile int currentBook = 0;
volatile int currentBookLength = godot1Length;
volatile const char* book = godot1;
const int numBooks = 1;//10;
String title[3] = godot1Title;

volatile int pos = 0;
volatile bool lastEncoderState = false;
volatile bool calcInd = false;

enum State {reading, clock, contrast};

State state = reading;

//contrast vars
uint8_t contr = 127;


//timing variables
unsigned long lastRender = 0;
bool shouldRender = true;
bool contrastChanged = false;
unsigned long nextClockChange = 60000;
unsigned long nextWordChange = 0;

int hours = 12;
int minutes = 00;
int clockPos = 1;

unsigned long lastStateButtonPress = 0;
unsigned long lastDetailButtonPress = 0;

void stateButtonPressed() {
    if(millis() > lastStateButtonPress + 200) {
        lastStateButtonPress = millis();
        switch(state) {
            case reading:
            state = contrast;
            break;
            case clock:
            switch(clockPos) {
                case 1:
                printTimeDigit(hours / 10, 0, true);
                printTimeDigit(hours % 10, 1, true);
                printTimeDigit(minutes / 10, 2, false);
                break;
    
                case 2:
                printTimeDigit(minutes / 10, 2, true);
                printTimeDigit(minutes % 10, 3, false);
                break;
    
                case 3:
                printTimeDigit(minutes % 10, 3, true);
                state = reading;
                nextWordChange = millis();
                nextClockChange = millis() + 60000;
                break;
                default:break;
            }
            shouldRender = true;
            clockPos++;
            break;
            case contrast:
            state = clock;
            printTimeDigit(hours / 10, 0, false);
            printTimeDigit(hours % 10, 1, false);
            clockPos = 1;
            shouldRender = true;
            break;
            default:
            break;
        }
    }
}

void detailButtonPressed() {
    if(millis() > lastDetailButtonPress + 200) {
        lastDetailButtonPress = millis();
        switch(state) {
            case reading:
            changeBook((currentBook + 1) % numBooks);
            break;
            case clock:
            printTime();
            shouldRender = true;
            state = reading;
            nextWordChange = millis();
            break;
            case contrast:
            state = reading;
            nextWordChange = millis();
            break;
            default:break;
        }
    }
}

void encoder() {
    bool encoderState = digitalRead(D6);
    bool incr;
    if(encoderState != lastEncoderState) {
        if(encoderState != digitalRead(D7)) {
            pos++;
            incr = true;
        }
        else {
            pos--;
            incr = false;
        }
        lastEncoderState = encoderState;

        switch(state) {
                case reading:
                if(incr) {
                    ind += currentBookLength/288 + 24;
                }
                else {
                    ind -= currentBookLength/288 + 24;
                }
                
                if(ind < 0) {
                    ind = 0;
                }
                calcInd = true;
                nextWordChange = millis();
                break;
                case contrast:
                if(incr) {
                    if(contr < 123) {
                        contr += 4;
                    }
                    else {
                        contr = 127;
                    }
                }
                else {
                    if(contr > 4){
                        contr -= 4;
                    }
                    else {
                        contr = 0;
                    }
                }
                contrastChanged = true;
                shouldRender = true;
                break;
                
                case clock:
                switch(clockPos) {
                    case 1:
                    if(incr) {
                        hours += 1;
                    }
                    else {
                        hours -= 1;
                    }
                    if(hours < 1) {
                        hours = 12;
                    }
                    else if(hours > 12) {
                        hours = 1;
                    }
                    printTimeDigit(hours / 10, 0, false);
                    printTimeDigit(hours % 10, 1, false);
                    shouldRender = true;
                    break;
                    case 2:
                    if(incr) {
                        minutes += 10; 
                        if(minutes >= 60) {
                            minutes = minutes % 60;
                        }
                    }
                    else {
                        if(minutes > 10) {
                            minutes -= 10;
                        }
                        else {
                            minutes = minutes % 10 + 50;
                        }
                    }

                    printTimeDigit(minutes / 10, 2, false);
                    shouldRender = true;
                    break;
                    
                    case 3:
                    if(incr) {
                        if(minutes % 10 == 9) {
                            minutes -= 9;
                        }
                        else {
                            minutes++;
                        }
                    }
                    else {
                        if(minutes % 10 == 0) {
                            minutes += 9;
                        }
                        else {
                            minutes--;
                        }
                    }
                    printTimeDigit(minutes % 10, 3, false);
                    shouldRender = true;
                    break;
                    
                    default:break;
                }
                
                break;
                
                default: break;
            }
        
    }

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
//Serial.end();
  WiFi.forceSleepBegin();
  delay(1);
  Wire.begin(D5, D1);
  Wire.setClock(800000L);

  pinMode(D2, INPUT_PULLUP);
  pinMode(D3, INPUT_PULLUP);

  pinMode(D6, INPUT_PULLUP);
  pinMode(D7, INPUT_PULLUP);

  lastEncoderState = digitalRead(D6);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setRotation(1);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(16, 0);
  display.display();
  display.setTextWrap(false);

  attachInterrupt(digitalPinToInterrupt(D6), encoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(D3), stateButtonPressed, FALLING);
  attachInterrupt(digitalPinToInterrupt(D2), detailButtonPressed, FALLING);


  changeBook(0);
  printTime();
}

void loop() {     

  switch(state) {
    case reading:
      if(millis() >= nextWordChange) {
          current = pgm_read_byte((const void*) (book + ind));
          
          if(calcInd) {
            calcInd = false;
        
            while (!(current == ' ' || current == '\n' || current == '\r' || current == '\t') && ind > 0) {
                ind--;
                current = pgm_read_byte((const void*) (book + ind));
            }
          }
          
          currentWord = "";
        
          while(current == ' ' || current == '\n' || current == '\r' || current == '\t') {
            ind++;
            current = pgm_read_byte((const void*) (book + ind));
          }
          
          while (!(current == ' ' || current == '\n' || current == '\r' || current == '\t')) {
            currentWord += current;
            ind++;
            current = pgm_read_byte((const void*) (book + ind));
          }
          ind++;
          Serial.println(ind);
          printProgressBar(float(ind) / float(currentBookLength));
          printWrapping(currentWord);
        
          if(ind > currentBookLength) {
            ind = 0;
          }
      }
    break;
    default: break;
  }

  if(state != clock && millis() >= nextClockChange) {
    nextClockChange += 60000;

    minutes++;

    if(minutes >= 60) {
        minutes = 0;
        hours++;
        if(hours > 12) {
            hours = 1;
        }
    }
    
    printTime();
  }

  if(shouldRender && millis() >= lastRender + 33) {
    shouldRender = false;
    lastRender = millis();
    if(contrastChanged) {
        contrastChanged = false;
        Serial.println(contr);
        display.ssd1306_command(SSD1306_SETCONTRAST);
        display.ssd1306_command(contr);
    }
    Serial.print("Render: "); Serial.println(millis());
    display.display();
  }
}

void printWrapping(String w) {
    clearBookText();
    display.setFont(&URW_Gothic_L_Book_20);
    int len = (int) w.length();
    int i = 0;
    int line = 0;
    String toPrint[4];
    uint16_t width[4];
    uint16_t wid = 0;
    while(i < len && line < 4) {
        toPrint[line] = getMaxString(w, &i, &wid);
        width[line] = wid;
//        Serial.println(toPrint[line]); Serial.println(i);
        line++;
    }

    int startY = 32;
    if(line > 3) {
        startY = 20;
    }
    for(int j = 0; j < line; j++) {
        display.setCursor(39 - (width[j])/2, startY + j * 24);
        display.print(toPrint[j]);
//        Serial.print("j: "); Serial.print(j); Serial.print(width[j]); Serial.println(toPrint[j]);
    }
    shouldRender = true;
    nextWordChange += 300 + 50 * len+ 50*line;
//    display.display();
//    delay(200 + 200*line);
}

String getMaxString(String w, int* s, uint16_t* width) {
    String prevString = "";
    String currentString = "";
    uint16_t prevW = 0;
    for(int i = *s + 1, l = w.length(); i <= l; i ++) {
        currentString = w.substring(*s, i);
        //test if fits. If doesnt fit, return prevString.

        int16_t x1, y1;
        uint16_t w, h;
        char charBuff[i - *s + 2];
        currentString.toCharArray(charBuff, i - *s + 2);
        display.getTextBounds(charBuff, (int16_t) 16, (int16_t) 128, &x1, &y1, &w, &h);

        if(w > 46) {
            *s = i - 1;
            *width = prevW;
            return prevString;
        }

        prevW = w;
        prevString = currentString;
    }

    *s = w.length();
    *width = prevW;
    return prevString;
}

void clearBookText() {
    display.fillRect(16, 0, 48, 98, BLACK);
}

void clearProgressBar() {
  display.fillRect(0, 64, 16, 64, BLACK);
}

void changeBook(int newBook) {
  currentBook = newBook;
  ind = 0;
  nextWordChange = millis();
//  switch (currentBook) {
//    case 1:
//      currentBookLength = godot2Length;
//      book = godot2;
//      memcpy(title, godot2Title, sizeof godot2Title);
//    break;
//    case 2:
//      currentBookLength = luckySpeechLength;
//      book = luckySpeech;
//      memcpy(title, luckySpeechTitle, sizeof luckySpeechTitle);
//      break;
//    case 3:
//        currentBookLength = joyceLength;
//        book = joyce;
//        memcpy(title, joyceTitle, sizeof joyceTitle);
//        break;
//    case 4:
//        currentBookLength = hamletLength;
//        book = hamlet;
//        memcpy(title, hamletTitle, sizeof hamletTitle);
//        break;
//    case 5:
//        currentBookLength = oneartLength;
//        book = oneart;
//        memcpy(title, oneartTitle, sizeof oneartTitle);
//        break;
//    case 6:
//        currentBookLength = macbethLength;
//        book = macbeth;
//        memcpy(title, macbethTitle, sizeof macbethTitle);
//        break;
//    case 7:
//        currentBookLength = bradburyLength;
//        book = bradbury;
//        memcpy(title, bradburyTitle, sizeof bradburyTitle);
//        break;
//    case 8:
//        currentBookLength = midsummerLength;
//        book = midsummer;
//        memcpy(title, midsummerTitle, sizeof midsummerTitle);
//        break;
//    case 9:
//        currentBookLength = austenLength;
//        book = austen;
//        memcpy(title, austenTitle, sizeof austenTitle);
//        break;
//    default:
      //waiting for godot act 1
      currentBookLength = godot1Length;
      book = godot1;
      memcpy(title, godot1Title, sizeof godot1Title);
//    break;
//  }
  printName();
  return;
}

void printName() {
    clearName();
    display.setFont(&URW_Gothic_L_Book_8);

    int len;
    
    int16_t x1, y1;
    uint16_t w, h;
    String toPrint;
    uint16_t x;
    for(int i = 0; i < 3; i++) {
        toPrint = title[i];

        len = toPrint.length() + 1;
        
        char charBuff[len];
        toPrint.toCharArray(charBuff, len);
        display.getTextBounds(charBuff, (int16_t) 16, (int16_t) 128, &x1, &y1, &w, &h);

        x = 39-w/2;

        if(x < 16) {
            x = 16;
        }
        display.setCursor(x, 106 + 10*i);
        display.print(toPrint);

    }
    shouldRender = true;
}

void clearName() {
    display.fillRect(16, 99, 48, 30, BLACK);
}

void printProgressBar(float p) {
    if(p > 1) {
        p = 1;
    }
    clearProgressBar();
    display.drawRect(6, 64, 8, 64, WHITE);
    uint16_t height = uint16_t(p * 59) + 1;
    display.fillRect(8, 126-height, 4, height, WHITE);

    shouldRender = true;
}

void printTime() {
    display.fillRect(4, 0, 12, 64, BLACK);
    display.setFont(&URW_Gothic_L_Book_15);
    display.setCursor(6, 13);
    display.print(hours / 10);

    display.setCursor(6, 28);
    display.print(hours % 10);

    display.setCursor(6, 43);
    display.print(minutes / 10);

    display.setCursor(6, 58);
    display.print(minutes % 10);
}

void printTimeDigit(int digit, int pos, bool white) {
    display.setFont(&URW_Gothic_L_Book_15);
    display.setCursor(6, pos * 15 + 13);
    if(white) {
        display.fillRect(4, pos*15, 12, 15, BLACK);
        display.print(digit);
    }
    else {
        display.fillRect(4, pos*15, 12, 15, WHITE);
        display.setTextColor(BLACK);
        display.print(digit);
        display.setTextColor(WHITE);
    }
}
