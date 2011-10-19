#include<opencv/cv.h>
#include<opencv/highgui.h>
#include<iostream>
#include<string>
#include<vector>
#include<stdio.h>

using namespace cv;
using namespace std;

class Detection {
public:
  int x;
  int y;
  int sim;
  Detection( int, int, int);
};

Detection::Detection( int x, int y, int sim)
{
  this->x = x;
  this->y = y;
  this->sim = sim;
}

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
  int Match( int);
  void Everything();
};

Document::Document( string src_name, string tmpl_name)
{
  this->src_name = src_name;
  this->tmpl_name = tmpl_name;
  src = NULL;
  tmpl = NULL;
}

bool Document::Load()
{
  src = cvLoadImage("image.bmp", CV_LOAD_IMAGE_COLOR);
  tmpl = cvLoadImage("template.bmp", CV_LOAD_IMAGE_COLOR);
  if(src == 0 || tmpl == 0) return false;

  cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0, 4, 4);
  result = cvCreateImage( cvSize( src->width - tmpl->width + 1, src->height - tmpl->height + 1), 32, 1);

  return true;
}

int Document::Match( int threshould)
{
  cvMatchTemplate(src, tmpl, result, CV_TM_CCOEFF);
  for( int y = 0; y < result->height; y++) {
    for( int x = 0; x < result->width; x++) {
      CvScalar s = cvGet2D(result,y,x);
      int xx = x + tmpl->width - 1;
      int yy = y + tmpl->height - 1;
      if(s.val[0] > threshould)
	detections.push_back( Detection( x, y, s.val[0]));
    }
  }
  return detections.size();
}

void Document::Everything()
{
  vector<Detection>::iterator it = detections.begin();
  while( it != detections.end()) {
    printf(" %3i, %3i: %20i\n", it->x, it->y, it->sim);
    it++;
  }
}

int main( int argc, char** argv)
{
  Document doc( "image.bmp", "template.bmp");
  puts( doc.Load() ? "Successfully loaded." : "Load failed.");
  printf( "%i markers detected.\n", doc.Match( 1170000000));
  doc.Everything();

  //cvSaveImage("result.png", src);
  //cvSaveImage("result_f.png", ftmp);

  return 0;
}

