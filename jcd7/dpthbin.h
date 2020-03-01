#pragma once
#include<opencv.hpp>
#include<string>
#include<fstream>
#include<stdio.h>
namespace dbn
{
	float* read_depth(std::string fstr,int rows,int cols)
	{
		std::fstream fp;
		fp.open(fstr, std::ios::in | std::ios::binary);
		float* res = NULL;
		if (fp)
		{
			int n = rows * cols;
			res = new float[n];
			for (int i = 0; i < n; i++)
			{
				int16_t val;
				fp.read((char*)(&val), sizeof(int16_t));
				//printf("%d", val);
				int val2 = val / 2;
				res[i] = val2;
			}
			fp.close();
		}
		
		return res;
	}
	void write_depth(float* depth, int rows, int cols, std::string fstr)
	{
		std::fstream fp;
		fp.open(fstr, std::ios::out | std::ios::binary);
		int n = rows * cols;
		for (int i = 0; i < n; i++)
		{
			int16_t val = depth[i];
			val = val * 2;
			fp.write((char*)(&val), sizeof(int16_t));
		}
		fp.close();
	}
};