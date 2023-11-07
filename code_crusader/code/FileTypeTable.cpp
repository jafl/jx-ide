/******************************************************************************
 FileTypeTable.cpp

	BASE CLASS = JXEditTable

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "FileTypeTable.h"
#include "globals.h"
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXColHeaderWidget.h>
#include <jx-af/jx/JXGetNewDirDialog.h>
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/JPainter.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;

enum
{
	kSuffixColumn = 1,
	kTypeColumn,
	kMacroColumn,
	kScriptColumn,
	kCRMColumn,
	kWrapColumn,
	kEditCmdColumn
};

const JCoordinate kInitColWidth[] =
{
	100, 120, 75, 80, 70, 70, 150
};

const JSize kColCount = sizeof(kInitColWidth) / sizeof(JCoordinate);

// geometry information

const JFileVersion kCurrentGeometryDataVersion = 3;
const JUtf8Byte kGeometryDataEndDelimiter      = '\1';

	// version  3 stores script path column width
	// version  2 stores edit cmd column width
	// version  1 stores CRM column width

// Type menu

static const JUtf8Byte* kTypeMenuStr =
	"  Adobe Flash %r"
	"| Ant %r"
	"| Assembly %r"
	"| ASP %r"
	"| AWK %r"
	"| Basic %r"
	"| Beta %r"
	"| Bison %r"
	"| Bourne shell (sh) %r"
	"| C shell (csh) %r"
	"| C/C++ source %r"
	"| C/C++ header %r"
	"| C# %r"
	"| Cobol %r"
	"| D %d"
	"| Eiffel %r"
	"| Erlang %r"
	"| FORTRAN %r"
	"| Go %r"
	"| HTML %r"
	"| INI %r"
	"| Java archive %r"
	"| Java properties %r"
	"| Java source %r"
	"| JavaScript %r"
	"| JSP %r"
	"| Lex %r"
	"| Lisp %r"
	"| Lua %r"
	"| Make %r"
	"| Matlab %r"
	"| Modula-2 module %r"
	"| Modula-2 interface %r"
	"| Modula-3 module %r"
	"| Modula-3 interface %r"
	"| Pascal %r"
	"| Perl %r"
	"| PHP %r"
	"| Python %r"
	"| Ratfor %r"
	"| REXX %r"
	"| Ruby %r"
	"| Scheme %r"
	"| SLang %r"
	"| SML %r"
	"| SQL %r"
	"| TCL %r"
	"| Vera source %r"
	"| Vera header %r"
	"| Verilog %r"
	"| Vim %r"
	"| XML %r"
	"| Other source %r"
	"| Static library %r"
	"| Shared library %r"
	"| Documentation %r"
	"| Other text %r"
	"| Binary %r"
	"| External %r";

enum
{
	kAdobeFlexTypeCmd = 1,
	kAntTypeCmd,
	kAssemblyTypeCmd,
	kASPTypeCmd,
	kAWKTypeCmd,
	kBasicTypeCmd,
	kBetaTypeCmd,
	kBisonTypeCmd,
	kBourneShellTypeCmd,
	kCShellTypeCmd,
	kCSourceTypeCmd,
	kCHeaderTypeCmd,
	kCSharpTypeCmd,
	kCobolTypeCmd,
	kDTypeCmd,
	kEiffelTypeCmd,
	kErlangTypeCmd,
	kFortranTypeCmd,
	kGoTypeCmd,
	kHTMLTypeCmd,
	kINITypeCmd,
	kJavaArchiveTypeCmd,
	kPropertiesTypeCmd,
	kJavaSourceTypeCmd,
	kJavaScriptTypeCmd,
	kJSPTypeCmd,
	kLexTypeCmd,
	kLispTypeCmd,
	kLuaTypeCmd,
	kMakeTypeCmd,
	kMatlabTypeCmd,
	kModula2ModuleTypeCmd,
	kModula2InterfaceTypeCmd,
	kModula3ModuleTypeCmd,
	kModula3InterfaceTypeCmd,
	kPascalTypeCmd,
	kPerlTypeCmd,
	kPHPTypeCmd,
	kPythonTypeCmd,
	kRatforTypeCmd,
	kREXXTypeCmd,
	kRubyTypeCmd,
	kSchemeTypeCmd,
	kSLangTypeCmd,
	kSMLTypeCmd,
	kSQLTypeCmd,
	kTCLTypeCmd,
	kVeraSourceTypeCmd,
	kVeraHeaderTypeCmd,
	kVerilogTypeCmd,
	kVimTypeCmd,
	kXMLTypeCmd,
	kOtherSourceTypeCmd,
	kStaticLibTypeCmd,
	kSharedLibTypeCmd,
	kDocumentationTypeCmd,
	kOtherTextTypeCmd,
	kBinaryTypeCmd,
	kExternalTypeCmd
};

static const TextFileType kMenuIndexToFileType[] =
{
	kAdobeFlexFT,
	kAntFT,
	kAssemblyFT,
	kASPFT,
	kAWKFT,
	kBasicFT,
	kBetaFT,
	kBisonFT,
	kBourneShellFT,
	kCShellFT,
	kCSourceFT,
	kCHeaderFT,
	kCSharpFT,
	kCobolFT,
	kDFT,
	kEiffelFT,
	kErlangFT,
	kFortranFT,
	kGoFT,
	kHTMLFT,
	kINIFT,
	kJavaArchiveFT,
	kPropertiesFT,
	kJavaSourceFT,
	kJavaScriptFT,
	kJSPFT,
	kLexFT,
	kLispFT,
	kLuaFT,
	kMakeFT,
	kMatlabFT,
	kModula2ModuleFT,
	kModula2InterfaceFT,
	kModula3ModuleFT,
	kModula3InterfaceFT,
	kPascalFT,
	kPerlFT,
	kPHPFT,
	kPythonFT,
	kRatforFT,
	kREXXFT,
	kRubyFT,
	kSchemeFT,
	kSLangFT,
	kSMLFT,
	kSQLFT,
	kTCLFT,
	kVeraSourceFT,
	kVeraHeaderFT,
	kVerilogFT,
	kVimFT,
	kXMLFT,
	kOtherSourceFT,
	kStaticLibraryFT,
	kSharedLibraryFT,
	kDocumentationFT,
	kUnknownFT,
	kBinaryFT,
	kExternalFT
};

static const JIndex kFileTypeToMenuIndex[] =
{
	kOtherTextTypeCmd,
	kCSourceTypeCmd,
	kCHeaderTypeCmd,
	kOtherSourceTypeCmd,
	kDocumentationTypeCmd,
	kHTMLTypeCmd,
	kEiffelTypeCmd,
	kFortranTypeCmd,
	kJavaSourceTypeCmd,
	kStaticLibTypeCmd,
	kSharedLibTypeCmd,
	0,					// kExecOutputFT
	0,					// kManPageFT
	0,					// kDiffOutputFT
	kAssemblyTypeCmd,
	kPascalTypeCmd,
	kRatforTypeCmd,
	kExternalTypeCmd,
	kBinaryTypeCmd,
	kModula2ModuleTypeCmd,
	kModula2InterfaceTypeCmd,
	kModula3ModuleTypeCmd,
	kModula3InterfaceTypeCmd,
	kAWKTypeCmd,
	kCobolTypeCmd,
	kLispTypeCmd,
	kPerlTypeCmd,
	kPythonTypeCmd,
	kSchemeTypeCmd,
	kBourneShellTypeCmd,
	kTCLTypeCmd,
	kVimTypeCmd,
	kJavaArchiveTypeCmd,
	kPHPTypeCmd,
	kASPTypeCmd,
	0,					// kSearchOutputFT
	kMakeTypeCmd,
	kREXXTypeCmd,
	kRubyTypeCmd,
	kLexTypeCmd,
	kCShellTypeCmd,
	kBisonTypeCmd,
	kBetaTypeCmd,
	kLuaTypeCmd,
	kSLangTypeCmd,
	kSQLTypeCmd,
	kVeraSourceTypeCmd,
	kVeraHeaderTypeCmd,
	kVerilogTypeCmd,
	kCSharpTypeCmd,
	kErlangTypeCmd,
	kSMLTypeCmd,
	kJavaScriptTypeCmd,
	kAntTypeCmd,
	kJSPTypeCmd,
	kXMLTypeCmd,
	kBasicTypeCmd,
	0,					// kUnused1FT
	kMatlabTypeCmd,
	kAdobeFlexTypeCmd,
	kINITypeCmd,
	kPropertiesTypeCmd,
	kGoTypeCmd,
	kDTypeCmd
};

// Script menu

static const JUtf8Byte* kScriptMenuStr    = "None %r %l";
static const JUtf8Byte* kScriptMenuEndStr = "New directory";

enum
{
	kNoScriptPathCmd = 1,

	kNewScriptPathCmd = 0	// offset from end of menu
};

/******************************************************************************
 Constructor

 ******************************************************************************/

