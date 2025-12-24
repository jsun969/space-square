#pragma once

#include <cstdint>
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
	std::uintmax_t sizeBytes;
	FileType type;
	std::vector<File> children;
};

} // namespace spsq
