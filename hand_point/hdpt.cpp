#include<string>
#include<iostream>
#include<opencv.hpp>
#include "..\jcd7\dpthbin.h"
#include "..\jcd7\pseudocolor.h"
using namespace cv;
using namespace std;
float scale = 1;
const int rows = 1280;
const int cols = 800;
fstream ot_file;
string get_folder(string file);
void OnMouse(int event, int x, int y, int flags, void* param)
{
	float* p = (float*)param;
	switch (event)
	{
	case EVENT_LBUTTONDOWN:
	{
		int x2 = x / scale;
		int y2 = y / scale;
		ot_file << x2 << " " << y2 << endl;
	}
	break;
	case EVENT_MOUSEMOVE:
	{
		int x2 = x / scale;
		int y2 = y / scale;
		float val = p[y2 * cols + x2];
		system("cls");
		printf("%04d,%04d,%f\n", x2, y2, val);
		break;
	}
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
		string outFile = folder_an + "\\point_list.txt";
		cout << outFile << endl;
		ot_file.open(outFile, ios::out);
		float* in_depth = dbn::read_depth(fpath, rows, cols);
		Mat org = psd2::pseudocolor(in_depth, rows, cols);
		Mat shw = org.clone();
		namedWindow("mouse", WINDOW_AUTOSIZE);
		setMouseCallback("mouse", OnMouse, in_depth);
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
			default:
				break;
			}
			resize(org, shw, Size(cols * scale, rows * scale));
		}
		destroyAllWindows();
		delete[]in_depth;
		ot_file.close();
	}

}

string get_folder(string file)
{
	int i = file.find_last_of('\\');
	string res = file.substr(0, i);
	return res;
}