FileTypeTable::FileTypeTable
	(
	const JArray<PrefsManager::FileTypeInfo>&		fileTypeList,
	const JArray<PrefsManager::MacroSetInfo>&		macroList,
	const JArray<PrefsManager::CRMRuleListInfo>&	crmList,

	JXTextButton*		addTypeButton,
	JXTextButton*		removeTypeButton,
	JXTextButton*		duplicateTypeButton,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXEditTable(1,1, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h),
	itsMacroList(macroList),
	itsCRMList(crmList),
	itsFont(JFontManager::GetDefaultFont())
{
	itsTextInput = nullptr;

	// font

	JString fontName;
	JSize fontSize;
	GetPrefsManager()->GetDefaultFont(&fontName, &fontSize);
	itsFont.Set(fontName, fontSize);

	const JSize rowHeight = 2*kVMarginWidth + JMax(
		JFontManager::GetDefaultFont().GetLineHeight(GetFontManager()),
		itsFont.GetLineHeight(GetFontManager()));
	SetDefaultRowHeight(rowHeight);

	// buttons

	itsAddTypeButton       = addTypeButton;
	itsRemoveTypeButton    = removeTypeButton;
	itsDuplicateTypeButton = duplicateTypeButton;

	ListenTo(itsAddTypeButton, std::function([this](const JXButton::Pushed&)
	{
		AddType();
	}));

	ListenTo(itsRemoveTypeButton, std::function([this](const JXButton::Pushed&)
	{
		RemoveType();
	}));

	ListenTo(itsDuplicateTypeButton, std::function([this](const JXButton::Pushed&)
	{
		DuplicateType();
	}));

	// type menu

	itsTypeMenu = jnew JXTextMenu(JString::empty, this, kFixedLeft, kFixedTop, 0,0, 10,10);
	itsTypeMenu->SetToHiddenPopupMenu();
	itsTypeMenu->SetMenuItems(kTypeMenuStr);
	itsTypeMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsTypeMenu->AttachHandlers(this,
		&FileTypeTable::UpdateTypeMenu,
		&FileTypeTable::HandleTypeMenu);

	// macro menu

	itsMacroMenu = CreateMacroMenu();
	itsMacroMenu->AttachHandlers(this,
		&FileTypeTable::UpdateMacroMenu,
		&FileTypeTable::HandleMacroMenu);

	// script menu

	itsScriptMenu = jnew JXTextMenu(JString::empty, this, kFixedLeft, kFixedTop, 0,0, 10,10);
	itsScriptMenu->SetToHiddenPopupMenu();
	itsScriptMenu->SetMenuItems(kScriptMenuStr);	// ensure non-empty
	itsScriptMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsScriptMenu->AttachHandlers(this,
		&FileTypeTable::UpdateScriptMenu,
		&FileTypeTable::HandleScriptMenu);

	// CRM menu

	itsCRMMenu = CreateCRMMenu();
	itsCRMMenu->AttachHandlers(this,
		&FileTypeTable::UpdateCRMMenu,
		&FileTypeTable::HandleCRMMenu);

	// regex for testing

	itsTestRegex = jnew JRegex(".");

	// data

	itsFileTypeList = jnew JArray<PrefsManager::FileTypeInfo>(fileTypeList);
	assert( itsFileTypeList != nullptr );
	FinishFileTypeListCopy(itsFileTypeList);
	itsFileTypeList->SetCompareFunction(PrefsManager::CompareFileTypeSpec);
	itsFileTypeList->Sort();

	for (JIndex i=1; i<=kColCount; i++)
	{
		AppendCols(1, kInitColWidth[i-1]);
	}

	AppendRows(itsFileTypeList->GetItemCount());

	UpdateButtons();
	ListenTo(&(GetTableSelection()));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FileTypeTable::~FileTypeTable()
{
	CleanOutFileTypeList(itsFileTypeList);
	jdelete itsFileTypeList;

	jdelete itsTestRegex;
}

/******************************************************************************
 CleanOutFileTypeList (private)

 ******************************************************************************/

void
FileTypeTable::CleanOutFileTypeList
	(
	JArray<PrefsManager::FileTypeInfo>* fileTypeList
	)
	const
{
	const JSize count = fileTypeList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		PrefsManager::FileTypeInfo info = fileTypeList->GetItem(i);
		info.Free();
	}
}

/******************************************************************************
 GetFileTypeList

 ******************************************************************************/

void
FileTypeTable::GetFileTypeList
	(
	JArray<PrefsManager::FileTypeInfo>* fileTypeList
	)
	const
{
	assert( !IsEditing() );

	CleanOutFileTypeList(fileTypeList);
	*fileTypeList = *itsFileTypeList;
	FinishFileTypeListCopy(fileTypeList);
}

/******************************************************************************
 FinishFileTypeListCopy (private)

 ******************************************************************************/

void
FileTypeTable::FinishFileTypeListCopy
	(
	JArray<PrefsManager::FileTypeInfo>* fileTypeList
	)
	const
{
	const JSize count = fileTypeList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		PrefsManager::FileTypeInfo info = fileTypeList->GetItem(i);

		info.suffix = jnew JString(*(info.suffix));
		assert( info.suffix != nullptr );

		info.complSuffix = jnew JString(*(info.complSuffix));
		assert( info.complSuffix != nullptr );

		if (info.scriptPath != nullptr)
		{
			info.scriptPath = jnew JString(*(info.scriptPath));
			assert( info.scriptPath != nullptr );
		}

		if (info.editCmd != nullptr)
		{
			info.editCmd = jnew JString(*(info.editCmd));
			assert( info.editCmd != nullptr );
		}

		// create regex when it is not our own private list

		info.nameRegex    = nullptr;
		info.contentRegex = nullptr;

		if (fileTypeList != itsFileTypeList)
		{
			info.CreateRegex();
		}

		(info.literalRange).SetToNothing();		// assume text changed

		fileTypeList->SetItem(i, info);
	}
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
FileTypeTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JPoint editCell;
	if (GetEditedCell(&editCell) && cell == editCell)
	{
		return;
	}

	HilightIfSelected(p, cell, rect);

	const PrefsManager::FileTypeInfo info = itsFileTypeList->GetItem(cell.y);
	if (cell.x == kSuffixColumn)
	{
		p.SetFont(itsFont);

		JRect r = rect;
		r.left += kHMarginWidth;
		p.String(r, *(info.suffix), JPainter::HAlign::kLeft, JPainter::VAlign::kCenter);

		p.SetFont(JFontManager::GetDefaultFont());
	}
	else if (cell.x == kTypeColumn)
	{
		const JString& str = itsTypeMenu->GetItemText(kFileTypeToMenuIndex[ info.type ]);
		p.String(rect, str, JPainter::HAlign::kCenter, JPainter::VAlign::kCenter);
	}
	else if (cell.x == kMacroColumn &&
			 info.type != kBinaryFT && info.type != kExternalFT)
	{
		const JString& str = itsMacroMenu->GetItemText(MacroIDToMenuIndex(info.macroID));
		p.String(rect, str, JPainter::HAlign::kCenter, JPainter::VAlign::kCenter);
	}
	else if (cell.x == kScriptColumn &&
			 info.type != kBinaryFT && info.type != kExternalFT)
	{
		if (info.scriptPath == nullptr)
		{
			p.String(rect, JGetString("NoScript::FileTypeTable"), JPainter::HAlign::kCenter, JPainter::VAlign::kCenter);
		}
		else
		{
			p.String(rect, *(info.scriptPath), JPainter::HAlign::kCenter, JPainter::VAlign::kCenter);
		}
	}
	else if (cell.x == kCRMColumn &&
			 info.type != kBinaryFT && info.type != kExternalFT)
	{
		const JString& str = itsCRMMenu->GetItemText(CRMIDToMenuIndex(info.crmID));
		p.String(rect, str, JPainter::HAlign::kCenter, JPainter::VAlign::kCenter);
	}
	else if (cell.x == kWrapColumn &&
			 info.type != kBinaryFT && info.type != kExternalFT)
	{
		p.String(rect,
			JGetString(info.wordWrap ? "WordWrapOn::FileTypeTable" : "WordWrapOff::FileTypeTable"),
			JPainter::HAlign::kCenter, JPainter::VAlign::kCenter);
	}
	else if (cell.x == kEditCmdColumn && info.editCmd != nullptr)
	{
		p.SetFont(itsFont);

		JRect r = rect;
		r.left += kHMarginWidth;
		p.String(r, *(info.editCmd), JPainter::HAlign::kLeft, JPainter::VAlign::kCenter);

		p.SetFont(JFontManager::GetDefaultFont());
	}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
FileTypeTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JTableSelection& s = GetTableSelection();
	s.ClearSelection();

	JPoint cell;
	if (button == kJXLeftButton && GetCell(pt, &cell))
	{
		s.SelectCell(cell);
		TableScrollToCell(cell);

		PrefsManager::FileTypeInfo info = itsFileTypeList->GetItem(cell.y);

		if (cell.x == kSuffixColumn)
		{
			BeginEditing(cell);
		}
		else if (cell.x == kTypeColumn)
		{
			itsTypeMenu->PopUp(this, pt, buttonStates, modifiers);
		}
		else if (cell.x == kMacroColumn &&
				 info.type != kBinaryFT && info.type != kExternalFT)
		{
			itsMacroMenu->PopUp(this, pt, buttonStates, modifiers);
		}
		else if (cell.x == kScriptColumn &&
				 info.type != kBinaryFT && info.type != kExternalFT)
		{
			itsScriptMenu->PopUp(this, pt, buttonStates, modifiers);
		}
		else if (cell.x == kCRMColumn &&
				 info.type != kBinaryFT && info.type != kExternalFT)
		{
			itsCRMMenu->PopUp(this, pt, buttonStates, modifiers);
		}
		else if (cell.x == kWrapColumn &&
				 info.type != kBinaryFT && info.type != kExternalFT)
		{
			info.wordWrap = !info.wordWrap;
			if (modifiers.meta())
			{
				const bool wrap = info.wordWrap;
				const JSize count   = itsFileTypeList->GetItemCount();
				for (JIndex i=1; i<=count; i++)
				{
					info          = itsFileTypeList->GetItem(i);
					info.wordWrap = wrap;
					itsFileTypeList->SetItem(i, info);
				}
				TableRefreshCol(kWrapColumn);
			}
			else
			{
				itsFileTypeList->SetItem(cell.y, info);
				TableRefreshCell(cell);
			}
		}
		else if (cell.x == kEditCmdColumn && info.type == kExternalFT)
		{
			BeginEditing(cell);
		}
	}
	else
	{
		ScrollForWheel(button, modifiers);
	}
}

