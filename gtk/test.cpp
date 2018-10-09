#include<gtkmm.h>
#include<fstream>
#include"gtk/sketch.h"
#include"gtk/graphview.h"
#include"src/nodeexpr.h"
using namespace std;
int main(int ac, char** av)
{//show basic usage
	GraphView<string> gr;
	string s[] = {"Dallas", "Houston", "Chicago", "Newyork", "San francisco", 
	"Los angeles", "Montana"};
	for(int i=0; i<7; i++) gr.insert_vertex(s[i]);
	gr.insert_edge(s[0], s[1], {100, 300});
	gr.insert_edge(s[1], s[2], {300, -900});
	gr.insert_edge(s[2], s[3], {300, 0});
	gr.insert_edge(s[2], s[4], {-1000, 200});
	gr.insert_edge(s[4], s[5], {200, 300});
	gr.insert_edge(s[2], s[6], {-700, -200});
	gr.set_width_height(s[0], 250, 230);
//	gr.set_outline(s[0], OutlineShape::Picture);
	gr.set_outline(s[2], OutlineShape::Diamond);
	gr.set_width_height(s[2], 60, 30);
	gr.set_arrow_color(s[2], {255, 0, 125, 150});
	gr.set_picture_file(s[0], "1.jpg");
	gr.set_outline_color(s[0], {0, 255,0,255});
	gr.dijkstra(s[0], s[4]);
	gr.set_path_find_color(255, 0, 255, 255);
	auto b = gr.generate_drawables(s[0], {800, 1200});

	auto app = Gtk::Application::create(ac, av);
	Gtk::Window win;
	Gtk::ScrolledWindow sc;
	SketchBook sketch;
	win.add(sc);
	sc.add(sketch);
	sketch.set_drawables(b.first, b.second);
	win.set_default_size(700, 700);
	win.show_all_children();
	return app->run(win);
}

