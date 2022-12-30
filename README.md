# getting2knowDace
Basic repository where DACE library will be treated as a thirdparty, where some mains will be used to use its functions and features.

## Developers in CLion Environment

1 - Clone the repository from CLion's Version control using the same URL:
`git@github.com:bryanitou/getting2knowDace.git`.

2 - Settings -> Build, Execution, Deployment -> CMake.

3 - Leave unchecked 'Reload CMake project on editing CMakeLists.txt or other CMake configuration files' and go to
Settings -> Advanced Settings and check 'Do not trigger CMake reload on external changes if auto-reload is disabled'.
(Version 2021.1 has this knob in the Registry under the name: `cmake.disable.auto.reload.external.changes`, older
versions do not have it)

4 - 'Enable profile' and set a 'Name' and 'Build type', like `Debug`.

5 - Change the 'Generator' to 'Unix Makefiles', now in 'CMake options' should appear `-G "Unix Makefiles"`.

6 - Add to options `-DCMAKE_INSTALL_PREFIX=../getting2knowDace-install-debug` (to match with `build_all.sh`).

7 - Set 'Build directory' to `build/getting2knowDace-build-debug` (to match with `build_all.sh`).

8 - 'Build options' are the `make` options. CLion sets automatically the number of threads to use.

9 - After accepting, CLion will run cmake. If it fails, check step 1 and reload cmake from CLion's window.

Note that you will have to reload CLion's CMake manually when necessary from the CMake bottom window.

# References

- DACE library: https://github.com/dacelib/dace