/******************************************************************************
 IsEditable (virtual)

 ******************************************************************************/

bool
FileTypeTable::IsEditable
	(
	const JPoint& cell
	)
	const
{
	const PrefsManager::FileTypeInfo info = itsFileTypeList->GetItem(cell.y);
	return cell.x == kSuffixColumn ||
				(cell.x == kEditCmdColumn && info.editCmd != nullptr);
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
FileTypeTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert( itsTextInput == nullptr );

	JTableSelection& s = GetTableSelection();
	s.ClearSelection();
	s.SelectCell(cell);

	itsTextInput = jnew JXInputField(this, kFixedLeft, kFixedTop, x,y, w,h);

	const PrefsManager::FileTypeInfo info = itsFileTypeList->GetItem(cell.y);
	if (cell.x == kSuffixColumn)
	{
		itsTextInput->GetText()->SetText(*info.suffix);
		itsTextInput->SetIsRequired();
	}
	else if (cell.x == kEditCmdColumn)
	{
		assert( info.editCmd != nullptr );
		itsTextInput->GetText()->SetText(*info.editCmd);
	}
	itsTextInput->SetFont(itsFont);
	return itsTextInput;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

bool
FileTypeTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	assert( itsTextInput != nullptr );

	PrefsManager::FileTypeInfo info = itsFileTypeList->GetItem(cell.y);
	const JString& text               = itsTextInput->GetText()->GetText();

	bool ok = itsTextInput->InputValid();
	if (ok && cell.x == kSuffixColumn)
	{
		const bool isRegex = text.GetFirstCharacter() == kContentRegexMarker;
		if (isRegex && text.GetCharacterCount() == 1)
		{
			ok = false;
			JGetUserNotification()->ReportError(
				JGetString("EmptyInput::FileTypeTable"));
		}
		else if (isRegex)
		{
			const JError err = itsTestRegex->SetPattern(text);
			err.ReportIfError();
			ok = err.OK();
		}

		if (ok)
		{
			*info.suffix = text;
		}
	}
	else if (ok && cell.x == kEditCmdColumn)
	{
		assert( info.editCmd != nullptr );
		*info.editCmd = text;
		info.editCmd->TrimWhitespace();
	}

	return ok;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
FileTypeTable::PrepareDeleteXInputField()
{
	itsTextInput = nullptr;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
FileTypeTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == &(GetTableSelection()))
	{
		UpdateButtons();
	}

	JXEditTable::Receive(sender, message);
}

