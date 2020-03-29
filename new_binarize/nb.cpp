/*
test new binarize
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
#include "..\jcd7_2\offset2target.h"
#include "..\s2k\s_k.h"
using namespace std;
using namespace cv;
float* cvti162f(int16_t* ivec, int n);
// ..\set\11 ref_1280_0307.bin 45face-00000269-ir.bin 0 1280 800 846.67 42 600 64 25
int main(int argc, char** argv)
{
	int rows, cols;
	string wrk_pth;
	string cur_pth;
	string ref_pth;
	float fxy;
	int baseline, wall, search_box, mbsize;
	int cur_mode;
	if (argc < 12)
	{
		printf("arg error\n");
		return 0;
	}
	{
		wrk_pth = string(argv[1]);
		ref_pth = string(argv[2]);
		cur_pth = string(argv[3]);
		sscanf_s(argv[4], "%d", &cur_mode);
		sscanf_s(argv[5], "%d", &rows);
		sscanf_s(argv[6], "%d", &cols);
		sscanf_s(argv[7], "%f", &fxy);
		sscanf_s(argv[8], "%d", &baseline);
		sscanf_s(argv[9], "%d", &wall);
		sscanf_s(argv[10], "%d", &search_box);
		sscanf_s(argv[11], "%d", &mbsize);
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
	Mat cur_ir = skg::spckl2png(wrk_pth + "\\" + cur_pth, rows, cols, cur_mode);
	imwrite(res_pth + "\\cur_ir.png", cur_ir);
	Mat cur;
	bnz::EnHance(cur_ir, cur);
	imwrite(res_pth + "\\cur.png", cur);
	Mat cur_nw;
	Mat cur_nwb;
	bnz::EnHance_th2(cur_ir, cur_nwb, &cur_nw);
	imwrite(res_pth + "\\cur_nwb.png", cur_nwb);
	imwrite(res_pth + "\\cur_nw.png", cur_nw);
	//f2tg
	vector<vector<Point>> cur_set = f2tg::seg_bin(cur);
	vector<vector<Point>> cnb_set = f2tg::seg_bin(cur_nwb);
	{
		fstream fs1, fs2;
		fstream fs3,fs4;
		fs1.open(res_pth + "\\cur_set.txt", ios::out);
		fs2.open(res_pth + "\\cnb_set.txt", ios::out);
		fs3.open(res_pth + "\\small_gray.txt", ios::out);
		fs4.open(res_pth + "\\other_gray.txt", ios::out);
		for (int i = 0; i < cur_set.size(); i++)
		{
			int len = cur_set[i].size();
			fs1 << cur_set[i].size() << endl;
			if (len <= 5)
			{
				for (int j = 0; j < len; j++)
				{
					int val = cur_ir.at<uchar>(cur_set[i][j]);
					fs3 << val << endl;
				}
			}
			else
			{
				for (int j = 0; j < len; j++)
				{
					int val = cur_ir.at<uchar>(cur_set[i][j]);
					fs4 << val << endl;
				}
			}
		}
		for (int i = 0; i < cnb_set.size(); i++)
		{
			fs2 << cnb_set[i].size() << endl;
		}
		fs1.close();
		fs2.close();
		fs3.close();
		fs4.close();
	}
	//
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < cols; x++)
		{
			ref.at<uchar>(y, x) = ref.at<uchar>(y, x) / 255;
			cur.at<uchar>(y, x) = cur.at<uchar>(y, x) / 255;
			cur_nwb.at<uchar>(y, x) = cur_nwb.at<uchar>(y, x) / 255;
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
	dbn::write_depth_txt(sftDepth2, rows, cols, res_pth + "\\soft_depth2.txt");
	//
	//sftofs: calculate offset
	//sftDepth2
	//
	Mat cur_cur_nwb(rows, cols, CV_8UC3, Scalar(0, 0, 0));
	{
		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < cols; x++)
			{
				if (cur.at<uchar>(y, x) > 0)
				{
					cur_cur_nwb.at<uchar>(y, 3 * x + 0) = 255;
				}
				if (cur_nwb.at<uchar>(y, x) > 0)
				{
					cur_cur_nwb.at<uchar>(y, 3 * x + 2) = 255;
				}
			}
		}
	}
	imwrite(res_pth + "\\cur_cur_nwb.png", cur_cur_nwb);
	{
		Mat out2;
		Mat peak2;
		Mat s12;
		Mat subpixeMap2;
		Mat max_ix2;
		ofst::up = 0;
		ofst::down = 0;
		int16_t* ofs3 = ofst::fastBlockMatchPadding_Y_first(ref, cur_nwb, out2, peak2, subpixeMap2, s12, max_ix2);
		float* sftofs3 = cvti162f(ofs3, n);

		float* sftDepth3 = fs2d::offset2depth(sftofs3, rows, cols, fxy, baseline, wall, search_box, mbsize);
		Mat sftDshw3 = psd2::pseudocolor(sftDepth3, rows, cols);
		imwrite(res_pth + "\\sftDshw3.png", sftDshw3);
		dbn::write_depth(sftDepth3, rows, cols, res_pth + "\\soft_depth3.raw");
		dbn::write_depth_txt(sftDepth3, rows, cols, res_pth + "\\soft_depth3.txt");
		delete[] ofs3;
		delete[] sftofs3;
	}
	{
		system(("mkdir " + res_pth+"\\th_abs").c_str());
		Mat out2;
		Mat peak2;
		Mat s12;
		Mat subpixeMap2;
		Mat max_ix2;
		ofst::up = 0;
		ofst::down = 0;
		//
		Mat cur2;
		Mat cur3;
		bnz::th_abs = 60;
		bnz::EnHance(cur_ir, cur2);
		bnz::EnHance_th2(cur_ir, cur3);
		bnz::th_abs = bnz::def_th_abs;
		imwrite(res_pth + "\\th_abs\\cur2.png", cur2);
		imwrite(res_pth + "\\th_abs\\cur3.png", cur3);
		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < cols; x++)
			{
				cur2.at<uchar>(y, x) = cur2.at<uchar>(y, x) / 255;
				cur3.at<uchar>(y, x) = cur3.at<uchar>(y, x) / 255;
			}
		}
		int16_t* ofs3 = ofst::fastBlockMatchPadding_Y_first(ref, cur2, out2, peak2, subpixeMap2, s12, max_ix2);
		float* sftofs3 = cvti162f(ofs3, n);
		float* sftDepth3 = fs2d::offset2depth(sftofs3, rows, cols, fxy, baseline, wall, search_box, mbsize);
		Mat sftDshw3 = psd2::pseudocolor(sftDepth3, rows, cols);
		imwrite(res_pth + "\\th_abs\\sftDshw3.png", sftDshw3);
		dbn::write_depth(sftDepth3, rows, cols, res_pth + "\\th_abs\\soft_depth3.raw");
		dbn::write_depth_txt(sftDepth3, rows, cols, res_pth + "\\th_abs\\soft_depth3.txt");
		delete[] ofs3;
		delete[] sftofs3;
		//
		int16_t* ofs4 = ofst::fastBlockMatchPadding_Y_first(ref, cur3, out2, peak2, subpixeMap2, s12, max_ix2);
		float* sftofs4 = cvti162f(ofs4, n);
		float* sftDepth4 = fs2d::offset2depth(sftofs4, rows, cols, fxy, baseline, wall, search_box, mbsize);
		Mat sftDshw4 = psd2::pseudocolor(sftDepth4, rows, cols);
		imwrite(res_pth + "\\th_abs\\sftDshw4.png", sftDshw4);
		dbn::write_depth(sftDepth4, rows, cols, res_pth + "\\th_abs\\soft_depth4.raw");
		dbn::write_depth_txt(sftDepth4, rows, cols, res_pth + "\\th_abs\\soft_depth4.txt");
		delete[] ofs4;
		delete[] sftofs4;
	}

	{
		string k_str = res_pth + "\\k";
		system(("mkdir " + k_str).c_str());
		float s = s_k::k2s(5, bnz::th_l);
		//
		Mat out2;
		Mat peak2;
		Mat s12;
		Mat subpixeMap2;
		Mat max_ix2;
		ofst::up = 0;
		ofst::down = 0;
		//
		Mat cur2;
		//bnz::th_abs = 60;
		bnz::s = s;
		bnz::EnHance_th2(cur_ir, cur2);
		//bnz::th_abs = bnz::def_th_abs;
		imwrite(k_str + "\\cur2.png", cur2);
		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < cols; x++)
			{
				cur2.at<uchar>(y, x) = cur2.at<uchar>(y, x) / 255;
			}
		}
		int16_t* ofs3 = ofst::fastBlockMatchPadding_Y_first(ref, cur2, out2, peak2, subpixeMap2, s12, max_ix2);
		float* sftofs3 = cvti162f(ofs3, n);
		float* sftDepth3 = fs2d::offset2depth(sftofs3, rows, cols, fxy, baseline, wall, search_box, mbsize);
		Mat sftDshw3 = psd2::pseudocolor(sftDepth3, rows, cols);
		imwrite(k_str + "\\sftDshw3.png", sftDshw3);
		dbn::write_depth(sftDepth3, rows, cols, k_str + "\\soft_depth3.raw");
		dbn::write_depth_txt(sftDepth3, rows, cols, k_str + "\\soft_depth3.txt");
		delete[] ofs3;
		delete[] sftofs3;
		//
	}
	//
	delete[] ofs2;
	delete[] sftofs2;
	delete[] sftDepth2;
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



