// Task 1 - Load a 512x512 image lena.raw
//        - Apply specified per-pixel transformation to each pixel
//        - Save as result.raw
#include <fstream>
#include <iostream>

// Size of the framebuffer
const unsigned int SIZE = 512;

// A simple RGB struct will represent a pixel in the framebuffer
struct Pixel {
  // TODO: Define correct data type for r, g, b channel
    unsigned char red, green, blue;
};

int main()
{
  // Initialize a framebuffer
  auto framebuffer = new Pixel[SIZE][SIZE];

  // TODO: Open file lena.raw (this is 512x512 RAW GRB format)
  FILE *inputFile;
  inputFile = fopen("lena.raw","rb");


  // TODO: Read data to framebuffer and close the file

//  for(unsigned int i = 0; i < SIZE; i++)
//  {
//      for(unsigned int j = 0; j < SIZE; j++)
//      {
//          std::fread(&framebuffer[i][j],sizeof(Pixel),1,inputFile);
//      }
//  }
  std::fread(framebuffer,sizeof(Pixel),512 * 512, inputFile);
  fclose(inputFile);

  // Traverse the framebuffer
  for (unsigned int y = 0; y < SIZE; y++) {
    for (unsigned int x = 0; x < SIZE; x++) {
      // TODO: Apply pixel operation
        float noise = (double)rand() / (double)((unsigned)RAND_MAX + 0.5)+0.25;
        framebuffer[x][y].red = framebuffer[x][y].red * noise;
        framebuffer[x][y].green = framebuffer[x][y].green * noise;
        framebuffer[x][y].blue = framebuffer[x][y].blue * noise;
//        {
//            framebuffer[x][y].red = 255;
//        }
//        if (framebuffer[x][y].green > 255)
//        {
//            framebuffer[x][y].green = 255;
//        }
//        if (framebuffer[x][y].blue > 255)
//        {
//            framebuffer[x][y].blue = 255;
//        }
//      Black and white only
//      unsigned int tmpRed = (framebuffer[x][y].red + framebuffer[x][y].green + framebuffer[x][y].blue) / 3;
//      framebuffer[x][y].red = (unsigned char) tmpRed;
//      framebuffer[x][y].green = (unsigned char) tmpRed;
//      framebuffer[x][y].blue = (unsigned char) tmpRed;
    }
  }

  // TODO: Open file result.raw
  std::cout << "Generating result.raw file ..." << std::endl;
  FILE *outputFile;
  outputFile = fopen("blackAndWhiteLena.raw","wb");
  // TODO: Write the framebuffer to the file and close it
  for(unsigned int i = 0; i < SIZE; i++)
  {
      for(unsigned int j = 0; j < SIZE; j++)
      {
          std::fwrite(&framebuffer[i][j],sizeof(Pixel),1,outputFile);
      }
  }
  fclose(outputFile);
  std::cout << "Done." << std::endl;
  delete[] framebuffer;
  return EXIT_SUCCESS;
}
