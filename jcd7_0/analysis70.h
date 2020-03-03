#pragma once
#include<vector>
#include<opencv.hpp>

namespace ansis
{
	std::vector<float> peak_sis(cv::Rect rt,cv::Mat peak,cv::Mat max_i)
	{
		std::vector<float> res;
		res.reserve(rt.width * rt.height);
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
					float v = (v2 > v3 ? v2 : v3);
					float val = (v1 - v)/v1;
					res.push_back(val);
				}
				else
				{
					res.push_back(-1);
				}
			}
		}
		return res;
	}
};