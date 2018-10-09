#include<utility>
#include"gtk/drawable.h"
#define MUL 2
using namespace std;

void Drawable::line_width(int w) { line_width_ = w; }
/*********************
The commonly available font families are: Normal, Sans, Serif and Monospace. 

The available styles are:

Normal
	the font is upright.

Oblique
	the font is slanted, but in a roman style.

Italic
	the font is slanted in an italic style.

The available weights are:

Ultra-Light
	the ultralight weight (= 200)

Light
	the light weight (=300)

Normal
	the default weight (= 400)

Bold
	the bold weight (= 700)

Ultra-Bold
	the ultra-bold weight (= 800)

Heavy
	the heavy weight (= 900)

The available variants are:

Normal
	

Small-Caps
	

The available stretch styles are:

Ultra-Condensed
	the smallest width

Extra-Condensed
	

Condensed
	

Semi-Condensed
	

Normal
	the normal width

Semi-Expanded
	

Expanded
	

Extra-Expanded
	

Ultra-Expanded
	the widest width
*****************/
Text::Text(Point s, Point e, string txt, string font_desc) 
{
	start_ = s;
	end_ = e;
	txt_ = txt;
	font_desc_ = font_desc;
//	auto wh = e - s;
//	int sz = std::max(wh.real() / txt.size() * MUL, 12.);
//	font_.set_size(12 * 1024);
}
void Text::operator()(const Cairo::RefPtr<Cairo::Context>& cr) 
{
	cr->save();
	Point p{(start_ + end_) / 2.0};
	Pango::FontDescription font{font_desc_};
	if(txt_ != "") {
		cr->set_source_rgba(color[0], color[1], color[2], color[3]);
		auto layout = Pango::Layout::create(cr);
		layout->set_font_description(font);
		layout->set_text(txt_);
		int text_width, text_height;
		layout->get_pixel_size(text_width, text_height);
		cr->move_to(p.x() - text_width / 2, p.y() - text_height / 2);
		layout->show_in_cairo_context(cr);
	}
	cr->restore();
}
void Text::txt(string s) {
	txt_ = s;
}
void Text::set_font(string s)
{
	font_desc_ = s;
}


Line::Line(initializer_list<Point> li)
{
	points_ = li;
	start_ = points_[0];
	end_ = points_[1];
}
void Line::operator()(const Cairo::RefPtr<Cairo::Context>& cr) 
{
	cr->save();
	cr->set_source_rgba(color[0], color[1], color[2], color[3]);
	cr->set_line_width(line_width_*0.5);
	cr->move_to(points_[0].x(), points_[0].y());
	for(auto& a : points_) cr->line_to(a.x(), a.y());
	cr->stroke();
	cr->restore();
}

Arrow::Arrow(Point s, Point e, int tip) 
{
	start_ = s;
	end_ = e;
	points_.push_back(s);
	if(tip > 1) {
		auto angle = arg(e - s);
		auto im1 = polar(10.0, angle - M_PI / 8);
		auto im2 = polar(10.0, angle + M_PI / 8);
		points_.push_back(s + im1);
		points_.push_back(s + im2);
		points_.push_back(s);
	}
	points_.push_back(e);
	if(tip > 0) {
		auto angle = arg(s - e);
		auto im1 = polar(10.0, angle - M_PI / 8);
		auto im2 = polar(10.0, angle + M_PI / 8);
		points_.push_back(e + im1);
		points_.push_back(e + im2);
		points_.push_back(e);
	}
}

void Drawable::set_rgba(double r, double g, double b, double a) 
{
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;
}

Box::Box(Point s, Point e)
{
	start_ = s;
	end_ = e;
	Point a{s.x(), e.y()};
	Point b{e.x(), s.y()};
	points_.push_back(s);
	points_.push_back(a);
	points_.push_back(e);
	points_.push_back(b);
	points_.push_back(s);
}

Curve::Curve(Point s, Point e) : Box(s, e)
{ }

void Curve::operator()(const Cairo::RefPtr<Cairo::Context>& cr)
{
	cr->save();
	cr->set_source_rgba(color[0], color[1], color[2], color[3]);
	cr->set_line_width(line_width_ * 0.5);
	cr->move_to(points_[0].x(), points_[0].y());
	cr->curve_to(points_[3].x(), points_[3].y(), points_[1].x(), points_[1].y(),
			points_[2].x(), points_[2].y());
	cr->stroke();
	cr->restore();
}

Diamond::Diamond(Point s, Point e) 
{
	start_ = s;
	end_ = e;
	Point a{(s.x() + e.x())/2, s.y()};
	Point b{s.x(), (s.y() + e.y())/2};
	Point c{(s.x() + e.x())/2, e.y()};
	Point d{e.x(), (s.y() + e.y())/2};
	points_.push_back(a);
	points_.push_back(b);
	points_.push_back(c);
	points_.push_back(d);
	points_.push_back(a);
}

Ellipse::Ellipse(Point s, Point e)
{
	start_ = s;
	end_ = e;
}
void Ellipse::operator()(const Cairo::RefPtr<Cairo::Context>& cr)
{
	Point c{(start_ + end_) / 2.0};
	int w = abs(start_.x() - end_.x());
	int h = abs(start_.y() - end_.y());
	if(w && h) {
		cr->save();
		cr->set_source_rgba(color[0], color[1], color[2], color[3]);
		cr->set_line_width(line_width_);
		cr->begin_new_sub_path();//to avoid trailing line
		cr->translate(c.x(), c.y());
		cr->scale(1.0, (double)h/w);
		cr->arc(0, 0, w, 0, 2 * M_PI);
		cr->stroke();
		cr->restore();
	}
}

Pix::Pix(string file, Point s, Point e) : Box(s - Point{3,3}, e + Point{3,3})
{
	start = s; end = e;
	filename = file;
}

void Pix::operator()(const Cairo::RefPtr<Cairo::Context>& cr) 
{///limited gtk dependency inside of this function
	if(!initialized) {
		Point im = end - start;//width + i * height
		image = Gdk::Pixbuf::create_from_file(filename);
		w = im.x();
		h = im.y();
		image = image->scale_simple(w, h, Gdk::INTERP_BILINEAR);
		initialized = true;
	}
	Box::operator()(cr);
	Gdk::Cairo::set_source_pixbuf(cr, image, start.x(), start.y());//anchor source
	cr->rectangle(start.x(), start.y(), w, h);					//at position x, y
	cr->fill();
}
		
