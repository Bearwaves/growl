#include "../thirdparty/CLI11.hpp"
#include "../thirdparty/rang.hpp"
#include "growl/version.h"
#include <iostream>
#include <string>

using rang::style;
using std::cout;
using std::endl;

void bundleAssets(std::string directory, std::string output);
void listAssets(std::string assets_bundle);

int main(int argc, char** argv) {
	CLI::App app{"growl-cmd"};
	app.set_help_all_flag("--help-all", "Show all help");
	app.require_subcommand();

	CLI::App* assets =
		app.add_subcommand("assets", "Tools for dealing with binary assets");
	assets->require_subcommand();

	CLI::App* bundle =
		assets->add_subcommand("bundle", "Bundle assets into a single file");
	std::string assets_dir;
	std::string assets_bundle{"assets.growl"};
	bundle
		->add_option("directory", assets_dir, "The assets directory to bundle")
		->required()
		->check(CLI::ExistingDirectory);
	bundle->add_option("output", "The output file for the bundled assets");
	bundle->callback([&assets_dir, &assets_bundle] {
		bundleAssets(assets_dir, assets_bundle);
	});

	CLI::App* list_assets =
		assets->add_subcommand("list", "List assets in a bundle");
	list_assets
		->add_option("bundle", assets_bundle, "The path to the assets bundle")
		->required()
		->check(CLI::ExistingFile);
	list_assets->callback([&assets_bundle] { listAssets(assets_bundle); });

	cout << "┏━━━━━━━━━━┓" << endl;
	cout << "┃" << style::bold << " 🐻 GROWL " << style::reset << "┃ "
		 << style::bold << GROWL_VERSION << style::reset << " "
		 << style::underline << "https://github.com/Bearwaves/growl"
		 << style::reset << endl;
	cout << "┗━━━━━━━━━━┛" << endl;
	CLI11_PARSE(app, argc, argv);
	return 0;
}
