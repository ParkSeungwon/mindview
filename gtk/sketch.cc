#include<gtkmm.h>
#include<fstream>
#include<iostream>
#include"gtk/drawable.h"
#include"gtk/sketch.h"
#include"src/graph.h"
using namespace std;

SketchBook::SketchBook()
{
	set_size_request(5000, 5000);
}

void SketchBook::set_drawables(Drawable** begin, Drawable** end) 
{
	begin_ = begin; end_ = end;
}

bool SketchBook::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) 
{
	for(auto** it = begin_; it != end_; it++) (**it)(cr);
	return true;
}

void SketchBook::refresh()
{
	auto win = get_window();
    if (win) {
        Gdk::Rectangle r(0, 0,
				get_allocation().get_width(), get_allocation().get_height());
        win->invalidate_rect(r, false);
    }
}


GraphSketch::GraphSketch(string root, int x, int y, Gtk::Window& parent)
	: root_position_{x, y}, root_{root}, parent_{parent}
{
	add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
	string s = getenv("HOME");
	ifstream f(s + "/.mindmap");
	f >> gv_;
	if(!gv_.find_vertex(root_)) gv_.insert_vertex(root_);
	for(auto* v = gv_.data(); v; v = v->vertex) //sync with file existence
		if(!File::exist(v->data.data.full_path) &&
				v->data.data.type != File::Type::Virtual)
			gv_.remove_vertex(v->data.data);
	repaint();
}

GraphSketch::~GraphSketch()
{
	if(!readonly_) {
		string s = getenv("HOME");
		ofstream f(s + "/.mindmap");
		f << gv_;
	}
}

void GraphSketch::readonly(bool ro) 
{
	readonly_ = ro;
}

void GraphSketch::repaint()
{
	auto dr = gv_.generate_drawables(root_, root_position_);
	set_drawables(dr.first, dr.second);
	refresh();
}
	
bool GraphSketch::on_button_release_event(GdkEventButton* e)
{
	if(e->button == 1) {
		to_ = {e->x, e->y};
		auto* v = gv_.pick(from_);
		if(!v || abs(from_ - to_) < 10) return true;
		File f = v->data.data;
		if(f == root_) root_position_ = to_;
		else {
			auto parent = gv_.find_parent(f);
			for(auto* e = gv_.find_vertex(parent)->edge; e; e = e->edge)
				if(e->vertex->data.data == f) e->weight += to_ - from_;
		}
		repaint();
	}
	return true;//does not propagate
}

void app_chooser(File f);
bool GraphSketch::on_button_press_event(GdkEventButton* e)
{
	auto* v = gv_.pick({e->x, e->y});
	if(e->button == 1) {
		from_ = {e->x, e->y};
		if(e->type == GDK_2BUTTON_PRESS && v &&
				v->data.data.type != File::Type::Virtual)
			app_chooser(v->data.data);
	} else if(e->button == 3 && v) {
		popup(v->data.data);
		repaint();
	}
	return true;//does not propagate
}

bool Win::on_button_press_event(GdkEventButton* e)
{
	cout << e->x << ' ' << e->y << endl;
	return true;
}

Win::Win(string s, int x, int y) : sketch_{s, x, y, *this}
{
	add(scwin_);
	scwin_.add(sketch_);
	set_default_size(1000, 900);
	show_all_children();
}

