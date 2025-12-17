#include "Scanner.hpp"
#include "fmt/base.h"
#include <cxxopts.hpp>
#include <fmt/ranges.h>
#include <functional>

int main(int argc, char* argv[]) {
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

	// Scan the directory
	spsq::Scanner scanner;
	auto files = scanner.scan(rootPath, depth);
	// DEBUG
	std::function<void(const spsq::File&, int)> printFile = [&](const spsq::File& file, int indent = 0) {
		std::string indentStr(indent * 2, ' ');
		auto fileTypeStr = file.type == spsq::FileType::Directory ? "Directory" : file.type == spsq::FileType::File ? "File"
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
