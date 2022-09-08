#pragma once
#include<vector>
#include<memory>
#include"gtk/graphview.h"

class SketchBook : public Gtk::DrawingArea
{
public:
	SketchBook();
	void set_drawables(Drawable** begin, Drawable** end);

protected:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
	void refresh();
	Drawable **begin_, **end_;
};

class GraphSketch : public SketchBook
{
public:
	GraphSketch(std::string root, int x, int y, Gtk::Window& parent);
	~GraphSketch();
	void readonly(bool);

protected:
	GraphView<File> gv_;
	Gtk::Window& parent_;
	bool on_button_press_event(GdkEventButton* e);
	bool on_button_release_event(GdkEventButton* e);
	void popup(File f);	

private:
	Point from_, to_, root_position_;
	File root_;
	bool readonly_ = false;
	void shape_chooser(File f);
	void virtual_chooser(File f);
	void color_chooser(File f);
	void file_chooser(File f);
	void font_chooser(File f);
	void resize(File f);
	void repaint();
};

class Win : public Gtk::Window
{
public:
	Win(std::string s, int x, int y);
	GraphSketch sketch_;

protected:
	//bool on_button_press_event(GdkEventButton* e);
	Gtk::ScrolledWindow scwin_;
};

