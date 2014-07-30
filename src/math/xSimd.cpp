#include <xForm.h>

static xSIMDProcessor * processor = NULL;			// pointer to SIMD processor
static xSIMDProcessor * generic = NULL;				// pointer to generic SIMD implementation
xSIMDProcessor * xSIMD::Processor = NULL;

/*
================
xSIMD::Init
================
*/
bool xSIMD::Initialized = false;
void xSIMD::Init()
{
  if(Initialized)
    return;
  
  xHeap::Init();

	generic = new xSIMD_Generic;
	generic->cpuid = CPUID_GENERIC;
  ::processor = NULL;
	xSIMD::Processor = generic;
  
  atexit(Shutdown);
  Initialized = true;
}

struct __xSIMD_Init__
{
  __xSIMD_Init__(){ xSIMD::Init(); }
} __xSIMD_Init__;

/*
================
xSIMD::Shutdown
================
*/
void __cdecl xSIMD::Shutdown()
{
  if(!Initialized)
    return;

  if(::processor != generic)
    delete ::processor;

  delete generic;
	generic = NULL;
  ::processor = NULL;
	xSIMD::Processor = NULL;
  Initialized = false;
}

/*
============
xSIMD::InitProcessor
============
*/
void xSIMD::InitProcessor(const xString& module, bool forceGeneric)
{
	xSIMDProcessor *newProcessor;

	cpuid_t cpuid = CPUID_GENERIC; // ??? get cpuid this

	if (forceGeneric) {

		newProcessor = generic;

	} else {

		if (!processor) {
			if ((cpuid & CPUID_MMX) && (cpuid & CPUID_SSE) && (cpuid & CPUID_SSE2) && (cpuid & CPUID_SSE3)) {
				processor = new xSIMD_SSE3;
			} else if ((cpuid & CPUID_MMX) && (cpuid & CPUID_SSE) && (cpuid & CPUID_SSE2)) {
				processor = new xSIMD_SSE2;
			} else if ((cpuid & CPUID_MMX) && (cpuid & CPUID_SSE)) {
				processor = new xSIMD_SSE;
			} else if ((cpuid & CPUID_MMX) && (cpuid & CPUID_3DNOW)) {
				processor = new xSIMD_3DNow;
			} else if ((cpuid & CPUID_MMX)) {
				processor = new xSIMD_MMX;
			} else {
				processor = generic;
			}
			processor->cpuid = cpuid;
		}

		newProcessor = processor;
	}

	if (newProcessor != xSIMD::Processor) {
		xSIMD::Processor = newProcessor;
		xForm::x->Print(PT_Message, "%s using %s for SIMD processing\n", module, xSIMD::Processor->Name());
	}

	if (cpuid & CPUID_FTZ) {
		//idLib::sys->FPU_SetFTZ(true);
		//xForm::x->Print(PT_Message, "enabled Flush-To-Zero mode\n");
	}

	if (cpuid & CPUID_DAZ) {
		//idLib::sys->FPU_SetDAZ(true);
		//xForm::x->Print(PT_Message, "enabled Denormals-Are-Zero mode\n");
	}
}

