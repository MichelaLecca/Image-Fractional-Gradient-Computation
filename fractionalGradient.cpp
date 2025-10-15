
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;


#define SAVE


// compute kernels for fractional gradient computation -- here the approximation proposed by 

double Mnorm (double sigma)
{
  double M = 1 - sigma + sigma/tgamma(sigma);
  return M;
}


Mat
MP2_KernelX (double sigma)
{
  Mat k = Mat::zeros(3, 3, CV_64F);
   
  double nn = Mnorm(sigma)*tgamma(sigma);

  k.at<double>(0, 0) = (2*tgamma(sigma)*(1-sigma) + 1)/(2*nn);
  k.at<double>(0, 1) = pow(2, sigma-1)/nn;
  k.at<double>(0, 2) = (pow(3, sigma)-1)/(2*nn) ;

  k.at<double>(1, 0) = - (pow(4, sigma) -pow(2, sigma))/(2*nn);
  k.at<double>(1, 2) = -k.at<double>(1, 0);
      
  k.at<double>(2, 0) = -k.at<double>(0, 2);
  k.at<double>(2, 1) = -k.at<double>(0, 1);
  k.at<double>(2, 2) = -k.at<double>(0, 0);
  
  return k;
}


Mat
MP2_KernelY (double sigma)
{
  Mat k = Mat::zeros(3, 3, CV_64F);
  double nn = Mnorm(sigma)*tgamma(sigma);

  k.at<double>(0, 0) = -(pow(3, sigma)- 1)/(2*nn);
  k.at<double>(0, 1) = -(pow(4, sigma) - pow(2, sigma))/(2*nn);
  k.at<double>(0, 2) = (2*tgamma(sigma)*(1-sigma) + 1)/(2*nn);

  k.at<double>(1, 0) = -pow(2, sigma-1)/ nn;
  k.at<double>(1, 2) = -k.at<double>(1, 0);
      
  k.at<double>(2, 0) = -k.at<double>(0, 2);
  k.at<double>(2, 1) = -k.at<double>(0, 1);
  k.at<double>(2, 2) = -k.at<double>(0, 0);
  
  return k;
}



// show image with real values  (double) -- for visualization task image values are rescaled over [0:255]
void
ShowDoubleImage (Mat dimage, int key)
{
  int    rows = dimage.rows, cols = dimage.cols;
  int    r, c, v;
  double x, range;
  Mat    image;
  double max = -65537;
  double min = -max;

  for(r = 0; r < rows; ++r)
    for(c = 0; c < cols; ++c)
      {
	if(dimage.at<double>(r, c) > max)
	  max = dimage.at<double>(r, c);

	if(dimage.at<double>(r, c) < min)
	  min = dimage.at<double>(r, c);
      }
  range = max - min;

  image.create(rows, cols, 0);

  for(r = 0; r < rows; ++r)
    for(c = 0; c < cols; ++c)
      {
	x = (dimage.at<double>(r, c) - min)/range;
	v = (int)(x*255.0);
	image.at<uchar>(r, c) = v;
      }


 namedWindow("image (double) ", WINDOW_NORMAL);
 imshow("image (double) ", image);
 waitKey(key);

 
 return;
}

// compute magnitude of fractional gradient with L-infty norm

Mat
ComputeMagnitudeLinf (Mat x, Mat y)
{
  int r, c, nr = x.rows, nc = y.cols;
  Mat m = Mat::zeros(nr, nc, CV_64F);

  for(r = 0; r < nr; ++r)
    for(c = 0; c < nc; ++c)
      {	
	m.at<double>(r, c) = abs(x.at<double>(r, c));
	m.at<double>(r, c) = max(m.at<double>(r, c),  abs(y.at<double>(r, c)));
        if(m.at<double>(r, c) > 255)
	   m.at<double>(r, c) = 255;
      }
  return m;
}

/*************************** MAIN PROGRAM *************************************************/

int
main (int argc, char* argv[])
{
  Mat    ima = imread(argv[1]);  // input image
  int    gssize = atoi(argv[2]); // gaussian smoothing; if < 0 no smoothing is performed
  double sigma = atof(argv[3]);  // fractional derivative order
  
  // convert input (color or gray level) image to double-valued grayscale 
  Mat lum, tmp;
  if(ima.type() == CV_8UC3)
    {
      Mat  tmp1;
      cvtColor(ima, tmp, COLOR_BGR2GRAY);
      if(gssize > 0)
	{
	  GaussianBlur(tmp, tmp1, Size(gssize, gssize), 0, 0);
	  tmp1.convertTo(lum, CV_64FC1);
	}
      else
	tmp.convertTo(lum, CV_64FC1);
    }
    else
    {
      if(gssize > 0)
	{
	  GaussianBlur(ima, tmp, Size(gssize, gssize), 0, 0);
	  tmp.convertTo(lum, CV_64FC1);
	}
      else
	ima.convertTo(lum, CV_64FC1); //, 1.0);
    }
 
  // compute kernels of fractional derivatives with order sigma
  Mat   kernX = MP2_KernelX(sigma);
  Mat   kernY = MP2_KernelY(sigma);

  cout << " Fractional derivatives kernels "<<endl;
  cout << kernX<< endl;
  cout << " -- " << endl;
  cout << kernY<< endl;
  
  
  // compute horizontal and vertical fractional derivatives xgrad and ygrad
  Mat    xgrad, ygrad;
      
  filter2D(lum, xgrad, -1, kernX, Point( -1, -1 ), 0, BORDER_DEFAULT);
  filter2D(lum, ygrad, -1, kernY, Point( -1, -1 ), 0, BORDER_DEFAULT);

  
  // compute magnitude with Linfty norm
  Mat magn = ComputeMagnitudeLinf (xgrad, ygrad);
  ShowDoubleImage(magn, 0);

  imwrite("magn.png", magn);
}
