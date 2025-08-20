#!/bin/bash

SKIP_CONFIRM=0

while getopts "y" opt; do
    case $opt in
        y)
            SKIP_CONFIRM=1
            ;;
        *)
            ;;
    esac
done

echo "WARNING: This script will format all .cpp, .h, .c, .hpp files in project directory."
echo "This script will overwrite the files with the formatted version."

if [ $SKIP_CONFIRM -eq 0 ]; then
        read -p "Do you want to continue? (y/n) " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
                echo "Aborted."
                exit 1
        fi
fi

if [ ! -f .clang-format ]; then
        echo "ERROR: .clang-format file not found."
        exit 1
fi

if ! command -v clang-format &>/dev/null; then
        echo "ERROR: clang-format not found."
        exit 1
fi

echo "Formatting files..."

find src include \( -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h" \) | xargs clang-format -i

echo "Done."

exit 0