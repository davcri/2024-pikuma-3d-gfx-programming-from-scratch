CPU renderer written while following https://pikuma.com/courses/learn-3d-computer-graphics-programming

![image](https://github.com/user-attachments/assets/68f666ab-547a-4b51-a8ad-fee1c5ccf5a1)

# Build and run

```sh
make build && make run
```

# Project dependencies

## OSX setup

```sh
brew install sdl2
brew install libpng
```

## Debugging

https://code.visualstudio.com/docs/cpp/launch-json-reference

### VSCode setup

1. Install ms-vscode.cpptools extension
2. Configure it to have autocomplete (see [this link](https://stackoverflow.com/questions/46258143/visual-studio-code-how-to-configure-includepath-for-better-intellisense-results)):

```json
"C_Cpp.default.includePath": [
  "${default}",
  "/opt/homebrew/lib",
  "/opt/homebrew/include",
  "/opt/homebrew/include/SDL2",
  "${workspaceRoot}"
]
```
