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
float* cvr_depth_p(float* depth, int rows, int cols, Point p, vector<Point>& area);
// ..\set\19 config.txt 64 25
// ..\set\15 config.txt 64 25
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
	ofst::set_env(-64, 32, mbsize);
	int16_t* ofs3 = ofst::fastBlockMatchPadding2(ref, cur);
	float* sftofs3 = cvti162f(ofs3, n);
	float* sftDepth3 = fs2d::offset2depth(sftofs3, rows, cols, fxy, baseline, wall);
	imwrite(res_pth + "\\sftDshw_6432.png", psd2::pseudocolor(sftDepth3, rows, cols));
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
	{
		string point_str = res_pth + "\\point2res";
		fstream fst_point;
		fst_point.open(oin_pth + "\\point_list.txt", ios::in);
		if (fst_point)
		{
			//
			Mat peakMatch52, subpixeMap52, score52, max_ix52;
			ofst::set_env(52, mbsize);
			int16_t* ofs2 = ofst::fastBlockMatchPadding2(ref, cur, &peakMatch52, &subpixeMap52, &score52, &max_ix52);
			float* sftofs2 = cvti162f(ofs2, n);
			float* sftDepth2 = fs2d::offset2depth(sftofs2, rows, cols, fxy, baseline, wall);
			imwrite(res_pth + "\\sftDshw_52.png", psd2::pseudocolor(sftDepth2, rows, cols));
			//
			Mat sftDshw = psd2::pseudocolor(sftDepth1, rows, cols);
			system(("mkdir " + point_str).c_str());
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
			fstream fs1,fs2;
			fs1.open(point_str + "\\point_depth1.txt", ios::out);
			fs2.open(point_str + "\\point_depth2.txt", ios::out);
			for (int i = 0; i < vec.size(); i++)
			{
				string pstr = point_str + format("\\%03d", i);
				system(("mkdir " + pstr).c_str());
				Mat buf1 = sftDshw.clone();
				int size = 10;
				circle(buf1, Point(vec[i].x, vec[i].y), 30, Scalar(255, 255, 255), 4);
				cv::line(buf1, Point(vec[i].x - size / 2, vec[i].y), Point(vec[i].x + size / 2, vec[i].y), Scalar(255, 255, 255), 1, 8, 0);
				cv::line(buf1, Point(vec[i].x, vec[i].y - size / 2), Point(vec[i].x, vec[i].y + size / 2), Scalar(255, 255, 255), 1, 8, 0);
				putText(buf1, format("%d", i), vec[i], FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(255, 255, 255));
				imwrite(pstr + "\\pos.png", buf1);
				vector<Point> pset;
				float* d2 = cvr_depth_p(sftDepth1, rows, cols, vec[i], pset);
				Mat d2_png = psd2::pseudocolor(d2, rows, cols);
				imwrite(pstr + "\\d2.png", d2_png);
				
				fs1 << d2[pset[0].y*cols+ pset[0].x] << " ";
				for (int j = 0; j < pset.size(); j++)
				{
					int xy = pset[j].y * cols + pset[j].x;
					fs1 << sftDepth1[xy] <<" "<< sftDepth2[xy] << " ";
					fs2 << d2[xy]<<" "<< sftDepth1[xy] <<" "<< max_ix.at<int16_t>(pset[j])- search_box << " " 
						<< sftDepth2[xy] << " " << max_ix52.at<int16_t>(pset[j])- ofst::r <<endl;
				}
				fs1 << endl;
				delete[] d2;
			}
			fs1.close();
			fs2.close();
			delete[] ofs2;
			delete[] sftofs2;
			delete[] sftDepth2;
		}
		fst_point.close();
	}
	delete[] ofs3;
	delete[] sftofs3;
	delete[] sftDepth3;
	return 0;
}

float area_val(vector<Point>& area, float* depth, int rows, int cols)
{
	int len = area.size();
	int max_x = area[0].x;
	int max_y = area[0].y;
	int min_x = area[0].x;
	int min_y = area[0].y;
	for (int i = 1; i < len; i++)
	{
		int x = area[i].x;
		int y = area[i].y;
		max_x = (max_x < x ? x : max_x);
		max_y = (max_y < y ? y : max_y);
		min_x = (min_x < x ? min_x : x);
		min_y = (min_y < y ? min_y : y);
	}
	max_x++;
	max_y++;
	min_x--;
	min_y--;
	vector<float> set;
	for (int x = min_x; x <= max_x; x++)
	{
		int y = min_y;
		if (x >= 0 && x < cols && y >= 0 && y < rows)
		{
			set.push_back(depth[y * cols + x]);
		}
	}
	for (int x = min_x; x <= max_x; x++)
	{
		int y = max_y;
		if (x >= 0 && x < cols && y >= 0 && y < rows)
		{
			set.push_back(depth[y * cols + x]);
		}
	}
	for (int y = min_y; y <= max_y; y++)
	{
		int x = min_x;
		if (x >= 0 && x < cols && y >= 0 && y < rows)
		{
			set.push_back(depth[y * cols + x]);
		}
	}
	for (int y = min_y; y <= max_y; y++)
	{
		int x = max_x;
		if (x >= 0 && x < cols && y >= 0 && y < rows)
		{
			set.push_back(depth[y * cols + x]);
		}
	}
	sort(set.begin(), set.end());
	int a = (set.size()) / 2;
	float res = set[a];
	return res;
}

float* cvr_depth_p(float* depth,int rows,int cols,Point p, vector<Point> &area)
{
	int n = rows * cols;
	float* res = new float[n];
	for (int i = 0; i < n; i++)
	{
		res[i] = depth[i];
	}
	Mat mask(rows, cols, CV_8UC1, Scalar(0));
	
	area.push_back(p);
	vector<Point> m_stack;
	m_stack.push_back(p);
	mask.at<uchar>(p) = 255;
	while (m_stack.size() > 0)
	{
		cv::Point cor = m_stack[m_stack.size() - 1];
		m_stack.pop_back();
		cv::Point cddt[4];
		cddt[0].x = cor.x; cddt[0].y = cor.y + 1;
		cddt[1].x = cor.x; cddt[1].y = cor.y - 1;
		cddt[2].x = cor.x + 1; cddt[2].y = cor.y;
		cddt[3].x = cor.x - 1; cddt[3].y = cor.y;
		for (int i = 0; i < 4; i++)
		{
			if (cddt[i].x >= 0 && cddt[i].x < cols && cddt[i].y >= 0 && cddt[i].y < rows)
			{
				int xy = cddt[i].y * cols + cddt[i].x;
				float val = depth[xy];
				if (mask.at<uchar>(cddt[i].y, cddt[i].x) == 0 && (val<0 || val>=1500))
				{
					mask.at<uchar>(cddt[i].y, cddt[i].x) = 255;
					area.push_back(cddt[i]);
					m_stack.push_back(cddt[i]);
				}
			}
		}
	}
	float v2 = area_val(area, depth, rows, cols);
	for (int i = 0; i < area.size(); i++)
	{
		int xy= area[i].y * cols + area[i].x;
		res[xy] = v2;
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