// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_TreeDirector_File
#define _H_TreeDirector_File

static const JUtf8Byte* kFileMenuStr =
"* %i NewTextFile::TreeDirector"
"|* %i NewTextFileFromTmpl::TreeDirector"
"|* %i NewProject::TreeDirector %l"
"|* %i OpenSomething::TreeDirector"
"|* %i __RecentProjects::TreeDirector"
"|* %i __RecentTextFiles::TreeDirector %l"
"|* %i PageSetup::JX"
"|* %i Print::JX"
"|* %i PrintTreeEPS::TreeDirector %l"
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
	kPSPageSetupCmd,
	kPrintPSCmd,
	kPrintEPSCmd,
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
#ifndef _H_jx_af_image_jx_jx_file_print
#define _H_jx_af_image_jx_jx_file_print
#include <jx-af/image/jx/jx_file_print.xpm>
#endif

static void ConfigureFileMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts(JString("#f", JString::kNoCopy));
	}
	menu->SetItemImage(kNewTextEditorCmd + offset, jx_file_new);
	menu->SetItemImage(kOpenSomethingCmd + offset, jx_file_open);
	menu->SetItemImage(kPrintPSCmd + offset, jx_file_print);
};

#endif