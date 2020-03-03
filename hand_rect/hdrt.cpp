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

Point p1;
Point p2;
Rect rt;
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
			//::line = mk_line(p1, p2);
			rt = Rect(p1, p2);
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
		cout << "hand_rect:";
		cin >> fpath;
		string folder = get_folder(get_folder(fpath));
		string folder_an = folder + "\\in_other";
		system(("mkdir " + folder_an).c_str());
		string outFile = folder_an + "\\rect_list.txt";
		cout << outFile << endl;
		ot_file.open(outFile, ios::out);
		//float* in_depth = dbn::read_depth(fpath, rows, cols);
		Mat org = imread(fpath);
		rt.width = -1;
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
				ot_file << rt.x << " " << rt.y <<" "<<rt.width<<" "<<rt.height<< endl;
				rt.width = -1;
			}
			break;
			default:
				break;
			}
			resize(org, shw, Size(cols * scale, rows * scale));
			Rect rt2;
			rt2.x = rt.x * scale;
			rt2.y = rt.y * scale;
			rt2.width = rt.width * scale;
			rt2.height = rt.height * scale;
			rectangle(shw, rt2, Scalar(255, 255, 255));
		}
		destroyAllWindows();
		ot_file.close();
	}

}



string get_folder(string file)
{
	int i = file.find_last_of('\\');
	string res = file.substr(0, i);
	return res;
}


