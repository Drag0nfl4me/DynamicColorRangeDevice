/*
 * Project DynamicColorRange_Program
 * Description: A new device that takes two pictures and compares the color change between every pixel. It then creates a new image with the data.
 * Author: Caden Gamache
 * Date: 04/02/2023 - 
 */
#include "uCamIII.h"

/***************************** Declare Variables *****************************/
int i, x, j;

const int REDPIN = A5, GREENPIN = A4, BLUEPIN = A3, BUTTONPIN = A2;
const int BLACK = 0, RED = 1, ORANGE = 2, YELLOW = 3, GREEN = 4, CYAN = 5, BLUE = 6, PURPLE = 7, WHITE = 8;
const int COLOR[] {BLACK, RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, PURPLE, WHITE};
float BRIGHTNESS = 10.0;
int currentColor;

int timer;
bool buttonPressed, snapPicture, test;

/**************************** Function Prototypes ****************************/
// void colorChange(int *array[][]);
// void colorChangeRGB(int *array[][]);

// Sets the 3W LED to a given color
void turnLed(int color, float brightnessPercent, int redPin, int greenPin, int bluePin);

int callbackSerial(uint8_t *buf, int len, int id);
int bmpHeader(uint8_t *buf, int len, int width, int height, int bits, int encoding = 0, int padding = 0);
int takeSnapshot();

/********************************* uCAM III *********************************/
long prepareCam(uCamIII_SNAP_TYPE snap    = uCamIII_SNAP_RAW, 
                uCamIII_IMAGE_FORMAT fmt  = uCamIII_RAW_8BIT, 
                uCamIII_RES res           = uCamIII_80x60, 
                uCamIII_CBE contrast      = uCamIII_DEFAULT,
                uCamIII_CBE brightness    = uCamIII_DEFAULT,
                uCamIII_CBE exposure      = uCamIII_DEFAULT,
                uCamIII_callback callback = NULL);

uCamIII<USARTSerial> ucam(Serial1, A0, 500);

uint8_t     imageBuffer[160*120*2 + 134];                           // max raw image 160x120x2byte + max BMP header size
int         imageSize     = 0;
int         imageType     = uCamIII_TYPE_SNAPSHOT;                  // default for the demo 
                                                                    // alternative: _TYPE_RAW & _TYPE_JPEG
int         imageSnapType = uCamIII_SNAP_RAW;
int         imageWidth    = 0;
int         imageHeight   = 0;
int         imagePxDepth  = 0;
int         imageTime     = 0;
char        lIP[16];

uCamIII_callback snapTarget = callbackSerial;


SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {
    Serial.begin(9600);
    waitFor(Serial.isConnected, 10000);

    pinMode(REDPIN,OUTPUT);
    pinMode(GREENPIN,OUTPUT);
    pinMode(BLUEPIN,OUTPUT);
    pinMode(BUTTONPIN,INPUT);
    pinMode(D7, OUTPUT);

    //Serial.printf("Initializing camera...");
    //ucam.init(115200);
    //Serial.printf("Done\n");

    snapTarget = callbackSerial;

    timer = millis();
}

void loop() {
  buttonPressed = digitalRead(A2);
  if(buttonPressed != snapPicture) {
      if(snapPicture) {
        snapPicture = !snapPicture;
      }
      snapPicture = buttonPressed;
  }
  if (snapPicture) {
    // if (snapPicture != test) {
       digitalWrite(D7,HIGH);
    //   takeSnapshot();
    //   for (i=0;i<(160*120)+1;i++) {
    //     Serial.printf("%02x",&imageBuffer[i]);
    //   }
    //   Serial.printf("\n");
    //   test = 1;
    // }
    String parse0 = Serial1.readStringUntil('FFD9');
    Serial.printf("%s",parse0.c_str());
  }
  if (snapPicture == 0) {
    // if (snapPicture != test) {
      digitalWrite(D7,LOW);
    //   test = 0;
    // }
  }
  if((millis()-timer) > 5000) {
    timer = millis();
    currentColor++;
    turnLed(COLOR[currentColor], BRIGHTNESS, REDPIN, GREENPIN, BLUEPIN);
    if (currentColor>8) {
      currentColor = 0;
    }
  }
}

