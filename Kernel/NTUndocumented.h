#pragma once
#include <ntddk.h>

typedef struct _KAPC_STATE {
	LIST_ENTRY ApcListHead[MaximumMode];
	struct _KPROCESS *Process;
	BOOLEAN KernelApcInProgress;
	BOOLEAN KernelApcPending;
	BOOLEAN UserApcPending;
} KAPC_STATE, *PKAPC_STATE, *PRKAPC_STATE;

typedef enum _SYSTEM_INFORMATION_CLASS
{
	SystemProcessInformation = 5
} SYSTEM_INFORMATION_CLASS;

typedef enum _MEMORY_INFORMATION_CLASS
{
	MemoryBasicInformation,
	MemoryWorkingSetInformation,
	MemoryMappedFilenameInformation,
	MemoryRegionInformation,
	MemoryWorkingSetExInformation

} MEMORY_INFORMATION_CLASS;


typedef struct _MEMORY_BASIC_INFORMATION {
	PVOID  BaseAddress;
	PVOID  AllocationBase;
	INT32  AllocationProtect;
	SIZE_T RegionSize;
	INT32  State;
	INT32  Protect;
	INT32  Type;
} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;


extern "C" __declspec(dllimport) NTKERNELAPI NTSTATUS IoCreateDriver(IN PUNICODE_STRING DriverName, OPTIONAL IN PDRIVER_INITIALIZE InitializationFunction);

extern "C" __declspec(dllimport) NTKERNELAPI VOID KeStackAttachProcess(__inout struct _KPROCESS * PROCESS, __out PRKAPC_STATE ApcState);

extern "C" __declspec(dllimport) NTKERNELAPI VOID KeUnstackDetachProcess(__in PRKAPC_STATE ApcState);

extern "C" __declspec(dllimport) NTKERNELAPI NTSTATUS NTAPI MmCopyVirtualMemory(IN PEPROCESS FromProcess, IN PVOID FromAddress, IN PEPROCESS ToProcess, OUT PVOID ToAddress, IN SIZE_T BufferSize, IN KPROCESSOR_MODE PreviousMode, OUT PSIZE_T NumberOfBytesCopied);

extern "C" __declspec(dllimport) NTSYSAPI NTSTATUS NTAPI ZwQuerySystemInformation(IN SYSTEM_INFORMATION_CLASS SystemInformationClass, OUT PVOID SystemInformation, IN ULONG SystemInformationLength, OUT PULONG ReturnLength OPTIONAL);

extern "C" __declspec(dllimport) NTSYSAPI NTSTATUS NTAPI ZwQueryVirtualMemory(IN HANDLE ProcessHandle, IN PVOID BaseAddress, IN MEMORY_INFORMATION_CLASS MemoryInformationClass, OUT PVOID MemoryInformation, IN SIZE_T MemoryInformationLength, OUT PSIZE_T ReturnLength OPTIONAL);

extern "C" __declspec(dllimport) NTKERNELAPI NTSTATUS PsLookupProcessByProcessId(IN HANDLE ProcessId, OUT PEPROCESS *Process);

extern "C" __declspec(dllimport) NTKERNELAPI PVOID PsGetProcessSectionBaseAddress(__in PEPROCESS Process);

extern "C" __declspec(dllimport) NTKERNELAPI PPEB NTAPI PsGetProcessPeb(IN PEPROCESS Process);