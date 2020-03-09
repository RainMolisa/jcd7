#pragma once
#include<opencv.hpp>
#include<vector>
using namespace cv;
using namespace std;

#define jabs(a) ((a)>0 ? (a):-(a))
namespace sdsg
{
	/*typedef struct xy
	{
		int x;
		int y;
	public:
		xy() { x = 0; y = 0; };
		xy(int v1, int v2) { x = v1; y = v2; }
	};*/
	typedef cv::Point xy;
	vector<vector<xy>> seed_seg(float* depth, int rows, int cols, float dif_th = 20)
	{
		vector<vector<xy>> res;
		Mat_<int> mask(rows, cols, -1);
		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < cols; x++)
			{
				if (mask(y, x) < 0)
				{
					mask(y, x) = 1;
					vector<xy> area;
					area.push_back(xy(x, y));
					vector<xy> m_stack;
					m_stack.push_back(xy(x, y));
					while (m_stack.size() > 0)
					{
						xy cor = m_stack[m_stack.size() - 1];
						m_stack.pop_back();
						xy cddt[4];
						cddt[0].x = cor.x; cddt[0].y = cor.y + 1;
						cddt[1].x = cor.x; cddt[1].y = cor.y - 1;
						cddt[2].x = cor.x + 1; cddt[2].y = cor.y;
						cddt[3].x = cor.x - 1; cddt[3].y = cor.y;
						for (int i = 0; i < 4; i++)
						{
							if (cddt[i].x >= 0 && cddt[i].x < cols && cddt[i].y >= 0 && cddt[i].y < rows)
							{
								float v1 = depth[(cor.y) * cols + (cor.x)];
								float v2 = depth[(cddt[i].y) * cols + (cddt[i].x)];
								if (mask(cddt[i].y, cddt[i].x) < 0 && jabs(v1 - v2) < dif_th)
								{
									mask(cddt[i].y, cddt[i].x) = 1;
									area.push_back(cddt[i]);
									m_stack.push_back(cddt[i]);
								}
							}
						}
					}
					res.push_back(area);
				}
			}
		}
		return res;
	}
	Vec3b getColor(int v)
	{
		Vec3b res;
		int r = (v >> 16)& (0xFF);
		int g = (v >> 8)& (0xFF);
		int b = v & (0xFF);
		res[0] = r;
		res[1] = g;
		res[2] = b;

		return res;
	}
	Mat show_seg(vector<vector<xy>>& seg, int rows, int cols)
	{
		Mat res(rows, cols, CV_8UC3);
		float sg_size = seg.size();
		for (int i = 0; i < seg.size(); i++)
		{
			float val = (float(i) / sg_size) * (0xFFFFFF);
			Vec3b v3 = getColor(val);
			for (int j = 0; j < seg[i].size(); j++)
			{
				int x = seg[i][j].x;
				int y = seg[i][j].y;
				res.at<uchar>(y, 3 * x + 0) = v3[2];
				res.at<uchar>(y, 3 * x + 1) = v3[1];
				res.at<uchar>(y, 3 * x + 2) = v3[0];
			}
		}
		return res;
	}
};

#undef jabs
