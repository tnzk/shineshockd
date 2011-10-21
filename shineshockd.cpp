#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include "document.h"
 
using namespace cv;
using namespace std;

int main( int argc, char** argv)
{
  if( argc < 2) {
    cout << "Usage: " << argv[0] << " threshould" << endl;
    return -1;
  }
  
  double threshould = atof( argv[1]);
  
  Document doc( "image.bmp", "template.bmp");
  puts( doc.Load() ? "Successfully loaded." : "Load failed.");
  printf( "%i markers detected.\n", doc.Match( threshould));
  doc.Everything();

  return 0;
}

