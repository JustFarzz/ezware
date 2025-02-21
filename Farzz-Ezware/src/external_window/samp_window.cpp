#include <Windows.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <ShlObj.h>

#include "samp_window.h"
#include "../ezware/ezware.h"

#include "procedure/delete_cache_client.h"
#include "procedure/open_phpmyadmin.h"
#include "procedure/cef_update_procedure.h"
#include "procedure/import_maps.h"

//#include "../icons/IconsFontAwesome5.h"
#include "../icons/IconsFontAwesome6.h"

#include "monitoring/cpu_process.h"
#include "monitoring/memory_process.h"

using namespace std;

EzWare ezWare;

DirectorySettings dirSettings;
bool showSettings = false;

bool BrowseFolder(char* path)
{
	BROWSEINFO bi = { 0 };
	bi.lpszTitle = "Pilih Folder";
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (pidl != NULL)
	{
		SHGetPathFromIDList(pidl, path);
		return true;
	}
	return false;
}

void ShowSettingsWindow(bool* show)
{
	if (*show)
	{
		ImGui::SetNextWindowFocus();
		ImGui::SetNextWindowSize(ImVec2(800, 300));

		ImGui::Begin("Settings", show, 
			ImGuiWindowFlags_NoResize);

		const float inputWidth = 600.0f;
		const float buttonWidth = 40.0f;
		const float spacing = 10.0f;

		SettingsField fields[] =
		{
			{"RP GM", dirSettings.folderGm1, "##1"},
			{"MAP GM", dirSettings.folderGm2, "##2"},
			{"CLIENT", dirSettings.folderClient, "##3"},
			{"CEF Cache", dirSettings.folderCacheCef, "##4"},
			{"Source CEF", dirSettings.folderSourceCEF1, "##5"},
			{"Dest CEF", dirSettings.folderDestinationCEF1, "##6"},
			{"Import Map", dirSettings.folderDestinationMap, "##7"},
			{"Dest Map", dirSettings.folderSourceMap, "##8"}
		};


		ImGui::Text("Set Direktori");

		for (const auto& field : fields)
		{
			ImGui::Text("%s", field.label);
			ImGui::SameLine(120);

			ImGui::PushItemWidth(inputWidth);
			ImGui::InputText(field.id, field.path, MAX_PATH, ImGuiInputTextFlags_ReadOnly);
			ImGui::PopItemWidth();

			ImGui::SameLine();
			if (ImGui::Button(("..." + string(field.id)).c_str(), ImVec2(buttonWidth, 0)))
			{
				BrowseFolder(field.path);
			}
		}
		ImGui::End();
	}
}

void Window_SAMP(bool* show)
{
	if (*show)
	{
		ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.1f, 0.1f, 0.4f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.1f, 0.1f, 0.4f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.1f, 0.1f, 0.16f, 1.0f));

		ImGui::SetNextWindowSize(ImVec2(700, 450));

		ImGui::Begin("Samp Project", show, 
			ImGuiWindowFlags_NoResize);

		const char* buttons[] =
		{
			"RP GM", "MAP GM", "CLIENT",
			"DEL CAC", "MYADMIN", "UPDT CEF",
		};

		float startY = 43.0f;
		float buttonSpacing = 68.0f;
		ImVec2 buttonSize(100, 50);

		for (int i = 0; i < 6; i++)
		{
			ImGui::SetCursorPos(ImVec2(50, startY + i * buttonSpacing));

			if (ImGui::Button(buttons[i], buttonSize))
			{
				switch (i)
				{
					case 0: ShellExecute(0, "open", dirSettings.folderGm1, NULL, NULL, SW_SHOWDEFAULT); break;
					case 1: ShellExecute(0, "open", dirSettings.folderGm2, NULL, NULL, SW_SHOWDEFAULT); break;
					case 2: ShellExecute(0, "open", dirSettings.folderClient, NULL, NULL, SW_SHOWDEFAULT); break;
					case 3: DeleteCacheCef(dirSettings.folderCacheCef); break;
					case 4: OpenPhpMyAdmin(); break;
					case 5: CefProcedur(dirSettings.folderSourceCEF1, dirSettings.folderDestinationCEF1); break;
				}
			}

			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				{
					switch (i)
					{
						case 0:
						{
							ImGui::Text("Klik untuk membuka Roleplay Gamemode");
						}
						break;

						case 1:
						{
							ImGui::Text("Klik untuk membuka Mapping Gamemode");
						}
						break;

						case 2:
						{
							ImGui::Text("Klik untuk membuka Client SAMP");
						}
						break;

						case 3:
						{
							ImGui::Text("Klik untuk menghapus Cache CEF");
						}
						break;

						case 4:
						{
							ImGui::Text("Klik untuk membuka PHPMyAdmin");
						}
						break;

						case 5:
						{
							ImGui::Text("Klik untuk mengupdate CEF");
						}
						break;
					}
				}
				ImGui::EndTooltip();
			}
		}

		ImVec2 buttonMap = ImVec2(100, 50);
		ImVec2 windowSizeMap = ImGui::GetWindowSize();
		ImGui::SetCursorPos(ImVec2(
			windowSizeMap.x - buttonMap.x * 5.0f,
			windowSizeMap.y - buttonMap.y * 2.2f
		));

		if (ImGui::Button("IMP MAP", buttonMap))
		{
			ImportMapProcedure(dirSettings.folderSourceMap, dirSettings.folderDestinationMap);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Click untuk mengimport map");
			ImGui::EndTooltip();
		}


		ImVec2 buttonSizeSetting = ImVec2(40, 40);
		ImVec2 windowSizeSetting = ImGui::GetWindowSize();
		ImGui::SetCursorPos(ImVec2(
			windowSizeSetting.x - buttonSizeSetting.x * 1.4f,
			windowSizeSetting.y - buttonSizeSetting.y * 10.48f
		));

		ImGui::PushFont(ezWare.GetIconFont());
		if (ImGui::Button(ICON_FA_GEAR "", buttonSizeSetting))
		{
			showSettings = !showSettings;
		}

		ImGui::PopFont();
		ShowSettingsWindow(&showSettings);

