/******************************************************************************
 LibraryNode.h

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#ifndef _H_LibraryNode
#define _H_LibraryNode

#include "FileNodeBase.h"

class SubprojectConfigDialog;

class LibraryNode : public FileNodeBase
{
public:

	LibraryNode(ProjectTree* tree, const JString& fileName);
	LibraryNode(std::istream& input, const JFileVersion vers,
				  ProjectNode* parent);

	~LibraryNode() override;

	void	OpenFile() const override;
	void	OpenComplementFile() const override;
	void	EditSubprojectConfig();

	bool	OpenProject(const bool silent = false);

	void	StreamOut(std::ostream& output) const override;
	bool	IncludedInMakefile() const override;
	void	BuildMakeFiles(JString* text,
						   JPtrArray<JTreeNode>* invalidList,
						   JPtrArray<JString>* libFileList,
						   JPtrArray<JString>* libProjPathList) const override;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	bool	itsIncludeInDepListFlag;	// true => include in Make.files
	JString	itsProjFileName;			// can be relative
	bool	itsShouldBuildFlag;			// true => open and build sub-project

	SubprojectConfigDialog*	itsSubprojDialog;

private:

	void	LibraryNodeX();
	void	UpdateSubprojectConfig();
};

#endif
