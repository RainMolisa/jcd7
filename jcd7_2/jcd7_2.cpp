/*
zhe shi yi ge que ding san ban mei you wen ti de shiyan
*/

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
#include "..\jcd7_0\seed_seg.h"
#include "hand_denoise.h"
#include "offset2target.h"
using namespace std;
using namespace cv;
Mat dif_offset(float* in_depth, float* calDepth, int rows, int cols, int len);
Mat cut_patch(Mat img, Rect pos);
float* cvti162f(int16_t* ivec, int n);

// ..\set\01 ref_640.bin 40cm_800x640-00001395-ir.bin 640 800 943 40 600 64 19
// ..\set\02 ref_640.bin 60cm_800x640-00001065-ir.bin 640 800 943 40 600 64 19
// ..\set\04 ref_1280_0.bin 40cm_subpixel1_y1_x64-00000347-ir.bin 1280 800 846.67 42 600 64 25
// ..\set\10 ref_dn005.bin f1-00000796-ir.bin 1280 800 936.4 40 600 64 25
// ..\set\11 ref_1280_0307.bin 45face-00000269-ir.bin 1280 800 846.67 42 600 64 25
// ..\set\12 ref_1280_0307.bin face40cm-00010632-ir.bin 1280 800 846.67 42 600 64 25
// ..\set\15 ref_1280x800_600mm.bin 2020_03_12_13_30_49__1280_800_Speckle.bin 1280 800 941.3 40 600 64 25
// ..\set\15 ref_1280x800_1000mm.bin 2020_03_12_13_30_49__1280_800_Speckle.bin 1280 800 941.3 40 1000 64 25
// ..\set\16 ref_1280x800_600mm.bin 2020_03_12_13_31_05__1280_800_Speckle.bin 1280 800 941.3 40 600 64 25
// ..\set\16 ref_1280x800_1000mm.bin 2020_03_12_13_31_05__1280_800_Speckle.bin 1280 800 941.3 40 1000 64 25
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
	}
	//
	Mat ref = binref(wrk_pth + "\\" + ref_pth, rows, cols);
	//ref = bft::bin_filter02(ref);
	imwrite(res_pth + "\\ref.png", ref);
	Mat cur_ir = skg::spckl2png(wrk_pth + "\\" + cur_pth, rows, cols, 0);
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
	Mat out2;
	Mat peak2;
	Mat s12;
	Mat subpixeMap2;
	Mat max_ix2;
	ofst::up = 0;
	ofst::down = 0;
	int16_t* ofs2 = ofst::fastBlockMatchPadding_Y_first(ref, cur, out2, peak2, subpixeMap2, s12, max_ix2);
	float* sftofs2 = cvti162f(ofs2, n);
	imwrite(res_pth + "\\offset_shw.png", fs2d::show_offset2(sftofs2, rows, cols));

	float* sftDepth2 = fs2d::offset2depth(sftofs2, rows, cols, fxy, baseline, wall, search_box, mbsize);
	Mat sftDshw2 = psd2::pseudocolor(sftDepth2, rows, cols);
	imwrite(res_pth + "\\sftDshw2.png", sftDshw2);
	dbn::write_depth(sftDepth2, rows, cols, res_pth + "\\soft_depth2.raw");
	//
	//sftofs: calculate offset
	//sftDepth2
	//
	float inv_d = fs2d::offset2depth(int16_t(ofst::invalidXoffset), fxy, baseline, wall);
	for (int i = 0; i < n; i++)
	{
		float val = sftDepth2[i];
		if (val >= 1500)
		{
			sftDepth2[i] = inv_d;
		}
	}
	const int edge_cols = 60;
	const int edge_rows = 15;
	{
		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < edge_cols; x++)
			{
				sftDepth2[y * cols + x] = inv_d;
			}
		}
		for (int y = 0; y < rows; y++)
		{
			for (int x = cols - edge_cols; x < cols; x++)
			{
				sftDepth2[y * cols + x] = inv_d;
			}
		}
		for (int y = 0; y < edge_rows; y++)
		{
			for (int x = 0; x < cols; x++)
			{
				sftDepth2[y * cols + x] = inv_d;
			}
		}
		for (int y = rows - edge_rows; y < rows; y++)
		{
			for (int x = 0; x < cols; x++)
			{
				sftDepth2[y * cols + x] = inv_d;
			}
		}
	}
	
	vector<vector<Point>> dp_set=sdsg::seed_seg(sftDepth2, rows, cols);
	float *dpth2 =sgns::denoise(sftDepth2, rows, cols, dp_set);
	string fs_str = oin_pth + "\\hand_denoise_dpth.raw";
	float* dn_depth = dbn::read_depth(fs_str, rows, cols);
	if (!dn_depth)
	{
		dn_depth=hdde::hand_dn(dpth2, rows, cols, dp_set, fs_str);
	}
	imwrite(res_pth + "\\dn_depth.png", psd2::pseudocolor(dn_depth, rows, cols));
	float* invs_ofst = fs2d::depth2offset(dn_depth, rows, cols, fxy, baseline, wall, inv_d);
	Mat fake_trg = f2tg::offset2targ(ref, invs_ofst);
	imwrite(res_pth + "\\fake_trg.png", fake_trg);
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < cols; x++)
		{
			fake_trg.at<uchar>(y, x) = fake_trg.at<uchar>(y, x) / 255;
		}
	}
	float* fake_offs = NULL;
	float* fake_depth = NULL;
	{
		Mat out;
		Mat peak;
		Mat s1;
		Mat subpixeMap;
		Mat max_ix;
		ofst::up = 0;
		ofst::down = 0;
		int16_t* fake_ofs = ofst::fastBlockMatchPadding_Y_first(ref, fake_trg, out, peak, subpixeMap, s1, max_ix);
		fake_offs = cvti162f(fake_ofs, n);
		delete[] fake_ofs;
		fake_depth = fs2d::offset2depth(fake_offs, rows, cols, fxy, baseline, wall, search_box, mbsize);
		imwrite(res_pth + "\\fake_depth.png", psd2::pseudocolor(fake_depth, rows, cols));
	}
	//
	
	delete[] ofs2;
	delete[] dn_depth;
	delete[] sftofs2;
	delete[] sftDepth2;
	delete[] dpth2;
	delete[] invs_ofst;
	delete[] fake_offs;
	delete[] fake_depth;
	return 0;
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

Mat cut_patch(Mat img, Rect pos)
{
	Mat res(pos.height, pos.width, CV_8UC1);
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

float* cvti162f(int16_t* ivec, int n)
{
	float* res = new float[n];
	for (int i = 0; i < n; i++)
	{
		res[i] = ivec[i];
	}
	return res;
}

