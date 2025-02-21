#include "open_phpmyadmin.h"

void OpenPhpMyAdmin()
{
	ShellExecute(0, "open", "chrome.exe", "http://localhost/phpmyadmin", NULL, SW_SHOWNORMAL);
}