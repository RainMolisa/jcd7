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
// ..\set\19 config.txt 64 25
int main(int argc, char** argv)
{
	int rows, cols;
	string wrk_pth;
	string cur_pth;
	string ref_pth;
	float fxy;
	int baseline, wall, search_box, mbsize;
	int cur_mode;
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
	ofst::set_env(search_box, mbsize);
	ofst::up = 0;
	ofst::down = 0;
	//
	Mat peakMatch,subpixeMap;
	Mat score1,max_ix;
	int16_t* ofs1 = ofst::fastBlockMatchPadding2(ref, cur,
		&peakMatch, &subpixeMap,&score1,&max_ix);
	float* sftofs1 = cvti162f(ofs1, n);
	float* sftDepth1 = fs2d::offset2depth(sftofs1, rows, cols, fxy, baseline, wall);
	imwrite(res_pth + "\\sftDshw1.png", psd2::pseudocolor(sftDepth1, rows, cols));
	dbn::write_depth(sftDepth1, rows, cols, res_pth + "\\soft_depth1.raw");
	dbn::write_depth_txt(sftDepth1, rows, cols, res_pth + "\\soft_depth1.txt");
	//
	{
		string point_str = res_pth + "\\point_res";
		fstream fst_point;
		fst_point.open(oin_pth + "\\point_list.txt", ios::in);
		if (fst_point)
		{
			Mat sftDshw =psd2::pseudocolor(sftDepth1, rows, cols);
			system(("mkdir " + point_str).c_str());
			Mat buf1 = sftDshw.clone();
			vector<Point> vec;
			while (!fst_point.eof())
			{
				int x = -1, y = -1;
				fst_point >> x >> y;
				if (x >= 0 && y >= 0)
				{
					vec.push_back(Point(x, y));
				}
			}
			//
			fstream fs4;
			fs4.open(point_str + "\\point_offset.txt", ios::out);
			for (int i = 0; i < vec.size(); i++)
			{
				int size = 10;
				circle(buf1, Point(vec[i].x, vec[i].y), 30, Scalar(255, 255, 255), 4);
				cv::line(buf1, Point(vec[i].x - size / 2, vec[i].y), Point(vec[i].x + size / 2, vec[i].y), Scalar(255, 255, 255), 1, 8, 0);
				cv::line(buf1, Point(vec[i].x, vec[i].y - size / 2), Point(vec[i].x, vec[i].y + size / 2), Scalar(255, 255, 255), 1, 8, 0);
				putText(buf1, format("%d", i), vec[i], FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(255, 255, 255));
				int x = vec[i].x;
				int y = vec[i].y;
				fs4 << vec[i].x <<" "<< vec[i].y <<" "<< ofs1[y*cols+x]<<" ";
				fs4 << subpixeMap.at<int16_t>(y, x)<<" ";
				fs4 << sftDepth1[y * cols + x] << " ";
				fs4 << endl;
			}
			fs4.close();
			imwrite(point_str + "\\pos.png", buf1);
			//
			fstream fs2;
			fs2.open(point_str + "\\point_result.txt", ios::out);
			fstream fs3;
			fs3.open(point_str + "\\point_best_x.txt", ios::out);
			for (int i = 0; i < score1.size[2]; i++)
			{
				for (int j = 0; j < vec.size(); j++)
				{
					int x, y;
					x = vec[j].x;
					y = vec[j].y;
					int16_t val = score1.at<int16_t>(y, x, i);
					fs2 << val << " ";
				}
				fs2 << endl;
			}
			for (int j = 0; j < vec.size(); j++)
			{
				int x, y;
				x = vec[j].x;
				y = vec[j].y;
				int i = max_ix.at<int16_t>(y, x);
				int v = score1.at<int16_t>(y, x, i);
				fs3 << i << " " << v << endl;
			}
			fs2.close();
			fs3.close();

			fst_point.close();
		}
	}
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