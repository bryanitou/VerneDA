# Function to get all the available cores: $(n_cores)
function n_cores()
{
  # Note: getconf is more portable than nproc
  #echo $(nproc --all)
  #echo "$(getconf _NPROCESSORS_ONLN)"
  getconf _NPROCESSORS_ONLN
}

# Function to check installed packages. First it tries with dpkg, then with command, then throws error.
function check_package()
{
  REQUIRED_PKG=$1
  echo -n "|--> Checking for ${REQUIRED_PKG}... "
  PKG_OK=$(dpkg-query -W --showformat='${Status}\n' "${REQUIRED_PKG}" 2> /dev/null | grep "install ok installed")
  if [ "${PKG_OK}" == "" ]; then # Try to search the target with command
    PGR_OK=$(command -v "${REQUIRED_PKG}")
    if [ "${PGR_OK}" == "" ]; then # Give up
      echo "ERROR. Package not found. Please install it with: sudo apt install ${REQUIRED_PKG}"
      exit 1
    else # Found via command -v
      echo "OK (COMMAND)"
    fi
  else # Found via dpkg
    echo "OK (DPKG)"
  fi
}


# Check system requirements
# sudo apt install cmake make gcc g++
echo "CHECKING FOR REQUIRED SYSTEM PACKAGES..."
check_package cmake
check_package make
check_package gcc
check_package g++
echo "ALL PACKAGES OK."
echo " "

threads=$(n_cores)
build_type="Debug"
current_dir=$(pwd)
build_dir="$current_dir/build/VerneDA-build-debug"
install_dir="$current_dir/build/VerneDA-install-debug"
cmake -DBUILD_DACE_AFOSSA_LIBS=ON -DBUILD_DACE_MASTER_LIBS=OFF -DBUILD_WITH_MATLAB=OFF -DCMAKE_BUILD_TYPE="$build_type" -DCMAKE_INSTALL_PREFIX="$install_dir" -S "$current_dir" -B "$build_dir"
cmake --build "${build_dir}" -- -j "${threads}" && cmake --install "${build_dir}"
cmake --build "${build_dir}" -- -j "${threads}" && cmake --install "${build_dir}"
