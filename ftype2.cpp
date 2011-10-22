#include <string.h>
#include <iostream>
#include <ft2build.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include FT_FREETYPE_H

#include "utf8decoder.h"

using namespace std;
using namespace cv;

int main( int argc, char** argv){
  FT_Library library;
  FT_Face    face;
  FT_GlyphSlot slot;
  FT_UInt glyph_index;
  FT_Error error;

  char text[] = "a日b本語テスト彳";
  char filename[] = "test.bmp";
  int pen_x, pen_y, n;
  int num_chars = (int)strlen( text);

  unsigned char fcr = 0;
  unsigned char fcg = 0;
  unsigned char fcb = 0;

  FT_Init_FreeType( &library );
  FT_New_Face( library, "mplus-1c-heavy.ttf", 0, &face );
  slot = face->glyph;
  FT_Set_Char_Size( face, 0, 16 * 64, 300, 300);

  int cpos_x = 50;
  int cpos_y = 60;
  int cpos_w = 640;
  int cpos_h = 50;

  IplImage* img = cvLoadImage("image.bmp", CV_LOAD_IMAGE_COLOR);
  IplImage* dst = cvCloneImage(img);

  Utf8Decoder u8d(text, num_chars);

  pen_x = 300;
  pen_y = 200;
  for ( n = 0;n < u8d.length(); n++ ){
    int i;
    FT_Bitmap bitmap;
    long int unicode_index = u8d.get(n);

    FT_Load_Char( face, unicode_index, FT_LOAD_RENDER);
    bitmap = slot->bitmap;

    int offset_y = cpos_h - bitmap.rows + cpos_y;
    for( i = 0; i < bitmap.rows * bitmap.width; i++){
      int x = ( i % bitmap.width) + cpos_x;
      int y = offset_y + ( i / bitmap.width);
      unsigned char* src_r = (unsigned char*)(dst->imageData + dst->widthStep * y + x * 3);
      unsigned char* src_g = (unsigned char*)(dst->imageData + dst->widthStep * y + x * 3 + 1);
      unsigned char* src_b = (unsigned char*)(dst->imageData + dst->widthStep * y + x * 3 + 2);
      double gph_a = (double)bitmap.buffer[i] / 255.0;
      double src_a = 1.0 - gph_a;

      int tmp;
      tmp = fcr * gph_a + *src_r * src_a;
      *src_r = (tmp > 255) ? 255 : tmp;

      tmp = fcg * gph_a + *src_g * src_a;
      *src_g = (tmp > 255) ? 255 : tmp;

      tmp = fcb * gph_a + *src_b * src_a;
      *src_b = (tmp > 255) ? 255 : tmp;
    }
    cpos_x += bitmap.width + 5;
    pen_x += slot->advance.x >> 6;
  }
  cvSaveImage( "ftyperesult.png", dst);

}
