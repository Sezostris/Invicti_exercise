Hello!

## External requirements
In this project, I've made use of the following external libraries:
`HTTP Requests`: https://github.com/yhirose/cpp-httplib
Added as a git submodule.

`Parsing HTML`: https://github.com/lexborisov/myhtml 
Added as a git submodule.


## Installation:
- Download, compile, and install myhtml. Provide the correct path to it in CMakeLists.txt
- In CLI:
```bash
git submodule init
git submodule update
```

Follow installation steps for myhtml.

Then:
```bash
cmake .
make
```

3. Launch the application
```bash
./main
```

