// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_MemoryDir_Actions
#define _H_MemoryDir_Actions

static const JUtf8Byte* kActionsMenuStr =
"* %i __SaveWindowSizeAsDefault::MemoryDir"
;

enum {
	kSavePrefsCmd=1,
};


static void ConfigureActionsMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts(JString("#a", JString::kNoCopy));
	}
};

#endif