//=========================================[ Monitoring CPU: Active ]=========================================//
		
		if (!g_monitorInitialized || !g_processMonitor.IsProcessRunning())
		{
			g_processMonitor.InitializeEmptyHistory();
			g_monitorInitialized = g_processMonitor.Initialize(L"gta_sa.exe", dirSettings.folderClient);
		}

		// CPU Monitoring
		if (g_monitorInitialized)
		{
			ImGui::SetCursorPos(ImVec2(200, 48.0f));
			float currentUsage = g_processMonitor.GetCPUUsage();
			const std::vector<float>& history = g_processMonitor.GetHistory();

			if (g_processMonitor.HasHistory())
			{
				if (g_processMonitor.IsProcessRunning())
				{
					ImGui::Text("SAMP CPU Usage: %.1f%%", currentUsage * 100.0f);
				}
				else
				{
					ImGui::Text("Process Stopped - Last Known CPU Usage: %.1f%%", currentUsage * 100.0f);
				}

				ImGui::SetCursorPos(ImVec2(200, 80.0f));

				ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));

				ImGui::PlotLines("##CPU",
					history.data(),
					static_cast<int>(history.size()),
					0,
					"SAMP CPU",
					0.0f,
					1.0f,
					ImVec2(400, 100));

				ImGui::PopStyleColor();

				if (g_processMonitor.IsProcessRunning())
				{
					Sleep(50);
				}
			}
			else
			{
				ImGui::Text("SAMP process not found");
			}
		}
		else
		{
			g_monitorInitialized = g_processMonitor.Initialize(L"gta_sa.exe", dirSettings.folderClient);
			ImGui::SetCursorPos(ImVec2(200, 48.0f));
			ImGui::Text("Initializing monitoring...");
		}

//=========================================[ Monitoring Memmory : Active ]=========================================//
		
		if (!g_memoryMonitorInitialized || !g_memoryMonitor.IsProcessRunning())
		{
			g_memoryMonitor.InitializeEmptyHistory();
			g_memoryMonitorInitialized = g_memoryMonitor.Initialize(L"gta_sa.exe", dirSettings.folderClient);
		}

		if (g_memoryMonitorInitialized)
		{
			ImGui::SetCursorPos(ImVec2(200, 200.0f));
			float currentMemoryUsage = g_memoryMonitor.GetMemoryUsage();
			const std::vector<float>& memHistory = g_memoryMonitor.GetMemoryHistory();
			SIZE_T peakMemory = g_memoryMonitor.GetPeakMemoryUsage();

			if (g_memoryMonitor.HasHistory())
			{
				if (g_memoryMonitor.IsProcessRunning())
				{
					ImGui::Text("SAMP Memory Usage: %.1f MB", currentMemoryUsage);
					/*ImGui::Text("Peak Memory Usage: %.1f MB",
						static_cast<float>(peakMemory) / (1024.0f * 1024.0f));*/
				}
				else
				{
					ImGui::Text("Process Stopped - Last Known Memory Usage: %.1f MB",
						currentMemoryUsage);
				}

				ImGui::SetCursorPos(ImVec2(200, 232.0f));

				ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 1.0f, 0.5f, 1.0f));

				float maxMemory = 0.0f;
				for (float mem : memHistory) {
					maxMemory = max(maxMemory, mem);
				}
				maxMemory = max(maxMemory, currentMemoryUsage);

				ImGui::PlotLines("##Memory",
					memHistory.data(),
					static_cast<int>(memHistory.size()),
					0,
					"SAMP Memory (MB)",
					0.0f,
					maxMemory * 1.2f,
					ImVec2(400, 100));

				ImGui::PopStyleColor();

				if (g_memoryMonitor.IsProcessRunning())
				{
					Sleep(50);
				}
			}
			else
			{
				ImGui::Text("Memory monitoring not available");
			}
		}
		
		ImGui::End();

		ImGui::PopStyleColor(3);
	}
}