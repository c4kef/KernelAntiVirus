#include "NTUndocumented.h"
#include "ProcessLister.h"
#include "UserModeBridge.h"
#include <wdf.h>

DRIVER_INITIALIZE DriverEntry;

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	DbgPrintEx(0, 0, "Driver is unloaded!\n");
}

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(RegistryPath);

	DriverObject->DriverUnload = Unload;
	
	DbgPrintEx(0, 0, "Driver is loaded!\n");

	Dump((HANDLE)8452);

	return STATUS_SUCCESS;
}
