#include <iostream>
#include <fstream>
#include <string>
#include <vector>
//#include <stdio.h>
#include "document.h"
 
using namespace cv;
using namespace std;

int main( int argc, char** argv)
{
  if( argc < 2) {
    cout << "Usage: " << argv[0] << " threshould [number of markers]" << endl;
    return -1;
  }
  
  double threshould = atof( argv[1]);
  int n_markers = argc < 3 ? 0 : atoi( argv[2]);

  Document doc( "image.bmp", "template.bmp");
  puts( doc.Load() ? "Successfully loaded." : "Load failed.");
  int n_found = doc.Match( threshould);
  printf( "%i markers detected.\n", n_found);
  if( n_markers != 0 && n_found != n_markers) {
    cout << "Too much or less markers; " << n_found << " found but " << n_markers << " expected." << endl;
    cout << "Hint: you can change the threshould to adjust the number of the markers found." << endl;
    cout << "      Lowering the threshould will find more markers, or vice versa." << endl;
    return -1;
  }

  int i = 1;
  string s;
  ifstream ifile("script.txt");
  while( ifile && getline( ifile, s)) {
    cout << i << ": " << s << endl;
    doc.Say( (char*)s.c_str(), i - 1);
    i++;
  }

  doc.Everything();

  return 0;
}

