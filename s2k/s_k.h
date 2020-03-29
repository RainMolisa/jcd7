#pragma once
namespace s_k
{
	int ps = 7;
	int n = ps * ps;
	float k2s(float k,float th)
	{
		float fn = n;
		float s = (fn / th - k) / (fn - k);
		return (1-s);
	}
}
