#include "../contrib/CLI11.hpp"
#include "../contrib/rang.hpp"
#include <growl/version.h>
#include <iostream>
#include <string>

using rang::style;
using std::cout;
using std::endl;

void bundleAssets(std::string directory, std::string output);

int main(int argc, char** argv) {
	CLI::App app{"growl-cmd"};
	app.set_help_all_flag("--help-all", "Show all help");

	CLI::App* assets =
		app.add_subcommand("assets", "Tools for dealing with binary assets");
	app.require_subcommand();
	CLI::App* bundle =
		assets->add_subcommand("bundle", "Bundle assets into a single file");
	assets->require_subcommand();

	std::string assets_dir;
	std::string output{"assets.dat"};
	bundle
		->add_option("directory", assets_dir, "The assets directory to bundle")
		->required()
		->check(CLI::ExistingDirectory);
	bundle->add_option("output", "The output file for the bundled assets");
	bundle->callback(
		[&assets_dir, &output] { bundleAssets(assets_dir, output); });

	cout << "┏━━━━━━━━━━┓" << endl;
	cout << "┃" << style::bold << " 🐻 GROWL " << style::reset << "┃ "
		 << style::bold << Growl_VERSION << style::reset << " "
		 << style::underline << "https://github.com/Bearwaves/growl"
		 << style::reset << endl;
	cout << "┗━━━━━━━━━━┛" << endl;
	CLI11_PARSE(app, argc, argv);
	return 0;
}
