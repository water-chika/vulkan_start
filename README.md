# How to build

Windows need clang to build because msvc doesn't support many C++23 features.

```cmake -S . -B build -G Ninja```

# How to run

## run cube demo

```cd build; ./demo```

## run mesh demo

```cd build; ./demo mesh```

## run on linux display:

login to console and

```cd build; ./cube_display```
