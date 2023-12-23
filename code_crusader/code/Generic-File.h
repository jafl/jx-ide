// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_Generic_File
#define _H_Generic_File

static const JUtf8Byte* kFileMenuStr =
"* %i NewTextFile::Generic"
"|* %i NewTextFileFromTmpl::Generic"
"|* %i NewProject::Generic %l"
"|* %i OpenSomething::Generic"
"|* %i __RecentProjects::Generic"
"|* %i __RecentTextFiles::Generic %l"
"|* %i CloseWindow::JX"
"|* %i Quit::JX"
;

enum {
	kNewTextEditorCmd=1,
	kNewTextTemplateCmd,
	kNewProjectCmd,
	kOpenSomethingCmd,
	kRecentProjectMenuCmd,
	kRecentTextMenuCmd,
	kCloseCmd,
	kQuitCmd,
};

#ifndef _H_jx_af_image_jx_jx_file_new
#define _H_jx_af_image_jx_jx_file_new
#include <jx-af/image/jx/jx_file_new.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_file_open
#define _H_jx_af_image_jx_jx_file_open
#include <jx-af/image/jx/jx_file_open.xpm>
#endif

static void ConfigureFileMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts(JString("#f", JString::kNoCopy));
	}
	menu->SetItemImage(kNewTextEditorCmd + offset, jx_file_new);
	menu->SetItemImage(kOpenSomethingCmd + offset, jx_file_open);
};

#endif