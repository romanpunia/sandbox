#include "core/sandbox.h"
#include "core/demo.h"

int main()
{
	Vitex::HeavyRuntime Scope;
Entry:
	{
		HeavyApplication::Desc Init;
		Init.GraphicsDevice.VSyncMode = VSync::On;
		Init.Activity.Maximized = true;
		Init.Activity.Title = "Sandbox";
		Init.Directory = "content";
#ifdef _DEBUG
		Init.GraphicsDevice.Debug = true;
#else
		Init.GraphicsDevice.Debug = false;
#endif
		HeavyApplication* App = new Sandbox(&Init);
		App->Start();
		Memory::Release(App);

		if (Demo::GetSource().empty())
		{
			Demo::GetSource().~basic_string();
			goto Exit;
		}

		App = new Demo(&Init);
		App->Start();
		Memory::Release(App);

		goto Entry;
	}
Exit:
	return 0;
}