#pragma once
#include<opencv.hpp>
namespace bnz
{
	const static float binsize = 7;
	//const static float th0 = 1.1;// 1.1;

	float th_l = 1.08125;// 1.03125;//
	float th_h = 7;// 8;
	float th_abs = 5;// 5;

	const static float pixelnum = binsize * binsize, center = binsize / 2;

	void EnHance(cv::Mat& InputImg, cv::Mat& EnhanceImg)
	{
		EnhanceImg= cv::Mat(InputImg.rows, InputImg.cols, CV_8UC1);
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
				}  /*7*7�Ҷ�ֵ�ĺ�*/

				mean = sum / pixelnum;    /*7*7��ֵ*/

				uchar PixelValue = InputImg.at<uchar>(i, j); /*��ǰ7*7�����ĵ��ֵ*/
				//ThresBinary(mean, PixelValue);        /*PixelValue��ֵ�ָ�*/
				//ThresBinaryMode2(mean, PixelValue);

				if ((PixelValue * pixelnum >= sum * th_l) && \
					(PixelValue * pixelnum < sum * th_h) && (PixelValue > th_abs))
					PixelValue = 255;
				else
					PixelValue = 0;
				EnhanceImg.at<uchar>(i, j) = PixelValue;    /*��ǿͼ*/

			}
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
				EnhanceImg.at<uchar>(i, j) = PixelValue;    /*��ǿͼ*/
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
				}  /*7*7�Ҷ�ֵ�ĺ�*/

				mean = sum / pixelnum;    /*7*7��ֵ*/

				uchar PixelValue = InputImg.at<uchar>(i, j); /*��ǰ7*7�����ĵ��ֵ*/
				//ThresBinary(mean, PixelValue);        /*PixelValue��ֵ�ָ�*/
				//ThresBinaryMode2(mean, PixelValue);

				if ((PixelValue * pixelnum >= sum * th_l))
					PixelValue = 255;
				else
					PixelValue = 0;
				EnhanceImg.at<uchar>(i, j) = PixelValue;    /*��ǿͼ*/

			}
		}
	}
};