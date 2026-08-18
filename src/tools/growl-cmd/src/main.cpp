#include "../thirdparty/CLI11.hpp"
#include "../thirdparty/rang.hpp"
#include "growl/core/error.h"
#include "growl/version.h"
#include <iostream>
#include <string>

using rang::fg;
using rang::style;
using std::cout;
using std::endl;

void bundleAssets(std::string directory, std::string output, bool no_dev);
void listAssets(std::string assets_bundle);
Growl::Error createShader(
	std::string shader_name, std::string assets_dir, bool fragment, bool vertex,
	bool uniforms);

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
	bool no_dev;
	bundle->add_flag("--no-dev", no_dev, "Disable developer debug info");
	bundle->callback([&assets_dir, &assets_bundle, &no_dev] {
		bundleAssets(assets_dir, assets_bundle, no_dev);
	});

	CLI::App* list_assets =
		assets->add_subcommand("list", "List assets in a bundle");
	list_assets
		->add_option("bundle", assets_bundle, "The path to the assets bundle")
		->required()
		->check(CLI::ExistingFile);
	list_assets->callback([&assets_bundle] { listAssets(assets_bundle); });

	CLI::App* shaders =
		app.add_subcommand("shaders", "Tool for working with shaders");
	shaders->require_subcommand();

	std::string shader_name;
	CLI::App* shaders_new =
		shaders->add_subcommand("new", "Create new shader pack");
	shaders_new->add_option("name", shader_name, "Shader pack name")
		->required();
	shaders_new
		->add_option(
			"directory", assets_dir, "The directory to place the new shader")
		->required()
		->check(CLI::ExistingDirectory);
	bool fragment{false};
	bool vertex{false};
	bool uniforms{false};
	shaders_new->add_flag(
		"-f,--fragment", fragment, "Whether to include fragment shaders");
	shaders_new->add_flag(
		"-v,--vertex", vertex, "Whether to include vertex shaders");
	shaders_new->add_flag(
		"-u,--uniforms", uniforms, "Whether to include custom uniforms");
	shaders_new->callback(
		[&shader_name, &assets_dir, &fragment, &vertex, &uniforms] {
			if (auto err = createShader(
					shader_name, assets_dir, fragment, vertex, uniforms)) {
				cout << fg::red << "Failed to create shader: " << err->message()
					 << style::reset << endl;
				exit(1);
			}
		});

	cout << "┏━━━━━━━━━━┓" << endl;
	cout << "┃" << style::bold << " 🐻 GROWL " << style::reset << "┃ "
		 << style::bold << GROWL_VERSION << style::reset << " "
		 << style::underline << "https://github.com/Bearwaves/growl"
		 << style::reset << endl;
	cout << "┗━━━━━━━━━━┛" << endl;
	CLI11_PARSE(app, argc, argv);
	return 0;
}
