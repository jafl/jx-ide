// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_SourceDirector_Preferences
#define _H_SourceDirector_Preferences

static const JUtf8Byte* kPreferencesMenuStr =
"* %i __Preferences::SourceDirector"
"|* %i __ToolbarButtons::SourceDirector"
"|* %i __CustomCommands::SourceDirector"
"|* %i __MacWinXEmulation::SourceDirector %l"
"|* %i __SaveWindowSizeAsDefault::SourceDirector"
;

#include "SourceDirector-Preferences-enum.h"


static void ConfigurePreferencesMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts("#p");
	}
};

#endif
