#pragma once
#include<opencv.hpp>
using namespace cv;

namespace psd1
{
	Vec3b get_color(int v)
	{
		Vec3b res;
		if (v < 200 || v>1475)
		{
			res[0] = 0;
			res[1] = 0;
			res[2] = 0;
			return res;
		}
		if (v <= 455)
		{
			res[0] = 255;
			res[1] = 0;
			res[2] = v - 200;
		}
		else
		{
			if (v <= 710)
			{
				res[0] = 710 - v;
				res[1] = 0;
				res[2] = 255;
			}
			else
			{
				if (v <= 965)
				{
					res[0] = 0;
					res[1] = v - 710;
					res[2] = 255;
				}
				else
				{
					if (v <= 1220)
					{
						res[0] = 0;
						res[1] = 255;
						res[2] = 1220 - v;
					}
					else
					{
						if (v <= 1475)
						{
							res[0] = v - 1220;
							res[1] = 255;
							res[2] = 0;
						}
					}
				}
			}
		}
		return res;
	}
	Mat pseudocolor(float* p, int rows, int cols)
	{
		int n = rows * cols;
		Mat res(rows, cols, CV_8UC3);
		int pmax = 1475;
		int pmin = 200;

		for (int i = 0; i < n; i++)
		{
			int x = i % cols;
			int y = i / cols;
			float val = p[i];
			val = (val > pmax ? pmax : val);
			val = (val < pmin ? pmin : val);
			Vec3b clr = get_color(val);
			res.at<uchar>(y, 3 * x + 0) = clr[2];
			res.at<uchar>(y, 3 * x + 1) = clr[1];
			res.at<uchar>(y, 3 * x + 2) = clr[0];
		}
		return res;
	}
};

namespace psd2
{
	Vec3b hsvToRgb(float h, float s, float v)
	{
		Vec3b rgb(0, 0, 0);
		int i = int(h * 6);
		float f = h * 6 - i;
		float p = v * (1 - s);
		float q = v * (1 - f * s);
		float t = v * (1 - (1 - f) * s);
		float r = 0.0, g = 0.0, b = 0.0;
		int key = i % 6;
		switch (key)
		{
		case 0:
			r = v, g = t, b = p;
			break;
		case 1:
			r = q, g = v, b = p;
			break;
		case 2:
			r = p, g = v, b = t;
			break;
		case 3:
			r = p, g = q, b = v;
			break;
		case 4:
			r = t, g = p, b = v;
			break;
		case 5:
			r = v, g = p, b = q;
			break;
		}
		rgb[0] = r * 255;
		rgb[1] = g * 255;
		rgb[2] = b * 255;
		return rgb;
	}
	Vec3b get_color(int val)
	{
		if (val < 100 || val>2254)
		{
			Vec3b res;
			res[0] = 0;
			res[1] = 0;
			res[2] = 0;
			return res;
		}
		float h = 0;
		float s = 1.0;
		float v = 1.0;
		if (val <= 459)
			h = val - 100;
		else
		{
			if (val <= 818)
				h = val - 459;
			else
			{
				if (val <= 1177)
					h = val - 818;
				else
				{
					if (val < 1536)
						h = val - 1177;
					else
					{
						if (val < 1895)
							h = val - 1536;
						else
						{
							if (val < 2254)
								h = val - 1895;
						}
					}
				}
			}
		}
		h = float(h);
		h = h / 360.0;
		float v_max = 1.00;
		float v_min = 0.25;
		v = 1.0 - (float(v_max - v_min) / 2154.0) * float(val - 100);
		s = v;
		Vec3b res = hsvToRgb(h, s, v);
		return res;
	}
	Mat pseudocolor(float* dpth, int rows, int cols)
	{
		Mat res(rows, cols, CV_8UC3, Scalar(0, 0, 0));
		int n = rows * cols;
		for (int i = 0; i < n; i++)
		{
			int x = i % cols;
			int y = i / cols;
			float val = dpth[i];
			Vec3b v = get_color(val);
			res.at<uchar>(y, 3 * x + 0) = v[2];
			res.at<uchar>(y, 3 * x + 1) = v[1];
			res.at<uchar>(y, 3 * x + 2) = v[0];
		}
		return res;
	}
};