/******************************************************************************
 AddType (private)

 ******************************************************************************/

void
FileTypeTable::AddType()
{
	if (EndEditing())
	{
		PrefsManager::FileTypeInfo info(jnew JString, nullptr, nullptr, kUnknownFT,
										  kEmptyMacroID, kEmptyCRMRuleListID,
										  true, nullptr, true, jnew JString, nullptr);
		assert( info.suffix != nullptr && info.complSuffix != nullptr );
		itsFileTypeList->AppendItem(info);
		AppendRows(1);
		BeginEditing(JPoint(kSuffixColumn, itsFileTypeList->GetItemCount()));
	}
}

/******************************************************************************
 RemoveType (private)

 ******************************************************************************/

void
FileTypeTable::RemoveType()
{
	JPoint cell;
	if ((GetTableSelection()).GetFirstSelectedCell(&cell))
	{
		CancelEditing();

		PrefsManager::FileTypeInfo info = itsFileTypeList->GetItem(cell.y);
		info.Free();

		itsFileTypeList->RemoveItem(cell.y);
		RemoveRow(cell.y);
	}
}

/******************************************************************************
 DuplicateType (private)

 ******************************************************************************/

void
FileTypeTable::DuplicateType()
{
	JPoint cell;
	if ((GetTableSelection()).GetFirstSelectedCell(&cell) && EndEditing())
	{
		PrefsManager::FileTypeInfo info = itsFileTypeList->GetItem(cell.y);

		info.suffix = jnew JString(*(info.suffix));
		assert( info.suffix != nullptr );

		info.nameRegex    = nullptr;
		info.contentRegex = nullptr;

		info.complSuffix = jnew JString(*(info.complSuffix));
		assert( info.complSuffix != nullptr );

		if (info.scriptPath != nullptr)
		{
			info.scriptPath = jnew JString(*(info.scriptPath));
			assert( info.scriptPath != nullptr );
		}

		if (info.editCmd != nullptr)
		{
			info.editCmd = jnew JString(*(info.editCmd));
			assert( info.editCmd != nullptr );
		}

		itsFileTypeList->AppendItem(info);
		AppendRows(1);
		BeginEditing(JPoint(kSuffixColumn, itsFileTypeList->GetItemCount()));
	}
}

