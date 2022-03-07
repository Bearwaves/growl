#include "../contrib/rang.hpp"
#include <iostream>
#include <string>

using rang::style;
using std::cout;
using std::endl;

void bundleAssets(std::string assets_dir, std::string output) {
	cout << "Building assets in " << style::bold << assets_dir << style::reset
		 << " to " << style::bold << output << style::reset << "." << endl;
}
