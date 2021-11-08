// Task 3 - Implement Bresenham drawing alg.
//        - Draw a star using lines
//        - Make use of std::vector to define the shape
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>

#include <ppgso/ppgso.h>

// Size of the framebuffer
const unsigned int SIZE = 512;

struct Point {
  int x,y;
};

// Bresenham drawing algorithm
void drawLine(ppgso::Image& framebuffer, Point& from, Point& to) {
    int x1 = from.x, x2 = to.x;
    int y1 = from.y, y2 = to.y;

    if (x1 == x2 && y1 == y2)
    {
        framebuffer.setPixel(x1, y1, 255, 255, 255);
        return;
    }

    int e;

    int x_move = x1;
    int y_move = y1;

    if(abs(x2 - x1) > abs(y2 - y1))
        e = -(x2 - x1);
    else
        e = -(y2 - y1);

    if(e > 0)
    {
        e = -e;
        int temp = x1;
        x1 = x2;
        x2 = temp;
        temp = y1;
        y1 = y2;
        y2 = temp;
        y_move = y1;
        x_move = x1;
    }

    if(abs(x2-x1) > abs(y2-y1))
    {
        while (x_move <= x2)
        {
            framebuffer.setPixel(x_move, y_move, 255, 255, 255);
            x_move += 1;
            if ((y2 - y1) > 0)
                e += 2 * (y2 - y1);
            else
                e += 2 * (y1 - y2);
            if (e > 0)
            {
                if ((y2 - y1) > 0)
                    y_move += 1;
                else
                    y_move -= 1;
                e -= 2 * (x2 - x1);
            }
        }
    }
    else
    {
        while (y_move <= y2)
        {
            framebuffer.setPixel(x_move, y_move, 255, 255, 255);
            y_move += 1;
            if ((x2 - x1) > 0)
                e += 2 * (x2 - x1);
            else
                e += 2 * (x1 - x2);
            if (e > 0)
            {
                if ((x2 - x1) > 0)
                    x_move += 1;
                else
                    x_move -= 1;
                e -= 2 * (y2 - y1);
            }
        }
    }


  // TODO: Implement Bresenham drawing algorithm
}

int main()
{
  // Use ppgso::Image as our framebuffer
  ppgso::Image framebuffer(SIZE, SIZE);

  // TODO: Generate star points
  std::vector<Point> points;


//    for (double i = 0; i < 2 * 3.14; i += (3.14 / 6)){ // presnejsie
  for (double i = 0; i < 2 * M_PI; i += (M_PI / 6)){  // viac krive

      points.push_back(Point {(int)(256 + (cos(i) * 50)), (int)(256 + (sin(i) * 50))});
      points.push_back(Point {(int)(256 + (cos(i) * 100)), (int)(256 + (sin(i) * 100))});

  }

  // Draw lines
  for(unsigned int i = 0; i < points.size() - 1; i += 2)
    drawLine(framebuffer, points[i], points[i+1]);

  // Save the result
  std::cout << "Generating task2_bresenham.bmp file ..." << std::endl;
  ppgso::image::saveBMP(framebuffer, "task2_bresenham.bmp");

  std::cout << "Done." << std::endl;
  return EXIT_SUCCESS;
}

