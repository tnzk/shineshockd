#include "document.h"

Detection::Detection( int x, int y, double sim)
{
  this->x = x;
  this->y = y;
  this->sim = sim;
}

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

int Document::Match( double threshould)
{
  cvMatchTemplate(src, tmpl, result, CV_TM_CCOEFF_NORMED);
  for( int y = 0; y < result->height; y++) {
    for( int x = 0; x < result->width; x++) {
      CvScalar s = cvGet2D(result,y,x);
      int xx = x + tmpl->width - 1;
      int yy = y + tmpl->height - 1;
      if(s.val[0] >= threshould)
	detections.push_back( Detection( x, y, s.val[0]));
    }
  }
  return detections.size();
}

void Document::Everything()
{
  IplImage* img;
  vector<Detection>::iterator it = detections.begin();
  while( it != detections.end()) {
    printf(" %3i, %3i: %20.20f\n", it->x, it->y, it->sim);
    img = cvCloneImage( src);
    cvRectangle( img, cvPoint( it->x, it->y), cvPoint( it->x + tmpl->width, it->y + tmpl->height), CV_RGB( 255, 255, 128), 0, 0);    
    it++;
  }
  cvSaveImage( "hoge.png", img);
}
