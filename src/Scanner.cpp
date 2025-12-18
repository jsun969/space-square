#include "Scanner.hpp"
#include <filesystem>
#include <fmt/core.h>
#include <queue>
#include <stdexcept>

namespace spsq {

namespace fs = std::filesystem;

std::uintmax_t Scanner::calcDirSize(const std::string& path) {
	std::uintmax_t size = 0;
	try {
		for (const auto& entry : fs::recursive_directory_iterator(
						 path, fs::directory_options::skip_permission_denied)) {
			try {
				if (entry.is_regular_file()) {
					size += entry.file_size();
				}
			} catch (const fs::filesystem_error&) {
				// Skip files we can't access
				// TODO
			}
		}
	} catch (const fs::filesystem_error&) {
		// Can't access directory at all
		// TODO
	}
	return size;
}

void Scanner::propagateDirSizes(File& file) {
	if (file.type != FileType::Directory) {
		return;
	}
	if (file.children.empty()) {
		// Directory file but no children
		file.size_bytes = calcDirSize(file.path);
		return;
	}
	std::uintmax_t size = 0;
	for (auto& child : file.children) {
		propagateDirSizes(child);
		size += child.size_bytes;
	}
	file.size_bytes = size;
}

File Scanner::scan(std::string rootPath, int depth) {
	fs::path path(rootPath);

	if (!fs::exists(path)) {
		throw std::runtime_error("Path does not exist: " + rootPath);
	}
	if (!fs::is_directory(path)) {
		throw std::runtime_error("Path is not a directory: " + rootPath);
	}

	File root;
	root.name = path.filename().string();
	if (root.name.empty()) root.name = path.string();
	root.path = fs::absolute(path).string();
	root.type = FileType::Directory;
	root.size_bytes = 0;

	// BFS scan directories
	std::queue<File*> q;
	q.push(&root);

	for (int i = 0; i < depth; ++i) {
		int levelSize = q.size();
		if (levelSize == 0) break;

		while (levelSize > 0) {
			File* currentFile = q.front();
			q.pop();
			levelSize--;

			fs::path currentPath(currentFile->path);
			try {
				for (const auto& entry : fs::directory_iterator(currentPath,
																												fs::directory_options::skip_permission_denied)) {
					try {
						File child;
						child.name = entry.path().filename().string();
						child.path = fs::absolute(entry.path()).string();

						if (entry.is_directory()) {
							child.type = FileType::Directory;
							child.size_bytes = 0;
						} else if (entry.is_regular_file()) {
							child.type = FileType::File;
							child.size_bytes = entry.file_size();
						} else {
							child.type = FileType::Other;
							child.size_bytes = 0;
						}

						currentFile->children.push_back(std::move(child));
					} catch (const fs::filesystem_error&) {
						// Skip files we can't access
						// TODO
					}
				}
			} catch (const fs::filesystem_error&) {
				// Can't access directory at all
				// TODO
			}

			for (auto& child : currentFile->children) {
				if (child.type == FileType::Directory) {
					q.push(&child);
				}
			}
		}
	}

	// DEBUG
	// TODO: Print files size (maybe path)
	fmt::println("Finished scanning up to depth {}", depth);
	// DEBUG
	propagateDirSizes(root); // TODO: Use du?

	return root;
}

} // namespace spsq
