# Very simple CSPI library

## To install CSPI library in $HOME/Arduino:
```
cmake -B build
cmake --build build
cmake --install build
```

## Compile example (master/slave sends/receives the alphabet)
```
cmake -B build
cmake --build build
cmake --build build --target upload build
```
