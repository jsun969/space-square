#include "Scanner.hpp"
#include "fmt/base.h"
#include <fmt/ranges.h>
#include <functional>

int main() {
	spsq::Scanner scanner;
	auto files = scanner.scan(".", 2);
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
	return 0;
}
