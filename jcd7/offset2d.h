#pragma once
#include<opencv.hpp>
#include<string>
#include<fstream>
#include<stdio.h>
namespace fs2d
{
	float* read_offset(std::string fstr, int rows, int cols)
	{
		std::fstream fp;
		fp.open(fstr, std::ios::in | std::ios::binary);
		int n = rows * cols;
		float* res = new float[n];
		for (int i = 0; i < n; i++)
		{
			int16_t val;
			fp.read((char*)(&val), sizeof(int16_t));
			//printf("%d", val);
			//int val2 = val / 2;
			res[i] = val;
		}
		fp.close();
		return res;
	}
	cv::Mat show_offset(float* fst, int rows, int cols)
	{
		int n = rows * cols;
		float* fst2 = new float[n];
		for (int i = 0; i < n; i++)
		{
			fst2[i] = fst[i] / 16.0;
		}
		float min_val = fst2[0];
		float max_val = fst2[0];
		for (int i = 1; i < rows * cols; i++)
		{
			float val = fst2[i];
			min_val = (val < min_val ? val : min_val);
			max_val = (val > max_val ? val : max_val);
		}
		cv::Mat res(rows, cols, CV_8UC1,Scalar(0));
		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < cols; x++)
			{
				int i = y * cols + x;
				float val = fst2[i];
				val = (val - min_val) / (max_val-min_val);
				val = val * 255;
				res.at<uchar>(y, x) = val;
			}
		}
		delete[] fst2;
		return res;
	}
	cv::Mat show_offset2(float* fst, int rows, int cols)
	{
		int n = rows * cols;
		float* fst2 = new float[n];
		for (int i = 0; i < n; i++)
		{
			fst2[i] = fst[i] / 16.0;
		}
		float min_val = fst2[0];
		float max_val = fst2[0];
		for (int i = 1; i < rows * cols; i++)
		{
			float val = fst2[i];
			min_val = (val < min_val ? val : min_val);
			max_val = (val > max_val ? val : max_val);
		}
		cv::Mat res(rows, cols, CV_8UC1, Scalar(0));
		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < cols; x++)
			{
				int i = y * cols + x;
				float val = fst2[i];
				val = (val - min_val) / (max_val - min_val);
				val = val * 255;
				res.at<uchar>(y, x) = val;
			}
		}
		delete[] fst2;
		return res;
	}
	float* offset2depth(float* fst,int rows,int cols,
		float fxy,int baseline,int wall,int search_box,int mbsize)
	{
		int l = -(search_box - mbsize / 2);
		int n = rows * cols;
		float* res = new float[n];
		for (int i = 0; i < n; i++)
		{
			float val = fst[i];
			float dval;
			if ((fxy * baseline != wall * ((val) / 16.0)))
			{
				dval = (fxy * baseline * wall) / \
					(fxy * baseline - wall * ((val) / 16.0));
			}
			else
			{
				dval = 0;
			}
			res[i] = dval;
		}
		return res;
	}
	float offset2depth(float ofset, float fxy, int baseline, int wall)
	{
		float dval = (fxy * baseline * wall) / \
			(fxy * baseline - wall * ((ofset) / 16.0));
		return dval;
	}
	float* depth2offset(float* depth,int rows,int cols,
		float fxy,int baseline,int wall,float inv_d)
	{
		int n = rows * cols;
		float* offset = new float[n];
		for (int i = 0; i < n; i++)
		{
			float val = depth[i];
			/*if (val >= inv_d - 0.5 && val <= inv_d + 0.5)
			{
				offset[i] = 0;
			}
			else*/
			{
				float buf = fxy * baseline;
				float buf2 = (buf / float(wall) - buf / val);
				offset[i] = 16.0 * buf2;
			}
			
		}
		return offset;
	}
};

