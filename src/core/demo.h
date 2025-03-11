#ifndef DEMO_H
#define DEMO_H
#include "../core/globals.h"

class demo : public heavy_application
{
private:
	static string source;

public:
	explicit demo(heavy_application::desc* conf);
	~demo() override;
	void window_event(window_state state, int x, int y) override;
	void initialize() override;
	void dispatch(timer* time) override;
	void publish(timer* time) override;

public:
	static void set_source(const string& resource);
	static string& get_source();
};
#endif