#pragma once
#include<vector>
#include<opencv.hpp>
#include<vector>
#include<algorithm>
namespace dsp
{
	float* dwnsp01(float* offset,int rows,int cols)
	{
		int r2 = rows / 2;
		int c2 = cols / 2;
		int n = r2 * c2;
		float* res = new float[n];
		for (int y = 0; y < r2; y++)
		{
			for (int x = 0; x < c2; x++)
			{
				cv::Point p1(2 * x, 2 * y);
				cv::Point p2(2 * x + 1, 2 * y);
				cv::Point p3(2 * x, 2 * y + 1);
				cv::Point p4(2 * x + 1, 2 * y + 1);
				float v1 = offset[p1.y * cols + p1.x];
				//float v2 = offset[p2.y * cols + p2.x];
				//float v3 = offset[p3.y * cols + p3.x];
				//float v4 = offset[p4.y * cols + p4.x];
				//float v = (v1 + v2 + v3 + v4) / 4;
				float v = v1 / 2;
				res[y * c2 + x] = v;
			}
		}
		return res;
	}
	inline float max4(float v1,float v2,float v3,float v4)
	{
		float v;
		float vs[3];
		vs[0] = v2;
		vs[1] = v3;
		vs[2] = v4;
		v = v1;
		for (int i = 0; i < 3; i++)
		{
			if (v < vs[i])
			{
				v = vs[i];
			}
		}
		return v;
	}
	float* dwnsp02(float* depth, int rows, int cols)
	{
		int r2 = rows / 2;
		int c2 = cols / 2;
		int n = r2 * c2;
		float* res = new float[n];
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
				float v = max4(v1, v2, v3, v4);
				v = v / 2;
				res[y * c2 + x] = v;
			}
		}
		return res;
	}
	float mediam(std::vector<float> &set)
	{
		std::sort(set.begin(), set.end());
		int n=set.size();
		float res;
		if (n % 2 == 0)
		{
			float r1 = set[n / 2];
			//float r2 = set[n / 2 - 1];
			//res = (r1 + r2) / 2;
			res = r1;
		}
		else
		{
			res = set[(n-1) / 2];
		}
		return res;
	}
	float* dwnsp03(float* depth, int rows, int cols)
	{
		int r2 = rows / 2;
		int c2 = cols / 2;
		int n = r2 * c2;
		float* res = new float[n];
		for (int y = 0; y < r2; y++)
		{
			for (int x = 0; x < c2; x++)
			{
				cv::Point p1(2 * x, 2 * y);
				cv::Point p2(2 * x + 1, 2 * y);
				cv::Point p3(2 * x, 2 * y + 1);
				cv::Point p4(2 * x + 1, 2 * y + 1);
				std::vector<float> set;
				set.reserve(4);
				float v1 = depth[p1.y * cols + p1.x];
				float v2 = depth[p2.y * cols + p2.x];
				float v3 = depth[p3.y * cols + p3.x];
				float v4 = depth[p4.y * cols + p4.x];
				set.push_back(v1);
				set.push_back(v2);
				set.push_back(v3);
				set.push_back(v4);
				float v = mediam(set);
				v = v / 2;
				res[y * c2 + x] = v;
			}
		}
		return res;
	}
};