#include "Scanner.hpp"
#include "fmt/base.h"
#include <cxxopts.hpp>
#include <fmt/ranges.h>
#include <functional>

int main(int argc, char* argv[]) {
	using namespace spsq;

	// Parse command line arguments
	cxxopts::Options options("spsq", "View your space in squares");

	options.add_options()("path", "Path to scan", cxxopts::value<std::string>()->default_value(".")) //
			("d,depth", "Depth of directory traversal", cxxopts::value<int>()->default_value("3"))			 //
			("h,help", "Print help information");

	options.parse_positional({ "path" });
	options.positional_help("[path]");

	auto result = options.parse(argc, argv);

	if (result.count("help")) {
		fmt::println("{}", options.help());
		return 0;
	}

	std::string rootPath = result["path"].as<std::string>();
	int depth = result["depth"].as<int>();

	if (depth < 1 || depth > 9) {
		fmt::println(stderr, "Error: Depth must be between 1 and 9");
		return 1;
	}

	// Scan the directory
	Scanner scanner;
	File files;
	try {
		files = scanner.scan(rootPath, depth);
	} catch (const std::runtime_error& e) {
		fmt::println(stderr, "Error: {}", e.what());
		return 1;
	}
	// DEBUG
	std::function<void(const File&, int)> printFile = [&](const File& file, int indent = 0) {
		std::string indentStr(indent * 2, ' ');
		auto fileTypeStr = file.type == FileType::Directory ? "Directory" : file.type == FileType::File ? "File"
																																																		: "Other";
		fmt::print("{}- {} ({} bytes) [{}]\n", indentStr, file.name, file.size_bytes, fileTypeStr);
		for (const auto& child : file.children) {
			printFile(child, indent + 1);
		}
	};
	printFile(files, 0);
	// DEBUG
	return 0;
}
