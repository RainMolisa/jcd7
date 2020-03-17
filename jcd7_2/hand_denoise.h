#pragma once
#include<opencv.hpp>
#include<vector>
#include"..\jcd7\pseudocolor.h"
#include"..\jcd7\dpthbin.h"
#include "..\jcd7_0\seed_seg.h"
namespace hdde
{
	const std::string win_name = "point";
	//float scale = 1;
	cv::Mat_<int> seg_map;
	std::vector<std::vector<cv::Point>>* setp;
	float* org;
	int rows, cols;
	const int go_step = 15;

	Point p1;
	int i1;
	Point p2;
	bool m_flag = false;
	cv::Rect shw_rt(0, 0, 700,700);
	//
	cv::Mat img_rect(cv::Mat& img, cv::Rect area);
	void OnMouse(int event, int x, int y, int flags, void* param)
	{
		float* p = (float*)param;
		switch (event)
		{
		case cv::EVENT_LBUTTONDOWN:
		{
			p1.x = shw_rt.x + x;
			p1.y = shw_rt.y + y;
			i1 = seg_map(p1);
			m_flag = true;
			break;
		}
		case cv::EVENT_MOUSEMOVE:
		{
			if (m_flag)
			{
				p2.x = shw_rt.x + x;
				p2.y = shw_rt.y + y;
				float val = org[p2.y * cols + p2.x];
				for (int i = 0; i < (*setp)[i1].size(); i++)
				{
					p[(*setp)[i1][i].y * cols + (*setp)[i1][i].x] = val;
				}
			}
			//system("cls");

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
			cv::Mat shw1 = psd2::pseudocolor(res, rows, cols);
			cv::Mat shw2 = img_rect(shw1, shw_rt);
			imshow(win_name, shw2);
			int key = cv::waitKey(5);
			switch (key)
			{
			case 'w':
				shw_rt.y = shw_rt.y - go_step;
				shw_rt.y = (shw_rt.y <0 ? 0 : shw_rt.y);
				break;
			case 's':
				shw_rt.y = shw_rt.y + go_step;
				shw_rt.y = (shw_rt.y >= rows-shw_rt.height ? rows - shw_rt.height : shw_rt.y);
				break;
			case 'a':
				shw_rt.x = shw_rt.x - go_step;
				shw_rt.x = (shw_rt.x < 0 ? 0 : shw_rt.x);
				break;
			case 'd':
				shw_rt.x = shw_rt.x + go_step;
				shw_rt.x = (shw_rt.x >= cols - shw_rt.width ? cols - shw_rt.width : shw_rt.x);
				break;
			case 'm':
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
		std::vector<std::vector<cv::Point>> s2 = sdsg::seed_seg(res, rows, cols);
		float* res2 = sgns::denoise(res, rows, cols, s2);
		delete[] res;
		dbn::write_depth(res2, rows, cols, ot_file);
		return res2;
	}
	cv::Mat img_rect(cv::Mat& img,cv::Rect area)
	{
		int r = area.height;
		int c = area.width;
		cv::Mat res(r,c,CV_8UC3,Scalar(0,0,0));
		for (int y = area.y; y < area.y+r; y++)
		{
			for (int x = area.x; x < area.x+c; x++)
			{
				if (y >= 0 && y < rows && x >= 0 && x < cols)
				{
					int x0 = x - area.x;
					int y0 = y - area.y;
					res.at<uchar>(y0, 3 * x0 + 0) = img.at<uchar>(y, 3 * x + 0);
					res.at<uchar>(y0, 3 * x0 + 1) = img.at<uchar>(y, 3 * x + 1);
					res.at<uchar>(y0, 3 * x0 + 2) = img.at<uchar>(y, 3 * x + 2);
				}
				
			}
		}

		return res;
	}
};