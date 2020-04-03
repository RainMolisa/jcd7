#include <fstream>
#include <vector>
#include "..\jcd7\offset2d.h"

using namespace std;
using namespace cv;
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
	//
	string res_pth = wrk_pth + "\\res";
	string oin_pth = wrk_pth + "\\in_other";
	system(("mkdir " + oin_pth).c_str());
	system(("rd /Q /S " + res_pth).c_str());
	system(("mkdir " + res_pth).c_str());
	//
	int n = 2 * search_box + 1;
	float* ost = new float[n];
	for (int i = -search_box; i <= search_box; i++)
	{
		int j = i + search_box;
		ost[j] = i*16;
	}
	float* dpth = fs2d::offset2depth(ost,n,1, fxy, baseline, wall);

	fstream fs1;
	fs1.open(res_pth+"\\offset2dpth.txt",ios::out);
	for (int i = 0; i < n; i++)
	{
		fs1 << ost[i]/16.0 << " " <<dpth[i]<< endl;
	}
	fs1.close();
	fstream fs2;
	fs2.open(res_pth + "\\function.txt", ios::out);
	fs2<< fs2d::o2d_function(fxy, baseline, wall) <<endl;
	fs2.close();
	delete[] ost;
	delete[] dpth;
	return 0;
}


