// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_SymbolDirector_Preferences
#define _H_SymbolDirector_Preferences

static const JUtf8Byte* kPreferencesMenuStr =
"* %i __Symbols::SymbolDirector"
"|* %i __ToolbarButtons::SymbolDirector"
"|* %i __FileTypes::SymbolDirector"
"|* %i __ExternalEditors::SymbolDirector"
"|* %i __FileManagerWebBrowser::SymbolDirector"
"|* %i __Miscellaneous::SymbolDirector %l"
"|* %i __SaveWindowSizeAsDefault::SymbolDirector"
;

#include "SymbolDirector-Preferences-enum.h"


static void ConfigurePreferencesMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts("#p");
	}
};

#endif
