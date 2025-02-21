#include "delete_cache_client.h"

namespace fs = std::filesystem;

void DeleteCacheCef(const string& folderCacheCef)
{
	try
	{
		if (fs::exists(folderCacheCef) && fs::is_directory(folderCacheCef))
		{
			for (const auto& entry : fs::directory_iterator(folderCacheCef))
			{
				fs::remove_all(entry.path());
			}
		}
	}
	catch (const fs::filesystem_error& e)
	{
		cerr << "Error: " << e.what() << endl;
	}
}