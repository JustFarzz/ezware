#include "import_maps.h"

void ImportMapProcedure(const char* sourceDir, const char* destDir)
{
    try
    {
        // Hapus semua file di folder tujuan kecuali ImportMaps.bat
        for (const auto& entry : fs::directory_iterator(destDir))
        {
            if (entry.is_regular_file())
            {
                std::string filename = entry.path().filename().string();
                if (filename != "ImportMaps.bat")
                {
                    fs::remove(entry.path());
                }
            }
        }

        // Salin semua file dari folder sumber kecuali ExportMaps.bat
        for (const auto& entry : fs::directory_iterator(sourceDir))
        {
            if (entry.is_regular_file())
            {
                std::string filename = entry.path().filename().string();
                if (filename != "ExportMaps.bat")
                {
                    fs::path destPath = fs::path(destDir) / filename;
                    fs::copy_file(entry.path(), destPath, fs::copy_options::overwrite_existing);
                }
            }
        }

        MessageBox(NULL, "File map berhasil diimpor!", "Berhasil", MB_ICONINFORMATION | MB_OK);
    }
    catch (const fs::filesystem_error& e)
    {
        std::string errorMsg = "Terjadi kesalahan saat impor: " + std::string(e.what());
        MessageBox(NULL, errorMsg.c_str(), "Error", MB_ICONERROR | MB_OK);
    }
    catch (const std::exception& e)
    {
        std::string errorMsg = "Kesalahan tidak terduga: " + std::string(e.what());
        MessageBox(NULL, errorMsg.c_str(), "Error", MB_ICONERROR | MB_OK);
    }
}