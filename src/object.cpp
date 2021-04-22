#include "object.hpp"
#include "Parser.h"
#include "vm.hpp"
#include "gc.hpp"

ObjectClosure::ObjectClosure(VM* oVM, ObjectFunction* func)
{
    type = OBJ_CLOSURE;
    function = func;
    upvalueCount = func->upValueCount;
    func->module = oVM->currentModule;
    upValues = std::vector<ObjectUpvalue*>(func->upValueCount);
    for (int i = 0; i < func->upValueCount; i++) {
        upValues[i] = nullptr;
    }
}

void ObjectInstance::on_destroy()
{
    scope<ObjectString> obj = new_scope<ObjectString>("destroy");
    obj->hash = hashString("destroy");
    Value oInitializer;
    if (klass->methods.Get(obj.get(), oInitializer)) {
        m_pVm->Push(Fox_Object(this));
        m_pVm->CallValue(oInitializer, 0);
    }
}

/* --------- Module Impl---------------------------------------------- */

void ObjectModule::define_func(const std::string& name, NativeFn func)
{
    m_oVM.Push(m_oVM.new_string(name));
    m_oVM.Push(m_oVM.new_object<ObjectNative>(func));
    m_vVariables.Set(Fox_AsString(m_oVM.PeekStart(0)), m_oVM.PeekStart(1));
    m_oVM.Pop();
    m_oVM.Pop();
}

void ObjectModule::raw_klass(const std::string& name, NativeMethods& methods)
{
	m_oVM.Push(m_oVM.new_string(name));
	m_oVM.Push(m_oVM.new_object<ObjectClass>(Fox_AsString(m_oVM.PeekStart(0))));
	m_vVariables.Set(m_oVM.new_string(name), m_oVM.PeekStart(1));
	ObjectClass *pKlass = Fox_AsClass(m_oVM.Pop());
	m_oVM.Pop();
	m_oVM.Push(pKlass);
	for (auto &it : methods) {
		NativeFn func = it.second;

        m_oVM.Push(m_oVM.new_string(it.first));
		m_oVM.Push(m_oVM.new_object<ObjectNative>(func));

		pKlass->methods.Set(Fox_AsString(m_oVM.PeekStart(1)), m_oVM.PeekStart(2));

		m_oVM.Pop();
		m_oVM.Pop();
	}
	m_oVM.Pop();
}

/* ------------------------------------------------------------------- */

/* --------- Utils Impl---------------------------------------------- */

template <>
std::string utils::arg<std::string>(int ac, Value* av, const int i)
{
	if (i < 0 && i > ac)
		throw std::runtime_error("csvsdvdsv");
    return av[i].as<ObjectString>()->string;
}

template <>
double utils::arg<double>(int ac, Value* av, const int i)
{
	if (i < 0 && i > ac)
		throw std::runtime_error("csvsdvdsv");
    return av[i].as<double>();
}

template <>
bool utils::arg<bool>(int ac, Value* av, const int i)
{
    if (i < 0 && i > ac)
		throw std::runtime_error("csvsdvdsv");
    return av[i].as<bool>();
}

template <>
int utils::arg<int>(int ac, Value* av, const int i)
{
    if (i < 0 && i > ac)
		throw std::runtime_error("csvsdvdsv");
    return static_cast<int>(av[i].as<double>());
}
/* ------------------------------------------------------------------- */
