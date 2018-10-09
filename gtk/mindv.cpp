#include<gtkmm.h>
#include"gtk/sketch.h"
#include"gtk/graphview.h"
#include"src/graph.h"
#include"src/nodeexpr.h"
using namespace std;

GraphView<File> GV;

int main(int ac, char** av)
{
	int i = 0;
	string s = ac < 2 ? "." : av[1];
	int x = ac < 3 ? 500 : atoi(av[2]);
	int y = ac < 4 ? 500 : atoi(av[3]);
	auto app = Gtk::Application::create(i, av);
	Win win{s, x, y};
	if(ac > 4 && !strcmp(av[4], "ro")) win.sketch_.readonly(true);
//	win.sketch_.set_drawables(a.first, a.second);
	win.set_title("File Browser inspired by MindMap");
	return app->run(win);
}


