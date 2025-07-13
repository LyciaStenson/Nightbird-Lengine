#pragma once

#ifdef _WIN32
	#ifdef PROJECT_BUILD
		#define NB_PROJECT __declspec(dllexport)
	#else
		#define NB_PROJECT __declspec(dllimport)
	#endif
#else
	#define NB_PROJECT
#endif