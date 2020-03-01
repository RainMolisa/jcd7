#pragma once
#include<string>
#include<stdio.h>
#include<opencv.hpp>

cv::Mat binref(std::string path,int rows,int cols)
{
	FILE* fp = fopen(path.c_str(), "rb");
	uchar* data = new uchar[cols * rows / 8];
	fread(data, sizeof(uchar), cols * rows / 8, fp);
	fclose(fp);
	uchar* output = new uchar[cols * rows];
	cv::Mat res(rows, cols, CV_8UC1);
	for (int i = 0; i < cols * rows / 8; i++) {
		for (int j = 0; j < 8; j++) {
			if ((data[i] >> j & 1) == 1) {
				output[i * 8 + j] = 255;
			}
			else {
				output[i * 8 + j] = 0;
			}
		}
	}
	for (int i = 0; i < cols * rows; i++)
	{
		int x = i % cols;
		int y = i / cols;
		res.at<uchar>(y, x) = output[i];
	}
	delete[]data;
	delete[]output;
	return res;
}

cv::Mat binref2(std::string path, int rows, int cols)
{
	cv::Mat r1 = binref(path, cols, rows);
	cv::Mat ig2(rows, cols, CV_8UC1, cv::Scalar(0));
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < cols; x++)
		{
			ig2.at<uchar>(y, x) = r1.at<uchar>(x, y);
		}
	}
	return ig2;
}


