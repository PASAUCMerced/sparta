#!/bin/bash

set -e

echo -e "This script will do an out-of-tree build of ParTI into the '\x1b[1;34mbuild\x1b[0m' directory by calling Meson."
echo
echo -e "To make things easier, you can write your personalized Meson options in '\x1b[1;34mbuild.args\x1b[0m',"
echo -e "and define environment variables in '\x1b[1;34mbuild.env\x1b[0m' which will be passed to Meson."
echo -e "If you want to use alternate compilers (e.g. Intel C++ Compilers), please write '\x1b[1;34mCC=icc CXX=icpc\x1b[0m' to '\x1b[1;34mbuild.env\x1b[0m'."
echo "================================================================================"
echo

declare -a MESON_ENV
[ -e build-new.env ] && MESON_ENV=("${MESON_ENV[@]}" $(<build-new.env))
for i in "${MESON_ENV[@]}"
do
    export "$i"
done

declare -a MESON_ARGS
[ -e build-new.config ] && MESON_ARGS=("${MESON_ARGS[@]}" $(<build-new.config))
MESON_ARGS=("${MESON_ARGS[@]}" "$@")

if [ -e build/build.ninja ]
then
    if [ "${#MESON_ARGS[@]}" -eq 0 ]
    then
        ninja -C build reconfigure
    else
        meson configure build "${MESON_ARGS[@]}"
    fi
else
    meson build "${MESON_ARGS[@]}"
fi

ninja -C build

echo -e "Finished. Check the '\x1b[1;34mbuild\x1b[0m' directory for results."
