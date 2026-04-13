#include "EditorApplication.h"

#include "Core/SceneObject.h"

int main(int argc, char** argv)
{
	Nightbird::Editor::EditorApplication editorApp;
	return editorApp.Run(argc, argv);
}
