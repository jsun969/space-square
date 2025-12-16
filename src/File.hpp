#pragma once

#include <string>
#include <vector>

namespace spsq {

enum class FileType {
	Directory,
	File,
	Other,
};

struct File {
	std::string name;
	std::string path;
	int size_bytes;
	FileType type;
	std::vector<File> children;
};

} // namespace spsq
