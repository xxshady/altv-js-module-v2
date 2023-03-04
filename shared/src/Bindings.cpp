#include "Bindings.h"
#include "module/IResource.h"

static v8::MaybeLocal<v8::Module> ResolveModuleCallback(v8::Local<v8::Context> context, v8::Local<v8::String> specifier, v8::Local<v8::FixedArray> assertions, v8::Local<v8::Module> referrer)
{
    js::IResource* resource = js::IResource::GetFromContext(context);
    if(!resource) return v8::MaybeLocal<v8::Module>();
    std::string name = *v8::String::Utf8Value(context->GetIsolate(), specifier);
    js::Binding& binding = js::Binding::Get(name);
    if(!binding.IsValid()) return v8::MaybeLocal<v8::Module>();
    return v8::MaybeLocal<v8::Module>(binding.GetCompiledModule(resource));
}

v8::Local<v8::Module> js::Binding::Compile(IResource* resource)
{
    v8::Isolate* isolate = resource->GetIsolate();
    v8::Local<v8::Context> context = resource->GetContext();
    std::string moduleName = "internal:" + name;
    v8::ScriptOrigin origin{
        isolate, v8::String::NewFromUtf8(isolate, moduleName.c_str()).ToLocalChecked(), 0, 0, false, -1, v8::Local<v8::Value>(), false, false, true, v8::Local<v8::PrimitiveArray>()
    };
    v8::ScriptCompiler::Source source{ v8::String::NewFromUtf8(isolate, src.c_str()).ToLocalChecked(), origin };
    v8::MaybeLocal<v8::Module> maybeModule = v8::ScriptCompiler::CompileModule(isolate, &source);
    if(maybeModule.IsEmpty())
    {
        // todo: Log an internal error
        return v8::Local<v8::Module>();
    }
    v8::Local<v8::Module> mod = maybeModule.ToLocalChecked();
    // todo: add global alt
    v8::Maybe<bool> result = mod->InstantiateModule(context, &ResolveModuleCallback);
    if(result.IsNothing() || !result.ToChecked())
    {
        // todo: Log an internal error
        return v8::Local<v8::Module>();
    }
    compiledModuleMap.insert({ resource, Persistent<v8::Module>(isolate, mod) });
    return mod;
}

v8::Local<v8::Module> js::Binding::GetCompiledModule(IResource* resource)
{
    v8::Isolate* isolate = resource->GetIsolate();
    v8::Local<v8::Module> mod;

    auto it = compiledModuleMap.find(resource);
    if(it == compiledModuleMap.end()) mod = Compile(resource);
    else
        mod = it->second.Get(isolate);

    return mod;
}

void js::Binding::CleanupForResource(IResource* resource)
{
    for(auto& [_, binding] : __bindings)
    {
        binding.compiledModuleMap.erase(resource);
    }
}
