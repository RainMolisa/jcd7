#pragma once
#include <cmath>
#include <string>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <io.h>

namespace ofst
{
	int search_box = 64;
	int mbsize = 19;// 23;// 19;	//search window size
	int l = -(search_box - mbsize / 2);// -64;
	int r = (search_box - mbsize / 2);// 32;
	int up = -1;// -1;
	int down = 1;// = 1;
	static const int infOffset = 0;//initialize Y offset
	static const int invalidXoffset = 64; //default X offset for invalid pixel
	static const int minNum1Rec = 8;// 32;  //if num 1 in a receive block is smaller than this, deems as invalid
	static const int goodMatchThres = 32;// 0.5;// 0.5;
	static const int signDisparity = 1; //+1: disparity=reference - received; -1: disparity=received - reference

	static const int a11 = 1; // 0, 1, 2, 4
	static const int b01 = -1; // -1, -2, 0
	static const int d = 4; // 0, 1

	const static float binsize = 7;
	const static float th0 = 1.1;// 1.1;

	const static float th_l = 1.03125;// 1.03125;
	const static float th_h = 7;// 8;
	const static float th_abs = 5;// 5;

	const static float pixelnum = binsize * binsize, center = binsize / 2;

	void set_env(int sbx,int mbs)
	{
		search_box = sbx;
		mbsize = mbs;
		l = -(search_box - mbsize / 2);
		r = (search_box - mbsize / 2);
	}

	int subpixel_4bit(int numerator, int denom);

