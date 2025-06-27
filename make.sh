#!/usr/bin/env bash

# a simple utility to make and clean build dir (run only if the build dir exists!)

function cd_and_start {
    curr_dir=$(pwd)

    #cd "builds" || { echo "Error: Could not change to 'build' directory. Aborting."; exit 1; }
    if ! cd "build" 2>/dev/null; then
        printf "Build directory not found... Do you want to generate the build directory? (y/Y/S/s/n/N) "
        read ans
        case $ans in
        "y"|"Y"|"S"|"s")
            if cmake -B build -DCMAKE_BUILD_TYPE=Release; then
                echo -e "Build generated with sucess! Re-run the script"
                # exit 0
            else
                echo -e "Could not generate the Build with CMake command... Try again"
                exit 1
            fi
        ;;
        *)
            exit 0
        ;;
        esac
    fi


    if [[ "$(basename "$(pwd)")" == "build" ]]; then
        echo -e "Entered 'build' directory.\n"

        if [[ -f "rhythin" && -x "rhythin" ]]; then
            echo "Executable 'rhythin' found in 'build' directory. Proceeding with copy and cleanup."
            
            if sudo cp rhythin /usr/bin; then
                echo -e "rhythin copied to /usr/bin. Initiating cleanup..."

                if rm -r rhythin; then
                    echo -e "Cleaned 'rhythin' successfully and exiting."
                    cd "$curr_dir" || { echo "Error: Could not return to original directory."; exit 1; }
                    exit 0
                else
                    echo "Error: Failed to remove 'rhythin' binary from build directory."
                    cd "$curr_dir"
                    exit 1
                fi
            else
                echo "Error: Failed to copy 'rhythin' to /usr/bin. Check permissions."
                cd "$curr_dir"
                exit 1
            fi
        else
            echo "Executable 'rhythin' not found or not executable in 'build' directory."
            echo "Attempting to run 'make' to build 'rhythin'..."
            
            if make; then
                echo "'make' completed successfully. Please re-run this script to copy and clean up."
                cd "$curr_dir"
                exit 0
            else
                echo "Error: 'make' command failed. Could not build 'rhythin'."
                cd "$curr_dir"
                exit 1
            fi
        fi
    else
        echo "Error: Current directory is not 'build' after attempting to change."
        cd "$curr_dir"
        exit 1
    fi
}

cd_and_start