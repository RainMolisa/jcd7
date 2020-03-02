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
	int16_t* ofs2f = ofst::fastBlockMatchPadding_Y_first(ref, cur, out2, peak2, subpixeMap2, s12, max_ix2, false);
	float* sftofs2f = cvti162f(ofs2f, n);

	float* sftDepth2 = fs2d::offset2depth(sftofs2, rows, cols, fxy, baseline, wall, search_box, mbsize);
	Mat sftDshw2 = psd2::pseudocolor(sftDepth2, rows, cols);
	imwrite(res_pth + "\\sftDshw2.png", sftDshw2);
	dbn::write_depth(sftDepth2, rows, cols, res_pth + "\\soft_depth2.raw");
	//
	float* sftDepth2f = fs2d::offset2depth(sftofs2f, rows, cols, fxy, baseline, wall, search_box, mbsize);
	Mat sftDshw2f = psd2::pseudocolor(sftDepth2f, rows, cols);
	imwrite(res_pth + "\\sftDshw2f.png", sftDshw2f);
	dbn::write_depth(sftDepth2f, rows, cols, res_pth + "\\soft_depth2f.raw");
	//sftofs: calculate offset
	//sftDepth2
	{
		fstream fst_point;
		fst_point.open(oin_pth + "\\point_list.txt", ios::in);
		if (fst_point)
		{
			system(("rd /Q /S " + res_pth + "\\point").c_str());
			system(("mkdir " + res_pth + "\\point").c_str());
			Mat buf1 = sftDshw2.clone();
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
			for (int i = 0; i < vec.size(); i++)
			{
				int size = 10;
				circle(buf1, Point(vec[i].x, vec[i].y), 30, Scalar(255, 255, 255), 4);
				cv::line(buf1, Point(vec[i].x - size / 2, vec[i].y), Point(vec[i].x + size / 2, vec[i].y), Scalar(255, 255, 255), 1, 8, 0);
				cv::line(buf1, Point(vec[i].x, vec[i].y - size / 2), Point(vec[i].x, vec[i].y + size / 2), Scalar(255, 255, 255), 1, 8, 0);
				putText(buf1, format("%d", i), vec[i], FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(0, 0, 0));
			}
			imwrite(res_pth + "\\point\\pos.png", buf1);
			fstream fs2;
			fs2.open(res_pth + "\\point\\point_result.txt", ios::out);
			for (int i = 0; i < s12.size[2]; i++)
			{
				for (int j = 0; j < vec.size(); j++)
				{
					int x, y;
					x = vec[j].x;
					y = vec[j].y;
					int16_t val = s12.at<int16_t>(y, x, i);
					fs2 << val << " ";
				}
				fs2 << endl;
			}
			fs2.close();
			fst_point.close();
		}
	}
	{
		fstream fs_line;
		fs_line.open(oin_pth + "\\line_list.txt", ios::in);
		if (fs_line)
		{
			system(("rd /Q /S " + res_pth + "\\line").c_str());
			system(("mkdir " + res_pth + "\\line").c_str());
			vector<vector<Point>> line_set;
			vector<Point> val;
			while (!fs_line.eof())
			{
				
				int x = -1, y = -1;
				fs_line >> x >> y;
				if (x != -1 && y != -1)
				{
					if (x != -10 && y != -10)
					{
						val.push_back(Point(x, y));
					}
					else
					{
						line_set.push_back(val);
						val.clear();
					}
				}
			}
			fs_line.close();
			Mat buf1 = sftDshw2.clone();
			fstream fso1, fso2;
			fso1.open(res_pth + "\\line\\line_depthT.txt", ios::out);
			fso2.open(res_pth + "\\line\\line_depthF.txt", ios::out);
			for (int i = 0; i < line_set.size(); i++)
			{
				
				for (int j = 0; j < line_set[i].size(); j++)
				{
					int x = line_set[i][j].x;
					int y = line_set[i][j].y;
					circle(buf1, line_set[i][j], 1, Scalar(255, 255, 255), 1);
					float v1 = sftDepth2[y * cols + x];
					float v2 = sftDepth2f[y * cols + x];
					fso1 << v1 << " ";
					fso2 << v2 << " ";
				}
				fso1 << endl;
				fso2 << endl;
				putText(buf1, format("%d", i), line_set[i][0], FONT_HERSHEY_COMPLEX_SMALL, 2, Scalar(0, 0, 0), 2);
			}
			imwrite(res_pth + "\\line\\line_pos.png", buf1);
			fso1.close();
			fso2.close();
		}
	}


	//
	delete[] ofs2;
	delete[] sftofs2;
	delete[] sftDepth2;
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

