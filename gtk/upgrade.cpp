#include<fstream>
#include<experimental/filesystem>
#include"gtk/graphview.h"
using namespace std;
using namespace std::experimental::filesystem;

GraphView<File> gv;
void read(istream& is, File parent) {
	for(string filename; get_line(is, filename);) {
		cout << filename << " 처리중 ..." <<  endl;
		int show, x, y, outline, arrowline, type, w, h, color[3][4];
		is >> show >> x >> y >> outline >> arrowline >> type >> w >> h;
		w = max(w, 50);
		h = max(h, 30);
		for(int i=0; i<3; i++) for(int j=0; j<4; j++) is >> color[i][j];
		if(!show) continue;
		File f;
		f.full_path = parent.full_path + '/' + filename;
		f.name = filename;
		f.type = type == 8 ? File::Type::File : File::Type::Directory;
		gv.insert_vertex(f);
		gv.insert_edge(parent, f, {x, y});
		gv.set_show(f, show);
		gv.set_outline(f, static_cast<OutlineShape>(outline));
		if(outline == 0) gv.set_picture_file(f, f.full_path);
		gv.set_arrowline(f, static_cast<ArrowShape>(arrowline));
		gv.set_width_height(f, w, h);
		array<unsigned char, 4> col[3];
		for(int i=0; i<3; i++) for(int j=0; j<4; j++) col[i][j] = color[i][j];
		gv.set_text_color(f, col[0]);
		gv.set_arrow_color(f, col[1]);
		gv.set_outline_color(f, col[2]);
		if(is_directory(f.full_path)) {
			cout << "entering " << f.full_path << endl;
			ifstream mi(f.full_path + "/.mindmap");
			read(mi, f);
		}
	}
}

int main(int ac, char** av)
{
	if(ac < 2) return 0;
	string s = av[1];
	File file;
	file.full_path = canonical(s).string();
	file.name = canonical(s).filename();
	file.type = File::Type::Directory;
	{
		ifstream f("mindmap");
		f >> gv;
	}
	ifstream f(s + "/.mindmap");
	gv.insert_vertex(file);
	read(f, file);
	ofstream f2("/home/zezeon/.mindmap");
	f2 << gv;
}