/******************************************************************************
 UpdateTypeMenu (private)

 ******************************************************************************/

void
FileTypeTable::UpdateTypeMenu()
{
	JPoint cell;
	const bool ok = (GetTableSelection()).GetFirstSelectedCell(&cell);
	assert( ok );

	const PrefsManager::FileTypeInfo info = itsFileTypeList->GetItem(cell.y);
	itsTypeMenu->CheckItem(kFileTypeToMenuIndex[ info.type ]);
}

/******************************************************************************
 HandleTypeMenu (private)

 ******************************************************************************/

void
FileTypeTable::HandleTypeMenu
	(
	const JIndex index
	)
{
	JPoint cell;
	const bool ok = (GetTableSelection()).GetFirstSelectedCell(&cell);
	assert( ok );

	const TextFileType newType = kMenuIndexToFileType [ index-1 ];

	PrefsManager::FileTypeInfo info = itsFileTypeList->GetItem(cell.y);
	if (newType != info.type)
	{
		info.type = newType;
		if (info.type == kExternalFT)
		{
			info.editCmd = jnew JString;
			assert( info.editCmd != nullptr );
			BeginEditing(JPoint(kEditCmdColumn, cell.y));
		}
		else
		{
			jdelete info.editCmd;
			info.editCmd = nullptr;
		}
		itsFileTypeList->SetItem(cell.y, info);

		TableRefreshRow(cell.y);

		// info.editCmd must not be nullptr

		if (info.type == kExternalFT)
		{
			BeginEditing(JPoint(kEditCmdColumn, cell.y));
		}
	}
}

