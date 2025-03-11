#include "core/sandbox.h"
#include "core/demo.h"

int main()
{
	vitex::heavy_runtime scope;
entry:
	{
		heavy_application::desc init;
		init.graphics_device.vsync_mode = vsync::on;
		init.activity.maximized = true;
		init.activity.title = "Sandbox";
		init.directory = "content";
#ifdef _DEBUG
		init.graphics_device.debug = true;
#else
		init.graphics_device.debug = false;
#endif
		heavy_application* app = new sandbox(&init);
		app->start();
		memory::release(app);

		if (demo::get_source().empty())
		{
			demo::get_source().~basic_string();
			goto exit;
		}

		app = new demo(&init);
		app->start();
		memory::release(app);

		goto entry;
	}
exit:
	return 0;
}