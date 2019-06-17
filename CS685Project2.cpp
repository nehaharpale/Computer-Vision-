#include "pch.h" 
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core.hpp>
#include<numeric>
#include<iostream>
#include<chrono>
#include<ctime>
#include<fstream>

using namespace std;
using namespace cv;

//Quantize the color levels as per the level specified
inline uchar reduceValgeneral(const uchar val, int l )
{
	int remain = 256 / l;
	int i=1;
	if (val < remain)
	{
		return 0; // return 0
	}
	else
	{
		i = 0;
	}
	int new_remain = 0;
	int addr = 0;
	addr = remain; //store the actal value
	while (i == 0)
	{
		new_remain = remain;
		remain = remain + addr;
		if (val < remain)
		{
			i = 1;
			return new_remain;
		}
		
		
	}
}

//Get the all color values of each pixel and call the quantize it for the entire image  
void Quantize_cha(Mat &img, int fl, int l, int r, int c)
{
	uchar* pixelPtr = img.data;
	for (int row = 0; row < r; row++) //pixel X-coordinate
	{
		for (int column = 0; column < c; column++) //pixel Y-coordinate
		{
			const int pi = row * c * 3 + column * 3;
			pixelPtr[pi + 0] = reduceValgeneral(pixelPtr[pi + 0], l); // B
			pixelPtr[pi + 1] = reduceValgeneral(pixelPtr[pi + 1], l); // G
			pixelPtr[pi + 2] = reduceValgeneral(pixelPtr[pi + 2], l); // R
		}
	}
}

//Toon_shading
void part1()
{
	//read image file
	Mat tempImage = imread("Parrot.jpg", IMREAD_COLOR);
	int rows = tempImage.rows;
	int cols = tempImage.cols;
	int L[4] = { 2, 4, 8, 16 };
	int Fl = 256;
	cv::imshow("Parrot", tempImage);
	for (int i = 0; i < 4; i++)
	{
		cv::Mat retImage = tempImage.clone();
		Quantize_cha(retImage, Fl, L[i], rows, cols);
		std::ostringstream name;
		name << "Parrot" << i << ".jpg";
		//store image file
		cv::imwrite(name.str(), retImage);
	}



	waitKey(0);
}
//Get the Saturaion value for each pixel and quantize it. Do this for entire image
void Quantize_channels1(Mat &img, int fl, int l, int r, int c)
{
	uchar* pixelPtr = img.data;
	for (int row = 0; row < r; row++) //pixel X-coordinate
	{
		for (int column = 0; column < c; column++) //pixel Y-coordinate
		{
			const int pi = row * c * 3 + column * 3;
			pixelPtr[pi + 1] = reduceValgeneral(pixelPtr[pi + 1], l); // S
		}
	}

}

//Color Compression
void part2()
{
	//read image file
	Mat tempImage = imread("Parrot.jpg", IMREAD_COLOR);
	Mat tempImage1;
	//Convert to HSV
	cvtColor(tempImage, tempImage1, COLOR_BGR2HSV); 
	int rows = tempImage1.rows;
	int cols = tempImage1.cols;
	int L[4] = { 2, 4, 8, 16 };
	//int L = 2;
	int Fl = 256;
	cv::imshow("Parrot", tempImage1);
	for (int i = 0; i < 4; i++)
	{
		cv::Mat retImage = tempImage1.clone();
		Quantize_channels1(retImage, Fl, L[i], rows, cols);
		std::ostringstream name;
		name << "Parrot" << 2 << i << ".jpg";
		Mat retImage1;
		//Convert to BGR
		cvtColor(retImage, retImage1, COLOR_HSV2BGR);
		cv::imwrite(name.str(), retImage1);
	}
	waitKey(0);
}
//HoughCircle Detection
void part3()
{
	//read image file
	Mat tempImage1 = imread("LiDAR01.png", IMREAD_COLOR);
	int rows = tempImage1.rows;
	int cols = tempImage1.cols;
	Mat tempImage = Mat(rows, cols, tempImage1.type());
	//Convert to GrayScale
	cvtColor(tempImage1, tempImage, COLOR_BGR2GRAY);
	
	//Compute current time
	auto start = std::chrono::system_clock::now();
	//Smooth the Image
	medianBlur(tempImage, tempImage, 3);
	vector<Vec3f> circles;
	//Detect circles
	HoughCircles(tempImage, circles, HOUGH_GRADIENT, 1, rows / 16, 400, 20, 0, 40);
	vector<Vec3f> fcircles = circles;
	//Detect circles
	HoughCircles(tempImage, circles, HOUGH_GRADIENT, 1, rows / 16, 300, 16, 0, 30);
	fcircles.insert(fcircles.end(), circles.begin(), circles.end());
	
	//open the circles.xlss file
	ofstream myfile;
	myfile.open("circles.xlsx");
	for (size_t i = 0; i < fcircles.size(); i++)
	{
		Point center(cvRound(fcircles[i][0]), cvRound(fcircles[i][1]));
		int radius = cvRound(fcircles[i][2]);
		//store the x and y coordinates with radius
		myfile <<"\n" << fcircles[i][0] << "\t" << fcircles[i][1] << "\t" << fcircles[i][2];
		// draw the circle center
		circle(tempImage1, center, 3, Scalar(0, 0, 0), -1, 8, 0);
		// draw the circle outline
		circle(tempImage1, center, radius, Scalar(0, 0, 255), 3, 8, 0);
	}
	//close the xlsx file
	myfile.close();
	//save the image with detected circles
	cv::imwrite("hough1.jpg", tempImage1);
	//compute the time
	auto end = std::chrono::system_clock::now();
	//Total time required for operation
	std::chrono::duration<double> elapsed_seconds = end - start;
	cout << "Time to complete hough detection: " << elapsed_seconds.count() << "s\n";
	waitKey(0);

}

int main()
{
	//part1();
	//part2();
	part3();

}



