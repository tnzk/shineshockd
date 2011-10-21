#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <string>

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
  IplImage* result;
  CvFont font;
  vector<Detection> detections;

  Document( string, string);
  bool Load();
  int Match( double);
  void Everything();
};