/******************************************************************************
 UpdateScriptMenu (private)

 ******************************************************************************/

void
FileTypeTable::UpdateScriptMenu()
{
	itsScriptMenu->SetMenuItems(kScriptMenuStr);

	JPoint cell;
	const bool ok = GetTableSelection().GetFirstSelectedCell(&cell);
	assert( ok );
	const PrefsManager::FileTypeInfo info = itsFileTypeList->GetItem(cell.y);

	if (info.scriptPath == nullptr)
	{
		itsScriptMenu->CheckItem(kNoScriptPathCmd);
	}

	JPtrArray<JString> menuText(JPtrArrayT::kDeleteAll);
	menuText.SetCompareFunction(JCompareStringsCaseInsensitive);
	menuText.SetSortOrder(JListT::kSortAscending);

	JString sysDir, userDir;
	if (PrefsManager::GetScriptPaths(&sysDir, &userDir))
	{
		BuildScriptMenuItems(sysDir,  false, &menuText);
		BuildScriptMenuItems(userDir, true,  &menuText);

		const JSize count = menuText.GetItemCount();
		JString itemText, nmShortcut;
		for (JIndex i=1; i<=count; i++)
		{
			// We have to extract user/sys here because otherwise we would
			// have to keep extra state while building the sorted list.

			itemText = *menuText.GetItem(i);

			JStringIterator iter(&itemText, JStringIterator::kStartAtEnd);
			iter.BeginMatch();
			const bool found = iter.Prev(" (");
			assert( found );
			const JStringMatch& m = iter.FinishMatch(true);

			nmShortcut = m.GetString();
			iter.RemoveAllNext();
			iter.Invalidate();

			itsScriptMenu->AppendItem(itemText, JXMenu::kRadioType);
			itsScriptMenu->SetItemNMShortcut(itsScriptMenu->GetItemCount(), nmShortcut);

			// By not stripping off the leading whitespace on nmShorcut, we
			// can do a direct string comparison with the original strings.

			if (info.scriptPath != nullptr && *info.scriptPath == itemText &&
				(( info.isUserScript && nmShortcut == JGetString("UserScriptMarker::FileTypeTable")) ||
				 (!info.isUserScript && nmShortcut == JGetString("SysScriptMarker::FileTypeTable"))))
			{
				itsScriptMenu->CheckItem(itsScriptMenu->GetItemCount());
			}
		}
	}

	itsScriptMenu->ShowSeparatorAfter(itsScriptMenu->GetItemCount());
	itsScriptMenu->AppendMenuItems(kScriptMenuEndStr);
}

