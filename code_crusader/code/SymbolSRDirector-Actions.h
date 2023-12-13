// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_SymbolSRDirector_Actions
#define _H_SymbolSRDirector_Actions

static const JUtf8Byte* kActionsMenuStr =
"* %i __CopySelectedNames::SymbolSRDirector %l"
"|* %i __Update::SymbolSRDirector"
"|* %i __ShowCClassTree::SymbolSRDirector"
"|* %i __ShowDClassTree::SymbolSRDirector"
"|* %i __ShowGoStructInterfaceTree::SymbolSRDirector"
"|* %i __ShowJavaClassTree::SymbolSRDirector"
"|* %i __ShowPHPClassTree::SymbolSRDirector %l"
"|* %i __CloseWindow::SymbolSRDirector"
"|* %i __CloseAll::SymbolSRDirector"
;

enum {
	kCopySelNamesCmd=1,
	kUpdateCmd,
	kShowCTreeCmd,
	kShowDTreeCmd,
	kShowGoTreeCmd,
	kShowJavaTreeCmd,
	kShowPHPTreeCmd,
	kCloseWindowCmd,
	kCloseAllCmd,
};

#ifndef _H_jx_af_image_jx_jx_edit_copy
#define _H_jx_af_image_jx_jx_edit_copy
#include <jx-af/image/jx/jx_edit_copy.xpm>
#endif
#ifndef _H_jcc_show_c_tree
#define _H_jcc_show_c_tree
#include "jcc_show_c_tree.xpm"
#endif
#ifndef _H_jcc_show_d_tree
#define _H_jcc_show_d_tree
#include "jcc_show_d_tree.xpm"
#endif
#ifndef _H_jcc_show_go_tree
#define _H_jcc_show_go_tree
#include "jcc_show_go_tree.xpm"
#endif
#ifndef _H_jcc_show_java_tree
#define _H_jcc_show_java_tree
#include "jcc_show_java_tree.xpm"
#endif
#ifndef _H_jcc_show_php_tree
#define _H_jcc_show_php_tree
#include "jcc_show_php_tree.xpm"
#endif

static void ConfigureActionsMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts(JString("#a", JString::kNoCopy));
	}
	menu->SetItemImage(kCopySelNamesCmd + offset, jx_edit_copy);
	menu->SetItemImage(kShowCTreeCmd + offset, jcc_show_c_tree);
	menu->SetItemImage(kShowDTreeCmd + offset, jcc_show_d_tree);
	menu->SetItemImage(kShowGoTreeCmd + offset, jcc_show_go_tree);
	menu->SetItemImage(kShowJavaTreeCmd + offset, jcc_show_java_tree);
	menu->SetItemImage(kShowPHPTreeCmd + offset, jcc_show_php_tree);
};

#endif
