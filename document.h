#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H
#include "ttgsubtable.hpp"
#include "utf8decoder.h"

using namespace std;

class Detection {
public:
  int x;
  int y;
  double sim;
  Detection( int, int, double);
};

class Document {
public:
  string src_name;
  string tmpl_name;
  IplImage* src;
  IplImage* tmpl;
  IplImage* said;
  IplImage* result;
  CvFont font;
  vector<Detection> detections;
  CTTGSUBTable gsubt;

  // To use FreeType
  FT_Library library;
  FT_Face face;
  FT_UInt glyph_index;

  Document( string, string);
  bool Load();
  int Match( double);
  bool Say( char*, int);
  void Everything();
};
