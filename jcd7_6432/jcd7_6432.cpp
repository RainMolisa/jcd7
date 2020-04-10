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
// ..\set\19 config.txt -64 32 25
// ..\set\20 config.txt -64 32 25

int main(int argc, char** argv)
{
	int rows, cols;
	string wrk_pth;
	string cur_pth;
	string ref_pth;
	float fxy;
	int baseline, wall, lft,rgt, mbsize;
	int cur_mode;
	if (argc < 5)
	{
		printf("arg error\n");
		return 0;
	}
	{
		wrk_pth = string(argv[1]);
		string cfg_str = string(argv[2]);
		sscanf_s(argv[3], "%d", &lft);
		sscanf_s(argv[4], "%d", &rgt);
		sscanf_s(argv[5], "%d", &mbsize);
		fstream cfg_fs;
		cfg_fs.open(wrk_pth + "\\" + cfg_str, ios::in);
		cfg_fs >> ref_pth;
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
	Mat cur;
	bnz::th_abs = 5;
	bnz::EnHance(cur_ir, cur);
	imwrite(res_pth + "\\cur.png", cur);
	Mat ref = binref(wrk_pth + "\\" + ref_pth, rows, cols);
	imwrite(res_pth + "\\ref.png", ref);
	//
	int n = rows * cols;
	ofst::set_env(64, mbsize);
	ofst::up = 0;
	ofst::down = 0;
	//
	Mat peakMatch, subpixeMap;
	Mat score1, max_ix;
	int16_t* ofs1 = ofst::fastBlockMatchPadding2(ref, cur,
		&peakMatch, &subpixeMap, &score1, &max_ix);
	float* sftofs1 = cvti162f(ofs1, n);
	float* sftDepth1 = fs2d::offset2depth(sftofs1, rows, cols, fxy, baseline, wall);
	imwrite(res_pth + "\\sftDshw1.png", psd2::pseudocolor(sftDepth1, rows, cols));
	dbn::write_depth(sftDepth1, rows, cols, res_pth + "\\soft_depth1.raw");
	dbn::write_depth_txt(sftDepth1, rows, cols, res_pth + "\\soft_depth1.txt");
	//
	ofst::set_env(lft, rgt, mbsize);
	int16_t* ofs3 = ofst::fastBlockMatchPadding2(ref, cur);
	float* sftofs3 = cvti162f(ofs3, n);
	float* sftDepth3 = fs2d::offset2depth(sftofs3, rows, cols, fxy, baseline, wall);
	imwrite(res_pth + "\\sftDshw_6432.png", psd2::pseudocolor(sftDepth3, rows, cols));
	dbn::write_depth_txt(sftDepth3, rows, cols, res_pth + "\\soft_depth6432.txt");
	//
	
	
	delete[] ofs3;
	delete[] sftofs3;
	delete[] sftDepth3;
	return 0;
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





