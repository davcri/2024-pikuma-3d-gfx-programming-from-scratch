# Build and run

```sh
make build
make run
```

# Project dependencies

## OSX setup

```sh
brew install sdl2
```

### VSCode setup

1. Install ms-vscode.cpptools extension
2. Configure it to have autocomplete:

````json
"C_Cpp.default.includePath": [
  "/opt/homebrew/lib",
  "/opt/homebrew/include/SDL2"
]```
````
