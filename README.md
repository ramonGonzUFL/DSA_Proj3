# House Search Project

## Contributors
- Ramon Gonzalez Fernandez
- Delaney Seguin
- Kaylee Zamora

## Build Instructions

### For an IDE
In a general sense, run the "main" target in the cmake configuration. Make sure to open the project folder in your ide of choice, and instruct it to run in the project folder, not in the executable folder.

### Or for CLI
```sh
# In the `project` folder
cmake -S . -B build
cmake --build build -j --target main
./build/bin/main # Unix
```
or
```bash
build\bin\main.exe # Windows
```

