/*
 * Project: ESP32-Cam_DyanmicColorRangeDevice
 * Description: Merges two images by looking at the RGB values of every pixel and finding the difference between them. The code places the highest -
                red, green, and blue values from either image as the minuened. This ensures the order you place the images doesn't effect the outcome.
 * Author: Caden Gamache
 * Date: 04/18/2023
`*/

/******************************** Declare Variables ********************************/
PImage img;          // The source image
PImage img2;         // The source image
PImage mergedImage;  // Creates an image for showing the merged product
int cellsize = 1;    // Dimensions of each cell in the grid
int cols, rows;      // Number of columns and rows in our system

void setup() {
  size(481,360);
  img = loadImage("Shepherd.jpg");     // Load the image
  img2  = loadImage("Cube.jpg");  // Load the image
  cols = width/cellsize;             // Calculate # of columns
  rows = height/cellsize;            // Calculate # of rows
  
  mergedImage = createImage(img.width, img.height, RGB);
}

void draw() {
  background(0);

  img.loadPixels();
  img2.loadPixels();
  mergedImage.loadPixels();
  // Cycle through columns
  for ( int i = 0; i < cols;i++) {
    // Cycle through rows
    for ( int j = 0; j < rows;j++) {
      int x = i*cellsize + cellsize/2; // x position of current pixel
      int y = j*cellsize + cellsize/2; // y position of current pixel
      int loc = x + y*width;           // Pixel array location
      
      color c = img.pixels[loc];       // Grab the color of image 1
      color c2 = img2.pixels[loc];     // Grab the color of image 2
      
      // Seperates the red, green, and blue values of image 1 into three numbers
      int r, g, b;
      r = (c & 0xFF0000)>>16;
      g = (c & 0x00FF00)>>8;
      b = c & 0x0000FF;
      
      // Seperates the red, green, and blue values of image 2 into three numbers
      int r2, g2, b2;
      r2 = (c2 & 0xFF0000)>>16;
      g2 = (c2 & 0x00FF00)>>8;
      b2 = c2 & 0x0000FF;
      
      int r3 = 0, g3 = 0, b3 = 0;
      // Red
      if (r>r2){          // If the red value from the first image is greater than the second, place the 1st value in the menuened
        r3  = r-r2<<16;
      }
      else if (r<r2){     // If the red value from the second image is greater than the First, place the 2nd value in the menuened
        r3  = r2-r<<16;
      }
      else if (r == r2){  // If the red value from both images are equal, ignore subtraction
        r3  = r2<<16;
      }
      
      // Green
      if (g>g2){          // If the green value from the first image is greater than the second, place the 1st value in the menuened
        g3  = g-g2<<8;
      }
      else if (g<g2){     // If the green value from the second image is greater than the First, place the 2nd value in the menuened
        g3  = g2-g<<8;
      }
      else if (g == g2){  // If the green value from both images are equal, ignore subtraction
        g3  = g2<<8;
      }
      
      // Blue
      if (b>b2){          // If the blue value from the first image is greater than the second, place the 1st value in the menuened
        b3  = b-b2;
      }
      else if (b<b2){     // If the blue value from the second image is greater than the First, place the 2nd value in the menuened
        b3  = b2-b;
      }
      else if (b == b2){  // If the blue value from both images are equal, ignore subtraction
        b3  = b2;
      }
      
      // Combine the colors back into a single hex code
      int colorOut;
      colorOut = r3|g3|b3;
      
      // Sets the current pixel color to the combined value
      mergedImage.pixels[loc]  = colorOut;
    }
  }
  mergedImage.updatePixels();
  // Display the merged image
  image(mergedImage,0,0);
}
