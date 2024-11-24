# VerneDA
Basic repository where DACE library will be treated as a thirdparty, where some mains will be used to use its functions and features.

## Installation

Follow the steps to successfully build the project, this section is dedicated to the people
who have just downloaded the repository, set-ups and installations are detailed here.

- Go to dependencies section and install all of them.

Install git submodules
```bash
# Download the repository from github, ensure github is installed before
git clone git@github.com:bryanitou/VerneDA.git

# Move into the project folder
cd VerneDA

# Initialize the submodules
git submodule init
git submodule sync
git submodule update
```

Install third parties, this bash code will unpack all the tared packages in this folder. This is 
done to economize space in the repository.
```shell
bash scripts/install_3rdparties.sh
```
##  Build (CLion Developers follow the instructions in the last section)

Set the installation prefix of the application in the recommended folder using the CMake options 
(change it if needed).

Consider the following two CMAKE OPTIONS:

|         Option         | Type | Default | Description                                            |
|:----------------------:|:----:|:-------:|--------------------------------------------------------|
| BUILD_DACE_MASTER_LIBS | BOOL |   OFF   | Builds DACE library from a frozen version Oct 22, 2022 |
| BUILD_DACE_AFOSSA_LIBS | BOOL |   OFF   | Builds DACE library from Alberto Fossa Fork.           |

```shell
cmake -DCMAKE_INSTALL_PREFIX=build/VerneDA-install-debug -DBUILD_DACE_<choice>_LIBS=ON
```

Build the project (8 is the number of threads to be used by the build):
```shell
make -j8
```

Test project with the 'dace_vsod' (DACE Very Simple Orbit Determination) executable:
```shell
./build/VerneDA-install-debug/bin/dace_vsod
```
Results will be generated in the following location:
```tree
out/tbp/  
├── deltas_expression_RK4_1.png  
├── deltas_expression_RK4_2.png  
├── deltas_expression_RK4.dd  
└── taylor_expression_RK4.avd  
```
If you can find all the results, you have successfully installed the project.


Remarks:

- If some error occur during the build, please communicate it for troubleshooting.
- Build is only targeted to be in **_Debug_** mode.
- **_Release_** mode TBD. If you want to do it should check for Optimization Flags to be accepted 
by the CMake check, should be done to ensure optimal execution of the application (memory leaks, etc.)
## Tutorials

A brief section dedicated to tutorials has been created, these tutorials can be found in:

```tree
src/main/tutorials/  
├── Example1-dace_test.cpp  
├── Example2-dace_test.cpp  
├── Example3-dace_test.cpp  
└── Example4-dace_test.cpp  
```

Navigate through each of them and read carefully the documentation. After the run, a python 
script will be called in order to plot the results.

## Outputs

Some output formats made _in the house_ for post-process recognition purposes:

| Extension | Format |      Description      | Details                                                                       |
|:---------:|:------:|:---------------------:|-------------------------------------------------------------------------------|
|   *.dd    |  CSV   |      Delta Dump       | Delta ID (Identifier), within it the variables (physical) and its information |
|   *.avd   |  CSV   | Algebraic Vector Dump | Variable (physical) and its information to re-construct the taylor polynomial |


## Dependencies

Should install the following dependencies in order to make the project work:

- gcc version >= 11.3.0
- g++ version >= 11.3.0
- cmake version >= 3.22.1
- make >= 4.3

```shell
sudo apt-get install gcc g++ cmake make
```

## Deployment

TODO

## Project structure

```html
├── common  
│   └── tools-py  
├── doc  
├── scripts  
└── src  
    ├── 3rdparty  
    ├── core  
    │   ├── base  
    │   └── tools  
    └── main  
        └── tutorials
```
TBD: Add description of each place.

## MATLAB interface: MEX files

MEX files are used in order to call the C++ program from MATLAB. Hence, the C++ project can be used as a propagation engine, embedded in a GNC (Guidance, Navigation and Control) feedback loop, or other engineering purposes.

Before launching MATLAB, some libraries have to be preload (libstdc).
For this reason, a very simple bash script has been written to load the libstdc library, the following command should be run in terminal:

```shell
bash scripts/matlab
```

Don't hesitate to modify the `script/matlab` file since it was created as a convention for some developers who had saved matlab in that filesystem pattern.

In MATLAB, in order to build the MEX file:

1. Run `mex_build.m`
2. Test the matlab files found in `src/main/matlab/example`

### MATLAB in VM

For some reason, matlab in VM tries to find hardware parameters such as the graphics card.
This leads to a bug when executing (not compiling) the code, to avoid this, run in matlab's command line:

```matlab
opengl save software
```

### MATLAB mex files

