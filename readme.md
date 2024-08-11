CPU renderer written while following https://pikuma.com/courses/learn-3d-computer-graphics-programming

# Build and run

```sh
make build && make run
```

# Project dependencies

## OSX setup

```sh
brew install sdl2
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
  "/opt/homebrew/include/SDL2",
  "${workspaceRoot}"
]
```
