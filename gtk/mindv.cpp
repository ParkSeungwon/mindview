#include<gtkmm.h>
#include"gtk/sketch.h"
#include"gtk/graphview.h"
#include"src/graph.h"
#include"src/nodeexpr.h"
#include"src/option.h"
using namespace std;

GraphView<File> GV;

int main(int ac, char** av)
{
	CMDoption co{
		{"directory", "base directory to show", "."},
		{"x", "root x position", 500},
		{"y", "root y position", 500},
		{"readonly", "readonly - does not write to savefile .mindmap", false}
	};
	if(!co.args(ac, av)) return 0;
	int i = 0;
	auto app = Gtk::Application::create(i, av);
	Win win{co.get<const char*>("directory"), co.get<int>("x"), co.get<int>("y")};
	if(co.get<bool>("readonly")) win.sketch_.readonly(true);
//	win.sketch_.set_drawables(a.first, a.second);
	win.set_title("File Browser inspired by MindMap");
	return app->run(win);
}


