#pragma once
#include<opencv.hpp>
#include<vector>
namespace f2tg
{
	std::vector<std::vector<cv::Point>> seg_bin(cv::Mat img)
	{
		int rows = img.rows;
		int cols = img.cols;
		std::vector<std::vector<cv::Point>> set;
		cv::Mat_<int> mask(rows, cols, 0);
		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < cols; x++)
			{
				if (img.at<uchar>(y, x) == 255)
				{
					if (mask(y, x) == 0)
					{
						mask(y, x) = 255;
						std::vector<cv::Point> area;
						area.push_back(cv::Point(x, y));
						std::vector<cv::Point> m_stack;
						m_stack.push_back(cv::Point(x, y));
						while (m_stack.size() > 0)
						{
							cv::Point cor = m_stack[m_stack.size() - 1];
							m_stack.pop_back();
							cv::Point cddt[4];
							cddt[0].x = cor.x; cddt[0].y = cor.y + 1;
							cddt[1].x = cor.x; cddt[1].y = cor.y - 1;
							cddt[2].x = cor.x + 1; cddt[2].y = cor.y;
							cddt[3].x = cor.x - 1; cddt[3].y = cor.y;
							for (int i = 0; i < 4; i++)
							{
								if (cddt[i].x >= 0 && cddt[i].x < cols && cddt[i].y >= 0 && cddt[i].y < rows)
								{
									int val = img.at<uchar>(cddt[i].y, cddt[i].x);
									if (mask(cddt[i].y, cddt[i].x) == 0 && val == 255)
									{
										mask(cddt[i].y, cddt[i].x) = 255;
										area.push_back(cddt[i]);
										m_stack.push_back(cddt[i]);
									}
								}
							}
						}
						set.push_back(area);
					}
				}
			}
		}
		return set;
	}
	cv::Mat offset2targ(cv::Mat &ref,cv::Mat &cur,float* offset,int16_t inalid)
	{
		int rows = ref.rows;
		int cols = ref.cols;
		cv::Mat ref2 = ref.clone();
		cv::Mat cur2 = cur.clone();
		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < cols; x++)
			{
				ref2.at<uchar>(y, x) = (ref2.at<uchar>(y, x) > 0 ? 255 : 0);
				cur2.at<uchar>(y, x) = (cur2.at<uchar>(y, x) > 0 ? 255 : 0);
			}
		}
		//imshow("a", cur2);
		//waitKey();
		cv::Mat res(rows, cols, CV_8UC1,Scalar(0));
		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < cols; x++)
			{
				int i = y * cols + x;
				float offset_v = (offset[i]) / 16.0;
				int buf = round(offset_v);
				int x0 = x + buf;
				if (x0 >= 0 && x0 < cols)
				{
					if (!(inalid/16 >= buf-1 && inalid / 16 <= buf + 1))
					{
						res.at<uchar>(y, x) = ref2.at<uchar>(y, x0);
					}
					
				}
			}
		}

		std::vector<std::vector<cv::Point>> res_set = seg_bin(res);
		const float coincdnc_dgr_hold = 0.30;
		cv::Mat res2(rows, cols, CV_8UC1,Scalar(0));
		for (int i = 0; i < res_set.size(); i++)
		{
			float res_seti_len = res_set[i].size();
			float sum = 0;
			for (int j = 0; j < res_set[i].size(); j++)
			{
				if (cur2.at<uchar>(res_set[i][j]) > 0)
				{
					sum++;
				}
			}
			float coincdnc_dgr = sum / res_seti_len;
			if (coincdnc_dgr >= coincdnc_dgr_hold)//
			{
				for (int j = 0; j < res_set[i].size(); j++)
				{
					res2.at<uchar>(res_set[i][j]) = 255;
				}
			}
		}
		return res2;
	}
};