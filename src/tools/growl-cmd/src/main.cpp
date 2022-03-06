#include "../contrib/CLI11.hpp"
#include <string>

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
	bundle
		->add_option("directory", assets_dir, "The assets directory to bundle")
		->required()
		->check(CLI::ExistingDirectory);

	CLI11_PARSE(app, argc, argv);
	return 0;
}
