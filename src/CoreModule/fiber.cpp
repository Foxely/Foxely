
#include <iostream>
#include "library/library.h"
#include "value.hpp"
#include "foxely.h"

Value initNative(VM* pVM, int argCount, Value* args)
{
    Fox_FixArity(pVM, argCount, 1);
    Fox_PanicIfNot(pVM, Fox_IsClosure(args[0]), "Expected a function.");

    return Fox_Object(new_ref<ObjectFiber>(Fox_AsClosure(args[0])));
}

Value callNative(VM* pVM, int argCount, Value* args)
{
    Fox_Arity(pVM, argCount, 0, 1);

    ref<ObjectFiber> pFiber = Fox_AsFiber(args[-1]);

    bool hasValue = argCount > 0;

    // if (wrenHasError(fiber))
    //     RETURN_ERROR_FMT("Cannot $ an aborted fiber.", verb);

    // You can't call a called fiber, but you can transfer directly to it,
    // which is why this check is gated on `isCall`. This way, after resuming a
    // suspended fiber, it will run and then return to the fiber that called it
    // and so on.
    if (pFiber->m_pCaller != nullptr)
    {
        fprintf(stderr, "Fiber has already been called.\n");
        return Fox_Nil;
    }

    if (pFiber->m_pCaller != nullptr)
    {
        fprintf(stderr, "Fiber has already been called.\n");
        return Fox_Nil;
    }

    // if (pFiber->state == FIBER_ROOT) RETURN_ERROR("Cannot call root fiber.");
    
    // Remember who ran it.
    pFiber->m_pCaller = pVM->m_pCurrentFiber;

    if (pFiber->m_vFrames[0].closure->function->arity != argCount)
    {
        fprintf(stderr, "Fiber: <method call>: Expected %d arguments but got %d.\n",
        pFiber->m_vFrames[0].closure->function->arity, argCount);
        return Fox_Nil;
    }

    // When the calling fiber resumes, we'll store the result of the call in its
    // stack. If the call has two arguments (the fiber and the value), we only
    // need one slot for the result, so discard the other slot now.
    if (hasValue)
        pVM->m_pCurrentFiber->m_pStackTop--;

    if (pFiber->m_iFrameCount == 1 &&
        pFiber->m_vFrames[0].ip == pFiber->m_vFrames[0].closure->function->chunk.m_vCode.begin())
    {
        // The fiber is being started for the first time. If its function takes a
        // parameter, bind an argument to it.
        if (pFiber->m_vFrames[0].closure->function->arity == 1)
        {
            pFiber->m_pStackTop++;
            pVM->m_pCurrentFiber = pFiber;
            return hasValue ? args[0] : Fox_Nil;
        }
    }
    else
    {
        // Go to the first argument of the closure
        pFiber->m_pStackTop -= argCount + 1;
        // push the new arg in the first slot
        *pFiber->m_pStackTop = args[0];
        // advance all the arguments and add 2 more slots who it will be popped when exit this native function
        pFiber->m_pStackTop += argCount + 2;
    }

    pVM->m_pCurrentFiber = pFiber;
    return Fox_Nil;
}

Value yieldNative(VM* pVM, int argCount, Value* args)
{
    Fox_Arity(pVM, argCount, 0, 1);

    ref<ObjectFiber> pCurrent = pVM->m_pCurrentFiber;
    pVM->m_pCurrentFiber = pCurrent->m_pCaller;

    // Unhook this fiber from the one that called it.
    pCurrent->m_pCaller = nullptr;
    // current->state = FIBER_OTHER;

    if (pVM->m_pCurrentFiber != nullptr)
    {
        if (argCount == 0)
        {
            // Make the caller's run method return null.
            // pVM->m_pCurrentFiber->m_pStackTop[0] = Fox_Nil;
            return Fox_Nil;
        }
        else
        {
            // When the yielding fiber resumes, we'll store the result of the yield
            // call in its stack. Since Fiber.yield(value) has two arguments (the Fiber
            // class and the value) and we only need one slot for the result, discard
            // the other slot now.
            pCurrent->m_pStackTop--;
            pVM->m_pCurrentFiber->m_pStackTop++;
            return args[0];
        }
            // Make the caller's run method return the argument passed to yield.
            // pVM->m_pCurrentFiber->m_pStackTop[-1] = args[0];
    }
    return Fox_Nil;
}

Value abortNative(VM* pVM, int argCount, Value* args)
{
    Fox_Arity(pVM, argCount, 0, 1);
    Fox_PanicIfNot(pVM, Fox_IsString(args[0]), "Expect string type.");

    pVM->result = INTERPRET_ABORT;
    
    if (argCount == 1)
        pVM->PrintError(pVM->m_pCurrentFiber.get(), Fox_AsCString(args[0]));
    else
        pVM->PrintError(pVM->m_pCurrentFiber.get(), "");


    return Fox_Nil;
}

void DefineCoreFiber(VM* pVM)
{
    NativeMethods oMethods =
	{
		std::make_pair<std::string, NativeFn>("new", initNative),
		std::make_pair<std::string, NativeFn>("yield", yieldNative),
		std::make_pair<std::string, NativeFn>("abort", abortNative),
	};

    NativeMethods oBuiltInMethods =
	{
		std::make_pair<std::string, NativeFn>("call", callNative),
	};

    pVM->DefineLib("core", "Fiber", oMethods);
    pVM->DefineBuiltIn(pVM->fiberMethods, oBuiltInMethods);
}