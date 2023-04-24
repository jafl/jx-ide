/******************************************************************************
 DiffDocument.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_DiffDocument
#define _H_DiffDocument

#include "TextDocument.h"
#include "DiffFileDialog.h"	// for DiffType
#include <jx-af/jcore/JError.h>

class JXTextMenu;
class JXTextButton;
class DiffEditor;

class DiffDocument : public TextDocument
{
public:

	static JError	CreatePlain(const JString& fullName, const JString& cmd,
								const JFontStyle& defStyle,
								const JString& name1, const JFontStyle& removeStyle,
								const JString& name2, const JFontStyle& insertStyle,
								const bool silent = false,
								DiffDocument* origDoc = nullptr);
	static JError	CreateCVS(const JString& fullName, const JString& getCmd,
							  const JString& diffCmd, const JFontStyle& defStyle,
							  const JString& name1, const JFontStyle& removeStyle,
							  const JString& name2, const JFontStyle& insertStyle,
							  const bool silent = false,
							  DiffDocument* origDoc = nullptr);
	static JError	CreateSVN(const JString& fullName, const JString& getCmd,
							  const JString& diffCmd, const JFontStyle& defStyle,
							  const JString& name1, const JFontStyle& removeStyle,
							  const JString& name2, const JFontStyle& insertStyle,
							  const bool silent = false,
							  DiffDocument* origDoc = nullptr);
	static JError	CreateGit(const JString& fullName,
							  const JString& get1Cmd, const JString& get2Cmd,
							  const JString& diffCmd, const JFontStyle& defStyle,
							  const JString& name1, const JFontStyle& removeStyle,
							  const JString& name2, const JFontStyle& insertStyle,
							  const bool silent = false,
							  DiffDocument* origDoc = nullptr);

	~DiffDocument() override;

protected:

	enum Type
	{
		kPlainType,
		kCVSType,
		kSVNType,
		kGitType
	};

protected:

	DiffDocument(const Type type, const JString& fullName,
				 const JString& getCmd,
				 const JString& diffCmd, const JFontStyle& defStyle,
				 const JString& name1, const JFontStyle& removeStyle,
				 const JString& name2, const JFontStyle& insertStyle);

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	const Type		itsType;
	const JString	itsFullName;
	const JString	itsGetCmd;
	const JString	itsDiffCmd;
	const JString	itsName1;
	const JString	itsName2;

	const JFontStyle	itsDefaultStyle;
	const JFontStyle	itsRemoveStyle;
	const JFontStyle	itsInsertStyle;

	DiffEditor*		itsDiffEditor;
	JXTextMenu*		itsDiffMenu;
	JXTextButton*	itsDiffButton;

private:

	static JError	CreateOutputFiles(JString* tempFileName, int* tempFileFD,
									  JString* errFileName, int* errFileFD);
	static JError	FillOutputFile(const JString& path, const JString& cmd,
								   JString* resultFullName);

	void	Init(const JString& fullName);

	void	ReadDiff(std::istream& input);

	void	UpdateDiffMenu();
	void	HandleDiffMenu(const JIndex index);

	static TextEditor*	ConstructDiffEditor(TextDocument* document,
											const JString& fileName,
											JXMenuBar* menuBar,
											TELineIndexInput* lineInput,
											TEColIndexInput* colInput,
											JXScrollbarSet* scrollbarSet);

public:

	// JError classes

	class DiffFailed : public JError
		{
		public:

			DiffFailed(const JString& s, const bool isFileName);
			DiffFailed(const JError& err);

		private:

			void	SetMessage(const JString& msg);
		};
};

#endif
