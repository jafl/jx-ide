/******************************************************************************
 FileNameDisplay.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_FileNameDisplay
#define _H_FileNameDisplay

#include <jx-af/jx/JXFileInput.h>

class TextDocument;
class FileDragSource;

class FileNameDisplay : public JXFileInput
{
public:

	FileNameDisplay(TextDocument* doc, FileDragSource* dragSource,
					  JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~FileNameDisplay();

	void	SetTE(JXTEBase* te);
	void	DiskCopyIsModified(const bool mod);

	virtual void		HandleKeyPress(const JUtf8Character& c, const int keySym,
									   const JXKeyModifiers& modifiers) override;
	virtual bool	InputValid() override;

protected:

	class StyledText : public JXFileInput::StyledText
	{
		public:

		StyledText(FileNameDisplay* field, JFontManager* fontManager)
			:
			JXFileInput::StyledText(field, fontManager),
			itsCBField(field)
		{ };

		protected:

		virtual void	AdjustStylesBeforeBroadcast(
							const JString& text, JRunArray<JFont>* styles,
							JStyledText::TextRange* recalcRange,
							JStyledText::TextRange* redrawRange,
							const bool deletion) override;

		private:

		FileNameDisplay*	itsCBField;
	};

protected:

	virtual void	HandleFocusEvent() override;
	virtual void	HandleUnfocusEvent() override;

private:

	enum Action
	{
		kCancel,
		kSaveAs,
		kRename
	};

private:

	JXTEBase*	itsTE;					// not owned
	JString		itsOrigFile;
	Action		itsUnfocusAction;

	TextDocument*		itsDoc;			// not owned
	FileDragSource*	itsDragSource;	// not owned
	bool			itsDiskModFlag;
	bool			itsCBHasFocusFlag;

private:

	void	UpdateDisplay(const bool hasFocus);

	// not allowed

	FileNameDisplay(const FileNameDisplay& source);
	const FileNameDisplay& operator=(const FileNameDisplay& source);
};

#endif
