#pragma once
#include<stdio.h>
#include<opencv.hpp>
namespace bnz
{
	const static float binsize = 7;
	//const static float th0 = 1.1;// 1.1;

	const float def_th_l = 1.03125;// 1.03125;//
	const float def_th_h = 8;// 8;
	const float def_th_abs = 70;// 5;

	float th_l = def_th_l;// 1.03125;//
	float th_h = def_th_h;// 8;
	float th_abs = def_th_abs;// 5;

	const static float pixelnum = binsize * binsize;
	
	const float def_x = 255;
	const float def_s = 0;//(0.0/255.0)

	float x = def_x;
	float s = def_s;

	void EnHance(cv::Mat& InputImg, cv::Mat& EnhanceImg,cv::Mat* mn=NULL)
	{
		EnhanceImg= cv::Mat(InputImg.rows, InputImg.cols, CV_8UC1);
		float mean;
		int halfWind = (binsize - 1) / 2;
		//printf("%f\n", th_l);
		for (int i = 0; i < InputImg.rows; i++)
		{
			for (int j = 0; j < InputImg.cols; j++)
			{
				int sum = 0;
				float pnum = 0;
				for (int h = i - halfWind; h <= i + halfWind; h++)
				{
					if (h >= 0 && h < InputImg.rows)
					{
						uchar* InputImgRow = InputImg.ptr<uchar>(h);
						for (int w = j - halfWind; w <= j + halfWind; w++)
						{
							if (w >= 0 && w < InputImg.cols)
							{
								sum += InputImgRow[w];
								pnum++;
							}
						}
					}
				}  /*7*7灰度值的和*/

				uchar PixelValue = InputImg.at<uchar>(i, j); /*当前7*7块中心点的值*/
				//ThresBinary(mean, PixelValue);        /*PixelValue阈值分割*/
				//ThresBinaryMode2(mean, PixelValue);
				
				if ((PixelValue * pnum >= sum * th_l) && \
					(PixelValue * pnum < sum * th_h) && (PixelValue > th_abs))
					PixelValue = 255;
				else
					PixelValue = 0;
				EnhanceImg.at<uchar>(i, j) = PixelValue;    /*增强图*/

			}
		}
	}
	void EnHance_th2(cv::Mat& InputImg, cv::Mat& EnhanceImg,cv::Mat* skImg2=NULL)
	{
		
		int ilist[2];
		ilist[0] = x * s;
		ilist[1] = x;
		cv::Mat img2 = cv::Mat(InputImg.rows, InputImg.cols, CV_8UC1);
		cv::Mat bin1;
		//bnz::th_l = 1.03125;
		EnHance(InputImg, bin1);
		for (int y = 0; y < bin1.rows; y++)
		{
			for (int x = 0; x < bin1.cols; x++)
			{
				int i = (bin1.at<uchar>(y,x)) / 255;
				img2.at<uchar>(y, x) = ilist[i];
			}
		}
		//bnz::th_l = 1.13953;
		//th_abs = 0;
		EnHance(img2, EnhanceImg);
		//bnz::th_l = 1.03125;
		//th_abs = 5;
		if (skImg2 != NULL)
		{
			(*skImg2) = img2.clone();
		}
	}



	void EnHance1(cv::Mat& InputImg, cv::Mat& EnhanceImg)
	{
		EnhanceImg = cv::Mat(InputImg.rows, InputImg.cols, CV_8UC1);
		float mean;
		int halfWind = (binsize - 1) / 2;

		for (int i = 0; i < InputImg.rows; i++)
		{
			for (int j = 0; j < InputImg.cols; j++)
			{
				uchar PixelValue = InputImg.at<uchar>(i, j);
				if ((PixelValue > th_abs))
					PixelValue = 255;
				else
					PixelValue = 0;
				EnhanceImg.at<uchar>(i, j) = PixelValue;    /*增强图*/
			}
		}
	}
	void EnHance2(cv::Mat& InputImg, cv::Mat& EnhanceImg)
	{
		EnhanceImg = cv::Mat(InputImg.rows, InputImg.cols, CV_8UC1);
		float mean;
		int halfWind = (binsize - 1) / 2;

		for (int i = 0; i < InputImg.rows; i++)
		{
			for (int j = 0; j < InputImg.cols; j++)
			{
				int sum = 0;
				for (int h = i - halfWind; h <= i + halfWind; h++)
				{
					if (h >= 0 && h < InputImg.rows)
					{
						uchar* InputImgRow = InputImg.ptr<uchar>(h);
						for (int w = j - halfWind; w <= j + halfWind; w++)
						{
							if (w >= 0 && w < InputImg.cols)
							{
								sum += InputImgRow[w];
							}
						}
					}
				}  /*7*7灰度值的和*/

				mean = sum / pixelnum;    /*7*7均值*/

				uchar PixelValue = InputImg.at<uchar>(i, j); /*当前7*7块中心点的值*/
				//ThresBinary(mean, PixelValue);        /*PixelValue阈值分割*/
				//ThresBinaryMode2(mean, PixelValue);

				if ((PixelValue * pixelnum >= sum * th_l))
					PixelValue = 255;
				else
					PixelValue = 0;
				EnhanceImg.at<uchar>(i, j) = PixelValue;    /*增强图*/

			}
		}
	}
};


