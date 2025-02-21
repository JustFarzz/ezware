#include "cef_update_procedure.h"

namespace fs = filesystem;

void CefProcedur(const std::string& source, const std::string& destination)
{
	try {
		if (!fs::exists(source) || !fs::is_directory(source))
		{
			cerr << "Source folder tidak ditemukan!" << endl;
			return;
		}

		fs::create_directories(destination);

		for (const auto& entry : fs::directory_iterator(source))
		{
			const auto& path = entry.path();
			auto destPath = destination + "\\" + path.filename().string();

			cout << "Copying: " << path.string() << " -> " << destPath << endl;

			if (fs::is_directory(path))
			{
				fs::create_directories(destPath);
				fs::copy(path, destPath, fs::copy_options::recursive);
			}
			else
			{
				fs::copy(path, destPath);
			}
		}
	}
	catch (const fs::filesystem_error& e)
	{
		cerr << "Error: " << e.what() << endl;
	}
}