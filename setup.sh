#!/usr/bin/env bash
ORIG_INPUT_PARAMS="$@"
params="$(getopt -o d:ictpg -l directory:,install,clean,torch,python,global,nocuda --name "$(basename "$0")" -- "$@")"

if [ $? -ne 0 ]
then
    print_usage
fi

print_usage() {
	printf "bash $0 [-c <for clean>] [-i <for install>] [-t <for torch>] [-p <for python>] [-d <workspace_dir>]\n"
}

eval set -- "$params"
unset params

WITH_TORCH=0

while true; do
	case ${1} in
		-i|--install) INSTALL=true;shift;;
		-c|--clean) CLEAN=true;shift;;
		-t|--torch) WITH_TORCH=ON; shift;;
		-p|--python) WITH_PYTHON=true;shift;;
		-d|--directory) WS_DIR+=("${2}");shift 2;;
		-g|--global) GLOBAL=true;shift;;
		--nocuda) NOCUDA=true;shift;;
		--) shift;break;;
		*) print_usage
			exit 1 ;;
	esac
done

# Get directory of script
# https://stackoverflow.com/questions/59895/getting-the-source-directory-of-a-bash-script-from-within
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [[ ${INSTALL} ]]
then
	bash ${DIR}/cppsrc/setup.sh ${ORIG_INPUT_PARAMS}
	if [ $? -ne 0 ]; then
		echo "cppsrc build failed"
		exit 1
	fi
fi

if [[ ${WITH_PYTHON} ]]
then
	echo "Installing python bindings"
	pip install ${DIR}/cppsrc/core/python_bindings/
	if [ $? -ne 0 ]; then
		echo "python bindings build failed"
		exit 1
	fi
	pip install -e ${DIR}/python/learning/
	if [ $? -ne 0 ]; then
		echo "CoverageControlTorch Python package install failed"
		exit 1
	fi
fi

# if clean and WITH_PYTHON, then uninstall
if [[ ${CLEAN} ]] && [[ ${WITH_PYTHON} ]]
then
	echo "Cleaning python bindings"
	pip uninstall -y pyCoverageControl
	pip uninstall -y CoverageControlTorch
fi
# pip install -e ${DIR}/torch/python_bindings/
# if [ $? -ne 0 ]; then
# 	echo "pyCoverageControlTorch failed"
# 	exit 1
# fi
