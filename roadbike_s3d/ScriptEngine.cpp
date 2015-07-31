#include "ScriptEngine.h"

#include "as/add_on/scriptstdstring/scriptstdstring.h"

// AngelScript Runtime/Loader

#ifdef _DEBUG
#pragma comment(lib, "angelscriptd.lib")
#else
#pragma comment(lib, "angelscript.lib")
#endif

#define _supressWarnUnused(v) v;

ScriptEngine::ScriptEngine()
{
	this->pEngineInterface = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	assert(this->pEngineInterface != nullptr);
	auto r = this->pEngineInterface->SetMessageCallback(asFUNCTION(ScriptEngine::engineCallback), 0, asCALL_CDECL);
	assert(r >= 0);
	this->RegisterGlobalProperties();
	this->RegisterNativeFunctions();
}
ScriptEngine::~ScriptEngine()
{
	if (this->pRuntimeContext != nullptr)
	{
		this->pRuntimeContext->Release();
		this->pRuntimeContext = nullptr;
	}
	if (this->pEngineInterface != nullptr)
	{
		this->pEngineInterface->ShutDownAndRelease();
		this->pEngineInterface = nullptr;
	}
}

void ScriptEngine::_assertInvariantContracts()
{
	assert(this->pEngineInterface != nullptr);
}
void ScriptEngine::_ensureModuleStarted()
{
	assert(this->pCurrentModule != nullptr);
}

void ScriptEngine::RegisterNativeFunctions()
{
	this->_assertInvariantContracts();

	RegisterStdString(this->pEngineInterface);
	auto r = this->pEngineInterface->RegisterGlobalFunction("void print(const string& in)", asFUNCTION(ScriptEngine::NativeFunctions::print), asCALL_CDECL);
	assert(r >= 0);
	r = this->pEngineInterface->RegisterGlobalFunction("void update()", asFUNCTION(ScriptEngine::NativeFunctions::update), asCALL_CDECL);
	assert(r >= 0);
	r = this->pEngineInterface->RegisterGlobalFunction("bool getkey(int keycode)", asFUNCTION(ScriptEngine::NativeFunctions::getkey), asCALL_CDECL);
	assert(r >= 0);
	r = this->pEngineInterface->RegisterGlobalFunction("double floor(double)", asFUNCTION(ScriptEngine::NativeFunctions::floor), asCALL_CDECL);
	assert(r >= 0);
	r = this->pEngineInterface->RegisterGlobalFunction("double abs(double)", asFUNCTION(ScriptEngine::NativeFunctions::abs), asCALL_CDECL);
	assert(r >= 0);
	r = this->pEngineInterface->RegisterGlobalFunction("double sign(double)", asFUNCTION(ScriptEngine::NativeFunctions::sign), asCALL_CDECL);
	assert(r >= 0);
	r = this->pEngineInterface->RegisterGlobalFunction("bool getLeftKey()", asFUNCTION(ScriptEngine::NativeFunctions::leftKey), asCALL_CDECL);
	assert(r >= 0);
	r = this->pEngineInterface->RegisterGlobalFunction("bool getRightKey()", asFUNCTION(ScriptEngine::NativeFunctions::rightKey), asCALL_CDECL);
	assert(r >= 0);
}
void ScriptEngine::RegisterGlobalProperties()
{
	this->_assertInvariantContracts();

	auto r = this->pEngineInterface->RegisterGlobalProperty("int _frameCount", &this->_gp.frameCount);
}
void ScriptEngine::RegisterProperty(const String& signature, void* ptr)
{
	this->_assertInvariantContracts();

	auto r = this->pEngineInterface->RegisterGlobalProperty(signature.narrow().c_str(), ptr);
	assert(r >= 0);
}

void ScriptEngine::engineCallback(const asSMessageInfo* pMsg, void* pParam)
{
	// ref: http://www.angelcode.com/angelscript/sdk/docs/manual/doc_compile_script.html#doc_compile_script_msg
	_supressWarnUnused(pParam);

	switch (pMsg->type)
	{
	case asMSGTYPE_ERROR:
		OutputLog(LogDescription::Error, Widen(pMsg->section), L" (", pMsg->row, L", ", pMsg->col, L") : ERR : ", Widen(pMsg->message));
		break;
	case asMSGTYPE_WARNING:
		OutputLog(LogDescription::Error, Widen(pMsg->section), L" (", pMsg->row, L", ", pMsg->col, L") : WARN : ", Widen(pMsg->message));
		break;
	case asMSGTYPE_INFORMATION:
		OutputLog(LogDescription::Error, Widen(pMsg->section), L" (", pMsg->row, L", ", pMsg->col, L") : INFO : ", Widen(pMsg->message));
		break;
	default: assert(false);
	}
}

ScriptContext ScriptEngine::CreateContextFromScript(const FilePath& path)
{
	ScriptContext cont(this);
	cont.MakeFromScript(path);
	return cont;
}

