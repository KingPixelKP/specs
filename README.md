# Start-Up
## Clone 
``` bash
git clone https://github.com/KingPixelKP/C-Template.git <Directory-Name>
```
## Remove Remote
``` bash
git remote remove origin
```
# Remeber
If clangd is not recognising headers try changing this in vscode's settings
``` JavaScript
"clangd.arguments": [
    "--compile-commands-dir=build",
  ],
```
# Cmake
## Config
``` bash
cmake -S . -B build 
```
## Building
``` bash
cmake --build build 
```
## Testing
``` bash
cd build
ctest
cd ..
```