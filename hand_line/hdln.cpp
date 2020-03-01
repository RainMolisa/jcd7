#include<string>
#include<iostream>
#include<vector>
#include<opencv.hpp>
//#include "..\jcd7\dpthbin.h"
//#include "..\jcd7\pseudocolor.h"
using namespace cv;
using namespace std;
float scale = 1;
int rows = 640;
int cols = 800;
fstream ot_file;
string get_folder(string file);
vector<Point> mk_line(Point p1, Point p2);
Point p1;
Point p2;
vector<Point> line;
bool m_flag = false;

void OnMouse(int event, int x, int y, int flags, void* param)
{
	//float* p = (float*)param;
	switch (event)
	{
	case EVENT_LBUTTONDOWN:
	{
		p1.x = x / scale;
		p1.y = y / scale;
		m_flag = true;
	}
	break;
	case EVENT_MOUSEMOVE:
	{
		if (m_flag)
		{
			p2.x = x / scale;
			p2.y = y / scale;
			::line = mk_line(p1, p2);
		}
		break;
	}
	case EVENT_LBUTTONUP:
		m_flag = false;
		break;
	default:
		break;
	}
}
int main(int argc, char** argv)
{
	{
		string fpath;
		cin >> fpath;
		string folder = get_folder(get_folder(fpath));
		string folder_an = folder + "\\in_other";
		system(("mkdir " + folder_an).c_str());
		string outFile = folder_an + "\\line_list.txt";
		cout << outFile << endl;
		ot_file.open(outFile, ios::out);
		//float* in_depth = dbn::read_depth(fpath, rows, cols);
		Mat org = imread(fpath);
		rows = org.rows;
		cols = org.cols;
		Mat shw = org.clone();
		namedWindow("mouse", WINDOW_AUTOSIZE);
		setMouseCallback("mouse", OnMouse);
		bool flag = true;
		while (flag)
		{
			imshow("mouse", shw);
			int key = waitKey(5);
			switch (key)
			{
			case 'd':
				scale = scale + 0.02;
				break;
			case 'f':
				scale = scale - 0.02;
				break;
			case ' ':
				flag = false;
				break;
			case 'p':
			{
				for (int i = 0; i < ::line.size(); i++)
				{
					int x = ::line[i].x;
					int y = ::line[i].y;
					ot_file << x << " " << y << endl;
				}
				ot_file << -10 << " " << -10 << endl;
				::line.clear();
			}
				break;
			default:
				break;
			}
			resize(org, shw, Size(cols * scale, rows * scale));
			for (int i = 0; i < ::line.size(); i++)
			{
				int x = ::line[i].x;
				int y = ::line[i].y;
				x = x * scale;
				y = y * scale;
				shw.at<uchar>(y, 3 * x + 0) = 255;
				shw.at<uchar>(y, 3 * x + 1) = 255;
				shw.at<uchar>(y, 3 * x + 2) = 255;
			}
		}
		destroyAllWindows();
		ot_file.close();
	}

}

vector<Point> mk_line(Point p1, Point p2)
{
	vector<Point> res;
	int dx, dy, n, k;
	double xinc, yinc, x, y;
	dx = p2.x - p1.x;
	dy = p2.y - p1.y;
	if (abs(dx) > abs(dy))
	{
		n = abs(dx);
	}
	else
	{
		n = abs(dy);
	}
	xinc = double(dx) / double(n);
	yinc = double(dy) / double(n);
	x = p1.x;
	y = p1.y;
	for (k = 0; k < n; k++)
	{
		Point val(floor(x+0.5),floor(y+0.5));
		res.push_back(val);
		x += xinc;
		y += yinc;
	}
	return res;
}

string get_folder(string file)
{
	int i = file.find_last_of('\\');
	string res = file.substr(0, i);
	return res;
}


