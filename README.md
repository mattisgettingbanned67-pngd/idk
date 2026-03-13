# IL2CPP Offset Dumper (C++ CLI)

A Windows-friendly command-line `.exe` utility for dumping IL2CPP metadata details from:

- `global-metadata.dat`
- `GameAssembly.dll`

## Features

- Drag-and-drop friendly console input mode.
- Reads IL2CPP metadata type + method definitions.
- Dumps method list with namespace, class, method, token, and best-effort RVA match.
- Parses `GameAssembly.dll` export table and appends export RVAs.
- Writes output to `il2cpp_dump.csv` in the current directory.

## Build

```bash
cmake -S . -B build
cmake --build build --config Release
```

Output binary (Windows): `build/Release/il2cpp_offset_dumper.exe`

## Usage

### Interactive drag-and-drop mode

```bash
il2cpp_offset_dumper.exe
```

Then drag/drop:
1. `global-metadata.dat`
2. `GameAssembly.dll`

### Argument mode

```bash
il2cpp_offset_dumper.exe "C:\path\to\global-metadata.dat" "C:\path\to\GameAssembly.dll"
```

## Output format

- Main CSV section: methods from metadata (`Namespace,Class,Method,Token,RVA`)
- Secondary CSV section: exported symbols from `GameAssembly.dll`

> Note: many IL2CPP games do not expose all managed method symbols in the export table. Those methods will show `RVA = N/A` unless a symbol-based match is found.
