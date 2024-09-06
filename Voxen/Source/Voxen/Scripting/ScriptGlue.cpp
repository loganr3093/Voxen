#include "voxpch.h"
#include "Voxen/Scripting/ScriptGlue.h"

#include <mono/metadata/object.h>

namespace Voxen
{
#define VOX_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Voxen.InternalCalls::" #Name, Name)

	static void NativeLog(MonoString* string, int parameter)
	{
		char* cStr = mono_string_to_utf8(string);
		std::string str(cStr);
		mono_free(cStr);
		std::cout << str << ", " << parameter << std::endl;
	}

	static void NativeLog_Vector(Vector3* parameter, Vector3* outResult)
	{
		VOX_CORE_WARN("({0}, {1}, {2})", parameter->x, parameter->y, parameter->z);
		*outResult = glm::normalize(*parameter);
	}

	static float NativeLog_VectorDot(Vector3* parameter)
	{
		VOX_CORE_WARN("({0}, {1}, {2})", parameter->x, parameter->y, parameter->z);
		return glm::dot(*parameter, *parameter);
	}

	void ScriptGlue::RegisterFunctions()
	{
		VOX_ADD_INTERNAL_CALL(NativeLog);
		VOX_ADD_INTERNAL_CALL(NativeLog_Vector);
		VOX_ADD_INTERNAL_CALL(NativeLog_VectorDot);
	}
}