#!/bin/bash
# Pathfinder maintenance script
# Uncompress all 3rd party packages
# Useful to set everything up if the repository has just been cloned, or a new 3rd party is added later on
# Usage: bash ./scripts/install_3rdparties.sh
# Exit codes: 0 -> ok, extraction completed / 1 -> error, extraction not completed

function print_usage()
{
  echo "Usage: bash ./scripts/$(basename "$0")"\
       "[-c | --clean-dirs] [--source-dir path_to_dir] [--build-dir path_to_dir]"

  echo "Default does not clean directories, call it with the flag in order to wipe out previous extraction."
  echo "Default source directory is current working directory (cwd = '.')."
  echo "Default build directory is <source_dir>/build/pathfinder-3rdparty."
  echo " "

  echo "--> Example, called from project directory: bash ./scripts/$(basename "$0")"
  echo "--> The script will check if 3rd parties one by one: if extracted directory does not exist, it extracts it;"
  echo "    if extracted directory contents differ from compressed ones, overwrites them."
  echo "--> Checks filenames are correct."
  exit 1 # Stop execution and return
}

# Function to display ASCII art logo
function display_logo()
{
  echo "PP"
  echo "PP"
  echo "PP"
  echo "PP"
  echo "PP"
}

# Get parameters
clean_dirs=false
current_dir=$(realpath .)
source_dir="${current_dir}"
dir_3rdparty="${source_dir}/build/pathfinder-3rdparty"

while test $# != 0; do
  case "$1" in
  -c | --clean-dirs) clean_dirs=true ;;
  --source-dir)
              source_dir=$(realpath -m "${2%/}/")
              dir_3rdparty="${source_dir}/build/pathfinder-3rdparty"
              shift
              ;;
  --build-dir)
              dir_3rdparty=$(realpath -m "${2%/}/")
              shift
              ;;
  --)
    shift
    break
    ;;
  *) print_usage ;;
  esac
  shift
done

# If source directory does not exist, return error
if [[ ! -d "${source_dir}" ]]; then
  echo "Source directory ${source_dir} does not exist, check the path!"
  exit 1
fi

# If build dir does not exist, create it
if [[ ! -d "${dir_3rdparty}" ]]; then
  mkdir -pv "${dir_3rdparty}"
fi

# Welcome message
if [ "${SHLVL}" -lt 3 ]; then
  display_logo
  echo " "
  echo "WELCOME TO PATH-FINDER INSTALL 3RD PARTIES SCRIPT"
  echo "This script will extract 3rd parties from their compressed files"
  echo "Useful to set everything up if the repository has just been cloned"
  echo " "
fi

# List all the tar.gz under the "<source>/src/3rdparty/" folder
list_3rdparty=("${source_dir}"/src/3rdparty/*.tar.gz)

# Safety check: if there are no .gz files in the directory, return
if [ "${#list_3rdparty[@]}" -eq 0 ]; then
  echo "No '.tar.gz' files found on ${source_dir}/src/3rdparty/"
  echo "Finishing script now."
  exit 1
fi

# Info
echo "UNPACKING 3RD PARTY LIBRARIES:"

# Untar all files
for file_3rdparty in "${list_3rdparty[@]}"; do

  # Name of the target folder
  tared_folder=$(basename "${file_3rdparty%.tar.gz}")

  # Print name of currently processed file
  echo -n "|--> ${tared_folder}: "

  # Point to build directory
  tared_folder="${dir_3rdparty}/${tared_folder}"

  # Remove the target folder if already exists and forced to clean
  if [[ -d "${tared_folder}" ]] && [ "${clean_dirs}" = true ]; then
      echo -n "cleaning old dir... "
      rm -rf "${tared_folder}"
  fi

  # Untar file into the target path if it does not exist
  if [[ ! -d "${tared_folder}" ]]; then
    echo -n "dir nonexistent; unpacking archive... "
    tar -xzf "${file_3rdparty}" -C "${dir_3rdparty}/"

  else # Folder exists; check if it has the correct files
    folder_files=$(find "${tared_folder}" | cut -c 16- | sed 's\[./]\\g' | sort)
    tar_files=$(tar -tf "${file_3rdparty}" | sed 's\[./]\\g' | sort)

    # List missing files, safety check because tar -d returns error if files missing...
    missing_files=$(diff -y --suppress-common-lines <(echo "${tar_files}") <(echo "${folder_files}") | grep -c "<")

    # If files are missing, overwrite (ignore extra files ">" as they are compilation files)
    if [ "${missing_files}" -gt 0 ]; then
      echo -n "missing files; overwriting... "
      tar -xzf "${file_3rdparty}" -C "${dir_3rdparty}/"

    else # Now that we know that no files are missing, we can compare
      ignore='!/Mode/ && !/Uid/ && !/Gid/ && !/time/ && !/build/ && !/cmake/ && !/tmp/ && !/grib_api_config.h/'
      different_files=$(tar --compare --file="${file_3rdparty}" -C "${dir_3rdparty}" 2>&1 | awk "${ignore}" | wc -l)

      # If files are different, overwrite
      if [ "${different_files}" -gt 0 ]; then
        echo -n "modified files; overwriting... "
        tar -xzf "${file_3rdparty}" -C "${dir_3rdparty}/"
      fi
    fi
  fi

  # Safety check: ensure all tar.gz have a equally named folder on the expected path
  if [[ ! -d ${tared_folder} ]]; then
    echo "ERROR: unmatched output dir. Check for potential errors."
    exit 1
  else
    echo "OK"
  fi

done

# Finish script and return
echo "ALL 3RD PARTY LIBRARIES UNPACKED."

exit 0