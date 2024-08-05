#ifndef DEMO_H
#define DEMO_H
#include "../core/globals.h"

class Demo : public HeavyApplication
{
private:
	static String Source;

public:
	explicit Demo(HeavyApplication::Desc* Conf);
	~Demo() override;
	void WindowEvent(WindowState State, int X, int Y) override;
	void Initialize() override;
	void Dispatch(Timer* Time) override;
	void Publish(Timer* Time) override;

public:
	static void SetSource(const String& Resource);
	static String& GetSource();
};
#endif