	int16_t* fastBlockMatchPadding_Y_first(Mat& ref, Mat& rel, Mat& out, Mat& peakMatch,Mat& subpixeMap,Mat& score1,Mat& max_ix,bool is_subpixel=true)  /*øÈ∆•≈‰*/
	{

		//const int mbsize = 19;// 23;// 19;	//search window size
		int semi_mb = (int)(mbsize / 2);
#ifdef mk_log
		ofstream mycout("debug.txt");
#endif // mk_log


		int refHor;
		int refVer;
		int index;
		int offset;
		int height = ref.rows;
		int width = ref.cols;
		int height1 = ref.rows + 2 * semi_mb;
		int width1 = ref.cols + 2 * semi_mb;
		int* curMatch = (int*)malloc(sizeof(int) * height1 * width1);
		int* prevColMatch = (int*)malloc(sizeof(int) * height1 * width1 * (down - up + 1)); //store a vector along delta_Y for each pixel
		int* bestYOffset = (int*)malloc(sizeof(int) * height * width);
		int* lastPeak = (int*)malloc(sizeof(int) * height1 * width1);
		int* lastPeakLeft = (int*)malloc(sizeof(int) * height1 * width1);
		int* lastPeakRight = (int*)malloc(sizeof(int) * height1 * width1);
		int* compImage = (int*)malloc(sizeof(int) * height1 * width1);

		//int *compImage01 = (int*)malloc(sizeof(int)*height1*width1);//

		//Mat out = Mat(cur.rows, cur.cols, CV_8UC1);
		out.create(height, width, CV_8UC1);
		max_ix.create(height, width, CV_16SC1);

		int* horizSum = (int*)malloc(sizeof(int) * height1 * width1);

		memset(bestYOffset, infOffset, sizeof(int) * height * width);
		memset(lastPeak, -1, sizeof(int) * height1 * width1);
		memset(lastPeakLeft, 0, sizeof(int) * height1 * width1);
		memset(lastPeakRight, 0, sizeof(int) * height1 * width1);
		memset(prevColMatch, 0, sizeof(int) * height1 * width1 * (down - up + 1));

		//padding 0 to four sides of input image
		uint8_t* input0Pad = (uint8_t*)malloc(sizeof(uint8_t) * height1 * width1);
		uint8_t* ref0Pad = (uint8_t*)malloc(sizeof(uint8_t) * height1 * width1);
		uint8_t* ref_padding = (uint8_t*)malloc(sizeof(uint8_t) * (height1 + 2) * (width1 + r - l)); //the biggest ref padding 0
		for (int i = 0; i < height1; i++)
		{
			for (int j = 0; j < width1; j++)
			{

				if ((i < semi_mb) || (j < semi_mb) || (i >= height1 - semi_mb) || (j >= width1 - semi_mb))
				{
					input0Pad[i * width1 + j] = 0;
					ref0Pad[i * width1 + j] = 0;
				}
				else
				{
					input0Pad[i * width1 + j] = rel.at<uchar>(i - semi_mb, j - semi_mb);
					ref0Pad[i * width1 + j] = ref.at<uchar>(i - semi_mb, j - semi_mb);
				}

			}
		}
		for (int i = 0; i < height1 + 2; i++) {    //                         
			for (int j = 0; j < width1 + r - l; j++) {    //
				if ((i < semi_mb + 1) || (i >= height1 - semi_mb + 1) || (j < r + mbsize / 2) || (j >= width + r + mbsize / 2)) //”“±ﬂΩÁ
				{
					ref_padding[i * (width1 + r - l) + j] = 0;
				}
				else
				{
					ref_padding[i * (width1 + r - l) + j] = ref.at<uchar>(i - semi_mb - 1, j - (r + mbsize / 2));
				}
			}
		}


		int* refhorizSum = (int*)malloc(sizeof(int) * (height1 + 2) * (width1 + r - l));   //
		int* ref_conc = (int*)malloc(sizeof(int) * (height1 + 2) * (width1 + r - l));//
		int* ref_11_num = (int*)malloc(sizeof(int) * (height + 2) * (width + r - l));//
		for (int i = 0; i < height1 + 2; i++) {     //
			int sum = 0;
			for (int j = 0; j < width1 + r - l; j++) {
				if (j < mbsize)
				{
					sum += ref_padding[i * (width1 + r - l) + j];
					refhorizSum[i * (width1 + r - l) + j] = sum;
				}
				else
				{
					refhorizSum[i * (width1 + r - l) + j] =
						refhorizSum[i * (width1 + r - l) + j - 1] - ref_padding[i * (width1 + r - l) + j - mbsize] + ref_padding[i * (width1 + r - l) + j];
				}
			}
		}
		for (int j = 0; j < width1 + r - l; j++) {//
			int sum = 0;
			for (int i = 0; i < height1 + 2; i++) {//

				if (i < mbsize)
				{
					sum = sum + refhorizSum[i * (width1 + r - l) + j];
					ref_conc[i * (width1 + r - l) + j] = sum;
				}
				else
					//recursive partial sum
					ref_conc[i * (width1 + r - l) + j] =
					ref_conc[(i - 1) * (width1 + r - l) + j] - refhorizSum[(i - mbsize) * (width1 + r - l) + j] + refhorizSum[i * (width1 + r - l) + j];
			}
		}

		for (int i = 0; i < height + 2; i++) {
			for (int j = 0; j < width + r - l; j++) {
				ref_11_num[i * (width + r - l) + j] = ref_conc[(i + 2 * semi_mb) * (width + r - l + 2 * semi_mb) + j + 2 * semi_mb];  //ref block ==1 conclusion.//center
			}
		}



		double t = (double)getTickCount();

		//for fixed offsets, compute the block matching result for the whole image
		int dim3[3];
		dim3[0] = height;
		dim3[1] = width;
		dim3[2] = (down - up + 1) * (r - l + 1);
		score1.create(3, dim3, CV_16S);
		for (int n = l; n < r + 1; n++)   //-52 ~ 52 
		{
			for (int m = up; m <= down; m++)  //-1 ~ 1
			{
				int score_x = (m - up) * (r - l + 1) + (n - l);
				//1.create the image of 'AND' between received image and shifted reference image
				//2.run horizontal recursive sum
				index = 0;
				for (int i = 0; i < height1; i++)
				{
					int sum = 0;
					for (int j = 0; j < width1; j++)
					{
						refVer = i + m;
						refHor = j + n;
						if (refHor  < 0 || refHor >(width1 - 1) || refVer <0 || refVer >(height1 - 1))	//check the ref range
							compImage[index] = 0;
						else
							compImage[index] = input0Pad[i * width1 + j] & ref0Pad[refVer * width1 + refHor];

						if (j < mbsize)
						{
							sum = sum + compImage[index];
							horizSum[index] = sum;   //for column index < mbsize
						}
						else
							//recursive partial sum
							horizSum[index] = horizSum[index - 1] - compImage[index - mbsize] + compImage[index];
						index++;
					}
				}
				//3.run vertical recursive sum
				for (int j = 0; j < width1; j++)
				{
					int sum = 0;
					for (int i = 0; i < height1; i++)
					{

						if (i < mbsize)
						{
							sum = sum + horizSum[i * width1 + j];
							curMatch[i * width1 + j] = sum;
						}
						else {
							//recursive partial sum
							curMatch[i * width1 + j] = curMatch[(i - 1) * width1 + j] - horizSum[(i - mbsize) * width1 + j] + horizSum[i * width1 + j];//◊›œÚ¿€º”µΩi
						}
					}

				}
				//4. iteratively find the maximal match, record its left and right neighbors
				for (int i = mbsize - 1; i < height1; i++)
				{
					for (int j = mbsize - 1; j < width1; j++)
					{
						index = i * width1 + j;
						int tmp2 = curMatch[index] - (ref_11_num[(i + m - mbsize - up + 1) * (width + r - l) + r + j - l - mbsize + 1] - curMatch[index]) / d;
						int tmp = (d != 0) ? max(0, tmp2) : max(0, curMatch[index]);
						//score1.at<int16_t>(i - (mbsize - 1), j - (mbsize - 1), score_x) = curMatch[index];
						//score2.at<int16_t>(i - (mbsize - 1), j - (mbsize - 1), score_x) = tmp2;
						score1.at<int16_t>(i - (mbsize - 1), j - (mbsize - 1), score_x) = tmp;
						if (lastPeak[index] < tmp)//
						{
							lastPeak[index] = tmp;
							bestYOffset[(i - (mbsize - 1)) * width + (j - (mbsize - 1))] = m;
							out.at<uchar>(i - (mbsize - 1), j - (mbsize - 1)) = signDisparity * n - l;
							max_ix.at<int16_t>(i - (mbsize - 1), j - (mbsize - 1)) = score_x;
							if (n > l)
								lastPeakLeft[index] = prevColMatch[index * (down - up + 1) + m - up];
							else
								lastPeakLeft[index] = 0;
							if (n == r)
								lastPeakRight[index] = 0;
						}
						if (bestYOffset[(i - (mbsize - 1)) * width + (j - (mbsize - 1))] == m && n == (out.at<uchar>(i - (mbsize - 1), j - (mbsize - 1)) + l + 1))
							lastPeakRight[index] = tmp;
						prevColMatch[index * (down - up + 1) + m - up] = tmp;
					}
				}
				/*string name1 = format("\\score\\s1\\%01d_%03d.txt", m - up, n - l);
				string name2 = format("\\score\\s2\\%01d_%03d.txt", m - up, n - l);
				if (not_exist)
				{
					save_Mat_int(cut_Mat_int(score1, semi_mb), wrk_path + name1);
					save_Mat_int(cut_Mat_int(score2, semi_mb), wrk_path + name2);
				}*/
			}
		}


		t = ((double)getTickCount() - t) / getTickFrequency();
		std::cout << "Times passed in seconds: " << t << std::endl;

		//count number of 1s in each block
		int* intgImage0 = (int*)malloc(sizeof(int) * height1 * width1);
		int* intgImage1 = (int*)malloc(sizeof(int) * height1 * width1);
		memset(intgImage0, 0, sizeof(int) * height1 * width1);
		memset(intgImage1, 0, sizeof(int) * height1 * width1);


		for (int i = 0; i < height1; i++)
		{
			index = i * width1 + 0;
			intgImage0[index] = input0Pad[index];
		}
		for (int i = 0; i < height1; i++)
		{
			for (int j = 1; j < width1; j++)
			{
				index = i * width1 + j;
				intgImage0[index] = input0Pad[index] + intgImage0[index - 1];
			}
		}
		for (int j = 0; j < width1; j++)
		{
			index = j;
			intgImage1[index] = intgImage0[index];
		}
		for (int i = 1; i < height1; i++)
		{
			for (int j = 0; j < width1; j++)
			{
				index = i * width1 + j;
				intgImage1[index] = intgImage1[index - width1] + intgImage0[index];
			}
		}
		int* num1Count = (int*)malloc(sizeof(int) * height * width);
		for (int i = semi_mb + 1; i < height1 - semi_mb; i++)
		{
			for (int j = semi_mb + 1; j < width1 - semi_mb; j++)
			{
				num1Count[(i - semi_mb) * width + j - semi_mb] = intgImage1[(i + semi_mb) * width1 + j + semi_mb] - intgImage1[(i - semi_mb - 1) * width1 + j + semi_mb] - intgImage1[(i + semi_mb) * width1 + j - semi_mb - 1]
					+ intgImage1[(i - semi_mb - 1) * width1 + j - semi_mb - 1];
			}
		}
		num1Count[0] = intgImage1[(semi_mb + semi_mb) * width1 + semi_mb + semi_mb];
		for (int i = semi_mb + 1; i < height1 - semi_mb; i++)
		{
			num1Count[(i - semi_mb) * width] = intgImage1[(i + semi_mb) * width1 + semi_mb + semi_mb] - intgImage1[(i - semi_mb - 1) * width1 + semi_mb + semi_mb];
		}
		for (int j = semi_mb + 1; j < width1 - semi_mb; j++)
		{
			num1Count[j - semi_mb] = intgImage1[(semi_mb + semi_mb) * width1 + j + semi_mb] - intgImage1[(semi_mb + semi_mb) * width1 + j - semi_mb - 1];
		}
		Mat num1CountImg(rel.rows, rel.cols, CV_8UC1, Scalar::all(0));

		for (int i = 0; i < rel.rows; i++)
		{
			for (int j = 0; j < rel.cols; j++)
			{
				num1CountImg.at<uchar>(i, j) = num1Count[i * width + j];
			}
		}
#ifdef mk_log
		imwrite("num1CountImg.bmp", num1CountImg);
#endif
		//Mat peakMatch(rel.rows, rel.cols, CV_32SC1, Scalar::all(0));
		peakMatch = Mat(rel.rows, rel.cols, CV_32SC1, Scalar::all(0));
		for (int i = mbsize; i <= height1; i++)
		{
			for (int j = mbsize; j <= width1; j++)
			{
				peakMatch.at<int>(i - mbsize, j - mbsize) = lastPeak[(i - 1) * width1 + j - 1];
			}
		}

		//thresholding & subpixel
		//int16_t subpixel;
		int16_t  subpixel2;
		int16_t* bestXOffset = (int16_t*)malloc(sizeof(int16_t) * height * width);  //update
		subpixeMap.create(rel.rows,rel.cols,CV_16SC1);
		for (int i = 0; i < rel.rows; i++)
		{
			for (int j = 0; j < rel.cols; j++)
			{
				if ((peakMatch.at<int>(i, j) >= num1Count[i * width + j] / goodMatchThres) && (peakMatch.at<int>(i, j) > minNum1Rec))
				{
					if (is_subpixel)
					{
						index = (i + mbsize - 1) * width1 + j + mbsize - 1;
						//subpixel = round(16 * 0.5 * (lastPeakRight[index] - lastPeakLeft[index]) / (2 * peakMatch.at<int>(i, j) - lastPeakRight[index] - lastPeakLeft[index]));
						subpixel2 = subpixel_4bit(lastPeakRight[index] - lastPeakLeft[index], \
							2 * peakMatch.at<int>(i, j) - lastPeakRight[index] - lastPeakLeft[index]);
						//printf("sub:%d\n", subpixel2);
					}
					else
					{
						subpixel2 = 0;
					}
					bestXOffset[i * width + j] = (int16_t)(16 * (out.at<uchar>(i, j) + l) + subpixel2);  // add —«œÒÀÿ
					subpixeMap.at<int16_t>(i, j) = subpixel2;
				}
				else
				{
					bestYOffset[i * width + j] = 0;
					bestXOffset[i * width + j] = 63680; // 16 * (invalidXoffset - l);//740
					out.at<uchar>(i, j) = 0; // (invalidXoffset - l);
					subpixeMap.at<int16_t>(i, j) = 0;
				}
			}
		}
#ifdef mk_log
		ofstream loggerIn("inputBinaryImg.dat");
		ofstream loggerIn1("inputBinaryImgTranspose.dat");
		ofstream loggerIn2("inputBinaryImg_-90.dat");
		ofstream loggerRef("refBinaryImg.dat");
		ofstream loggerNum1("Num1InputBlock.dat");
		ofstream loggercurMatch("Num1curMatch.dat");
		ofstream loggerref11("Num1ref11.dat");
		ofstream loggerOutOffsetX("outputOffsetX.dat");
		ofstream loggerOutIntOffsetX("outputIntOffsetX.dat");
		ofstream loggerOutOffsetY("outputIntOffsetY.dat");
		ofstream loggerPeakMatch("outputPeakMatch.dat");
		ofstream loggerPeakMatchL("PeakMatchLeft.dat");
		ofstream loggerPeakMatchR("PeakMatchRight.dat");
#endif
#ifdef mk_log
		loggerIn.close();
		loggerIn1.close();
		loggerIn2.close();
		loggerRef.close();
		loggerNum1.close();
		loggerOutOffsetX.close();
		loggerOutIntOffsetX.close();
		loggerOutOffsetY.close();
		loggerPeakMatch.close();
		loggerPeakMatchL.close();
		loggerPeakMatchR.close();
#endif
		Mat offsetYImg(rel.rows, rel.cols, CV_8UC1, Scalar::all(0));
		int16_t* YoffsetStat = (int16_t*)malloc(sizeof(int16_t) * height / 16 * width / 16);
		memset(YoffsetStat, 0, sizeof(int16_t) * height * width / 256);
		for (int i = 0; i < rel.rows; i++)
		{
			for (int j = 0; j < rel.cols; j++)
			{
				//bestXOffset[i * width + j] = bestXOffset[i * width + j]; //bias it to be >=0
				offsetYImg.at<uchar>(i, j) = bestYOffset[i * width + j] - up; //bias it to be >=0
				index = ((int)floor(i / 16)) * width / 16 + (int)floor(j / 16);
				YoffsetStat[index] = YoffsetStat[index] + bestYOffset[i * width + j];
			}
		}
#ifdef mk_log
		imwrite("PeakMatchImage.bmp", peakMatch);
		imwrite("offsetYImage.bmp", offsetYImg);
		ofstream loggerYoffsetStat("YoffsetStat.dat");
#endif
#ifdef mk_log
		loggerYoffsetStat.close();
#endif
		free(curMatch);
		free(prevColMatch);
		free(bestYOffset);
		//free(bestXOffset);
		free(lastPeak);
		free(lastPeakLeft);
		free(lastPeakRight);
		free(compImage);
		free(horizSum);
		free(intgImage0);
		free(intgImage1);
		free(YoffsetStat);
#ifdef mk_log
		mycout.close();
#endif

		return bestXOffset;
	}

