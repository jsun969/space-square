#include "Scanner.hpp"
#include <filesystem>
#include <queue>
#include <stdexcept>

namespace spsq {

namespace fs = std::filesystem;

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
	root.size_bytes = 0; // FIXME: directory size can't be determined here

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
			for (const auto& entry : fs::directory_iterator(currentPath)) {
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
			}

			for (auto& child : currentFile->children) {
				if (child.type == FileType::Directory) {
					q.push(&child);
				}
			}
		}
	}

	return root;
}

} // namespace spsq
