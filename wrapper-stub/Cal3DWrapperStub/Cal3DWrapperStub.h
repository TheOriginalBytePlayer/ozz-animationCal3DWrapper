// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the CAL3DWRAPPERSTUB_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// CAL3DWRAPPERSTUB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CAL3DWRAPPERSTUB_EXPORTS
#define CAL3DWRAPPERSTUB_API __declspec(dllexport)
#else
#define CAL3DWRAPPERSTUB_API __declspec(dllimport)
#endif

// This class is exported from the dll
class CAL3DWRAPPERSTUB_API CCal3DWrapperStub {
public:
	CCal3DWrapperStub(void);
	// TODO: add your methods here.
};

extern CAL3DWRAPPERSTUB_API int nCal3DWrapperStub;

CAL3DWRAPPERSTUB_API int fnCal3DWrapperStub(void);
