#include <fstream>
#include "dpthbin.h"
#include "pseudocolor.h"
#include "offset2d.h"
#include "binref.h"
#include "offset.h"
#include "bin_filter.h"
#include "spckl2png.h"
#include "binarize.h"
using namespace std;
using namespace cv;
Mat dif_offset(float* in_depth, float* calDepth, int rows, int cols, int len);
Mat cut_patch(Mat img, Rect pos);
float* cvti162f(int16_t* ivec, int n);
// ..\set\01 ref_640.bin 40cm_800x640-00001395-ir.bin 640 800 943 40 600 64 19
// ..\set\04 ref_1280_0.bin 40cm_subpixel1_y1_x64-00000347-ir.bin 1280 800 846.67 42 600 64 25
int main(int argc, char** argv)
{
	int rows, cols;
	string wrk_pth;
	string cur_pth;
	string ref_pth;
	float fxy;
	int baseline, wall, search_box, mbsize;
	if (argc < 11)
	{
		printf("arg error\n");
		return 0;
	}
	{
		wrk_pth = string(argv[1]);
		ref_pth = string(argv[2]);
		cur_pth = string(argv[3]);
		sscanf_s(argv[4], "%d", &rows);
		sscanf_s(argv[5], "%d", &cols);
		sscanf_s(argv[6], "%f", &fxy);
		sscanf_s(argv[7], "%d", &baseline);
		sscanf_s(argv[8], "%d", &wall);
		sscanf_s(argv[9], "%d", &search_box);
		sscanf_s(argv[10], "%d", &mbsize);
	}
	string res_pth = wrk_pth + "\\res";
	system(("rd /Q /S " + res_pth).c_str());
	system(("mkdir " + res_pth).c_str());
	//
	//
	float* hd_depth = dbn::read_depth(wrk_pth + "\\hard_depth.bin", rows, cols);
	if (hd_depth != NULL)
	{
		imwrite(res_pth + "\\hard_depth.png", psd2::pseudocolor(hd_depth, rows, cols));
	}
	//
	Mat ref = binref(wrk_pth + "\\" + ref_pth, rows, cols);
	//ref = bft::bin_filter02(ref);
	imwrite(res_pth + "\\ref.png", ref);
	Mat cur_ir = skg::spckl2png(wrk_pth + "\\" + cur_pth, rows, cols,0);
	imwrite(res_pth + "\\cur_ir.png", cur_ir);
	Mat cur;
	bnz::EnHance(cur_ir, cur);
	imwrite(res_pth + "\\cur.png", cur);
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < cols; x++)
		{
			ref.at<uchar>(y, x) = ref.at<uchar>(y, x) / 255;
			cur.at<uchar>(y, x) = cur.at<uchar>(y, x) / 255;
		}
	}
	//
	int n = rows * cols;
	ofst::set_env(search_box, mbsize);
	Mat out1,out2;
	Mat peak1,peak2;
	Mat s11,s12;
	Mat subpixeMap1,subpixeMap2;
	Mat max_ix1,max_ix2;
	int16_t* ofs1 = ofst::fastBlockMatchPadding_Y_first(ref, cur, out1, peak1, subpixeMap1, s11, max_ix1);
	s11.release();
	ofst::up = 0;
	ofst::down = 0;
	int16_t* ofs2 = ofst::fastBlockMatchPadding_Y_first(ref, cur, out2, peak2, subpixeMap2, s12, max_ix2);
	s12.release();
	float* sftofs1 = cvti162f(ofs1, n);
	float* sftofs2 = cvti162f(ofs2, n);
	int16_t* ofs3 = ofst::fastBlockMatchPadding_Y_first(ref, cur, out2, peak2, subpixeMap2, s12, max_ix2,false);
	float* sftofs3 = cvti162f(ofs3, n);
	
	float* sftDepth1 = fs2d::offset2depth(sftofs1, rows, cols, fxy, baseline, wall, search_box, mbsize);
	Mat sftDshw1 = psd2::pseudocolor(sftDepth1, rows, cols);
	imwrite(res_pth + "\\sftDshw1.png", sftDshw1);
	dbn::write_depth(sftDepth1, rows, cols, res_pth + "\\soft_depth1.raw");

	float* sftDepth2 = fs2d::offset2depth(sftofs2, rows, cols, fxy, baseline, wall, search_box, mbsize);
	Mat sftDshw2 = psd2::pseudocolor(sftDepth2, rows, cols);
	imwrite(res_pth + "\\sftDshw2.png", sftDshw2);
	dbn::write_depth(sftDepth2, rows, cols, res_pth + "\\soft_depth2.raw");
	//
	float* sftDepth3 = fs2d::offset2depth(sftofs3, rows, cols, fxy, baseline, wall, search_box, mbsize);
	Mat sftDshw3 = psd2::pseudocolor(sftDepth3, rows, cols);
	imwrite(res_pth + "\\sftDshw2_nosub.png", sftDshw3);
	dbn::write_depth(sftDepth3, rows, cols, res_pth + "\\sftDshw2_nosub.raw");
	//fstDepth: input offset
	//sftofs: calculate offset
	
	
	
	
	//
	delete[] ofs1;
	delete[] sftofs1;
	delete[] ofs2;
	delete[] sftofs2;
	delete[] sftDepth1;
	delete[] sftDepth2;
	delete[] ofs3;
	delete[] sftofs3;
	return 0;
}

float* cvti162f(int16_t* ivec,int n)
{
	float* res = new float[n];
	for (int i = 0; i < n; i++)
	{
		res[i] = ivec[i];
	}
	return res;
}

Mat dif_offset(float* in_depth, float* calDepth, int rows, int cols, int len)
{
	Mat difres(rows, cols, CV_8UC1);
	int n = rows * cols;
	for (int i = 0; i < n; i++)
	{
		float val = in_depth[i] - calDepth[i];
		val = (val > 0 ? val : -val);
		if (val <= len)
		{
			float fval = (float(val) / float(len)) * 255.0;
			val = fval;
		}
		else
		{
			val = 255;
		}
		int y = i / cols;
		int x = i % cols;
		difres.at<uchar>(y, x) = val;
	}
	return difres;
}

Mat cut_patch(Mat img,Rect pos)
{
	Mat res(pos.height,pos.width,CV_8UC1);
	for (int y = pos.y; y < pos.y + pos.height; y++)
	{
		for (int x = pos.x; x < pos.x + pos.width; x++)
		{
			int y0 = y - pos.y;
			int x0 = x - pos.x;
			if (y >= 0 && y < img.rows && x >= 0 && x < img.cols)
			{
				res.at<uchar>(y0, x0) = img.at<uchar>(y, x);
			}
			else
			{
				res.at<uchar>(y0, x0) = 0;
			}
		}
	}
	return res;
}


