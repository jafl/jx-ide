// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_SearchDocument_Match
#define _H_SearchDocument_Match

static const JUtf8Byte* kMatchMenuStr =
"* %i __FirstMatch::SearchDocument %l"
"|* %i __PreviousMatch::SearchDocument"
"|* %i __NextMatch::SearchDocument %l"
"|* %i __OpenSelectionToMatch::SearchDocument"
;

#include "SearchDocument-Match-enum.h"


static void ConfigureMatchMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts("#m");
	}
};

#endif
