#pragma once
#include<sstream>
#include"gtk/drawable.h"
#include"src/nodeexpr.h"
#include"src/graph.h"
#define YIELD_POINT 30.0

template<class T> class GraphView : public Graph<NodeExpr<T>>
{//to show graph in gui
public:
	using G = Graph<NodeExpr<T>>;
	using V = Vertex<NodeExpr<T>>;
	~GraphView() {
		clear_drawables();
	}
	void set_show(T n, bool show) {
		auto* v = G::find_vertex(n);//n will change to NodeExpr
		v->data.show = show;//NodeExpr == only consider data
	}
	void toggle_show(T n) {
		auto* v = G::find_vertex(n);
		v->data.show = !v->data.show;
	}
	void set_shrink(T n, bool shrink) {
		auto* v = G::find_vertex(n);
		v->data.shrink = shrink;
	}
	void toggle_shrink(T n) {
		auto* v = G::find_vertex(n);
		v->data.shrink = !v->data.shrink;
	}
	void set_font(T n, std::string font_desc) {
		auto* v = G::find_vertex(n);
		v->data.font_desc = font_desc;
	}
	void set_outline(T n, OutlineShape outline) {
		auto* v = G::find_vertex(n);
		v->data.outline = outline;
	}
	OutlineShape get_outline(T n) {
		auto* v = G::find_vertex(n);
		return v->data.outline;
	}
	void set_arrowline(T n, ArrowShape arrowline) {
		auto* v = G::find_vertex(n);
		v->data.arrowline = arrowline;
	}
	ArrowShape get_arrowline(T n) {
		auto* v = G::find_vertex(n);
		return v->data.arrowline;
	}
	void set_width_height(T n, int w, int h) {
		auto* v = G::find_vertex(n);
		v->data.width = w;
		v->data.height = h;
	}
	int get_width(T n) {
		auto* v = G::find_vertex(n);
		return v->data.width;
	}
	int get_height(T n) {
		auto* v = G::find_vertex(n);
		return v->data.height;
	}
	void set_tooltip(T n, std::string s) {
		auto* v = G::find_vertex(n);
		v->data.tooltip = s;
	}
	void set_picture_file(T n, std::string s) {
		auto* v = G::find_vertex(n);
		v->data.picture_file = s;
	}
	void set_text_color(T n, std::array<unsigned char, 4> rgba) {
		auto* v = G::find_vertex(n);
		for(int i=0; i<4; i++) v->data.color[0][i] = rgba[i];
	}
	void set_arrow_color(T n, std::array<unsigned char, 4> rgba) {
		auto* v = G::find_vertex(n);
		for(int i=0; i<4; i++) v->data.color[1][i] = rgba[i];
	}
	void set_outline_color(T n, std::array<unsigned char, 4> rgba) {
		auto* v = G::find_vertex(n);
		for(int i=0; i<4; i++) v->data.color[2][i] = rgba[i];
	}

	std::pair<Drawable**, Drawable**> generate_drawables(T from, Point root_position) {
		auto* v = G::find_vertex(from);
		if(!v) return {nullptr, nullptr};
		clear_drawables();
		vpNpos.clear();
		generate_drawables(v, root_position);//defined in private.recursive generation.
		G::clearv();
		return {drawables_.data(), drawables_.data() + drawables_.size()};
	}
	V* pick(Point p) {
		for(auto a : vpNpos) if(abs(a.second - p) < 20) return a.first;
		return nullptr;
	}
	void set_path_find_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
		path_find_color_[0] = r;
		path_find_color_[1] = g;
		path_find_color_[2] = b;
		path_find_color_[3] = a;
	}


protected:
	std::map<V*, Point> vpNpos;//vertex pointer & absolute drawing position of that vertex 
	std::vector<Drawable*> drawables_;
	std::array<unsigned char, 4> path_find_color_= {255,0,0,255};

private:
	void clear_drawables() {
		for(auto* a : drawables_) delete a;
		drawables_.clear();
	}

	void generate_drawables(V* v, Point p) {
		vpNpos[v] = p;
		Point s = {p.x() - v->data.width / 2, p.y() - v->data.height / 2};
		Point e = {p.x() + v->data.width / 2, p.y() + v->data.height / 2};
		Drawable* pdraw;
		
		//inner data to text
		std::stringstream ss;
		ss << v->data.data;
		pdraw = new Text{s, e, ss.str(), v->data.font_desc};
		set_rgba(pdraw, v, 0);
		drawables_.push_back(pdraw);

		//outline
		switch(v->data.outline) {
		case OutlineShape::Rect: 	pdraw = new Box{s, e}; break;
		case OutlineShape::Picture: pdraw = new Pix{v->data.picture_file, s, e}; break;
		case OutlineShape::Diamond: pdraw = new Diamond{s, e}; break;
		case OutlineShape::Ellipse: pdraw = new Ellipse{s, e}; break;
		}
		set_rgba(pdraw, v, 2);
		drawables_.push_back(pdraw);
		
		//arrow and next node
		if(v->data.shrink) return;
		for(auto* e = v->edge; e; e = e->edge) {
			if(e->v < 2) {//if not visited or checked by dijkstra or etc
				e->v |= 2;//check visit. 1 is used by other functions to check shortest
				if(!e->vertex->data.show) continue;
				auto uv = YIELD_POINT * e->weight / abs(e->weight);//unit vector
				if(e->vertex->data.arrowline == ArrowShape::Direct) 
					pdraw = new Arrow{p + uv, p + e->weight - uv};
				else pdraw = new Curve{p + uv.real(), p + e->weight - uv.real()};
				if(e->v & 1) pdraw->set_rgba(path_find_color_[0] / 255., path_find_color_[1] / 255., path_find_color_[2] / 255., path_find_color_[3] / 255.);
				else set_rgba(pdraw, e->vertex, 1);
				drawables_.push_back(pdraw);
				generate_drawables(e->vertex, p + e->weight);//weight, point = complex
			}
		}
	}

	void set_rgba(Drawable* d, V* v, int k) {
		d->set_rgba(v->data.color[k][0] / 255., v->data.color[k][1] / 255.,
					v->data.color[k][2] / 255., v->data.color[k][3] / 255.);
	}
};
