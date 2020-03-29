#include<fstream>
#include<iostream>
#include"s_k.h"
using namespace std;
int main(int argc, char** argv)
{
	float s=s_k::k2s(5, 1.03125);
	cout << "s="<<s<<" mn=" <<s*255<< endl;
	return 0;
}