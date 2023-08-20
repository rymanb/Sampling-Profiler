#include "Profiler.h"


#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

#include <DbgHelp.h>
#pragma comment ( lib, "dbghelp.lib" )

// max symbol name
static PSYMBOL_INFO GetSymbol(DWORD64 address, PSYMBOL_INFO buff) {
	PDWORD64 displacement = 0;
	PSYMBOL_INFO symbol = (PSYMBOL_INFO)buff;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	symbol->MaxNameLen = MAX_SYM_NAME;
	SymFromAddr(GetCurrentProcess(), address, displacement, symbol);
	return symbol;
}

// callback function for timer
void ProfilerCallback()
{
	// suspend thread
	DWORD64 eip = 0;
	if (SuspendThread(Profiler::Instance().mainThread) != -1)
	{
		// get eip
		CONTEXT context = { 0 };
		context.ContextFlags = WOW64_CONTEXT_i386 | CONTEXT_CONTROL;
		GetThreadContext(Profiler::Instance().mainThread, &context);

		eip = context.Rip;


		// resume thread
		if (ResumeThread(Profiler::Instance().mainThread) == -1)
		{
			std::cout << "fail";
		}
	}
	else
	{
		std::cout << "fail";
	}

	// increment stack
	Profiler::Instance().stack[eip]++;



}

Profiler& Profiler::Instance()
{
	static Profiler instance;
	return instance;
}

void Profiler::Init()
{


	// Initialize the symbol handler
	SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
	if (!SymInitialize(GetCurrentProcess(), NULL, true)) {
		return;
	}
}

void Profiler::Start()
{
	// init thread
	mainThread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION, 0, GetCurrentThreadId());

	timerId = timeSetEvent(1, 1, (LPTIMECALLBACK)ProfilerCallback, 0, TIME_PERIODIC | TIME_CALLBACK_FUNCTION | TIME_KILL_SYNCHRONOUS);
}


void Profiler::Stop()
{
	// stop timer
	timeKillEvent(timerId);
}

void Profiler::Exit()
{

	// for name of function
	std::map<std::string, int> timesInFunction;

	// open file
	FILE* file = fopen("ProfileReport.csv", "w");
	if (file == NULL) {
		return;
	}

	// print stack
	PSYMBOL_INFO symbol = (PSYMBOL_INFO)malloc(sizeof(SYMBOL_INFO) + (MAX_SYM_NAME + 1) * sizeof(char));

	for (auto& val : stack)
	{
		auto eip = val.first;

		auto sym = GetSymbol(eip, symbol);

		std::string name = sym->Name;


		timesInFunction[name] = val.second;
	}

	int total = 0;

	// order by number of times in function
	std::vector<std::pair<std::string, int>> ordered;
	for (auto& pair : timesInFunction) {
		ordered.push_back(pair);
		total += pair.second;
	}

	std::sort(ordered.begin(), ordered.end(), [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
		return a.second > b.second;
		});

	fprintf(file, "Function,Hit Count,Percentage\n");


	for (auto& pair : ordered)
	{
		std::string function = pair.first;
		int times = pair.second;
		fprintf(file, "\"%s\",%d,%f\n", function.c_str(), times, (((float)times * 100) / total));
	}

	fclose(file);

	// close symbol handler
	SymCleanup(GetCurrentProcess());

	// free symbol
	free(symbol);



}
