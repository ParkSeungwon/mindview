#include<fstream>
#include<experimental/filesystem>
#include"src/option.h"
#include"src/nodeexpr.h"
#include"graphview.h"
using namespace std;

int main(int ac, char **av)
{// move file  to_dir, 1 : option wrong, 2 : file not specified, 3 : specify move or ren
	char home[100];
	strcpy(home, getenv("HOME"));
	strcat(home, "/.mindmap");
	CMDoption co{
		{"file", "file to move or rename", "default"},
		{"to", "destination directory", "default"},
		{"rename", "new filename", "default"},
		{"cfg", "mindmap config file", (const char*)home}
	};
	if(!co.args(ac, av)) return 1;
	string file{co.get<const char*>("file")}, to{co.get<const char*>("to")},
		rename{co.get<const char*>("rename")}, cfg{co.get<const char*>("cfg")};
	if(file == "default") return 2;
	if((to == "default") == (rename == "default")) return 3;

	GraphView<File> gr;
	{
		ifstream f{cfg};
		f >> gr;
	}
	
	namespace fs = experimental::filesystem;
	File fl{file};
	if(to != "default") { 
		File fto{to};
		try {
			auto *p = gr.find_vertex(fl);
			File from = gr.find_parent(fl);//throws, if this line passes, do insert edge
			fs::path fn = fs::path{to} /= fs::path{fl.full_path}.filename();
			gr.remove_edge(from, fl);
			fs::rename(fl.full_path, fn);
			gr.insert_edge(File{to}, fl, {30, 30});
			if(p) p->data.data = File{fn};
			gr.sub_apply(p->data, [to](NodeExpr<File> &f) { f.data.full_path =
					fs::path{to} /= fs::path{f.data.full_path}.filename(); });
		} catch(const char *e) {
			cerr << e << endl;
			return 4;
		}
	} else {
		File fren{rename};
		auto *p = gr.find_vertex(fl);//move next line
		fs::rename(p->data.data.full_path, rename);
		if(p) p->data.data = File(rename);//return error code check
	}

	ofstream f{cfg};
	f << gr;
}

