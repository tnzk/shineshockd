#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H
#include "ttgsubtable.hpp"
#include "utf8decoder.h"

using namespace std;

int main( int argc, char** argv)
{
  FT_Library library;
  FT_Face face;
  CTTGSUBTable gsubt;

  if( argc != 3) {
    cout << "Usage: " << argv[0] << " path_to_font [unicode_index]" << endl;
    return -1;
  }
  Utf8Decoder u8d(argv[2], (int)strlen(argv[2]));
  unsigned long unicode_index = u8d.get(0);

  printf("     Font specified: %s\n", argv[1]);
  printf("      Unicode index: 0x%04lx\n", unicode_index);

  // Initialize FreeType2
  FT_Error error;
  FT_Init_FreeType( &library );
  if( FT_New_Face( library, argv[1], 0, &face )) {
    printf("Couldn't open the font: %s\n", argv[1]);
    return -1;
  }

  // Parse GRUB table
  unsigned long length = 0;
  unsigned char* buf = (unsigned char*)malloc(2800000);
  error = FT_Load_Sfnt_Table( face, FT_MAKE_TAG('G','S','U','B'), 0, buf, &length);
  error = FT_Load_Sfnt_Table( face, FT_MAKE_TAG('G','S','U','B'), 0, buf, NULL);
  gsubt.LoadGSUBTable((FT_Bytes)buf);

  long int glyph_index = FT_Get_Char_Index( face, unicode_index);
  printf("        Glyph index: 0x%04lx\n", glyph_index);
  uint32_t substitute_glyph;
  printf("Vertical substitute: ");
  if( gsubt.GetVerticalGlyph( glyph_index, &substitute_glyph)) {
    printf("0x%04lx\n", substitute_glyph);
  } else {
    puts("none");
  }
  

  free(buf);

  return 0;
}