void turnLed(int color, float brightnessPercent, int redPin, int greenPin, int bluePin) {
    static int black = 0, red = 1, orange = 2, yellow = 3, green = 4, cyan = 5, blue = 6, purple = 7, white = 8;
    // quick reset, otherwise colors mix ever so often
    analogWrite(redPin, LOW);
    analogWrite(greenPin, LOW);
    analogWrite(bluePin, LOW);

    if(color == black) {
        analogWrite(redPin, LOW);
        analogWrite(greenPin, LOW);
        analogWrite(bluePin, LOW);
        //Serial.printf("Current LED color = black     \r");
    }
    if(color == red) {
        analogWrite(redPin, 255.0/(101.0-brightnessPercent));
        analogWrite(greenPin, LOW);
        analogWrite(bluePin, LOW);
        //Serial.printf("Current LED color = red       \r");
    }
    if(color == orange) {
        analogWrite(redPin, 235.0/(101.0-brightnessPercent));
        analogWrite(greenPin, 255.0/(101.0-brightnessPercent));
        analogWrite(bluePin, LOW);
        //Serial.printf("Current LED color = orange     \r");
    }
    if(color == yellow) {
        analogWrite(redPin, 200.0/(101.0-brightnessPercent));
        analogWrite(greenPin, 255.0/(101.0-brightnessPercent));
        analogWrite(bluePin, LOW);
        //Serial.printf("Current LED color = yellow     \r");
    }
    if(color == green) {
        analogWrite(redPin, LOW);
        analogWrite(greenPin, 255.0/(101.0-brightnessPercent));
        analogWrite(bluePin, LOW);
        //Serial.printf("Current LED color = green     \r");
    }
    if(color == cyan) {
        analogWrite(redPin, LOW);
        analogWrite(greenPin, 255.0/(101.0-brightnessPercent));
        analogWrite(bluePin, 255.0/(101.0-brightnessPercent));
        //Serial.printf("Current LED color = cyan     \r");
    }
    if(color == blue) {
        analogWrite(redPin, LOW);
        analogWrite(greenPin, LOW);
        analogWrite(bluePin, 255.0/(101.0-brightnessPercent));
        //Serial.printf("Current LED color = blue     \r");
    }
    if(color == purple) {
        analogWrite(redPin, 15.0/(101-brightnessPercent));
        analogWrite(greenPin, LOW);
        analogWrite(bluePin, 255.0/(101.0-brightnessPercent));
        //Serial.printf("Current LED color = purple     \r");
    }
    if(color == white) {
        analogWrite(redPin, 145.0/(101.0-brightnessPercent));
        analogWrite(greenPin, 255.0/(101.0-brightnessPercent));
        analogWrite(bluePin, 255.0/(101.0-brightnessPercent));
        //Serial.printf("Current LED color = white     \r");
    }
}

int callbackSerial(uint8_t *buf, int len, int id) {
  Log.info("Package %d (%d Byte) -> Serial", id, len);
  
  return Serial.write(buf, len);
}

int takeSnapshot() {

  int retVal;

  retVal = prepareCam(uCamIII_SNAP_JPEG, uCamIII_COMP_JPEG, uCamIII_640x480, 
                      uCamIII_DEFAULT, uCamIII_DEFAULT, uCamIII_DEFAULT, snapTarget);

  if (retVal <= 0) return retVal;

  if (retVal = imageSize = ucam.getPicture((uCamIII_PIC_TYPE)imageType)) {

    if (imageType == uCamIII_SNAP_JPEG)
    for (int received = 0, chunk = 0; (received < imageSize) && (chunk = ucam.getJpegData(&imageBuffer[constrain(received, 0, sizeof(imageBuffer)-512)], 512, snapTarget)); received += chunk);
    
    else if (imageSize <= sizeof(imageBuffer)) {
      int offset = bmpHeader(imageBuffer, sizeof(imageBuffer), imageWidth, imageHeight, imagePxDepth);
      retVal     = ucam.getRawData(&imageBuffer[offset], imageSize, NULL);

      if (imagePxDepth == 16) {
        for (int i = offset; i < imageSize + offset; i += 2)    // raw image comes big-endian and upside down from cam,
        {                                                       // this block corrects endianness
        uint8_t dmy = imageBuffer[i];
        imageBuffer[i] = imageBuffer[i+1];
        imageBuffer[i+1] = dmy;
        }
      }
      snapTarget(imageBuffer, imageSize + offset, 0);
    }
    digitalWrite(D7, LOW);

    return retVal;
  }

  return 0;
}

