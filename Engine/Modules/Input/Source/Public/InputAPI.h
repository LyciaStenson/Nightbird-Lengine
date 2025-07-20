#pragma once

//#include <string>
//#include <functional>
//#include <unordered_map>
//#include <deque>
//#include <bitset>
//
//#include <GLFW/glfw3.h>
//
//#ifdef INPUT_BUILD
//#define INPUT_API __declspec(dllexport)
//#else
//#define INPUT_API __declspec(dllimport)
//#endif
//
//extern "C"
//{
//	typedef void (*ActionCallback)(const char* actionName);
//	
//	INPUT_API void Input_Init(struct GLFWwindow* window);
//	INPUT_API void Input_Shutdown();
//
//	INPUT_API void Input_BindKey(const char* action, int key);
//	INPUT_API void Input_BindMouseButton(const char* action, int button);
//
//	INPUT_API void Input_SubscribeActionPressed(const char* action, ActionCallback callback);
//	INPUT_API void Input_SubscribeActionReleased(const char* action, ActionCallback callback);
//
//	INPUT_API void Input_ProcessEvents();
//}