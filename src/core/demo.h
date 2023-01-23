#ifndef DEMO_H
#define DEMO_H
#include "../core/globals.h"

class Demo : public Application
{
private:
	static std::string Source;

public:
	explicit Demo(Application::Desc* Conf);
	virtual ~Demo() override;
	void WindowEvent(WindowState State, int X, int Y) override;
	void Initialize() override;
	void Dispatch(Timer* Time) override;
	void Publish(Timer* Time) override;

public:
	static void SetSource(const std::string& Resource);
	static std::string& GetSource();
};
#endif