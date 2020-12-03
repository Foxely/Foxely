
#include <iostream>
#include "library/library.h"
#include "value.hpp"
#include "foxely.h"

Value initNative(VM* pVM, int argCount, Value* args)
{
    Fox_FixArity(pVM, argCount, 1);
    Fox_PanicIfNot(pVM, Fox_IsClosure(args[0]), "Expected a function.");

    return Fox_Object(pVM->gc.New<ObjectFiber>(Fox_AsClosure(args[0])));
}

Value callNative(VM* pVM, int argCount, Value* args)
{
    Fox_Arity(pVM, argCount, 0, 1);

    ObjectFiber* pFiber = Fox_AsFiber(args[-1]);

    std::cerr << "Call1" << std::endl;

    // if (wrenHasError(fiber))
    //     RETURN_ERROR_FMT("Cannot $ an aborted fiber.", verb);

    // if (isCall)
    // {
        // You can't call a called fiber, but you can transfer directly to it,
        // which is why this check is gated on `isCall`. This way, after resuming a
        // suspended fiber, it will run and then return to the fiber that called it
        // and so on.
        if (pFiber->m_pCaller != nullptr) //RETURN_ERROR("Fiber has already been called.");
            return Fox_Nil;

        // if (pFiber->state == FIBER_ROOT) RETURN_ERROR("Cannot call root fiber.");
        
        // Remember who ran it.
        pFiber->m_pCaller = pVM->m_pCurrentFiber;
    // }

    if (pFiber->m_iFrameCount == 0)
        return Fox_Nil;
        // RETURN_ERROR_FMT("Cannot $ a finished fiber.", verb);

    // When the calling fiber resumes, we'll store the result of the call in its
    // stack. If the call has two arguments (the fiber and the value), we only
    // need one slot for the result, so discard the other slot now.
    // if (hasValue) vm->fiber->m_pStackTop--;

    if (pFiber->m_iFrameCount == 1 &&
        pFiber->m_vFrames[0].ip == pFiber->m_vFrames[0].closure->function->chunk.m_vCode.begin())
    {
        // The fiber is being started for the first time. If its function takes a
        // parameter, bind an argument to it.
        if (pFiber->m_vFrames[0].closure->function->arity == 1)
        {
            // pFiber->m_pStackTop[0] = hasValue ? args[1] : Fox_Nil;
            pFiber->m_pStackTop[0] = Fox_Nil;
            pFiber->m_pStackTop++;
        }
    }
    else
    {
        // The fiber is being resumed, make yield() or transfer() return the result.
        pFiber->m_pStackTop[-1] = Fox_Nil;
        // pFiber->m_pStackTop[-1] = hasValue ? args[1] : Fox_Nil;
    }

    pVM->m_pCurrentFiber = pFiber;
    // pVM->run(pFiber);
    std::cerr << "Call End" << std::endl;
    return Fox_Nil;
}

void DefineCoreFiber(VM* pVM)
{
    NativeMethods oMethods =
	{
		std::make_pair<std::string, NativeFn>("init", initNative),
	};

    NativeMethods oBuiltInMethods =
	{
		std::make_pair<std::string, NativeFn>("call", callNative),
	};

    pVM->DefineClass("core", "Fiber", oMethods);
    pVM->DefineBuiltIn(pVM->fiberMethods, oBuiltInMethods);
}