void ScriptEngine::StartModule(const String& moduleName)
{
	this->_assertInvariantContracts();

	this->pCurrentModule = this->pEngineInterface->GetModule(moduleName.narrow().c_str(), asGM_ALWAYS_CREATE);
	assert(this->pCurrentModule != nullptr);
}
void ScriptEngine::SelectModule(const String& moduleName)
{
	this->_assertInvariantContracts();
	
	this->pCurrentModule = this->pEngineInterface->GetModule(moduleName.narrow().c_str());
	assert(this->pCurrentModule != nullptr);
}
void ScriptEngine::AddScript(const FilePath& path)
{
	this->_assertInvariantContracts();
	this->_ensureModuleStarted();

	auto reader = TextReader(path);
	auto scriptData = reader.readContents();
	reader.close();
	this->pCurrentModule->AddScriptSection(path.narrow().c_str(), scriptData.narrow().c_str());
}
void ScriptEngine::BuildModule()
{
	this->_assertInvariantContracts();
	this->_ensureModuleStarted();

	auto r = this->pCurrentModule->Build();
	if (r < 0)
	{
		throw std::exception("Compilation Failed");
	}
}
void ScriptEngine::StartExecution(const String& signature)
{
	this->_assertInvariantContracts();
	this->_ensureModuleStarted();

	auto pEntryPointFunc = this->pCurrentModule->GetFunctionByDecl(signature.narrow().c_str());
	if (pEntryPointFunc == nullptr)
	{
		throw std::exception(Format(L"Entry-point ", signature, L" is not found").narrow().c_str());
	}

	this->pRuntimeContext = this->pEngineInterface->CreateContext();
	assert(this->pRuntimeContext != nullptr);
	this->pRuntimeContext->Prepare(pEntryPointFunc);
	
	auto r = this->pRuntimeContext->Execute();
	if (r != asEXECUTION_FINISHED)
	{
		// not finished?
		if (r == asEXECUTION_EXCEPTION)
		{
			// error exit
			throw std::exception(Format(L"AngelScript Exception: ", Widen(this->pRuntimeContext->GetExceptionString())).narrow().c_str());
		}
	}
}

void ScriptEngine::NativeFunctions::print(const std::string& str)
{
	OutputLog(LogDescription::App, L"Script Output: ", Widen(str));
}
void ScriptEngine::NativeFunctions::update()
{
	auto ctx = asGetActiveContext();
	if (ctx != nullptr)
	{
		ctx->Suspend();
	}
}
bool ScriptEngine::NativeFunctions::getkey(int asc_keycode)
{
	return Key((wchar)asc_keycode).pressed;	// 押された瞬間の判定はスクリプト側でやってもらうことにした
}
double ScriptEngine::NativeFunctions::floor(double v){ return Math::Floor(v); }
double ScriptEngine::NativeFunctions::abs(double v){ return Math::Abs(v); }
double ScriptEngine::NativeFunctions::sign(double v){ return Math::Sign(v); }
bool ScriptEngine::NativeFunctions::leftKey(){ return Input::KeyLeft.pressed; }
bool ScriptEngine::NativeFunctions::rightKey(){ return Input::KeyRight.pressed; }

ScriptContext::ScriptContext(ScriptEngine* e) : engine_ref(e)
{

}
ScriptContext::~ScriptContext()
{
	if (this->pContext != nullptr)
	{
		this->pContext->Release();
		this->pContext = nullptr;
	}
	if (this->pModule != nullptr)
	{
		this->pModule = nullptr;
	}
}

void ScriptContext::MakeFromScript(const FilePath& path)
{
	this->engine_ref->_assertInvariantContracts();

	auto module_name = FileSystem::BaseName(path);

	// Make Module
	this->pModule = this->engine_ref->pEngineInterface->GetModule(module_name.narrow().c_str(), asGM_ALWAYS_CREATE);
	assert(this->pModule != nullptr);

	// Load Script
	auto reader = TextReader(path);
	auto scriptData = reader.readContents();
	reader.close();
	this->pModule->AddScriptSection(path.narrow().c_str(), scriptData.narrow().c_str());

	// Build Module
	this->pModule->Build();
	
	// Prepare Entry point
	this->Reenter();
}
void ScriptContext::_ensureModuleInitialized()
{
	// invariant contract
	assert(this->pModule != nullptr && this->pContext != nullptr);
}

void ScriptContext::Reenter()
{
	// Remake Context
	if (this->pContext != nullptr)
	{
		this->pContext->Release();
		this->pContext = nullptr;
	}
	this->pContext = this->engine_ref->pEngineInterface->CreateContext();
	assert(this->pContext != nullptr);

	auto pFunction = this->pModule->GetFunctionByDecl("void main()");
	if (pFunction == nullptr)
	{
		throw std::exception(Format(L"Method \"void main()\" is not defined.").narrow().c_str());
	}
	this->pContext->Prepare(pFunction);
	this->execTerminated = false;
}
void ScriptContext::Execute()
{
	this->_ensureModuleInitialized();

	if (this->execTerminated) return;

	auto r = this->pContext->Execute();
	if (r != asEXECUTION_SUSPENDED)
	{
		// end execution
		this->execTerminated = true;
	}
}
int ScriptContext::GetGlobalInt(const String& name)
{
	this->_ensureModuleInitialized();

	auto idx = this->pModule->GetGlobalVarIndexByDecl(Format(L"int ", name).narrow().c_str());
	if (idx < 0) throw std::exception(Format(L"Global Variable ", name, L" not found").narrow().c_str());
	auto ptr = this->pModule->GetAddressOfGlobalVar(idx);
	assert(ptr != nullptr);
	return *(int*)ptr;
}
double ScriptContext::GetGlobalDouble(const String& name)
{
	this->_ensureModuleInitialized();

	auto idx = this->pModule->GetGlobalVarIndexByDecl(Format(L"double ", name).narrow().c_str());
	if (idx < 0) throw std::exception(Format(L"Global Variable ", name, L" not found").narrow().c_str());
	auto ptr = this->pModule->GetAddressOfGlobalVar(idx);
	assert(ptr != nullptr);
	return *(double*)ptr;
}
Vec2 ScriptContext::GetGlobalDoubleVec2(const String& x, const String& y)
{
	return Vec2(this->GetGlobalDouble(x), this->GetGlobalDouble(y));
}
