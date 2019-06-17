#include "pch.h" 
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core.hpp>
#include<numeric>
#include<iostream>

using namespace std;
using namespace cv;


//Calculate the average pixel error
double avg_pixel_error(Mat orig, Mat nor, int rows, int cols)
{
	int r = nor.rows;
	int c = nor.cols;
	int ototal = 0;
	int ntotal = 0;
	for (int row = 0; row < rows; row++)
	{
		for (int column = 0; column < cols; column++)
		{
			ototal = ototal +  orig.at<uchar>(row, column);
		}
	}

	for (int row = 0; row < r; row++)
	{
		for (int column = 0; column < c; column++)
		{
			ntotal = ototal + nor.at<uchar>(row, column);
		}
	}
	double a = ototal * (rows * cols);
	double b = ntotal * (r * c);
	double d = (a / b);
	double power = pow(d, 2);
	double pos = sqrt(power);
	return pos;

}

//Do dot product between 2 Mat variables
double dot_product(Mat mat1, Mat mat2)
{
	double result;
	result = mat1.at<float>(0) * mat2.at<float>(0) + mat1.at<float>(1) * mat2.at<float>(1) 
		+ mat1.at<float>(2) * mat2.at<float>(2);
	return result;
}

int main()
{

	// call original transform
	vector<Point2f> srcimg(4); 
	vector<Point2f> dstimg(4); //33, 75, 63, 57
	srcimg[0] = Point2f(51, 33); 
	srcimg[1] = Point2f(50, 75);
	srcimg[2] = Point2f(76, 63);
	srcimg[3] = Point2f(91, 57);

	//mapped features on destination image
	dstimg[0] = Point2f(10, 10);
	dstimg[1] = Point2f(10, 38);
	dstimg[2] = Point2f(20, 24);
	dstimg[3] = Point2f(35, 24);

	//read image file
	Mat tempImage = imread("10.pgm", IMREAD_GRAYSCALE);
	int rows = tempImage.rows;
	int cols = tempImage.cols;

	//declare and define target file
	Mat norma = Mat(48, 40, tempImage.type()); 

	//define U, D, VT and calculate using SVD 
	float fb1[4];
	float fb2[4];
	Mat U, D, Vt;
	float* a = (float*)malloc(12 * 4 * sizeof(float));
	Mat AM(4, 3, CV_32F, a);
	for (int i = 0; i < 4; i++)
	{
		int j = i * 3;
		//int k = i * 6 + 3;
		a[j] = srcimg[i].x;
		a[j + 1] = srcimg[i].y;
		a[j + 2] = 1;
		fb1[i] = dstimg[i].x;
		fb2[i] = dstimg[i].y;

	}
	cv::SVD::compute(AM, D, U, Vt);

	//calculate A_plus matrix(3 by 3)
	vector<float> vec;
	D.copyTo(vec);

	//cout << "\n" << D;
	//cout << "\n" << U;
	//cout << "\n" << Vt;

	float dff[] = {
		vec[0],0,0,
		0,vec[1],0,
		0,0,vec[2],
	};
	Mat da(3, 3, CV_32F, dff);
	Mat di;
	cv::invert(da, di, DECOMP_SVD);
	//cout << "\n" << U.t();
	//cout << "\n" << di;
	//cout << "\n" << Vt.t();
	Mat A_plus;
	A_plus = Vt.t() * di * U.t();
	//cout << "\n" << A_plus;

	Mat ffb1(4, 1, CV_32F, fb1);
	Mat ffb2(4, 1, CV_32F, fb2);

	// calculate the values of b
	Mat c1;
	Mat c2;
	// Calculate individual vectors c1 and c2
	c1 = A_plus * ffb1;
	c2 = A_plus * ffb2;

	Mat afftm = (Mat_<float>(2, 3) << c1.at<float>(0), c1.at<float>(1), 
		c1.at<float>(2), c2.at<float>(0), c2.at<float>(1),
		c2.at<float>(2));
	//Affine transformation parameter 2 by 3 matrix
	cout << afftm;

	//Pixel by pixel performing affine transformation on original image
	const int dstx = 48;
	const int dsty = 40;
	int x = 0;
	int y = 0;
	int new_x = 0;
	int new_y = 0;
	Mat *coordinates;  
	for (int row = 0; row < rows; row++)
	{
		for (int column = 0; column < cols; column++)
			{
				float *imageCoordinates = new float[3]{ (float)row, (float)column, 1 };
				coordinates = new Mat(1, 3, CV_32F, imageCoordinates);
				new_x = dot_product(*coordinates, c1);
				new_y = dot_product(*coordinates, c2);
				//normalize new x and y values  
				if (new_x >= 0 && new_x < dstx && new_y >= 0 && new_y < dsty)
				{
					new_x = (int)new_x;
					new_y = (int)new_y;
					norma.at<uchar>(new_x, new_y) = tempImage.at<uchar>(row, column);
				}
				delete (imageCoordinates);
				delete (coordinates);
		}
	}
	//Stroing the normalized image
	norma.reshape(40, 48);
	cv::imwrite("10.jpg", norma);
	cv::imshow("Normalized_image", norma);

	//Calculate the average pixel error for given image and its normalized image
	double avg_pixel = 0;
	avg_pixel = avg_pixel_error(tempImage, norma, rows, cols);
	cout << avg_pixel;
	waitKey(0);
}



