#include "utf8decoder.h"

Utf8Decoder::Utf8Decoder( char* s, int n)
{
  this->s = s;
  this->n = n;
}

int Utf8Decoder::utf8_byte_type(char c)
{
  if(( c & 0xc0) == 0x80) return 0; // following byte
  if(( c & 0xe0) == 0xc0) return 2;
  if(( c & 0xf0) == 0xe0) return 3;
  if(( c & 0xf8) == 0xf0) return 4;
  if(( c & 0xfc) == 0xf8) return 5;
  if(( c & 0xfe) == 0xfc) return 6;
  return 1;
}

int Utf8Decoder::get_utf8_bytes( unsigned char* buf, unsigned char* src)
{
  int n = utf8_byte_type(*src);  
  for( int i = 0; i < n; i++) buf[i] = *(src + i);
  return n;
}

long int Utf8Decoder::get_unicode_index( unsigned char* buf, int n)
{
  long int dst = 0;

  if( utf8_byte_type(buf[0]) == 0) return 0;
  if( utf8_byte_type(buf[0]) == 1) return buf[0];

  for( int i = 0; i < n; i++) {
    int bitleft;
    unsigned char data = buf[i];
    if( utf8_byte_type(data) == 0) {
      // following byte
      bitleft = 6;
      data <<= 2;
    } else {
      // head byte
      bitleft = 7 - n;
      data <<= (n + 1);
    }
    while(bitleft) {
      dst |= (data & 0x80) >> 7;
      bitleft--;
      dst <<= 1;
      data <<= 1;
    }
  }
  return dst >> 1;
}

long int Utf8Decoder::get( int i)
{
  unsigned char utf8bytes[] = {0,0,0,0,0,0,0};
  int c = 0;
  int j;
  for( j = 0; j < this->n; j++) {
    if(utf8_byte_type(s[j])) c++;
    if( c > i ) break;
  }
  int m = this->get_utf8_bytes( utf8bytes, (unsigned char*)(this->s + j));
  return get_unicode_index( utf8bytes, m);
}

int Utf8Decoder::length()
{
  int c = 0;
  for( int i = 0; i < this->n; i++) if(utf8_byte_type(s[i])) c++;
  return c;
}
