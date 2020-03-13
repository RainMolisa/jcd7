#pragma once
#include<opencv.hpp>
#include<vector>
#include"..\jcd7\pseudocolor.h"
namespace hdde
{
	const std::string win_name = "point";
	float scale = 1;
	cv::Mat_<int> seg_map;
	std::vector<std::vector<cv::Point>>* setp;
	float* org;
	int rows, cols;

	Point p1;
	int i1;
	Point p2;
	bool m_flag = false;
	void OnMouse(int event, int x, int y, int flags, void* param)
	{
		float* p = (float*)param;
		switch (event)
		{
		case cv::EVENT_LBUTTONDOWN:
		{
			p1.x = x / scale;
			p1.y = y / scale;
			i1 = seg_map(p1);
			m_flag = true;
			break;
		}
		case cv::EVENT_MOUSEMOVE:
		{
			if (m_flag)
			{
				p2.x = x / scale;
				p2.y = y / scale;
				float val = org[p2.y * cols + p2.x];
				for (int i = 0; i < (*setp)[i1].size(); i++)
				{
					p[(*setp)[i1][i].y * cols + (*setp)[i1][i].x] = val;
				}
			}
			break;
		}
		case cv::EVENT_LBUTTONUP:
			m_flag = false;
			break;
		default:
			break;
		}
	}
	float* hand_dn(float* depth,int rows,int cols,\
		std::vector<std::vector<cv::Point>> dp_set,std::string ot_file)
	{
		hdde::rows = rows;
		hdde::cols = cols;
		int n = rows * cols;
		float* res = new float[n];
		for (int i = 0; i < n; i++)
		{
			res[i] = depth[i];
		}
		setp = &dp_set;
		org = depth;
		seg_map.create(rows, cols);
		for (int i = 0; i < dp_set.size(); i++)
		{
			for (int j = 0; j < dp_set[i].size(); j++)
			{
				seg_map(dp_set[i][j]) = i;
			}
		}
		//Mat sftDshw2 = psd2::pseudocolor(sftDepth2, rows, cols);
		cv::namedWindow(win_name, cv::WINDOW_AUTOSIZE);
		cv::setMouseCallback(win_name, OnMouse,res);
		bool flag = true;
		while (flag)
		{
			cv::Mat shw = psd2::pseudocolor(res, rows, cols);
			resize(shw, shw, Size(cols * scale, rows * scale));
			imshow(win_name, shw);
			int key = cv::waitKey(5);
			switch (key)
			{
			case 'd':
				scale = scale + 0.02;
				break;
			case 'f':
				scale = scale - 0.02;
				break;
			case 'z':
			{
				for (int i = 0; i < (*setp)[i1].size(); i++)
				{
					res[(*setp)[i1][i].y * cols + (*setp)[i1][i].x] = depth[(*setp)[i1][i].y * cols + (*setp)[i1][i].x];
				}
				break;
			}
			case ' ':
				flag = false;
				break;
			default:
				break;
			}
		}
		cv::destroyAllWindows();
		return res;
	}
};