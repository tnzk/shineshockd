class Utf8Decoder {
public:
  char* s;
  int n;
  Utf8Decoder(char*, int);
  long int get(int);
  int length();
private:
  int utf8_byte_type( char);
  int get_utf8_bytes( unsigned char*, unsigned char*);
  long int get_unicode_index( unsigned char*, int);
};


