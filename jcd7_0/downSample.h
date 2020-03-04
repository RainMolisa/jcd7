#pragma once
#include<vector>
#include<opencv.hpp>
namespace dsp
{
	float* dwnsp01(float* depth,int rows,int cols)
	{
		int r2 = rows / 2;
		int c2 = cols / 2;
		for (int y = 0; y < r2; y++)
		{
			for (int x = 0; x < c2; x++)
			{
				cv::Point p1(2 * x, 2 * y);
				cv::Point p2(2 * x + 1, 2 * y);
				cv::Point p3(2 * x, 2 * y + 1);
				cv::Point p4(2 * x + 1, 2 * y + 1);
				float v1 = depth[p1.y * cols + p1.x];
				float v2 = depth[p2.y * cols + p2.x];
				float v3 = depth[p3.y * cols + p3.x];
				float v4 = depth[p4.y * cols + p4.x];
			}
		}
	}
	float* dwnsp02(float* depth, int rows, int cols)
	{
		int r2 = rows / 2;
		int c2 = cols / 2;
		for (int y = 0; y < r2; y++)
		{
			for (int x = 0; x < c2; x++)
			{
				cv::Point p1(2 * x, 2 * y);
				cv::Point p2(2 * x + 1, 2 * y);
				cv::Point p3(2 * x, 2 * y + 1);
				cv::Point p4(2 * x + 1, 2 * y + 1);
				float v1 = depth[p1.y * cols + p1.x];
				float v2 = depth[p2.y * cols + p2.x];
				float v3 = depth[p3.y * cols + p3.x];
				float v4 = depth[p4.y * cols + p4.x];
			}
		}
	}
	float* dwnsp03(float* depth, int rows, int cols)
	{
		int r2 = rows / 2;
		int c2 = cols / 2;
		for (int y = 0; y < r2; y++)
		{
			for (int x = 0; x < c2; x++)
			{
				cv::Point p1(2 * x, 2 * y);
				cv::Point p2(2 * x + 1, 2 * y);
				cv::Point p3(2 * x, 2 * y + 1);
				cv::Point p4(2 * x + 1, 2 * y + 1);
				float v1 = depth[p1.y * cols + p1.x];
				float v2 = depth[p2.y * cols + p2.x];
				float v3 = depth[p3.y * cols + p3.x];
				float v4 = depth[p4.y * cols + p4.x];
			}
		}
	}
};