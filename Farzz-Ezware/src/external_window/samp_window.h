//=========================================[ Membuka Window SAMP project ]=========================================//

#ifndef SAMP_PROJECT
#define SAMP_PROJECT

#include "../../include/imgui/imgui.h"

void Window_SAMP(bool* show);

struct DirectorySettings
{
	char folderGm1[MAX_PATH] = "D:\\SAMP_REALITY\\2. GM\\1_Santara_Roleplay";
	char folderGm2[MAX_PATH] = "D:\\SAMP_REALITY\\2. GM\\LocalHost\\TStudio Ryherd V.4.0";
	char folderClient[MAX_PATH] = "D:\\GAME\\GTA SA\\SAMP 2";
	char folderCacheCef[MAX_PATH] = "D:\\GAME\\GTA SA\\SAMP 2\\cef\\cache";
	char folderSourceCEF1[MAX_PATH] = "D:\\SAMP_REALITY\\2. GM\\1_Santara_Roleplay\\gamemodes\\CEF";
	char folderDestinationCEF1[MAX_PATH] = "C:\\xampp\\htdocs\\CEF";
	char folderSourceMap[MAX_PATH] = "D:\\SAMP_REALITY\\2. GM\\LocalHost\\Studio Ryherd V.4.0\\scriptfiles\\tstudio\\ExportMaps";
	char folderDestinationMap[MAX_PATH] = "D:\\SAMP_REALITY\\2. GM\\LocalHost\\TStudio Ryherd V.4.0\\scriptfiles\\tstudio\\ImportMaps";
};

struct SettingsField
{
	const char* label;
	char* path;
	const char* id;
};

#endif