	Mat diff_offset(float* offset1,float* offset2,int rows,int cols,int range)
	{
		Mat res(rows, cols, CV_8UC1);
		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < cols; x++)
			{
				int i = y * cols + x;
				float v1 = offset1[i];
				float v2 = offset2[i];
				v1 = v1 / 16;
				v2 = v2 / 16;
				float val = (v1 > v2 ? v1 - v2 : v2 - v1);
				int v3;
				if (val >= range)
				{
					v3 = 255;
				}
				else
				{
					v3 = (val) / float(range) * 255.0;
				}
				res.at<uchar>(y, x) = v3;
			}
		}
		return res;
	}

	int subpixel_4bit(int numerator, int denom)
	{
		//subpixel formula: 0.5*numerator/denom
		//1. shift denom into range of [64 127]
		//2. represent inverse by at most 9 bit integer (<=256)
		int subpixel, bitshift;
		if (denom > 127)
		{
			bitshift = (int)floor(log2(denom / 64.0));
			denom = denom >> bitshift;
			subpixel = floor(0.5 + 16 * 0.5 * numerator * round(256.0 * 64.0 / denom) * pow(2, -8 - 6 - bitshift));
		}
		else {
			if (denom < 64)
			{
				bitshift = (int)ceil(log2(64.0 / denom));
				denom = denom << bitshift;
				subpixel = floor(0.5 + 16 * 0.5 * numerator * round(256.0 * 64.0 / denom) * pow(2, -8 - 6 + bitshift));
			}
			else {
				subpixel = floor(0.5 + 16 * 0.5 * numerator * round(256.0 * 64.0 / denom) * pow(2, -8 - 6));//floor(0.5+x)=round(x)
			}
		}
		return subpixel;
	}

	Vec3i get_scoreIndex(int offset)
	{
		Vec3i res;
		res[0] = (-1 - up) * (r - l + 1) + (offset - l);
		res[1] = ( 0 - up) * (r - l + 1) + (offset - l);
		res[2] = (+1 - up) * (r - l + 1) + (offset - l);
		return res;
	}
};

