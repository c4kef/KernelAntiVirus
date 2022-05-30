#include "NTUndocumented.h"
#include "ProcessLister.h"
#include "Utility.h"
#include "vector.hpp"

NTSTATUS ReadMemory(IN PCOPY_MEMORY pCopy)
{
	NTSTATUS status = STATUS_SUCCESS;

	PEPROCESS process = NULL;

	if (NT_SUCCESS(status))
	{
		__try {
			ProbeForRead((PVOID)pCopy->targetPtr, pCopy->size, 1);
			RtlCopyMemory((PVOID)pCopy->localbuf, (PVOID)pCopy->targetPtr, pCopy->size);
			//DbgPrintEx(0, 0, "Val: %p\n", (PVOID)pCopy->localbuf);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			DbgPrintEx(0, 0, "Address %p isn't accessible.\n", pCopy->targetPtr);
			status = STATUS_SEVERITY_INFORMATIONAL;
		}
	}

	return status;
}

TABLE_SEARCH_RESULT EnumerateNodes(IN ULONG_PTR baseAddress, IN PMM_AVL_TABLE Table, OUT PMMADDRESS_NODE* NodeOrParent)
{
	PMMADDRESS_NODE NodeToExamine;
	
	if (Table->NumberGenericTableElements == 0) {
		return TableEmptyTree;
	}

	NodeToExamine = (PMMADDRESS_NODE)GET_VAD_ROOT(Table);

	ktd::vector<PMMADDRESS_NODE, PagedPool, 'vten'> arr;
	arr.push_back(NodeToExamine->RightChild);
	arr.push_back(NodeToExamine->LeftChild);

	INT index = 0;
	for (;;) {

		if (arr.size() < index)
		{
			arr.Clear();
			DbgPrintEx(0, 0, "End array\n");
			return TableInsertAsRight;
		}

		PMMADDRESS_NODE node = (arr)[index++];

		if (node == NULL)
			continue;

		PMMADDRESS_NODE RightChild = node->RightChild;
		PMMADDRESS_NODE LeftChild = node->LeftChild;

		COPY_MEMORY mem;

		if (RightChild != NULL) {
			arr.push_back(RightChild->RightChild);
			arr.push_back(RightChild->LeftChild);

			PMMVAD_SHORT VpnCompare = (PMMVAD_SHORT)RightChild;
			ULONG_PTR startVpn = VpnCompare->StartingVpn;
			ULONG_PTR endVpn = VpnCompare->EndingVpn;
			ULONG_PTR computesPages = 0;

			startVpn |= (ULONG_PTR)VpnCompare->StartingVpnHigh << 32;
			endVpn |= (ULONG_PTR)VpnCompare->EndingVpnHigh << 32;

			computesPages = endVpn - startVpn + 0x1;

			ULONG_PTR val = computesPages * 0x1000;//0x1000 - size of page based system intel 20xx


			DbgPrintEx(0, 0, "[%p] -> StartVpn: %p     EndVpn: %p      Protection: %i        Pointer: %p,%p\n", (PUCHAR)RightChild, startVpn, endVpn, VpnCompare->u.VadFlags.Protection, baseAddress, val);
		}

		if (LeftChild != NULL) {
			arr.push_back(LeftChild->RightChild);
			arr.push_back(LeftChild->LeftChild);

			PMMVAD_SHORT VpnCompare = (PMMVAD_SHORT)LeftChild;
			ULONG_PTR startVpn = VpnCompare->StartingVpn;
			ULONG_PTR endVpn = VpnCompare->EndingVpn;
			ULONG_PTR computesPages = 0;

			startVpn |= (ULONG_PTR)VpnCompare->StartingVpnHigh << 32;
			endVpn |= (ULONG_PTR)VpnCompare->EndingVpnHigh << 32;

			computesPages = endVpn - startVpn + 0x1;

			ULONG_PTR val = computesPages * 0x1000;//0x1000 - size of page based system intel 20xx

			DbgPrintEx(0, 0, "[%p] -> StartVpn: %p     EndVpn: %p      Protection: %i        Pointer: %p,%p\n", (PUCHAR)LeftChild, startVpn, endVpn, VpnCompare->u.VadFlags.Protection, baseAddress, val);
		}
	};
}

NTSTATUS Dump(HANDLE idProc)
{
	PEPROCESS targetProcess;
	KAPC_STATE state;

	if (NT_SUCCESS(PsLookupProcessByProcessId(idProc, &targetProcess)))
	{
		PsLookupProcessByProcessId(idProc, &targetProcess);
		KeStackAttachProcess((_KPROCESS*)targetProcess, &state);

		ULONG_PTR baseAddress = (ULONG_PTR)PsGetProcessSectionBaseAddress(targetProcess);
		PMM_AVL_TABLE pTable = (PMM_AVL_TABLE)((PUCHAR)targetProcess + 0x7d8);
		PMM_AVL_NODE pNode = GET_VAD_ROOT(pTable);
		
		EnumerateNodes(baseAddress, pTable, &pNode);

		KeUnstackDetachProcess(&state);
		ObDereferenceObject(targetProcess);
	}

	return STATUS_SUCCESS;
}