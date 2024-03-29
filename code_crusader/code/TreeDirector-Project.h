// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_TreeDirector_Project
#define _H_TreeDirector_Project

static const JUtf8Byte* kProjectMenuStr =
"* %i ShowSymbolBrowser::TreeDirector"
"|* %i ViewManPage::TreeDirector %l"
"|* %i ShowFileList::TreeDirector"
"|* %i FindFile::TreeDirector"
"|* %i SearchFiles::TreeDirector"
"|* %i DiffFiles::TreeDirector %l"
"|* %i SaveAllTextFiles::TreeDirector"
"|* %i CloseAllTextFiles::TreeDirector"
;

#include "TreeDirector-Project-enum.h"

#ifndef _H_jcc_show_symbol_list
#define _H_jcc_show_symbol_list
#include "jcc_show_symbol_list.xpm"
#endif
#ifndef _H_jcc_view_man_page
#define _H_jcc_view_man_page
#include "jcc_view_man_page.xpm"
#endif
#ifndef _H_jcc_show_file_list
#define _H_jcc_show_file_list
#include "jcc_show_file_list.xpm"
#endif
#ifndef _H_jcc_search_files
#define _H_jcc_search_files
#include "jcc_search_files.xpm"
#endif
#ifndef _H_jcc_compare_files
#define _H_jcc_compare_files
#include "jcc_compare_files.xpm"
#endif
#ifndef _H_jx_af_image_jx_jx_file_save_all
#define _H_jx_af_image_jx_jx_file_save_all
#include <jx-af/image/jx/jx_file_save_all.xpm>
#endif

static void ConfigureProjectMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts("#p");
	}
	menu->SetItemImage(kShowSymbolBrowserCmd + offset, jcc_show_symbol_list);
	menu->SetItemImage(kViewManPageCmd + offset, jcc_view_man_page);
	menu->SetItemImage(kShowFileListCmd + offset, jcc_show_file_list);
	menu->SetItemImage(kSearchFilesCmd + offset, jcc_search_files);
	menu->SetItemImage(kDiffFilesCmd + offset, jcc_compare_files);
	menu->SetItemImage(kSaveAllTextCmd + offset, jx_file_save_all);
};

#endif