/******************************************************************************
 BuildScriptMenuItems (private)

	Builds a sorted list of the directories below the given path.

 ******************************************************************************/

void
FileTypeTable::BuildScriptMenuItems
	(
	const JString&		path,
	const bool			isUserPath,
	JPtrArray<JString>*	menuText
	)
	const
{
	JDirInfo* info = nullptr;
	if (JDirInfo::Create(path, &info))
	{
		info->ShowFiles(false);

		const JSize count = info->GetEntryCount();
		for (JIndex i=1; i<=count; i++)
		{
			auto* s = jnew JString(info->GetEntry(i).GetName());
			assert( s != nullptr );

			if (isUserPath)
			{
				*s += JGetString("UserScriptMarker::FileTypeTable");
			}
			else
			{
				*s += JGetString("SysScriptMarker::FileTypeTable");
			}

			menuText->InsertSorted(s);
		}
	}
}

/******************************************************************************
 HandleScriptMenu (private)

 ******************************************************************************/

void
FileTypeTable::HandleScriptMenu
	(
	const JIndex index
	)
{
	JPoint cell;
	const bool ok = (GetTableSelection()).GetFirstSelectedCell(&cell);
	assert( ok );

	PrefsManager::FileTypeInfo info = itsFileTypeList->GetItem(cell.y);

	const JSize itemCount = itsScriptMenu->GetItemCount();
	if (index == kNoScriptPathCmd)
	{
		jdelete info.scriptPath;
		info.scriptPath = nullptr;
	}
	else if (index == itemCount - kNewScriptPathCmd)
	{
		GetNewScriptDirectory();
	}
	else
	{
		if (info.scriptPath == nullptr)
		{
			info.scriptPath = jnew JString(itsScriptMenu->GetItemText(index));
			assert( info.scriptPath != nullptr );
		}
		else
		{
			*(info.scriptPath) = itsScriptMenu->GetItemText(index);
		}

		JString nmShortcut;
		const bool ok = itsScriptMenu->GetItemNMShortcut(index, &nmShortcut);
		assert( ok );
		info.isUserScript = nmShortcut == JGetString("UserScriptMarker::FileTypeTable");
	}

	itsFileTypeList->SetItem(cell.y, info);
	TableRefreshRow(cell.y);
}

/******************************************************************************
 GetNewScriptDirectory (private)

 ******************************************************************************/

void
FileTypeTable::GetNewScriptDirectory()
{
	JString sysDir, userDir;
	if (!PrefsManager::GetScriptPaths(&sysDir, &userDir))
	{
		JGetUserNotification()->ReportError(JGetString("NewDirNoHome::FileTypeTable"));
		return;
	}

	if (!CreateDirectory(userDir))
	{
		return;
	}

	auto* dlog =
		jnew JXGetNewDirDialog(JGetString("NewDirTitle::FileTypeTable"),
							  JGetString("NewDirPrompt::FileTypeTable"), JString::empty, userDir);
	assert( dlog != nullptr );

	if (dlog->DoDialog())
	{
		const JString fullName = dlog->GetNewDirName();
		CreateDirectory(fullName);

		JPoint cell;
		const bool ok = GetTableSelection().GetFirstSelectedCell(&cell);
		assert( ok );

		PrefsManager::FileTypeInfo info = itsFileTypeList->GetItem(cell.y);

		JString path, name;
		JSplitPathAndName(fullName, &path, &name);
		if (info.scriptPath == nullptr)
		{
			info.scriptPath = jnew JString(name);
			assert( info.scriptPath != nullptr );
		}
		else
		{
			*info.scriptPath = name;
		}

		info.isUserScript = true;

		itsFileTypeList->SetItem(cell.y, info);
		TableRefreshRow(cell.y);
	}
}

