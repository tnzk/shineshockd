#include<opencv/cv.h>
#include<opencv/highgui.h>
#include<iostream>
#include<stdio.h>

using namespace cv;
using namespace std;



int main( int argc, char** argv)
{
  IplImage *src = 0, *tmpl = 0;
  IplImage *ftmp;
  IplImage *copy_img;
  double min_value, max_value;
  CvPoint min_loc, max_loc;
  CvFont font;
  char* text = "hoge";
  CvSize text_size;
  
  src = cvLoadImage("image.bmp", CV_LOAD_IMAGE_COLOR);
  tmpl = cvLoadImage("template.bmp", CV_LOAD_IMAGE_COLOR);

  if(src == 0 || tmpl == 0){
    cout << "no files" << endl;
    return -1;
  }

  cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0, 4, 4);
  
  ftmp = cvCreateImage(cvSize(src->width - tmpl->width + 1, src->height - tmpl->height + 1), 32, 1);
    
  cvMatchTemplate(src, tmpl, ftmp, CV_TM_CCOEFF);
  cvMinMaxLoc(ftmp, &min_value, &max_value, &min_loc, &max_loc, NULL);
  //  cvRectangle(src, max_loc, cvPoint(max_loc.x + tmpl->width, max_loc.y + tmpl->height), CV_RGB(0, 244, 255), 3);

  printf("%i\n", ftmp->widthStep);
  printf("%i, %i\n", max_loc.x, max_loc.y);
  printf("%f\n", max_value);
  for( int y = 0; y < ftmp->height; y++) {
    for( int x = 0; x < ftmp->width; x++) {
      CvScalar s = cvGet2D(ftmp,y,x);
      int xx = x + 100 - 1;
      int yy = y + 100 - 1;
      //cvRectangle(src, cvPoint(xx,yy), cvPoint(xx + 1, yy + 1), CV_RGB(s.val[0],0,0), 1);
      if(s.val[0] > 1170000000) {
	printf( "%3i, %3i: %20.2f\n", x, y , s.val[0]);
	cvRectangle(src, cvPoint(x,y), cvPoint(x + tmpl->width, y + tmpl->height), CV_RGB(0, 244, 255), 1);
      }
    }
  }

  cvGetTextSize(text, &font, &text_size, 0);
  cvPutText(src, text, max_loc, &font, CV_RGB(255 , 0 , 255));

  cvSaveImage("result.png", src);
  cvSaveImage("result_f.png", ftmp);

  return 0;
}

