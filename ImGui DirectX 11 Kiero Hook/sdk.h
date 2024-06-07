#include <windows.h>
#include <iostream>


enum ERemovalReason
{
	REMOVE_PLAYER_DEFAULT = 0,
	REMOVE_PLAYER_CONNECTION_LOST = 1,
	REMOVE_PLAYER_KICKED = 2,
	REMOVE_PLAYER_BANNED = 3,
};

const struct CString
{
	std::basic_string<char, std::char_traits<char>, std::allocator<char> > _str;

	std::string getString() const {
		return _str;
	}
};

const struct CCountryTag
{
	int _nLinkIndex;
};

struct COption
{
	bool _bIsSelected;
};

struct CPersistentVtbl
{
	/*void* (__fastcall* __vecDelDtor)(unsigned int);
	void(__fastcall* Write)(CWriter*);
	void(__fastcall* WriteMembers)(CWriter*);
	void(__fastcall* Read)(CReader*);
	void(__fastcall* ReadMember)(CReader*, int);
	void(__fastcall* InitPostRead)(CString*, int, int);
	void(__fastcall* InitPostRead)();*/
};

struct CPersistent
{
	CPersistentVtbl* vfptr;
	int _TypeToken;
};

struct CCommand : CPersistent
{
	bool _bLogged;
	int _nCommandSender; 
	unsigned int _nRecipient; 
	unsigned __int16 _nRecipientPort;
	unsigned __int16 _nTickStamp;
	bool _bTargetedAsynchronous;
	bool _bMayBeRemoved;
	unsigned int _nIdentity;
};

struct CAddPlayerCommand : CCommand
{
	CString* _User;
	CString* _Name;
	DWORD _Null;
	int _nMachineId;
	bool _bHotjoin;
	__int64 a7;
};

struct CRemovePlayerCommand : CCommand 
{
	int _nMachineId;
	int unknown;
	ERemovalReason eReason;
};


struct CStartGameCommand : CCommand
{

};

struct CNameChange : CCommand
{
	__int64 _Name;
};


