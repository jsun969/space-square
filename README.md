# Space Square

## Usage

> Use `spsq --help` to see all available options.

```sh
spsq [path] -d <depth>
```

## FAQ

### Why it's taking so long to scan my disk?

It will take a significant amount of time because the program needs to recursively visit all files in directory to calculate directory sizes. It's not recommended to scan root directory `/` or home directory `~` unless you really want to.

### Why some of my files/directories are missing?

The squares are generated based on size. Even though the program is using fullscreen of your terminal, it's still a limited space. Therefore, some small files/directories may be omitted.

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
