# getting2knowDace
Basic repository where DACE library will be treated as a thirdparty, where some mains will be used to use its functions and features.

## Installation

Follow the steps to successfully build the project, this section is dedicated to the people
who have just downloaded the repository, set-ups and installations are detailed here.

- Go to dependencies section and install all of them.

Install git submodules
```bash
# Download the repository from github, ensure github is installed before
git clone git@github.com:bryanitou/getting2knowDace.git

# Move into the project folder
cd getting2knowDace

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
##  Build

Set the installation prefix of the application in the recommended folder using the CMake options 
(change it if needed).

Consider the following two CMAKE OPTIONS:

|         Option         | Type | Default | Description                                            |
|:----------------------:|:----:|:-------:|--------------------------------------------------------|
| BUILD_DACE_MASTER_LIBS | BOOL |   OFF   | Builds DACE library from a frozen version Oct 22, 2022 |
| BUILD_DACE_AFOSSA_LIBS | BOOL |   OFF   | Builds DACE library from Alberto Fossa Fork.           |

```shell
cmake -DCMAKE_INSTALL_PREFIX=build/getting2knowDace-install-debug -DBUILD_DACE_<choice>_LIBS=ON
```

Build the project (8 is the number of threads to be used by the build):
```shell
make -j8
```

Test project with the 'dace_vsod' (DACE Very Simple Orbit Determination) executable:
```shell
build/getting2knowDace-install-debug/bin/dace_vsod
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

## Programming guidelines

TODO

## Developers in CLion Environment

1 - Clone the repository from CLion's Version control using the same URL:
`git@github.com:bryanitou/getting2knowDace.git`.

2 - Enter to the project folder if you are not in already.

3 - Run:

```shell
bash scripts/install_3rdparties.sh
```

4 - Settings -> Build, Execution, Deployment -> CMake.

5 - Leave unchecked 'Reload CMake project on editing CMakeLists.txt or other CMake configuration files' and go to
Settings -> Advanced Settings and check 'Do not trigger CMake reload on external changes if auto-reload is disabled'.
(Version 2021.1 has this knob in the Registry under the name: `cmake.disable.auto.reload.external.changes`, older
versions do not have it)

6 - 'Enable profile' and set a 'Name' and 'Build type', like `Debug`.

7 - Change the 'Generator' to 'Unix Makefiles', now in 'CMake options' should appear `-G "Unix Makefiles"`.

8 - Add to options `-DCMAKE_INSTALL_PREFIX=../getting2knowDace-install-debug` (TODO: to match with `build_all.sh`).

9 - Add to options `-DBUILD_DACE_MASTER_LIBS=ON` OR `-DBUILD_DACE_MASTER_LIBS=OFF`. Only choose one, please read the 
above this CMAKE options.

10 - Set 'Build directory' to `build/getting2knowDace-build-debug` (TODO: to match with `build_all.sh`, this
script is still to be done).

11 - 'Build options' are the `make` options. CLion sets automatically the number of threads to use.

12 - After accepting, CLion will run cmake. If it fails, check step 1 and reload cmake from CLion's window.

Note that you will have to reload CLion's CMake manually when necessary from the CMake bottom window.

# References

- DACE library: https://github.com/dacelib/dace
