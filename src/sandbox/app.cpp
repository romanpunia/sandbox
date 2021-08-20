#include "app.h"
#include "core/sandbox.h"
#include "core/demo.h"

int main()
{
	Tomahawk::Initialize((uint64_t)Tomahawk::Preset::Game);
Entry:
	{
		Application::Desc Init;
		Init.GraphicsDevice.VSyncMode = VSync::None;
		Init.GraphicsDevice.Backend = RenderBackend::D3D11;
		Init.GraphicsDevice.Debug = true;
		Init.Activity.FreePosition = true;
		Init.Activity.Title = "Sandbox";
		Init.Activity.Hidden = true;
		Init.Directory = "sandbox";

		Application* App = new Sandbox(&Init);
		App->Start();
		TH_CLEAR(App);

		if (Demo::GetSource().empty())
		{
			Demo::GetSource().~basic_string();
			goto Exit;
		}

		Init.Activity.Title = "Demo";
		Init.Activity.Hidden = false;
		Init.Activity.Maximized = true;
		Init.Threads = 3;
		Init.Async = true;

		App = new Demo(&Init);
		App->Start();
		TH_CLEAR(App);

		goto Entry;
	}
Exit:
	Tomahawk::Uninitialize();

	return 0;
}