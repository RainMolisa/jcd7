#pragma once
#include<vector>
#include<opencv.hpp>

namespace ansis
{
	std::vector<cv::Vec4f> peak_sis(cv::Rect rt,cv::Mat peak,cv::Mat max_i)
	{
		std::vector<cv::Vec4f> res;
		int len = peak.size[2];
		for (int y = rt.y; y < rt.y + rt.height; y++)
		{
			for (int x = rt.x; x < rt.x + rt.width; x++)
			{
				int i = max_i.at< int16_t>(y, x);
				if (i - 1 >= 0 && i + 1 < len)
				{
					float v1 = peak.at<int16_t>(y, x, i);
					float v2 = peak.at<int16_t>(y, x, i - 1);
					float v3 = peak.at<int16_t>(y, x, i + 1);
					float v_max = (v2 > v3 ? v2 : v3);
					float v_min = (v2 < v3 ? v2 : v3);
					float val1 = (v1 - v_max);
					float val2 = (v1 - v_max) / v1;
					float val3 = (v1 - v_min);
					float val4 = (v1 - v_min) / v1;
					cv::Vec4f val;
					val[0] = val1;
					val[1] = val2;
					val[2] = val3;
					val[3] = val4;
					res.push_back(val);
				}
			}
		}
		return res;
	}
};