#pragma once
#include<opencv.hpp>
#include<vector>
namespace bft
{
	cv::Mat bin_filter01(cv::Mat img)
	{
		cv::Mat res = img.clone();
		for (int y = 1; y < res.rows-1; y++)
		{
			for (int x = 1; x < res.cols-1; x++)
			{
				int val = res.at<uchar>(y, x);
				if (val == 255)
				{
					int v1 = res.at<uchar>(y - 1, x);
					int v2 = res.at<uchar>(y + 1, x);
					int v3 = res.at<uchar>(y, x + 1);
					int v4 = res.at<uchar>(y, x - 1);
					if (v1 == 0 && v2 == 0 && v3 == 0 && v4 == 0)
					{
						res.at<uchar>(y, x) = 0;
					}
				}
			}
		}
		return res;
	}
	cv::Mat bin_filter02(cv::Mat img)
	{
		cv::Mat r1=bin_filter01(img);
		int rows=img.rows;
		int cols=img.cols;
		std::vector<std::vector<cv::Point>> set;
		cv::Mat_<int> mask(rows,cols,0);
		for(int y=0;y<rows;y++)
		{
			for(int x=0;x<cols;x++)
			{
				if(r1.at<uchar>(y,x)==255)
				{
					if(mask(y,x)==0)
					{
						mask(y,x)=255;
						std::vector<cv::Point> area;
						area.push_back(cv::Point(x,y));
						std::vector<cv::Point> m_stack;
						m_stack.push_back(cv::Point(x,y));
						while(m_stack.size()>0)
						{
							cv::Point cor=m_stack[m_stack.size()-1];
							m_stack.pop_back();
							cv::Point cddt[4];
							cddt[0].x = cor.x; cddt[0].y = cor.y + 1;
							cddt[1].x = cor.x; cddt[1].y = cor.y - 1;
							cddt[2].x = cor.x + 1; cddt[2].y = cor.y;
							cddt[3].x = cor.x - 1; cddt[3].y = cor.y;
							for(int i=0;i<4;i++)
							{
								if(cddt[i].x >= 0 && cddt[i].x < cols && cddt[i].y >= 0 && cddt[i].y < rows)
								{
									int val=r1.at<uchar>(cddt[i].y,cddt[i].x);
									if(mask(cddt[i].y, cddt[i].x) ==0 && val==255)
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
		cv::Mat res(rows,cols,CV_8UC1,cv::Scalar(0));
		for(int i=0;i<set.size();i++)
		{
			int len=set[i].size();
			if(len>=5)
			{
				for(int j=0;j<len;j++)
				{
					int x=set[i][j].x;
					int y=set[i][j].y;
					res.at<uchar>(y,x)=255;
				}
			}
		}
		return res;
	}
};




