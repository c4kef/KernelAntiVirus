#include "NTUndocumented.h"
#include "ProcessLister.h"
#include "Utility.h"

static PSYSTEM_PROCESS_INFORMATION GetRawProcessList()
{
	ULONG bufferSize = 0;
	PVOID bufferPtr = NULL;

	if (ZwQuerySystemInformation(SystemProcessInformation, 0, bufferSize, &bufferSize) == STATUS_INFO_LENGTH_MISMATCH)
	{
		bufferPtr = ExAllocatePool(NonPagedPool, bufferSize);

		if (bufferPtr != NULL)
		{
			ZwQuerySystemInformation(SystemProcessInformation, bufferPtr, bufferSize, &bufferSize);
		}
	}
	return (PSYSTEM_PROCESS_INFORMATION)bufferPtr;
}

static PLDR_DATA_TABLE_ENTRY GetMainModuleDataTableEntry(PPEB64 peb)
{
	if (SanitizeUserPointer(peb, sizeof(PEB64)))
	{
		if (peb->Ldr)
		{
			if (SanitizeUserPointer(peb->Ldr, sizeof(PEB_LDR_DATA)))
			{
				if (!peb->Ldr->Initialized)
				{
					int initLoadCount = 0;

					while (!peb->Ldr->Initialized && initLoadCount++ < 4)
					{
						DriverSleep(250);
					}
				}

				if (peb->Ldr->Initialized)
				{
					return CONTAINING_RECORD(peb->Ldr->InLoadOrderModuleList.Flink, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
				}
			}
		}
	}
	return NULL;
}

NTSTATUS GetProcessList()
{
	PSYSTEM_PROCESS_INFORMATION rawProcessList = GetRawProcessList();

	if (rawProcessList)
	{
		while (rawProcessList->NextEntryOffset)
		{
			PEPROCESS targetProcess;
			PKAPC_STATE state = NULL;

			if (NT_SUCCESS(PsLookupProcessByProcessId(rawProcessList->UniqueProcessId, &targetProcess)))
			{
				PVOID mainModuleBase = NULL;
				PVOID mainModuleEntryPoint = NULL;
				UINT32 mainModuleImageSize = 0;
				PWCHAR mainModuleFileName = NULL;
				BOOLEAN isWow64 = 0;
				__try
				{
					KeStackAttachProcess(targetProcess, &state);

					__try
					{
						mainModuleBase = PsGetProcessSectionBaseAddress(targetProcess);
						if (mainModuleBase)
						{
							PPEB64 peb = (PPEB64)PsGetProcessPeb(targetProcess);
							if (peb)
							{
								PLDR_DATA_TABLE_ENTRY mainModuleEntry = GetMainModuleDataTableEntry(peb);

								mainModuleEntry = SanitizeUserPointer(mainModuleEntry, sizeof(LDR_DATA_TABLE_ENTRY));

								if (mainModuleEntry)
								{
									mainModuleEntryPoint = mainModuleEntry->EntryPoint;
									mainModuleImageSize = mainModuleEntry->SizeOfImage;

									PIMAGE_NT_HEADERS64 pp = (PIMAGE_NT_HEADERS64)(((CHAR*)mainModuleBase + ((PIMAGE_DOS_HEADER)mainModuleBase)->e_lfanew + PE_HEADER_MAGIC_OFFSET));

									DbgPrintEx(0, 0, "---------------------\n");
									DbgPrintEx(0, 0, "Signature: %lu\n", pp->Signature);
									DbgPrintEx(0, 0, "Sum: %lu\n", pp->OptionalHeader.CheckSum);
									DbgPrintEx(0, 0, "---------------------\n");

									isWow64 = IS_WOW64_PE(mainModuleBase);

									mainModuleFileName = ExAllocatePool(NonPagedPool, 256 * sizeof(WCHAR));
									RtlZeroMemory(mainModuleFileName, 256 * sizeof(WCHAR));
									RtlCopyMemory(mainModuleFileName, mainModuleEntry->FullDllName.Buffer, 256 * sizeof(WCHAR));
								}
							}
						}
					}
					__except (GetExceptionCode())
					{
						DbgPrintEx(0, 0, "Peb Interaction Failed.\n");
					}
				}
				__finally
				{
					KeUnstackDetachProcess(&state);
				}

				ObDereferenceObject(targetProcess);
			}

			rawProcessList = (PSYSTEM_PROCESS_INFORMATION)(((CHAR*)rawProcessList) + rawProcessList->NextEntryOffset);
		}
		return STATUS_SUCCESS;
	}
}