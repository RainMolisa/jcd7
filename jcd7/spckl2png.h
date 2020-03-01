#pragma once
#include<opencv.hpp>
#include<string>
#include<fstream>
namespace skg
{
	cv::Mat spckl2png(std::string fname,int rows,int cols,int mode=0)
	{
		if (mode == 0)
		{
			cv::Mat img(rows, cols, CV_8UC1, cv::Scalar(0));
			std::fstream fp;
			fp.open(fname, std::ios::in | std::ios::binary);
			int n = rows * cols;
			for (int i = 0; i < n; i++)
			{
				fp.read((char*) & (img.data[i]), sizeof(uchar));
			}

			fp.close();
			return img;
		}
		if (mode == 1)
		{
			cv::Mat img(cols, rows, CV_8UC1, cv::Scalar(0));
			std::fstream fp;
			fp.open(fname, std::ios::in | std::ios::binary);
			int n = rows * cols;
			for (int i = 0; i < n; i++)
			{
				fp.read((char*) & (img.data[i]), sizeof(uchar));
			}
			fp.close();
			cv::Mat ig2(rows, cols, CV_8UC1, cv::Scalar(0));
			for (int y = 0; y < rows; y++)
			{
				for (int x = 0; x < cols; x++)
				{
					ig2.at<uchar>(y, x) = img.at<uchar>(x, y);
				}
			}
			return ig2;
		}
	}
};