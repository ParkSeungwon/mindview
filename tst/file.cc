#include<catch.hpp>
#include<experimental/filesystem>
#include<iostream>
using namespace std::experimental::filesystem;
using namespace std;

TEST_CASE("weak canonical") {
	path p{"tst/file.cc"};
	cout << (canonical(p.parent_path()) /= p.filename());
}

