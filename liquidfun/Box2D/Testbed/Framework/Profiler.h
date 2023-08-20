#pragma once


#include<Windows.h>
#include<mmsystem.h>
#include<map>

class Profiler
{
public:
	static Profiler& Instance();

	void Init();

	void Start();

	void Stop();

	void Exit();


	HANDLE mainThread;
	std::map<DWORD64, int> stack;


	// timer id
	MMRESULT timerId;
};
