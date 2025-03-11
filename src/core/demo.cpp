#include "demo.h"

demo::demo(heavy_application::desc* conf) : heavy_application(conf)
{
}
demo::~demo()
{
}
void demo::window_event(window_state new_state, int x, int y)
{
	switch (new_state)
	{
		case window_state::resize:
			renderer->resize_buffers((unsigned int)x, (unsigned int)y);
			if (scene != nullptr)
				scene->resize_buffers();
			break;
		case window_state::close:
			activity->message.setup(alert_type::warning, "Demo", "Do you want to go back to sandbox?");
			activity->message.button(alert_confirm::escape, "No", 1);
			activity->message.button(alert_confirm::defer, "Yes", 2);
			activity->message.result([this](int button)
			{
				if (button == 2)
					stop();
			});
			break;
		default:
			break;
	}
}
void demo::initialize()
{
	auto new_scene = content->load<scene_graph>(source);
	if (!new_scene)
		return stop();

	scene = *new_scene;
	scene->get_camera();
}
void demo::dispatch(timer* time)
{
	scene->dispatch(time);
}
void demo::publish(timer* time)
{
	renderer->clear(0, 0, 0);
	renderer->clear_depth();

	scene->publish(time);
	scene->submit();

	renderer->submit();
}
void demo::set_source(const string& resource)
{
	source = resource;
}
string& demo::get_source()
{
	return source;
}
string demo::source;