// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_TreeDirector_Preferences
#define _H_TreeDirector_Preferences

static const JUtf8Byte* kPreferencesMenuStr =
"* %i __Project::TreeDirector"
"|* %i __ToolbarButtons::TreeDirector"
"|* %i __FileTypes::TreeDirector"
"|* %i __ExternalEditors::TreeDirector"
"|* %i __FileManagerWebBrowser::TreeDirector"
"|* %i __Miscellaneous::TreeDirector %l"
"|* %i __SaveWindowSizeAsDefault::TreeDirector"
;

enum {
	kTreePrefsCmd=1,
	kToolBarPrefsCmd,
	kEditFileTypesCmd,
	kChooseExtEditorsCmd,
	kShowLocationPrefsCmd,
	kMiscPrefsCmd,
	kSaveWindSizeCmd,
};


static void ConfigurePreferencesMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts(JString("#p", JString::kNoCopy));
	}
};

#endif