long prepareCam(uCamIII_SNAP_TYPE snap, uCamIII_IMAGE_FORMAT fmt, uCamIII_RES res, uCamIII_CBE contrast,
                uCamIII_CBE brightness, uCamIII_CBE exposure,uCamIII_callback callback) {
  int retVal = 0;
  
  digitalWrite(D7, HIGH);

  ucam.hardReset();

  if (!(retVal = ucam.sync())) return -1;
  
  if (!(retVal = ucam.setImageFormat(fmt, res))) return -2;
  
  retVal = ucam.setCBE(contrast, brightness, exposure);

  if (imageType == uCamIII_TYPE_SNAPSHOT)
    if (!(retVal = ucam.takeSnapshot(snap))) return -3;

  imageTime = Time.now();

  if (fmt == uCamIII_COMP_JPEG) 
    if (!(retVal = ucam.setPackageSize(512))) return -4;

  // if we made it to here, we can set the global image variables accordingly 
  imageType = snap;             
  
  switch(res) 
  {
    case uCamIII_80x60:
      imageWidth  = 80;
      imageHeight = 60;
      break;
    case uCamIII_160x120:          
      imageWidth  = 160;
      imageHeight = 120;
      break;
    case uCamIII_320x240:
      imageWidth  = 320;
      imageHeight = 240;
      break;
    case uCamIII_640x480:
      imageWidth  = 640;
      imageHeight = 480;
      break;
    case uCamIII_128x96:
      imageWidth  = 128;
      imageHeight = 96;
      break;
    case uCamIII_128x128:
      imageWidth  = 128;
      imageHeight = 128;
      break;
    default:
      imageWidth  = 80;
      imageHeight = 60;
      break;
  }

  switch (fmt)
  {
    case uCamIII_COMP_JPEG:
      imagePxDepth = 24;
      break;
    case uCamIII_RAW_16BIT_RGB565:
    case uCamIII_RAW_16BIT_CRYCBY:
      imagePxDepth = 16;
      break;
    case uCamIII_RAW_8BIT:
    defaut:
      imagePxDepth = 8;
      break;
  }
  
  return retVal;
}

struct BITMAPFILEHEADER 
{
  uint16_t   bfType;            // file type BM (0x424d)                        [  0]
  uint32_t   bfSize;            // file size all together                       [  2]
  uint16_t   bfReserved1;       //                                              [  6]
  uint16_t   bfReserved2;       //                                              [  8]
  uint32_t   bfOffBits;         // offset of actual image data                  [ 10]
} __attribute__((__packed__));  //                                              ( 14)

// https://msdn.microsoft.com/en-us/library/windows/desktop/dd183381(v=vs.85).aspx
struct BITMAPV5HEADER
{
  // --------------------------------------------------------------------------
  // legacy BITMAPINFOHEADER: 
  uint32_t Size;              // Size of this header in bytes               [  0][ 14]
  int32_t  Width;             // Image width in pixels                      [  4][ 18]
  int32_t  Height;            // Image height in pixels                     [  8][ 22]
  uint16_t Planes;            // Number of color planes                     [ 12][ 26]
  uint16_t BitsPerPixel;      // Number of bits per pixel                   [ 14][ 28]
  uint32_t Compression;       // Compression methods used                   [ 16][ 30]
  uint32_t SizeOfBitmap;      // Size of bitmap in bytes                    [ 20][ 34]
  int32_t  HorzResolution;    // Horizontal resolution in pixels per meter  [ 24][ 38] 
  int32_t  VertResolution;    // Vertical resolution in pixels per meter    [ 28][ 42]
  uint32_t ColorsUsed;        // Number of colors in the image              [ 32][ 46]
  uint32_t ColorsImportant;   // Minimum number of important colors         [ 36][ 50]
  // --------------------------------------------------------------------   ( 40)( 54)
  // Fields added for V2:
  uint32_t RedMask;           // Mask identifying bits of red component     [ 40][ 54]
  uint32_t GreenMask;         // Mask identifying bits of green component   [ 44][ 58]
  uint32_t BlueMask;          // Mask identifying bits of blue component    [ 48][ 62]
  // --------------------------------------------------------------------   ( 52)( 66)
  // Fields added for V3:
  uint32_t AlphaMask;         // Mask identifying bits of alpha component   [ 52][ 66]
/*// --------------------------------------------------------------------   ( 56)( 70)
  // Fields added for V4:
  uint32_t CSType;            // Color space type                           [ 56][ 70]
  int32_t  RedX;              // X coordinate of red endpoint               [ 60][ 74]
  int32_t  RedY;              // Y coordinate of red endpoint               [ 64][ 78]
  int32_t  RedZ;              // Z coordinate of red endpoint               [ 68][ 82]
  int32_t  GreenX;            // X coordinate of green endpoint             [ 72][ 86]
  int32_t  GreenY;            // Y coordinate of green endpoint             [ 76][ 90]
  int32_t  GreenZ;            // Z coordinate of green endpoint             [ 80][ 94]
  int32_t  BlueX;             // X coordinate of blue endpoint              [ 84][ 98]
  int32_t  BlueY;             // Y coordinate of blue endpoint              [ 88][102]
  int32_t  BlueZ;             // Z coordinate of blue endpoint              [ 92][106]
  uint32_t GammaRed;          // Gamma red coordinate scale value           [ 96][110]
  uint32_t GammaGreen;        // Gamma green coordinate scale value         [100][114]
  uint32_t GammaBlue;         // Gamma blue coordinate scale value          [104][118]
  // --------------------------------------------------------------------   (108)(122)
  // Fields added for V5:
  uint32_t Intent;            //                                            [108][122]
  uint32_t ProfileData;       //                                            [112][126]
  uint32_t ProfileSize;       //                                            [116][130]
  uint32_t Reserved;          //                                            [120][134]
*/
} __attribute__((__packed__));

