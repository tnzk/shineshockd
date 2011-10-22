#include <stdio.h>
#include <string.h>
#include "utf8decoder.h"

int main( int argc, char** argv)
{
  char text[] = "a日b本語テスト彳";
  Utf8Decoder u8d(text, (int)strlen(text));
  for ( int i = 0; i < u8d.length(); i++ ){
    printf("%i: %lx\n", i, u8d.get(i));
  }
    
  return 0;
}
