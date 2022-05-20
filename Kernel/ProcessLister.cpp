#include "NTUndocumented.h"
#include "ProcessLister.h"
#include "Utility.h"
#include "vector.hpp"


TABLE_SEARCH_RESULT EnumerateNodes(IN PMM_AVL_TABLE Table, OUT PMMADDRESS_NODE* NodeOrParent)
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

		if (RightChild != NULL) {
			arr.push_back(RightChild->RightChild);
			arr.push_back(RightChild->LeftChild);

			PMMVAD_SHORT VpnCompare = (PMMVAD_SHORT)RightChild;
			ULONG_PTR startVpn = VpnCompare->StartingVpn;
			ULONG_PTR endVpn = VpnCompare->EndingVpn;

			startVpn |= (ULONG_PTR)VpnCompare->StartingVpnHigh << 32;
			endVpn |= (ULONG_PTR)VpnCompare->EndingVpnHigh << 32;

			DbgPrintEx(0, 0, "[%p] -> StartVpn: %Iu     EndVpn: %Iu      Protection: %i\n", (PUCHAR)RightChild, startVpn, endVpn, VpnCompare->u.VadFlags.Protection);
		}

		if (LeftChild != NULL) {
			arr.push_back(LeftChild->RightChild);
			arr.push_back(LeftChild->LeftChild);

			PMMVAD_SHORT VpnCompare = (PMMVAD_SHORT)LeftChild;
			ULONG_PTR startVpn = VpnCompare->StartingVpn;
			ULONG_PTR endVpn = VpnCompare->EndingVpn;

			startVpn |= (ULONG_PTR)VpnCompare->StartingVpnHigh << 32;
			endVpn |= (ULONG_PTR)VpnCompare->EndingVpnHigh << 32;

			DbgPrintEx(0, 0, "[%p] -> StartVpn: %Iu     EndVpn: %Iu      Protection: %i\n", (PUCHAR)LeftChild, startVpn, endVpn, VpnCompare->u.VadFlags.Protection);
		}
	};
}


NTSTATUS Dump(HANDLE idProc)
{
	PEPROCESS targetProcess;
	if (NT_SUCCESS(PsLookupProcessByProcessId(idProc, &targetProcess)))
	{
		PMM_AVL_TABLE pTable = (PMM_AVL_TABLE)((PUCHAR)targetProcess + 0x7d8);
		PMM_AVL_NODE pNode = GET_VAD_ROOT(pTable);
		EnumerateNodes(pTable, &pNode);
		ObDereferenceObject(targetProcess);
	}
	return STATUS_SUCCESS;
}