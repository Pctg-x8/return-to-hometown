#pragma once

#include <Siv3D.hpp>
#include <angelscript.h>

class ScriptEngine
{
	asIScriptEngine* pEngineInterface = nullptr;
	asIScriptModule* pCurrentModule = nullptr;
	asIScriptContext* pRuntimeContext = nullptr;

	struct GlobalProperties
	{
		int frameCount;
	};
	GlobalProperties _gp;
public:
	friend class ScriptContext;

	ScriptEngine();
	virtual ~ScriptEngine();

	ScriptContext CreateContextFromScript(const FilePath& path);

	void StartModule(const String& moduleName);
	void SelectModule(const String& moduleName);
	void AddScript(const FilePath& path);
	void BuildModule();
	void StartExecution(const String& signature);

	void setFrameCount(int fc){ this->_gp.frameCount = fc; }
	template <typename FuncT, typename ObjectT> void RegisterMethod(const String& signature, const FuncT& mtd, ObjectT& object)
	{
		this->pEngineInterface->RegisterGlobalFunction(signature.narrow().c_str(), asSMethodPtr<sizeof(FuncT)>::Convert(mtd), asCALL_THISCALL_ASGLOBAL, (void*)&object);
	}
	template <typename FuncT, typename ObjectT> void RegisterMethod(const String& signature, const FuncT& mtd, ObjectT* object)
	{
		this->pEngineInterface->RegisterGlobalFunction(signature.narrow().c_str(), asSMethodPtr<sizeof(FuncT)>::Convert(mtd), asCALL_THISCALL_ASGLOBAL, object);
	}
	template <typename FuncT> void RegisterFunction(const String& signature, const FuncT& mtd)
	{
		this->pEngineInterface->RegisterGlobalFunction(signature.narrow().c_str(), asFUNCTION(mtd), asCALL_CDECL);
	}
	void RegisterProperty(const String& signature, void* ptr);
private:
	// Contracts
	void _assertInvariantContracts();
	void _ensureModuleStarted();

	// Private Subprocedures
	void RegisterNativeFunctions();
	void RegisterGlobalProperties();
	static void engineCallback(const asSMessageInfo* pMsg, void* pParam);

	struct NativeFunctions
	{
		static void print(const std::string& str);
		static void update();
		static bool getkey(int asc_keycode);
		static double floor(double v);
		static double abs(double v);
		static double sign(double v);
		static bool leftKey();
		static bool rightKey();
	};
};

class ScriptContext
{
	ScriptEngine* engine_ref;
	asIScriptModule* pModule = nullptr;
	asIScriptContext* pContext = nullptr;
	bool execTerminated = false;
public:
	ScriptContext(ScriptContext&& c)
	{
		this->pModule = c.pModule;
		this->pContext = c.pContext;
		this->engine_ref = c.engine_ref;
		this->pContext->AddRef();
	}
	ScriptContext(ScriptEngine*);
	~ScriptContext();

	void MakeFromScript(const FilePath& path);

	void Execute();
	void Reenter();
	int GetGlobalInt(const String& name);
	double GetGlobalDouble(const String& name);
	Vec2 GetGlobalDoubleVec2(const String& x, const String& y);
private:
	void _ensureModuleInitialized();
};