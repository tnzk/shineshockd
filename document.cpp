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

  // Initialize FreeType2
  printf("Initialize FreeType2...");
  FT_Error error;
  FT_Init_FreeType( &library );
  FT_New_Face( library, "F910ComicW4.otf", 0, &face );
  slot = face->glyph;
  FT_Set_Char_Size( face, 0, 13 * 64, 300, 300);
  printf("Done.\n");

  // Pase GRUB table
  printf("Parse GSUB table...");
  unsigned long length = 0;
  unsigned char* buf = (unsigned char*)malloc(2800000);
  error = FT_Load_Sfnt_Table( face, FT_MAKE_TAG('G','S','U','B'), 0, buf, &length);
  error = FT_Load_Sfnt_Table( face, FT_MAKE_TAG('G','S','U','B'), 0, buf, NULL);
  gsubt.LoadGSUBTable((FT_Bytes)buf);
  printf("Done.\n");

}

bool Document::Load()
{

  // Load the source image and template
  // TODO: use any other file name
  src  = cvLoadImage("ex-t06.png", CV_LOAD_IMAGE_COLOR);
  tmpl = cvLoadImage("template.bmp", CV_LOAD_IMAGE_COLOR);
  if(src == 0 || tmpl == 0) return false;

  // Create target image and generate buffer for template matching
  said = cvCloneImage(src);
  result = cvCreateImage( cvSize( src->width - tmpl->width + 1, src->height - tmpl->height + 1), 32, 1);

  return true;
}

int Document::Match( double threshould)
{
  cvMatchTemplate(src, tmpl, result, CV_TM_CCOEFF_NORMED);
  for( int y = 0; y < result->height; y++) {
    for( int x = 0; x < result->width; x++) {
      CvScalar s = cvGet2D(result,y,x);
      if(s.val[0] >= threshould)
	detections.push_back( Detection( x, y, s.val[0]));
    }
  }
  return detections.size();
}

bool Document::Say( char* s, int detection_id)
{
  // Draw the string specified onto a marker detected
  Utf8Decoder u8d(s, (int)strlen(s));
  IplImage* dst = this->said;
  Detection detection = detections[detection_id]; 
  
  int cpos_x = detection.x + 200;
  int cpos_y = detection.y;
  // TODO: make these modifiable
  unsigned char fcr = 0x00;
  unsigned char fcg = 0x00;
  unsigned char fcb = 0x00;

  for ( int n = 0; n < u8d.length(); n++ ){
    // Get correspoindng glyph ID from Unicode index.
    // Use vertical substitution if the glyph has.
    long int unicode_index = u8d.get(n);
    long int glyph_index = FT_Get_Char_Index( face, unicode_index);
    uint32_t substitute_glyph;
    if( gsubt.GetVerticalGlyph( glyph_index, &substitute_glyph)) {
      glyph_index = substitute_glyph;
    }

    FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER | FT_LOAD_VERTICAL_LAYOUT);
    FT_Bitmap bitmap = slot->bitmap;
    int offset_x = cpos_x + slot->metrics.vertBearingX / 64;
    int offset_y = cpos_y + slot->metrics.vertBearingY / 64;
    for( int i = 0; i < bitmap.rows * bitmap.width; i++){

      int x = ( i % bitmap.width) + offset_x;
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
    cpos_y += (slot->metrics.vertAdvance) / 64;
  }
  return true;
}

void Document::Everything()
{
  cvSaveImage( "shineshok_result.png", this->said);
}

