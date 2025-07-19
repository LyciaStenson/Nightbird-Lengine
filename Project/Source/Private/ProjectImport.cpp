#include <ProjectImport.h>

#include <iostream>

#include <GetInputSystem.h>

#include <rttr/registration.h>

void ProjectInit()
{
	GetInputSystem();
}

RTTR_REGISTRATION
{
	rttr::registration::method("ProjectInit", &ProjectInit);
}