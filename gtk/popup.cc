#include<functional>
#include<chrono>
#include<experimental/filesystem>
#include"gtk/popup.h"
#include"gtk/sketch.h"
using namespace std;
using namespace Gtk;

VirtualDialog::VirtualDialog() 
{
	set_size_request(300, 200);
	vb = get_content_area();
	vb->pack_start(textview);
	add_button("_Ok", 1);
	add_button("_Cancel", 0);
	show_all_children();
}

AttributeDialog::AttributeDialog() : Picture(outline, "picture"), 
	Rect(outline, "rect"), Diamond(outline, "diamond"), Ellipse(outline, "ellipse"), 
	Plant(line, "plant"), Direct(line, "direct"), sl("Shape"), al("Arrow"),
	arrow(Gtk::Adjustment::create(1, 0, 255, 1, 10, 0)),
	shape(Gtk::Adjustment::create(1, 0, 255, 1, 10, 0))
{
	set_size_request(100,100);
	vb = get_content_area();
	vb->pack_start(name);
	vb->pack_start(hb1);
	vb->pack_start(hb2);
	vb->pack_start(hb3);
	vb->pack_start(hb4);
	hb1.pack_start(Picture, PACK_SHRINK);
	hb1.pack_start(Rect, PACK_SHRINK);
	hb1.pack_start(Diamond, PACK_SHRINK);
	hb1.pack_start(Ellipse, PACK_SHRINK);
	hb2.pack_start(Plant, PACK_SHRINK);
	hb2.pack_start(Direct, PACK_SHRINK);
	hb3.pack_start(al, PACK_SHRINK);
	hb3.pack_start(arrow);
	hb4.pack_start(sl, PACK_SHRINK);
	hb4.pack_start(shape);
	add_button("_Ok", 1);
	add_button("_Cancel", 0);
	outline_bts[0] = &Picture;
	outline_bts[1] = &Rect;
	outline_bts[2] = &Diamond;
	outline_bts[3] = &Ellipse;
	line_bts[0] = &Plant;
	line_bts[1] = &Direct;
	show_all_children();
}

ResizeDialog::ResizeDialog() : fixed_ratio("fix rate"), wl("width"), hl("height"),
	width(Gtk::Adjustment::create(1, 0, 1000, 1, 10, 0)), 
	height(Gtk::Adjustment::create(1, 0, 1000, 1, 10, 0))
{
	set_size_request(100, 100);
	Gtk::Box* box = get_content_area();
	box->pack_start(fixed_ratio, PACK_SHRINK);
	whb.pack_start(wl, PACK_SHRINK);
	whb.pack_end(width, PACK_SHRINK);
	hhb.pack_start(hl, PACK_SHRINK);
	hhb.pack_end(height, PACK_SHRINK);
	box->pack_start(whb, PACK_SHRINK);
	box->pack_start(hhb, PACK_SHRINK);
	width.set_numeric();
	height.set_numeric();
	width.signal_value_changed().connect(bind(&ResizeDialog::on_change, this, 'w'));
	height.signal_value_changed().connect(bind(&ResizeDialog::on_change, this, 'h'));
	fixed_ratio.signal_toggled().connect(bind(&ResizeDialog::on_toggle, this));
	add_button("_Ok", 1);
	add_button("_Cancel", 0);
	show_all_children();
}

void ResizeDialog::on_change(char c) 
{
	if(fixed_ratio.get_active()) {
		switch(c) {
			case 'w': height.set_value(width.get_value() / rate); break;
			case 'h': width.set_value(rate * height.get_value()); break;
		}
	}
}

void ResizeDialog::on_toggle()
{
	if(fixed_ratio.get_active()) rate = width.get_value() / height.get_value();
}
		

void GraphSketch::shape_chooser(File f) {
	AttributeDialog ad;
	ad.set_transient_for(parent_);
	ad.name.set_text(f.name);
	ad.outline_bts[static_cast<int>(gv_.get_outline(f))]->set_active();
	ad.line_bts[static_cast<int>(gv_.get_arrowline(f))]->set_active();

	auto* v = gv_.find_vertex(f);//used pointer for inner access
	ad.arrow.set_value(v->data.color[1][3]);//a
	ad.shape.set_value(v->data.color[2][3]);
	if(ad.run()) {
		for(int i=0; i<5; i++) if(ad.outline_bts[i]->get_active()) {
			gv_.set_outline(f, static_cast<OutlineShape>(i));
			if(!i) gv_.set_picture_file(f, f.full_path);
			break;
		}
		for(int i=0; i<2; i++) if(ad.line_bts[i]->get_active()) {
			gv_.set_arrowline(f, static_cast<ArrowShape>(i));
			break;
		}
		v->data.color[1][3] = ad.arrow.get_value();//arrow rbg'a'
		v->data.color[2][3] = ad.shape.get_value();//outline shape a
		bool same_name = false;
		if(f.name != ad.name.get_text())//if changed & no same name 
			v->data.data.name = ad.name.get_text();//name is just outlook
	}
}

void GraphSketch::virtual_chooser(File f)
{
	VirtualDialog vd;
	vd.set_transient_for(parent_);
	if(vd.run()) {
		File f2; stringstream ss;
		ss << "virtual" << chrono::system_clock::now().time_since_epoch().count();
		f2.full_path = f.full_path.substr(0, f.full_path.rfind('/') + 1) + ss.str();
		f2.name = vd.textview.get_buffer()->get_text();
		f2.type = File::Type::Virtual;
		gv_.insert_vertex(f2);
		gv_.insert_edge(f, f2, {50, 50});
		gv_.set_outline_color(f2, {0,0,0,0});
		gv_.set_outline(f2, OutlineShape::Rect);
	}
}

