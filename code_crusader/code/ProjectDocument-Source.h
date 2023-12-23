// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_ProjectDocument_Source
#define _H_ProjectDocument_Source

static const JUtf8Byte* kSourceMenuStr =
"* %i NewProjectGroup::ProjectDocument"
"|* %i AddFilesToProject::ProjectDocument"
"|* %i AddDirectoryTreeToProject::ProjectDocument"
"|* %i RemoveFiles::ProjectDocument %l"
"|* %i OpenSelectedFiles::ProjectDocument"
"|* %i OpenComplFiles::ProjectDocument %l"
"|* %i EditFilePath::ProjectDocument"
"|* %i EditSubprojConfig::ProjectDocument %l"
"|* %i DiffSmart::ProjectDocument"
"|* %i DiffVCS::ProjectDocument"
"|* %i OpenSelectedFileLocations::ProjectDocument %l"
"|* %i SaveAllTextFiles::ProjectDocument"
"|* %i CloseAllTextFiles::ProjectDocument"
;

enum {
	kNewGroupCmd=1,
	kAddFilesCmd,
	kAddDirTreeCmd,
	kRemoveSelCmd,
	kOpenFilesCmd,
	kOpenComplFilesCmd,
	kEditPathCmd,
	kEditSubprojConfigCmd,
	kDiffSmartCmd,
	kDiffVCSCmd,
	kShowLocationCmd,
	kSaveAllTextCmd,
	kCloseAllTextCmd,
};

#ifndef _H_jx_af_image_jx_jx_file_save_all
#define _H_jx_af_image_jx_jx_file_save_all
#include <jx-af/image/jx/jx_file_save_all.xpm>
#endif

static void ConfigureSourceMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts(JString("#s", JString::kNoCopy));
	}
	menu->SetItemImage(kSaveAllTextCmd + offset, jx_file_save_all);
};

#endif