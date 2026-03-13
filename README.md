# IL2CPP Mod Base (C++)

A minimal DLL scaffold for starting Unity IL2CPP mods.

## What this base gives you

- `DllMain` entrypoint that spins work onto a separate thread.
- Basic runtime resolution of common IL2CPP exports from `GameAssembly.dll`:
  - `il2cpp_domain_get`
  - `il2cpp_thread_attach`
  - `il2cpp_string_new`
- Helper functions to attach your mod thread to the IL2CPP domain and create managed strings.

## Project layout

- `include/il2cpp_base.hpp`: Public API surface for initialization + helpers.
- `src/il2cpp_base.cpp`: IL2CPP export resolution and wrappers.
- `src/dllmain.cpp`: DLL entry point and mod thread bootstrap.

## Build

This scaffold targets Windows game modding workflows.

```bash
cmake -S . -B build
cmake --build build --config Release
```

Then inject/load the built DLL with your preferred loader.

## Next steps

1. Add your preferred hooking library (for example, MinHook).
2. Add metadata/class/method resolution (direct IL2CPP APIs or a resolver library).
3. Install hooks inside `mod_entry()` and keep cleanup in `DLL_PROCESS_DETACH`.
