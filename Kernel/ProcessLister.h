#pragma once
#include <ntddk.h>

typedef struct _COPY_MEMORY
{
	ULONGLONG localbuf;         // Buffer address
	ULONGLONG targetPtr;        // Target address
	ULONGLONG size;             // Buffer size
	BOOLEAN   write;            // TRUE if write operation, FALSE if read
} COPY_MEMORY, * PCOPY_MEMORY;

typedef struct _MM_AVL_NODE // Size=24
{
	struct _MM_AVL_NODE* LeftChild; // Size=8 Offset=0
	struct _MM_AVL_NODE* RightChild; // Size=8 Offset=8

	union ___unnamed1666 // Size=8
	{
		struct
		{
			__int64 Balance : 2; // Size=8 Offset=0 BitOffset=0 BitCount=2
		};
		struct _MM_AVL_NODE* Parent; // Size=8 Offset=0
	} u1;
} MM_AVL_NODE, * PMM_AVL_NODE, * PMMADDRESS_NODE;

typedef struct _RTL_AVL_TREE // Size=8
{
	PMM_AVL_NODE BalancedRoot;
	void* NodeHint;
	unsigned __int64 NumberGenericTableElements;
} RTL_AVL_TREE, * PRTL_AVL_TREE, MM_AVL_TABLE, * PMM_AVL_TABLE;

union _EX_PUSH_LOCK // Size=8
{
	struct
	{
		unsigned __int64 Locked : 1; // Size=8 Offset=0 BitOffset=0 BitCount=1
		unsigned __int64 Waiting : 1; // Size=8 Offset=0 BitOffset=1 BitCount=1
		unsigned __int64 Waking : 1; // Size=8 Offset=0 BitOffset=2 BitCount=1
		unsigned __int64 MultipleShared : 1; // Size=8 Offset=0 BitOffset=3 BitCount=1
		unsigned __int64 Shared : 60; // Size=8 Offset=0 BitOffset=4 BitCount=60
	};
	unsigned __int64 Value; // Size=8 Offset=0
	void* Ptr; // Size=8 Offset=0
};

struct _MMVAD_FLAGS // Size=4
{
	unsigned long VadType : 3; // Size=4 Offset=0 BitOffset=0 BitCount=3
	unsigned long Protection : 5; // Size=4 Offset=0 BitOffset=3 BitCount=5
	unsigned long PreferredNode : 6; // Size=4 Offset=0 BitOffset=8 BitCount=6
	unsigned long NoChange : 1; // Size=4 Offset=0 BitOffset=14 BitCount=1
	unsigned long PrivateMemory : 1; // Size=4 Offset=0 BitOffset=15 BitCount=1
	unsigned long Teb : 1; // Size=4 Offset=0 BitOffset=16 BitCount=1
	unsigned long PrivateFixup : 1; // Size=4 Offset=0 BitOffset=17 BitCount=1
	unsigned long ManySubsections : 1; // Size=4 Offset=0 BitOffset=18 BitCount=1
	unsigned long Spare : 12; // Size=4 Offset=0 BitOffset=19 BitCount=12
	unsigned long DeleteInProgress : 1; // Size=4 Offset=0 BitOffset=31 BitCount=1
};
struct _MMVAD_FLAGS1 // Size=4
{
	unsigned long CommitCharge : 31; // Size=4 Offset=0 BitOffset=0 BitCount=31
	unsigned long MemCommit : 1; // Size=4 Offset=0 BitOffset=31 BitCount=1
};

union ___unnamed1951 // Size=4
{
	unsigned long LongFlags; // Size=4 Offset=0
	struct _MMVAD_FLAGS VadFlags; // Size=4 Offset=0
};

union ___unnamed1952 // Size=4
{
	unsigned long LongFlags1; // Size=4 Offset=0
	struct _MMVAD_FLAGS1 VadFlags1; // Size=4 Offset=0
};

typedef struct _MMVAD_SHORT // Size=64
{
	union
	{
		struct _RTL_BALANCED_NODE VadNode; // Size=24 Offset=0
		struct _MMVAD_SHORT* NextVad; // Size=8 Offset=0
	};
	unsigned long StartingVpn; // Size=4 Offset=24
	unsigned long EndingVpn; // Size=4 Offset=28
	unsigned char StartingVpnHigh; // Size=1 Offset=32
	unsigned char EndingVpnHigh; // Size=1 Offset=33
	unsigned char CommitChargeHigh; // Size=1 Offset=34
	unsigned char SpareNT64VadUChar; // Size=1 Offset=35
	long ReferenceCount; // Size=4 Offset=36
	union _EX_PUSH_LOCK PushLock; // Size=8 Offset=40
	union ___unnamed1951 u; // Size=4 Offset=48
	union ___unnamed1952 u1; // Size=4 Offset=52
	struct _MI_VAD_EVENT_BLOCK* EventList; // Size=8 Offset=56
} MMVAD_SHORT, * PMMVAD_SHORT;

#define GET_VAD_ROOT(Table) Table->BalancedRoot

NTSTATUS Dump(HANDLE idProc);