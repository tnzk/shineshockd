#include "document.h"

Detection::Detection( int x, int y, double sim)
{
  this->x = x;
  this->y = y;
  this->sim = sim;
}

Document::Document( string src_name, string tmpl_name)
{
  this->src_name = src_name;
  this->tmpl_name = tmpl_name;
  src = NULL;
  tmpl = NULL;

  FT_Init_FreeType( &library );
  FT_New_Face( library, "mplus-1c-heavy.ttf", 0, &face );
  slot = face->glyph;
  FT_Set_Char_Size( face, 0, 16 * 64, 300, 300);
}

bool Document::Load()
{
  src = cvLoadImage("image.bmp", CV_LOAD_IMAGE_COLOR);
  tmpl = cvLoadImage("template.bmp", CV_LOAD_IMAGE_COLOR);
  if(src == 0 || tmpl == 0) return false;

  said = cvCloneImage(src);
  cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0, 4, 4);
  result = cvCreateImage( cvSize( src->width - tmpl->width + 1, src->height - tmpl->height + 1), 32, 1);

  return true;
}

int Document::Match( double threshould)
{
  cvMatchTemplate(src, tmpl, result, CV_TM_CCOEFF_NORMED);
  for( int y = 0; y < result->height; y++) {
    for( int x = 0; x < result->width; x++) {
      CvScalar s = cvGet2D(result,y,x);
      int xx = x + tmpl->width - 1;
      int yy = y + tmpl->height - 1;
      if(s.val[0] >= threshould)
	detections.push_back( Detection( x, y, s.val[0]));
    }
  }
  return detections.size();
}

bool Document::Say( char* s, int detection_id)
{
  Utf8Decoder u8d(s, (int)strlen(s));
  IplImage* dst = this->said;
  Detection detection = detections[detection_id]; 
  
  int pen_x = 300;
  int pen_y = 200;
  int cpos_x = detection.x;
  int cpos_y = detection.y;
  int cpos_w = 640;
  int cpos_h = 50;
  unsigned char fcr = 0x00;
  unsigned char fcg = 0x54;
  unsigned char fcb = 0xff;

  for ( int n = 0; n < u8d.length(); n++ ){
    int i;
    FT_Bitmap bitmap;
    long int unicode_index = u8d.get(n);

    FT_Load_Char( face, unicode_index, FT_LOAD_RENDER);
    bitmap = slot->bitmap;

    int offset_y = cpos_h - bitmap.rows + cpos_y;
    for( i = 0; i < bitmap.rows * bitmap.width; i++){

      int x = ( i % bitmap.width) + cpos_x;
      int y = offset_y + ( i / bitmap.width);
      unsigned char* src_b = (unsigned char*)(dst->imageData + dst->widthStep * y + x * 3);
      unsigned char* src_g = (unsigned char*)(dst->imageData + dst->widthStep * y + x * 3 + 1);
      unsigned char* src_r = (unsigned char*)(dst->imageData + dst->widthStep * y + x * 3 + 2);
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
  return true;
}

void Document::Everything()
{
  vector<Detection>::iterator it = detections.begin();
  while( it != detections.end()) {
    printf(" %3i, %3i: %20.20f\n", it->x, it->y, it->sim);
    it++;
  }
  cvSaveImage( "shineshok_result.png", this->said);
}

