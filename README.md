# Remeber
If clangd is not recognising headers try changing this in vscode's settings
``` JavaScript
"clangd.arguments": [
    "--compile-commands-dir=build",
  ],
```
# Config
``` bash
cmake -S . -B build 
```
# Building
``` bash
cmake --build build 
```
# Testing
``` bash
cd build
ctest
cd ..
```