int bmpHeader(uint8_t *buf, int len, int width, int height, int bits, int encoding, int padding)  
{  
  Log.trace(__FUNCTION__); 

  uint32_t          headerSize      = sizeof(BITMAPFILEHEADER) 
                                    + sizeof(BITMAPV5HEADER);
  uint32_t          paletteSize     = (bits <= 8) ? (1 << (bits+2)) : 0;    // optional palette
  uint32_t          pixelDataSize   = height * ((width * (bits / 8)) + padding);  

  BITMAPFILEHEADER *bmpFileHeader   = (BITMAPFILEHEADER*)buf;
  BITMAPV5HEADER   *bmpInfoHeader   = (BITMAPV5HEADER*)(buf + sizeof(BITMAPFILEHEADER));
  uint8_t          *bmpPalette      = buf + headerSize; 

  if (len < headerSize + paletteSize) return 0;

  /// ToDo: where do we set encoding R5G6B5 vs. CrYCbY?

  memset(bmpFileHeader, 0, sizeof(BITMAPFILEHEADER));
  bmpFileHeader->bfType              = 0x4D42;                               // magic number "BM"
  bmpFileHeader->bfSize              = headerSize 
                                     + paletteSize
                                     + pixelDataSize;
  //bmpFileHeader->bfReserved1         =
  //bmpFileHeader->bfReserved2         = 0;
  bmpFileHeader->bfOffBits           = headerSize
                                     + paletteSize;

  memset(bmpInfoHeader, 0, sizeof(BITMAPV5HEADER));
  bmpInfoHeader->Size                = sizeof(BITMAPV5HEADER);  
  bmpInfoHeader->Width               = width;                               
  bmpInfoHeader->Height              = -height;                              // negative height flips vertically
                                                                            // so image isn't upside down 
                                                                            // but Google Chorme doesn't support that!!! 
  bmpInfoHeader->Planes              = 1;  
  bmpInfoHeader->BitsPerPixel        = bits;  
  //bmpInfoHeader->Compression         = 0; 
  bmpInfoHeader->SizeOfBitmap        = 0x03;                                 // BI_RGB (0), BI_BITFIELD (3)
  bmpInfoHeader->HorzResolution      = 0x1000; 
  bmpInfoHeader->VertResolution      = 0x1000; 
  bmpInfoHeader->ColorsUsed          =     
  bmpInfoHeader->ColorsImportant     = (bits <= 8) ? (1 << bits) : 0;        // <= 8bit use color palette
  if (bits == 16)
  {  
    bmpInfoHeader->RedMask           = 0x0000f800;
	bmpInfoHeader->GreenMask         = 0x000007e0;
	bmpInfoHeader->BlueMask          = 0x0000001f;
  }

  for (int c = 0; c < bmpInfoHeader->ColorsUsed; c++)                         // grayscale color table
  {
    uint8_t color = 255 * c / (bmpInfoHeader->ColorsUsed - 1);
    *(bmpPalette++) = color;
    *(bmpPalette++) = color;
    *(bmpPalette++) = color;
    *(bmpPalette++) = 0xFF;
  }
  Log.trace("BMP data offset: %d + %d + %d = %d, BMP file size: %d",  
    sizeof(BITMAPFILEHEADER), sizeof(BITMAPV5HEADER), paletteSize, bmpFileHeader->bfOffBits, bmpFileHeader->bfSize);
  return bmpFileHeader->bfOffBits;
}

// void colorChange(int *array[][]) {

// }

// void colorChangeRGB(int *array[][]) {

// }