Some links which the developer may find useful:
- libstdc linking error: https://es.mathworks.com/matlabcentral/answers/1907290-how-to-manually-select-the-libstdc-library-to-use-to-resolve-a-version-glibcxx_-not-found
- mex files examples: https://es.mathworks.com/help/matlab/matlab_external/c-mex-source-file.html#responsive_offcanvas
- hardware assertion: https://es.mathworks.com/matlabcentral/answers/438354-matlab-crashes-after-the-assertion-could-not-initialize-glx-failed
## Programming guidelines

TODO

## Developers in VSCODE Environment

1. Clone the repository from CLion's Version control using the same URL:
`git@github.com:bryanitou/VerneDA.git`.

2. Enter to the project folder if you are not in already.

3. Run:
   ```shell
   bash scripts/install_3rdparties.sh

4. Download the needed tools in vscode like C++, Make, etc.

5. In order to set the CMake configurations: create a file named `CMakePresets.json` and write the following inside:

```json
{
    "version": 6,
    "cmakeMinimumRequired": {
      "major": 3,
      "minor": 23,
      "patch": 0
    },
    "configurePresets": [
    {
        "name": "default",
        "displayName": "default",
        "description": "Write your description here.",
        "generator": "Unix Makefiles",
        "binaryDir": "${workspaceFolder}/build/VerneDA-install-debug",
        "cacheVariables": {
            "B": "VerneDA-build-debug",
            "CMAKE_BUILD_TYPE": "Debug",
            "CMAKE_INSTALL_PREFIX": "../VerneDA-install-debug",
            "BUILD_DACE_AFOSSA_LIBS": "ON"
        }
      }
  ]
}
```
In order to understand what you are doing or tuning these, please read instructions in the following section, points 6-11.

6. After that, you can set your own `launch.json` and `task.json` file in order to run/debug the programs. Here below you can find an example of each for the target `dace_vsod` program.

In `launch.json` in the configurations vector:
```json
...
{
    "name": "dace_vsod",
    "type": "cppdbg",
    "request": "launch",
    "program": "${workspaceFolder}/build/VerneDA-install-debug/bin/dace_vsod",
    "args": ["--config", "examples/translation_loads.json"],
    "cwd": "${workspaceFolder}",
    "preLaunchTask": "CMake: build dace_vsod"
},
...
```

In `tasks.json` in the configurations vector:
```json
{
    "type": "cmake",
    "label": "CMake: build dace_vsod",
    "command": "build",
    "targets": [
        "dace_vsod"
    ],
    "preset": "${command:cmake.activeBuildPresetName}",
    "group": {
        "kind": "build",
        "isDefault": true
    },
    "problemMatcher": [],
    "detail": "CMake template build task"
},
```

So basically, when you try to launch (from `launch.json`) an executable (in this case `dace_vsod`) it will build the program (from `tasks.json`) in the case that the code has been modified. As you can see,  in `launch.json` has the field: `preLaunchTask`, which connects the launch to the pretask.

7. You should be able now to launch any program of the project. If not, revise the instructions.


## Developers in CLion Environment

1. Clone the repository from CLion's Version control using the same URL:
`git@github.com:bryanitou/VerneDA.git`.

2. Enter to the project folder if you are not in already.

3. Run:
   ```shell
   bash scripts/install_3rdparties.sh
   ```

4. Settings -> Build, Execution, Deployment -> CMake.

5. Leave unchecked 'Reload CMake project on editing CMakeLists.txt or other CMake configuration files' and go to
Settings -> Advanced Settings and check 'Do not trigger CMake reload on external changes if auto-reload is disabled'.
(Version 2021.1 has this knob in the Registry under the name: `cmake.disable.auto.reload.external.changes`, older
versions do not have it)

6. 'Enable profile' and set a 'Name' and 'Build type', like `Debug`.

7. Change the 'Generator' to 'Unix Makefiles', now in 'CMake options' should appear `-G "Unix Makefiles"`.

8. Add to options `-DCMAKE_INSTALL_PREFIX=../VerneDA-install-debug` (TODO: to match with `build_all.sh`).

9. Add to options `-DBUILD_DACE_MASTER_LIBS=ON` OR `-DBUILD_DACE_AFOSSA_LIBS=ON`. Only choose one, please read the 
above this CMAKE options (build section) for further information.

10. Add to options `-DBUILD_WITH_MATLAB=ON`. This will try to look for the matlab header files to make the includes in the mex files.
Pay attention that if the matlab header files are not in the expected location, errors will arise.
Please take a look at file `scripts/matlab` to understand where matlab is expected to be found or line 225 from CMakeLists.txt.

11. Set 'Build directory' to `build/VerneDA-build-debug`.

12. 'Build options' are the `make` options. CLion sets automatically the number of threads to use.

13. After accepting, CLion will run cmake. If it fails, check step 1 and reload cmake from CLion's window.

Note that you will have to reload CLion's CMake manually when necessary from the CMake bottom window.

# References

- DACE library: https://github.com/dacelib/dace
