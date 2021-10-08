/******************************************************************************
 FileTypeTable.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_FileTypeTable
#define _H_FileTypeTable

#include <jx-af/jx/JXEditTable.h>
#include "PrefsManager.h"		// need definition of FileTypeInfo

class JXTextButton;
class JXTextMenu;
class JXInputField;
class JXColHeaderWidget;
class JXGetNewDirDialog;

class FileTypeTable : public JXEditTable
{
public:

	FileTypeTable(const JArray<PrefsManager::FileTypeInfo>& fileTypeList,
					const JArray<PrefsManager::MacroSetInfo>& macroList,
					const JArray<PrefsManager::CRMRuleListInfo>& crmList,
					JXTextButton* addTypeButton, JXTextButton* removeTypeButton,
					JXTextButton* duplicateTypeButton,
					JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	~FileTypeTable() override;

	void	GetFileTypeList(JArray<PrefsManager::FileTypeInfo>* fileTypeList) const;

	void	ReadGeometry(std::istream& input);
	void	WriteGeometry(std::ostream& output) const;
	void	SetColTitles(JXColHeaderWidget* widget) const;

	bool	IsEditable(const JPoint& cell) const override;

protected:

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;

	void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	JXInputField*	CreateXInputField(const JPoint& cell,
									  const JCoordinate x, const JCoordinate y,
									  const JCoordinate w, const JCoordinate h) override;
	bool			ExtractInputData(const JPoint& cell) override;
	void			PrepareDeleteXInputField() override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JArray<PrefsManager::FileTypeInfo>*				itsFileTypeList;	// info.*Regex always nullptr
	const JArray<PrefsManager::MacroSetInfo>&		itsMacroList;
	const JArray<PrefsManager::CRMRuleListInfo>&	itsCRMList;

	JXInputField*	itsTextInput;			// nullptr unless editing
	JFont			itsFont;
	JRegex*			itsTestRegex;

	JXTextMenu*		itsTypeMenu;
	JXTextMenu*		itsMacroMenu;
	JXTextMenu*		itsScriptMenu;
	JXTextMenu*		itsCRMMenu;

	JXTextButton*	itsAddTypeButton;
	JXTextButton*	itsRemoveTypeButton;
	JXTextButton*	itsDuplicateTypeButton;

	JXGetNewDirDialog*	itsNewDirDialog;

private:

	void	CleanOutFileTypeList(JArray<PrefsManager::FileTypeInfo>* fileTypeList) const;
	void	FinishFileTypeListCopy(JArray<PrefsManager::FileTypeInfo>* fileTypeList) const;

	void	AddType();
	void	RemoveType();
	void	DuplicateType();

	void	UpdateTypeMenu();
	void	HandleTypeMenu(const JIndex index);

	JXTextMenu*	CreateMacroMenu();
	void		UpdateMacroMenu();
	void		HandleMacroMenu(const JIndex index);
	JIndex		MacroIDToMenuIndex(const JIndex id) const;

	void	UpdateScriptMenu();
	void	HandleScriptMenu(const JIndex index);
	void	BuildScriptMenuItems(const JString& path, const bool isUserPath,
								 JPtrArray<JString>* menuText) const;

	void	GetNewScriptDirectory();
	void	CreateNewScriptDirectory();
	bool	CreateDirectory(const JString& path) const;

	JXTextMenu*	CreateCRMMenu();
	void		UpdateCRMMenu();
	void		HandleCRMMenu(const JIndex index);
	JIndex		CRMIDToMenuIndex(const JIndex id) const;

	void	UpdateButtons();
};

#endif
