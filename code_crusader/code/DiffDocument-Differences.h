// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_DiffDocument_Differences
#define _H_DiffDocument_Differences

static const JUtf8Byte* kDifferencesMenuStr =
"* %i __FirstDifference::DiffDocumentx %l"
"|* %i __PreviousDifference::DiffDocumentx"
"|* %i __NextDifference::DiffDocumentx"
;

enum {
	kFirstDiffCmd=1,
	kPrevDiffCmd,
	kNextDiffCmd,
};


static void ConfigureDifferencesMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts(JString("#d", JString::kNoCopy));
	}
};

#endif