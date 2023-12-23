// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_CommandMenu_Tasks
#define _H_CommandMenu_Tasks

static const JUtf8Byte* kTasksMenuStr =
"* %i __PerformOneOffTask::CommandMenu"
"|* %i __CustomizeThisMenu::CommandMenu %l"
"|* %i __AddToProjectNone::CommandMenu"
"|* %i __ManageProjectNone::CommandMenu %l"
;

enum {
	kRunCmd=1,
	kEditCmd,
	kAddToProjIndex,
	kManageProjIndex,
};


static void ConfigureTasksMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts(JString("#t", JString::kNoCopy));
	}
};

#endif