/******************************************************************************
 CreateDirectory (private)

 ******************************************************************************/

bool
FileTypeTable::CreateDirectory
	(
	const JString& path
	)
	const
{
	const JError err = JCreateDirectory(path);
	if (!err.OK())
	{
		const JUtf8Byte* map[] =
		{
			"dir", path.GetBytes(),
			"err", err.GetMessage().GetBytes()
		};
		const JString msg = JGetString("UnableToCreateDir::FileTypeTable", map, sizeof(map));
		JGetUserNotification()->ReportError(msg);
	}
	return err.OK();
}

/******************************************************************************
 Macro menu functions (private)

 ******************************************************************************/

#define CreateMenuFn CreateMacroMenu
#define UpdateMenuFn UpdateMacroMenu
#define IDToIndexFn  MacroIDToMenuIndex
#define FindIDFn     FindMacroID
#define HandleMenuFn HandleMacroMenu
#define MenuVar      itsMacroMenu
#define DataList     itsMacroList
#define StructIDVar  macroID
#define EmptyDataID  kEmptyMacroID
#include "FTTHandleMenu.th"
#undef CreateMenuFn
#undef UpdateMenuFn
#undef IDToIndexFn
#undef FindIDFn
#undef HandleMenuFn
#undef MenuVar
#undef DataList
#undef StructIDVar
#undef EmptyDataID

/******************************************************************************
 CRM menu functions (private)

 ******************************************************************************/

#define CreateMenuFn CreateCRMMenu
#define UpdateMenuFn UpdateCRMMenu
#define IDToIndexFn  CRMIDToMenuIndex
#define FindIDFn     FindCRMRuleListID
#define HandleMenuFn HandleCRMMenu
#define MenuVar      itsCRMMenu
#define DataList     itsCRMList
#define StructIDVar  crmID
#define EmptyDataID  kEmptyCRMRuleListID
#include "FTTHandleMenu.th"
#undef CreateMenuFn
#undef UpdateMenuFn
#undef IDToIndexFn
#undef FindIDFn
#undef HandleMenuFn
#undef MenuVar
#undef DataList
#undef StructIDVar
#undef EmptyDataID

/******************************************************************************
 UpdateButtons (private)

 ******************************************************************************/

void
FileTypeTable::UpdateButtons()
{
	if ((GetTableSelection()).HasSelection())
	{
		itsRemoveTypeButton->Activate();
		itsDuplicateTypeButton->Activate();
	}
	else
	{
		itsRemoveTypeButton->Deactivate();
		itsDuplicateTypeButton->Deactivate();
	}
}

/******************************************************************************
 ReadGeometry

 ******************************************************************************/

void
FileTypeTable::ReadGeometry
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentGeometryDataVersion)
	{
		JCoordinate w;
		input >> w;
		SetColWidth(kSuffixColumn, w);
		input >> w;
		SetColWidth(kTypeColumn, w);
		input >> w;
		SetColWidth(kMacroColumn, w);
		if (vers >= 3)
		{
			input >> w;
			SetColWidth(kScriptColumn, w);
		}
		if (vers >= 1)
		{
			input >> w;
			SetColWidth(kCRMColumn, w);
		}
		input >> w;
		SetColWidth(kWrapColumn, w);
		if (vers >= 2)
		{
			input >> w;
			SetColWidth(kEditCmdColumn, w);
		}
	}

	JIgnoreUntil(input, kGeometryDataEndDelimiter);
}

/******************************************************************************
 WriteGeometry

 ******************************************************************************/

void
FileTypeTable::WriteGeometry
	(
	std::ostream& output
	)
	const
{
	output << kCurrentGeometryDataVersion;
	output << ' ' << GetColWidth(kSuffixColumn);
	output << ' ' << GetColWidth(kTypeColumn);
	output << ' ' << GetColWidth(kMacroColumn);
	output << ' ' << GetColWidth(kScriptColumn);
	output << ' ' << GetColWidth(kCRMColumn);
	output << ' ' << GetColWidth(kWrapColumn);
	output << ' ' << GetColWidth(kEditCmdColumn);
	output << kGeometryDataEndDelimiter;
}

/******************************************************************************
 SetColTitles

 ******************************************************************************/

void
FileTypeTable::SetColTitles
	(
	JXColHeaderWidget* widget
	)
	const
{
	widget->SetColumnTitles("FileTypeTable", kColCount);
}
