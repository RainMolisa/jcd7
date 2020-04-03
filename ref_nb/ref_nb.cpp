#include <fstream>
#include <vector>
#include "..\jcd7\dpthbin.h"
#include "..\jcd7\pseudocolor.h"
#include "..\jcd7\offset2d.h"
#include "..\jcd7\binref.h"
#include "..\jcd7\offset.h"
#include "..\jcd7\bin_filter.h"
#include "..\jcd7\spckl2png.h"
#include "..\jcd7\binarize.h"


using namespace std;
using namespace cv;
float* cvti162f(int16_t* ivec, int n);
// ..\set\11 config_ref.txt 64 25
// ..\set\15 config_ref.txt 64 25
// ..\set\19 config_ref.txt 64 25
// ..\set\20 config_ref.txt 64 25

int main(int argc, char** argv)
{
	int rows, cols;
	string wrk_pth;
	string cur_pth;
	string ref_pth;
	float fxy;
	int baseline, wall, search_box, mbsize;
	int cur_mode;
	int ref_mode;
	if (argc < 5)
	{
		printf("arg error\n");
		return 0;
	}
	{
		wrk_pth = string(argv[1]);
		string cfg_str = string(argv[2]);
		sscanf_s(argv[3], "%d", &search_box);
		sscanf_s(argv[4], "%d", &mbsize);
		fstream cfg_fs;
		cfg_fs.open(wrk_pth+"\\" + cfg_str,ios::in);
		cfg_fs >> ref_pth;
		cfg_fs >> ref_mode;
		cfg_fs >> cur_pth;
		cfg_fs >> cur_mode;
		cfg_fs >> rows >> cols;
		cfg_fs >> fxy >> baseline >> wall;
		cfg_fs.close();
	}
	printf("start\n");
	string res_pth = wrk_pth + "\\res";
	string oin_pth = wrk_pth + "\\in_other";
	system(("mkdir " + oin_pth).c_str());
	system(("rd /Q /S " + res_pth).c_str());
	system(("mkdir " + res_pth).c_str());
	//
	float* hd_depth = dbn::read_depth(wrk_pth + "\\hard_depth.bin", rows, cols);
	if (hd_depth != NULL)
	{
		imwrite(res_pth + "\\hard_depth.png", psd2::pseudocolor(hd_depth, rows, cols));
		delete[] hd_depth;
	}
	//
	Mat cur_ir = skg::spckl2png(wrk_pth + "\\" + cur_pth, rows, cols, cur_mode);
	imwrite(res_pth + "\\cur_ir.png", cur_ir);
	Mat ref_ir = skg::spckl2png(wrk_pth + "\\" + ref_pth, rows, cols, ref_mode);
	imwrite(res_pth + "\\ref_ir.png", ref_ir);
	Mat cur_nwb, ref, ref_nwb;
	bnz::EnHance_th2(cur_ir, cur_nwb);
	imwrite(res_pth + "\\cur_nwb.png", cur_nwb);
	bnz::th_abs = 5;
	bnz::EnHance(ref_ir, ref);
	bnz::EnHance_th2(ref_ir,ref_nwb);
	bnz::th_abs = bnz::def_th_abs;
	imwrite(res_pth + "\\ref.png", ref);
	imwrite(res_pth + "\\ref_nwb.png", ref_nwb);
	//
	int n = rows * cols;
	ofst::set_env(search_box, mbsize);
	ofst::up = 0;
	ofst::down = 0;
	//
	int16_t* ofs1 = ofst::fastBlockMatchPadding2(ref, cur_nwb);
	float* sftofs1 = cvti162f(ofs1, n);
	float* sftDepth1 = fs2d::offset2depth(sftofs1, rows, cols, fxy, baseline, wall, search_box, mbsize);
	imwrite(res_pth + "\\sftDshw1.png", psd2::pseudocolor(sftDepth1, rows, cols));
	dbn::write_depth(sftDepth1, rows, cols, res_pth + "\\soft_depth1.raw");
	dbn::write_depth_txt(sftDepth1, rows, cols, res_pth + "\\soft_depth1.txt");
	//
	int16_t* ofs2 = ofst::fastBlockMatchPadding2(ref_nwb, cur_nwb);
	float* sftofs2 = cvti162f(ofs2, n);
	float* sftDepth2 = fs2d::offset2depth(sftofs2, rows, cols, fxy, baseline, wall, search_box, mbsize);
	imwrite(res_pth + "\\sftDshw2.png", psd2::pseudocolor(sftDepth2, rows, cols));
	dbn::write_depth(sftDepth2, rows, cols, res_pth + "\\soft_depth2.raw");
	dbn::write_depth_txt(sftDepth2, rows, cols, res_pth + "\\soft_depth2.txt");
	//
	{
		int dif_sum = 0;
		fstream fs;
		fs.open(res_pth + "\\dif_sum.txt",ios::out);
		Mat ref_dif(rows,cols,CV_8UC1,Scalar(0));
		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < cols; x++)
			{
				uchar val1 = ref.at<uchar>(y, x);
				uchar val2 = ref_nwb.at<uchar>(y, x);
				uchar v = val1 ^ val2;
				
				if (v > 0)
				{
					dif_sum++;
					ref_dif.at<uchar>(y, x) = 255;
				}
					
			}
		}
		fs << dif_sum << endl;
		imwrite(res_pth + "\\ref_dif.png", ref_dif);
		fs.close();
	}
	delete[] ofs1;
	delete[] sftofs1;
	delete[] sftDepth1;
	delete[] ofs2;
	delete[] sftofs2;
	delete[] sftDepth2;
}

float* cvti162f(int16_t* ivec, int n)
{
	float* res = new float[n];
	for (int i = 0; i < n; i++)
	{
		res[i] = ivec[i];
	}
	return res;
}


