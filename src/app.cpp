#include "core/sandbox.h"
#include "core/demo.h"

int main()
{
	Mavi::Initialize((uint64_t)Mavi::Preset::Game);
Entry:
	{
		Application::Desc Init;
		Init.GraphicsDevice.VSyncMode = VSync::On;
		Init.Activity.Maximized = true;
		Init.Activity.Title = "Sandbox";
		Init.Directory = "content";
#ifdef _DEBUG
		Init.GraphicsDevice.Debug = true;
#else
		Init.GraphicsDevice.Debug = false;
#endif
		Application* App = new Sandbox(&Init);
		App->Start();
		VI_CLEAR(App);

		if (Demo::GetSource().empty())
		{
			Demo::GetSource().~basic_string();
			goto Exit;
		}

		App = new Demo(&Init);
		App->Start();
		VI_CLEAR(App);

		goto Entry;
	}
Exit:
	Mavi::Uninitialize();

	return 0;
}