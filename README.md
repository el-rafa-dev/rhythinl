<img src="./gitsrc/rhythin_logo.png" alt="Rhythin Logo" style="display: block; margin-left: auto; margin-right:auto; width: 100%; height: 100%;">

# Rhythin Programming Language
RhythinLang will be a statically typed, interpreted and compiled (not JIT) programming language focused on being objective and direct to execute on Command Line Interface (CLI), facilitating learning for both beginners and programmers with experience in the area.

## Current Status

The current status is development and tests.

## When will be released?

The first version for general use will be when it has the basic syntax of a language. Types, structures, etc.



## How use/Example
```
; this is only a little example
; the semicolon characther (; or #) is for comments inline
; here we will be create the main function 
; thats will be a func type.
```
```
def main:func() -> [
    printnl("Hello World from Rhythin!") ;; writing a message on console/terminal
    finish(0) ;; sucess code
]
```

## Building on Linux
To build and run the project on linux, do this below:

clone the repository with:
```
git clone https://github.com/el-rafa-dev/rhythinl.git
```
Run this command:
```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```
The build will be finished and you can run Rhythin with:

```bash
./build/rhythin
```
## Supporting
Go to [support.md](./SUPPORT.md) to see how support the project

## License

```
MIT License

Copyright (c) [2025] [El-Rafa-Dev (Rafael Sousa)]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## Contact
Contact with me via telegram! My user id: [@rafaprobr](https://t.me/rafaprobr)

## A picture of --help and --version
<img src="https://files.catbox.moe/subfwd.png"></img>