void GraphSketch::color_chooser(File f) {
	int result;
	{
		Gtk::Dialog dia;
		dia.set_transient_for(parent_);
		dia.add_button("_Text",1);
		dia.add_button("_Arrow",2);
		dia.add_button("_Shape",3);
		dia.add_button("Cancel",0);
		result = dia.run();
	}
	if(0 < result && result < 4) {
		Gtk::ColorChooserDialog dia;
		dia.set_transient_for(parent_);
		if(dia.run() == Gtk::RESPONSE_OK) {
			auto color = dia.get_rgba();
			array<unsigned char, 4> ar = {
				color.get_red() * 255, color.get_green() * 255, 
				color.get_blue() * 255, color.get_alpha() * 255};
			switch(result) {
				case 1: gv_.set_text_color(f, ar); break;
				case 2: gv_.set_arrow_color(f, ar); break;
				case 3: gv_.set_outline_color(f, ar); break;
			}
		}
	}
}

void app_chooser(File f) {
	const char* p[] = {"h", "hpp", "cc", "cpp", "c", "py", "txt", "js", "html", "css"
					, "tex"};
	string command, ext = f.full_path.substr(f.full_path.rfind('.') + 1);
	for(auto *a : p) if(ext == a) command = "gvim --remote-tab-silent '";
	if(f.name == "Makefile") command = "gvim --remote-tab-silent '";
	if(command == "") command = "python -c 'import webbrowser,sys; webbrowser.open(sys.argv[1])' '";
	command += f.full_path + "'";//for spaced file name
	system(command.data());
}

void GraphSketch::file_chooser(File f) {
	Gtk::FileChooserDialog dia("Choose files to expose or unexpose");
	dia.set_transient_for(parent_);
	dia.set_select_multiple();
	dia.set_current_folder(f.full_path);
	dia.add_button("_Ok", 1);
	dia.add_button("_Cancel", 0);
	if(dia.run() == 1) {
		string s = dia.get_filename();
		File f2{s};
		bool edge_exist = false;
		if(auto v2 = gv_.find_vertex(f2)) {//vertex exists
			auto* v = gv_.find_vertex(f);
			for(auto* e = v->edge; e; e = e->edge)
				if(e->vertex == v2) edge_exist = true;//edge exists
			if(edge_exist) gv_.toggle_show(f2);
			else {
				gv_.insert_edge(f, f2, {50, 50});
				gv_.set_show(f2, true);
			}
		} else {//if new
			gv_.insert_vertex(f2);
			gv_.insert_edge(f, f2, {50, 50});
			gv_.set_width_height(f2, f2.name.size() * 10, 30);
			if(f2.type == File::Type::File) 
				gv_.set_outline(f2, OutlineShape::Rect);
		}
	}
}

void GraphSketch::font_chooser(File f) {
	Gtk::FontChooserDialog dia("Choose font");
	dia.set_transient_for(parent_);
	dia.set_preview_text("Alice in the 원더랜드!!");
	auto* v = gv_.find_vertex(f);
	dia.set_font(v->data.font_desc);
	int result = dia.run();
	if(result == Gtk::RESPONSE_OK) gv_.set_font(f, dia.get_font());
}

void GraphSketch::resize(File f) {
	ResizeDialog dia;
	dia.set_transient_for(parent_);
	dia.width.set_value(gv_.get_width(f));
	dia.height.set_value(gv_.get_height(f));
	if(dia.run()) gv_.set_width_height(f,
			dia.width.get_value_as_int(), dia.height.get_value_as_int());
}

void GraphSketch::popup(File f) {
	int result;
	{
		Gtk::Dialog dia("Selelct one");
		dia.set_transient_for(parent_);
		dia.add_button("_Shape",1);
		dia.add_button("_Color",2);
		dia.add_button("_Font", 9);
		if(f.type != File::Type::Virtual) dia.add_button("_Open",3);
		dia.add_button("_Resize",4);
		if(f.type == File::Type::Directory) dia.add_button("_Expose", 5);
		if(f.type != File::Type::Virtual) {
			dia.add_button("_Virtual", 6);
			dia.add_button("Shrin_K", 8);
		} else dia.add_button("_Delete", 7);
		dia.add_button("Cu_T", 10);
		if(cut_.full_path != "" && f.type == File::Type::Directory)
			dia.add_button("_Paste", 11);
		result = dia.run();
	}

	switch(result) {
	case 1: shape_chooser(f); break;
	case 2: color_chooser(f); break;
	case 3: app_chooser(f); break;
	case 4: resize(f); break;
	case 5: file_chooser(f); break;
	case 6: virtual_chooser(f); break;
	case 7: gv_.remove_vertex(f); break;
	case 8: gv_.toggle_shrink(f); break;
	case 9: font_chooser(f); break;
	case 10: cut_ = f; break;
	case 11: {
		gv_.move(cut_, f);
		gv_.sub_apply(cut_, [cut = this->cut_, f](NodeExpr<File> &n) { 
				size_t i = n.data.full_path.rfind('/' + cut.name + '/' + n.data.name);
				if(i != string::npos) n.data.full_path.replace(0, i, f.full_path);
			});
		auto *pv = gv_.find_vertex(cut_);
		string path = pv->data.data.full_path = f.full_path + '/' + cut_.name;
		cout << cut_.full_path << endl << path << endl;
		experimental::filesystem::rename(cut_.full_path, path);
		cut_.full_path = ""; 
		}
	default: ;
	}
}
