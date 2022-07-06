#!/bin/bash
###############################################################################
# Helper file to generate the APG project
###############################################################################

show_help()
{
    echo "--------------------------------------------------------------------"
    echo "Build Automated Playlist Generator Player."
    echo "Optional arguments are:"
    echo "    --release"
    echo "        Builds the release version."
    echo "    --regenerate-project"
    echo "        Regenerates the xcode project, without building."
    echo "    --juce-path"
    echo "        Optional path to your JUCE checkout. Must be JUCE 6"
    echo "    --output-path"
    echo "        Sets the build output."
    echo "        By default the project is placed in a projects folder"
    echo "        in the root of the directory."
    echo "--------------------------------------------------------------------"
    exit 0
}

print_banner()
{
    echo
    echo "====================================================================="
    echo "$1"
    echo "====================================================================="
}
#------------------------------------------------------------------------------
abort()
{
    print_banner "Error: $1"
    exit 1
}
#------------------------------------------------------------------------------
create_project_using_cmake()
{
    if [ -z "$JUCE_DIR" ]; then
        echo "Using default juce path"
        JUCE_DIR="$APG_ROOT/external/JUCE"
    fi

    print_banner "Generating project files from CMake"
    mkdir -p $BUILD_OUTPUT_DIR  > /dev/null 2>&1 || abort "Failed to create project folder"

    if [[ "$OSTYPE" == "darwin"* ]]; then
        GENERATOR="Xcode"
        ADDITIONAL_BUILD_FLAGS=""
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        GENERATOR="Unix Makefiles"
        ADDITIONAL_BUILD_FLAGS="-DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++"
    fi

  # Creates the project -S: CMakeLists location  -B: directory to place build files
    cmake -G "$GENERATOR" -S $APG_ROOT -B $BUILD_OUTPUT_DIR \
    -DJUCE_DIR=$JUCE_DIR -DBUILD_OUTPUT_DIR=$BUILD_OUTPUT_DIR \
    $ADDITIONAL_BUILD_FLAGS
}
#------------------------------------------------------------------------------
build_project()
{
    APG_PROJECT="$BUILD_OUTPUT_DIR/APG.xcodeproj"
    xcodebuild \
        -project       "${APG_PROJECT}" \
        -target        APG              \
        -configuration $CONFIGURATION   \
        || abort "Project build failed"
}

###############################################################################
# Locations and configuration
: ${APG_ROOT="$( cd "$(dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"}
: ${BUILD_OUTPUT_DIR:="$APG_ROOT/projects"}
: ${CONFIGURATION:=Debug}
: ${REGENERATE_PROJECT:=no}

 XCODE_TRANSIENT_FILES="$BUILD_OUTPUT_DIR/XcodeTransientFilesSafeToDelete"


 while [ -n "$1" ]; do
     case "$1" in
         --release)
             CONFIGURATION=Release
             ;;
         --regenerate-project)
             REGENERATE_PROJECT=yes
             ;;
        --clean)
             rm -rf $BUILD_OUTPUT_DIR && print_banner "Project folder deleted"
             exit 0
             ;;
         --juce-path)
             shift
             PARAM="$1"
             if [ -z "$PARAM" ]; then
                 echo "--juce-path must be passed with an absolute path to a juce directory"
                 exit 1
             else
                 JUCE_DIR="$PARAM"
             fi
             ;;
         --output-path)
             shift
             PARAM="$1"
             if [ -z "$PARAM" ]; then
                 echo "--output-path must be passed with a value"
                 exit 1
             else
                 BUILD_OUTPUT_DIR="$PARAM"
             fi
             ;;
         --help)
             show_help
             ;;
        * )
            echo "Unknown option $1."
            echo "Showing usage."
            show_help
            ;;
     esac
     shift
 done
#------------------------------------------------------------------------------
create_project_using_cmake || abort "project generation failed"

print_banner "Project creation successful"
if [ $REGENERATE_PROJECT == "yes" ]; then
    exit 0
fi

build_project || abort "Build failed"
print_banner "Project built"