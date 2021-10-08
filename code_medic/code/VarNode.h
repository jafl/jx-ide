/******************************************************************************
 VarNode.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_VarNode
#define _H_VarNode

#include <jx-af/jcore/JNamedTreeNode.h>
#include <jx-af/jcore/JFontStyle.h>

class VarCmd;

class VarNode : public JNamedTreeNode
{
	friend class InitVarNodeTask;

public:

	VarNode(const bool shouldUpdate = true);
	VarNode(JTreeNode* parent, const JString& name, const JString& value);

	~VarNode();

	bool			ValueIsValid() const;
	virtual JString	GetFullName(bool* isPointer = nullptr) const = 0;
	const JString&	GetValue() const;
	void			SetValue(const JString& value);
	void			SetValid(const bool valid);
	JFontStyle		GetFontStyle();

	static JFontStyle	GetFontStyle(const bool isValid, const bool isNew);

	JSize	GetBase() const;
	void	SetBase(const JSize base);

	bool	IsPointer() const;
	void	MakePointer(const bool pointer, const bool adjustOpenable = true);

	bool	OKToOpen() const override;

	void	ShouldUpdate(const bool update);

	static bool	ShouldUpdate(const Message& message);

	void	GetLastChild(VarNode** child);

	VarNode*		GetVarParent();
	const VarNode*	GetVarParent() const;
	bool			GetVarParent(VarNode** parent);
	bool			GetVarParent(const VarNode** parent) const;

	VarNode*		GetVarChild(const JIndex index);
	const VarNode*	GetVarChild(const JIndex index) const;

	static void	TrimExpression(JString* s);

	// called by GetLocalVarsCmd

	void	UpdateValue();
	void	UpdateValue(VarNode* root);
	void	UpdateFailed(const JString& data);

protected:

	void	NameChanged() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

	JString	GetFullNameForCFamilyLanguage(bool* isPointer) const;
	JString	GetPathForCFamilyLanguage() const;

private:

	const bool		itsShouldListenToLinkFlag;
	VarCmd*	itsValueCommand;
	JString			itsValue;
	bool			itsIsPointerFlag;
	bool			itsValidFlag;
	bool			itsNewValueFlag;
	VarCmd*	itsContentCommand;

	// used for base conversion

	JSize		itsBase;		// 0 => default, 1 => ASCII
	bool		itsCanConvertBaseFlag;
	JString*	itsOrigValue;

	// optimized updating

	bool	itsShouldUpdateFlag;
	bool	itsNeedsUpdateFlag;

private:

	void	VarTreeNodeX();

	void	UpdateChildren();
	void	Update(VarNode* node);
	void	UpdateContent();
	void	StealChildren(VarNode* node);
	void	MergeChildren(VarNode* node);

	void	ConvertToBase();

	bool	SameElements(const VarNode* node) const;
};


/******************************************************************************
 ValueIsValid

 ******************************************************************************/

inline bool
VarNode::ValueIsValid()
	const
{
	return itsValidFlag;
}

/******************************************************************************
 GetValue

 ******************************************************************************/

inline const JString&
VarNode::GetValue()
	const
{
	return itsValue;
}

/******************************************************************************
 IsPointer

 ******************************************************************************/

inline bool
VarNode::IsPointer()
	const
{
	return itsIsPointerFlag;
}

/******************************************************************************
 Base conversion

	Zero means don't perform any conversion.

 ******************************************************************************/

inline JSize
VarNode::GetBase()
	const
{
	return itsBase;
}

inline void
VarNode::SetBase
	(
	const JSize base
	)
{
	itsBase = base;
	ConvertToBase();
}

#endif
