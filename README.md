# Space Square

## Development

```sh
mkdir build
cd build
cmake ..

cmake --build ./build -j
```

### Trubleshooting

- Clangd can't find `compile_commands.json` which causes errors in header files

  ```sh
  ln -s build/compile_commands.json compile_commands.json
  ```
