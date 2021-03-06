#ifndef DEMO_H
#define DEMO_H
#include "../app.h"

class Demo : public Application
{
private:
	static std::string Source;

public:
	explicit Demo(Application::Desc* Conf);
	virtual ~Demo() override;
	void WindowEvent(WindowState State, int X, int Y) override;
	void ScriptHook(VMGlobal* Global) override;
	void Initialize(Application::Desc* Conf) override;
	void Publish(Timer* Time) override;
	void Update(Timer* Time);
	void Simulation(Timer* Time);
	void Synchronize(Timer* Time);

public:
	static void SetSource(const std::string& Resource);
	static std::string& GetSource();
};
#endif