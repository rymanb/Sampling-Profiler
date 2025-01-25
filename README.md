### Implementation Details:  
- Implementation exists in Profiler.h and Profiler.cpp in liquidfun/Box2D/Testbed/Framework
- Profiler is a Sampling profiler that every 1ms will grap the rip from the main thread and store it and the amount of times it has been called in a map.
- I chose 1ms because it is a good balance between accuracy and performance. If it was too low it would be too accurate but would have a large performance hit. If it was too high it would be less accurate but would have a smaller performance hit. I ran some tests and found that 1ms was a good balance.
- Once the profiler is done profiling it will output the data to a file called "ProfileReport.csv" which contains the function name, the amount of times it was called, and the percentage of time it took up.
- to get the filename from the rip I used the function "SymFromAddr"
- to handle pause and resume of main thread i store the handle to the main thread and then use SuspendThread and ResumeThread to pause and resume the main thread.
- to handle calling the profiler every 1ms i used the function timeSetEvent which takes in a callback function, the time in ms, and a user data pointer. The callback function is called every 1ms and updates the profiler. Upon exiting the program the callback function is stopped by calling timeKillEvent.


### Integration Details:  
- Include the Profiler.h and Profiler.cpp from liquidfun/Box2D/Testbed/Framework to the project you want to profile.
- Include "Profiler.h" to your main file
- Call the Profiler::Init() function to initialize the profiler.
- Call the Profiler::Start() function to start the profiler.
- Call your game loop.
- Call the Profiler::Stop() function to stop the profiler.
- Call the Profiler::Exit() function to shutdown the profiler.
- example:
```cpp
#include "Profiler.h"
static void Run()
{
	 Profiler::Instance().Init();

	 Profiler::Instance().Start();

	RunMainLoop();

	 Profiler::Instance().Stop();

	 Profiler::Instance().Exit();
}
```
   
### Requirements  
- Add profiler.h and profiler.cpp to the project you want to profile.
- include profiler.h in the file you want to profile.
- call the above functions in integration details to start and stop the profiler.
- only works on windows.

### Output  
- upon program exit it outputs a csv file called "ProfileReport.csv" in the directory as your exe which contains the function name, the amount of times it was called, and the percentage of time it took up.


</br>  

* * * * * 

<img src="liquidfun/Box2D/Documentation/Programmers-Guide/html/liquidfun-logo-square-small.png"
alt="LiquidFun logo" style="float:right;" />

LiquidFun Version [1.1.0][]


