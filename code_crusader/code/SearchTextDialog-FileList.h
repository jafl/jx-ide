// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_SearchTextDialog_FileList
#define _H_SearchTextDialog_FileList

static const JUtf8Byte* kOptionsMenuStr =
"* %b %i __ShowTextOfEachMatch::SearchTextDialog"
"|* %b %i __ListFilesThatDoNotMatch::SearchTextDialog %l"
"|* %i __AddFiles::SearchTextDialog"
"|* %i __RemoveSelectedFiles::SearchTextDialog"
"|* %i __RemoveAllFiles::SearchTextDialog %l"
"|* %i __LoadFileSet::SearchTextDialog"
"|* %i __SaveFileSet::SearchTextDialog"
;

#include "SearchTextDialog-FileList-enum.h"


static void ConfigureOptionsMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts("#o");
	}
};

#endif
