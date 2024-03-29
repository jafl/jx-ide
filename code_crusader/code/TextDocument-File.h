// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_TextDocument_File
#define _H_TextDocument_File

static const JUtf8Byte* kFileMenuStr =
"* %i NewTextFile::TextDocument"
"|* %i NewTextFileFromTmpl::TextDocument"
"|* %i NewProject::TextDocument %l"
"|* %i OpenSomething::TextDocument"
"|* %i __RecentProjects::TextDocument"
"|* %i __RecentTextFiles::TextDocument %l"
"|* %i SaveFile::TextDocument"
"|* %i SaveFileAs::TextDocument"
"|* %i SaveCopyAs::TextDocument"
"|* %i SaveAsTemplate::TextDocument"
"|* %i Revert::TextDocument"
"|* %i SaveAllTextFiles::TextDocument %l"
"|* %i __FileFormat::TextDocument %l"
"|* %i DiffFiles::TextDocument"
"|* %i DiffSmart::TextDocument"
"|* %i DiffVCS::TextDocument"
"|* %i __Compare::TextDocument %l"
"|* %i ShowInFileMgr::TextDocument %l"
"|* %i PageSetup::JX"
"|* %i Print::JX %l"
"|* %i PageSetupStyledText::TextDocument"
"|* %i PrintStyledText::TextDocument %l"
"|* %i CloseWindow::JX"
"|* %i CloseAllTextFiles::TextDocument %l"
"|* %i Quit::JX"
;

#include "TextDocument-File-enum.h"

#ifndef _H_jx_af_image_jx_jx_file_new
#define _H_jx_af_image_jx_jx_file_new
#include <jx-af/image/jx/jx_file_new.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_file_open
#define _H_jx_af_image_jx_jx_file_open
#include <jx-af/image/jx/jx_file_open.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_file_save
#define _H_jx_af_image_jx_jx_file_save
#include <jx-af/image/jx/jx_file_save.xpm>
#endif
#ifndef _H_jcc_file_save_copy_as
#define _H_jcc_file_save_copy_as
#include "jcc_file_save_copy_as.xpm"
#endif
#ifndef _H_jx_af_image_jx_jx_file_revert_to_saved
#define _H_jx_af_image_jx_jx_file_revert_to_saved
#include <jx-af/image/jx/jx_file_revert_to_saved.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_file_save_all
#define _H_jx_af_image_jx_jx_file_save_all
#include <jx-af/image/jx/jx_file_save_all.xpm>
#endif
#ifndef _H_jcc_compare_files
#define _H_jcc_compare_files
#include "jcc_compare_files.xpm"
#endif
#ifndef _H_jcc_compare_backup
#define _H_jcc_compare_backup
#include "jcc_compare_backup.xpm"
#endif
#ifndef _H_jcc_compare_vcs
#define _H_jcc_compare_vcs
#include "jcc_compare_vcs.xpm"
#endif
#ifndef _H_jx_af_image_jx_jx_file_print
#define _H_jx_af_image_jx_jx_file_print
#include <jx-af/image/jx/jx_file_print.xpm>
#endif
#ifndef _H_jcc_file_print_with_styles
#define _H_jcc_file_print_with_styles
#include "jcc_file_print_with_styles.xpm"
#endif

static void ConfigureFileMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts("#f");
	}
	menu->SetItemImage(kNewTextEditorCmd + offset, jx_file_new);
	menu->SetItemImage(kOpenSomethingCmd + offset, jx_file_open);
	menu->SetItemImage(kSaveFileCmd + offset, jx_file_save);
	menu->SetItemImage(kSaveCopyAsCmd + offset, jcc_file_save_copy_as);
	menu->SetItemImage(kRevertCmd + offset, jx_file_revert_to_saved);
	menu->SetItemImage(kSaveAllFilesCmd + offset, jx_file_save_all);
	menu->SetItemImage(kDiffFilesCmd + offset, jcc_compare_files);
	menu->SetItemImage(kDiffSmartCmd + offset, jcc_compare_backup);
	menu->SetItemImage(kDiffVCSCmd + offset, jcc_compare_vcs);
	menu->SetItemImage(kPrintPTCmd + offset, jx_file_print);
	menu->SetItemImage(kPrintPSCmd + offset, jcc_file_print_with_styles);
};

#endif
