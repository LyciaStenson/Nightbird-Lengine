//#include <Input.h>
//#include <InputInternal.h>
//
//using namespace Nightbird;
//
//extern "C"
//{
//	void Input_Init(struct GLFWwindow* window)
//	{
//		Input::Get().Init(window);
//	}
//
//	void Input_Shutdown()
//	{
//		Input::Shutdown();
//	}
//
//	void Input_BindKey(const char* action, int key)
//	{
//		Input::Get().BindKey(action, key);
//	}
//
//	void Input_BindMouseButton(const char* action, int button)
//	{
//		Input::Get().BindMouseButton(action, button);
//	}
//	
//	void Input_SubscribeActionPressed(const char* action, ActionCallback callback)
//	{
//		Input::Get().SubscribeActionPressed(action, callback);
//	}
//	
//	void Input_SubscribeActionReleased(const char* action, ActionCallback callback)
//	{
//		Input::Get().SubscribeActionReleased(action, callback);
//	}
//	
//	void Input_ProcessEvents()
//	{
//		Input::Get().ProcessEvents();
//	}
//}