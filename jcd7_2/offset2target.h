#pragma once
#include<opencv.hpp>
namespace f2tg
{
	cv::Mat offset2targ(cv::Mat ref,float* offset)
	{
		int rows = ref.rows;
		int cols = ref.cols;
		cv::Mat res(rows, cols, CV_8UC1,Scalar(0));
		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < cols; x++)
			{
				int i = y * cols + x;
				float offset_v = (offset[i]) / 16;
				int buf = round(offset_v);
				int x0 = x + buf;
				if (x0 >= 0 && x0 < cols)
				{
					res.at<uchar>(y, x) = ref.at<uchar>(y, x0)*255;
				}
			}
		}
		return res;
	}
};