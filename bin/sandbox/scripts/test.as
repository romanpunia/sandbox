#pragma name("test-script")
#pragma enable("all")

bool test_bool = false;
int8 test_int8 = 1;
int16 test_int16 = 2;
int test_int = 3;
int64 test_int64 = 4;
uint8 test_uint8 = 5;
uint16 test_uint16 = 6;
uint test_uint = 7;
uint64 test_uint64 = 8;
float test_float = 9.0f;
double test_double = 10.0;
string test_string = "hello, world!";
console@ debug = console();

void write_test_int()
{
	debug.writeLine(test_string + ": " + formatInt(test_int));
}
void hide_console()
{
	debug.hide();
}
void show_console()
{
	debug.show();
}