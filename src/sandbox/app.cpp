#include "app.h"
#include "core/sandbox.h"
#include "core/demo.h"

int main()
{
	Tomahawk::Initialize();
Entry:
	{
		Application::Desc Init;
		Init.GraphicsDevice.VSyncMode = VSync_Disabled;
		Init.GraphicsDevice.Backend = RenderBackend_D3D11;
		Init.GraphicsDevice.Debug = true;
		Init.Activity.FreePosition = true;
		Init.Activity.Title = "Sandbox";
		Init.Activity.Hidden = true;
		Init.Directory = "sandbox";

		Application* App = new Sandbox(&Init, "./scenes/spot.xml");
		App->Run(&Init);
		delete App;

		if (Demo::GetSource().empty())
		{
			Demo::GetSource().~basic_string();
			goto Exit;
		}

		Init.Activity.Title = "Demo";
		Init.Activity.Hidden = false;
		Init.Activity.Maximized = true;
		Init.Directory = "sandbox";
		Init.Threading = EventWorkflow_Multithreaded;

		App = new Demo(&Init);
		App->Run(&Init);
		delete App;

		goto Entry;
	}
Exit:
	Tomahawk::Uninitialize();

	return 0;
}