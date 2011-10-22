#include <ft2build.h>
#include FT_FREETYPE_H

int main( int argc, char** argv)
{

  FT_Library library;
  FT_Face face;
  int error;

  // Initialize freetype2
  error = FT_Init_FreeType( &library );
  if( error) {
    puts("Initialize failed.");
    return -1;
  }
  puts("Successfully initialized.");
  
  error = FT_New_Face( library, "mplus-1c-heavy.ttf", 0, &face );
  if ( error == FT_Err_Unknown_File_Format ) {
    puts("Unknown font format.");
    return -1;
  }
  else if ( error ) {
    puts("Could not load the font face somehow.");
    return -1;
  }
  puts("Font face successfully loaded.");

  
  error = FT_Set_Char_Size( face, 0, 16*64, 300, 300 );
  if( error) {
    puts("faild(1)");
    return -1;
  }
  error = FT_Set_Pixel_Sizes( face, 0, 16 );
  if( error) {
    puts("faild(2)");
    return -1;
  }
  puts("Successfully congigured.");
  
  error = FT_Load_Glyph( face, 'c', FT_LOAD_DEFAULT);
  if( error) {
    puts("faild(2)");
    return -1;
  }
  puts("Glyph loaded.");

  return 0;
}
