/*
To determine the relationship between the binarization threshold and the depth map noise
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
#include "..\jcd7_2\offset2target.h"
using namespace std;
using namespace cv;
Mat dif_offset(float* in_depth, float* calDepth, int rows, int cols, int len);
Mat cut_patch(Mat img, Rect pos);
float* cvti162f(int16_t* ivec, int n);
void process(string res_pth, string folder, int mod, \
	float st, float ed, float num, \
	Mat cur_ir, int rows, int cols, \
	Mat ref, int baseline, int wall, int search_box, int mbsize, float fxy);
// ..\set\01 ref_640.bin 40cm_800x640-00001395-ir.bin 640 800 943 40 600 64 19
// ..\set\02 ref_640.bin 60cm_800x640-00001065-ir.bin 640 800 943 40 600 64 19
// ..\set\04 ref_1280_0.bin 40cm_subpixel1_y1_x64-00000347-ir.bin 1280 800 846.67 42 600 64 25
// ..\set\10 ref_dn005.bin f1-00000796-ir.bin 1280 800 936.4 40 600 64 25
// ..\set\11 ref_1280_0307.bin 45face-00000269-ir.bin 1280 800 846.67 42 600 64 25
// ..\set\12 ref_1280_0307.bin face40cm-00010632-ir.bin 1280 800 846.67 42 600 64 25
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
	vector<vector<Point>> ref_seg = f2tg::seg_bin(ref);
	float ref_mn_area = 0;
	float ref_num = 0;
	for (int i = 0; i < ref_seg.size(); i++)
	{
		if (ref_seg[i].size() > 5)
		{
			ref_num++;
			ref_mn_area = ref_mn_area + ref_seg[i].size();
		}
	}
	ref_mn_area = ref_mn_area / ref_num;
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

	//process(res_pth, "bin0", 0, 30, 200, 70, \
		cur_ir, rows, cols, ref, baseline, wall, search_box, mbsize, fxy);
	process(res_pth, "bin2", 2, 1.00, 1.10, 70, \
		cur_ir, rows, cols, ref, baseline, wall, search_box, mbsize, fxy);
	
	

	//
	delete[] ofs2;
	delete[] sftofs2;
	delete[] sftDepth2;
	return 0;
}

void process(string res_pth,string folder,int mod,\
			float st,float ed,float num,\
			Mat cur_ir,int rows,int cols,\
			Mat ref, int baseline,int wall,int search_box,int mbsize, float fxy)
{
	ofst::set_env(search_box, mbsize);
	Mat out2;
	Mat peak2;
	Mat s12;
	Mat subpixeMap2;
	Mat max_ix2;
	ofst::up = 0;
	ofst::down = 0;
	system(("mkdir " + res_pth + "\\"+folder).c_str());
	int n = rows * cols;
	//
	Size size = Size(cols, rows);
	//
	VideoWriter vwer1;
	vwer1.open(res_pth + "\\" + folder+"\\depth.mp4",
		VideoWriter::fourcc('m', 'p', '4', 'v'), 20, size, true);
	VideoWriter vwer2;
	vwer2.open(res_pth + "\\" + folder + "\\bin.mp4",
		VideoWriter::fourcc('m', 'p', '4', 'v'), 20, size, false);
	//
	fstream fs;
	fs.open(res_pth + "\\" + folder + "\\res.txt",ios::out);
	//
	float stp = (ed - st) / num;
	int i = 0;
	for (float hd = st; hd <= ed; hd = hd + stp)
	{
		system(("mkdir " + res_pth + "\\" + folder + format("\\%03d", i)).c_str());
		Mat cur;
		if (mod == 0)
		{
			bnz::th_abs = hd;
			bnz::EnHance1(cur_ir, cur);
		}
		if (mod == 1)
		{
			bnz::th_l = hd;
			bnz::EnHance2(cur_ir, cur);
		}
		if (mod == 2)
		{
			bnz::th_l = hd;
			bnz::EnHance(cur_ir, cur);
		}
		imwrite(res_pth + "\\" + folder + format("\\%03d", i)+"\\cur.png", cur);
		vwer2.write(cur);
		vector<vector<Point>> cur_seg= f2tg::seg_bin(cur);
		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < cols; x++)
			{
				cur.at<uchar>(y, x) = cur.at<uchar>(y, x) / 255;
			}
		}
		
		int16_t* ofs2 = ofst::fastBlockMatchPadding_Y_first(ref, cur, out2, peak2, subpixeMap2, s12, max_ix2);
		float* sftofs2 = cvti162f(ofs2, n);
		//imwrite(res_pth + "\\" + folder + format("\\%03d", i) + "\\offset_shw.png", fs2d::show_offset2(sftofs2, rows, cols));

		float* sftDepth2 = fs2d::offset2depth(sftofs2, rows, cols, fxy, baseline, wall, search_box, mbsize);
		Mat sftDshw2 = psd2::pseudocolor(sftDepth2, rows, cols);
		imwrite(res_pth + "\\" + folder + format("\\%03d", i) + "\\sftDshw2.png", sftDshw2);
		putText(sftDshw2,format("%f", hd),Point(0,150),
			FONT_HERSHEY_COMPLEX,2.0,Scalar(0,0,255),2);
		dbn::write_depth(sftDepth2, rows, cols, res_pth + "\\" + folder + format("\\%03d", i) + "\\soft_depth2.raw");
		vector<vector<Point>> segr = sdsg::seed_seg(sftDepth2,rows,cols);
		int sm_area_sum = 0;
		for (int i = 0; i < segr.size(); i++)
		{
			if (segr[i].size() < 1600)
			{
				sm_area_sum++;
			}
		}
		int sm_spak = 0;
		for (int i = 0; i < cur_seg.size(); i++)
		{
			if (cur_seg[i].size() <= 5)
			{
				sm_spak++;
			}
		}
		fs << hd <<" "<<sm_area_sum<<" "<<sm_spak<< endl;
		delete[] ofs2;
		delete[] sftofs2;
		delete[] sftDepth2;
		//
		i++;
		vwer1.write(sftDshw2);
		
	}
	fs.close();
	vwer1.release();
	vwer2.release();
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

