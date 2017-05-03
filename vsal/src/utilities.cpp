#include "vsal/utilities.h"

#include <cctype>

// Boost
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

using namespace boost::filesystem;

const std::string SPECIAL_CHARACTERS = ".[{}()*+?|^$";

namespace vsal
{
	bool is_image(const std::string& path)
	{
		if (!is_regular_file(path)) return false;

		// Get extension
		boost::filesystem::path fs_path(path);
		std::string ext = fs_path.extension().string();
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

		boost::regex filter(IMAGE_FILTER);
		boost::smatch what;
		return boost::regex_match(ext, what, filter);
	}

	bool is_pattern(const std::string& path)
	{
		for (const char& c : SPECIAL_CHARACTERS)
			if (path.find(c) != std::string::npos) return true;
		return false;
	}

	bool is_valid_pattern(const std::string& path)
	{
		if (path.empty()) return false;
		boost::filesystem::path fs_path(path);
		std::string parent_path = fs_path.parent_path().string();
		std::string filename = fs_path.filename().string();
		return (!is_pattern(parent_path) && is_pattern(filename));
	}
}   // namespace vsal
