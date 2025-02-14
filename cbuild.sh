#!/usr/bin/env bash

      # Function to display help information
      show_help() {
          echo "Usage: $0 [OPTION]"
          echo "Options:"
          echo "  -c, --clean [debug|release|debug-coverage]    Clean the specified build type."
          echo "  -b, --build [debug|release|debug-coverage]    Build the specified build type."
          echo "  -i, --install [debug|release|debug-coverage]  Install the specified build type."
          echo "  -a, --all                                     Clean, build, and install all types."
          echo "  -h, --help                                    Show this help message."
          echo "  -z, --initialize                              Initialize the build directories."
          exit 0
      }

      # Function to clean the build
      clean_build() {
          local build_type=$1
          echo "Cleaning ${build_type} build..."
          cmake --build cmake-build-"${build_type}" --target clean
          echo "Clean finished for ${build_type} build."
      }

      # Function to compile the build
      compile_build() {
          local build_type=$1
          echo "Building ${build_type} build..."
          cmake --build cmake-build-"${build_type}" --target all
          echo "Build finished for ${build_type} build."
      }

      # Function to install the build
      install_build() {
          local build_type=$1
          echo "Installing ${build_type} build..."
          cmake --build cmake-build-"${build_type}" --target install
          echo "Install finished for ${build_type} build."
      }

      # Function to clean all builds
      clean_all() {
          echo "Cleaning all builds..."
          clean_build "debug"
          clean_build "release"
          clean_build "debug-coverage"
          echo "Clean finished for all builds."
      }

      # Function to build all builds
      build_all() {
          echo "Building all builds..."
          compile_build "debug"
          compile_build "release"
          compile_build "debug-coverage"
          echo "Build finished for all builds."
      }

      # Function to install all builds
      install_all() {
          echo "Installing all builds..."
          install_build "debug"
          install_build "release"
          install_build "debug-coverage"
          echo "Install finished for all builds."
      }

      # Function to initialize build directories
      initialize() {
          echo "Initializing build directories..."
          rm -rf cmake-build-debug
          rm -rf cmake-build-release
          rm -rf cmake-build-debug-coverage
          cmake -S . -B cmake-build-debug
          cmake -S . -B cmake-build-release
          cmake -S . -B cmake-build-debug-coverage
          echo "Initialization finished."
      }

      # Function to enter interactive mode
      interactive_mode() {
          echo "Select an option:"
          echo "1) Clean Debug"
          echo "2) Clean Release"
          echo "3) Clean Debug-Coverage"
          echo "4) Build Debug"
          echo "5) Build Release"
          echo "6) Build Debug-Coverage"
          echo "7) Clean and Build All"
          echo "8) Install Debug"
          echo "9) Install Release"
          echo "10) Install Debug-Coverage"
          echo "11) Install All"
          echo "12) Initialize"
          read -r -p "Enter your choice [1-12]: " choice

          case $choice in
              1) clean_build "debug" ;;
              2) clean_build "release" ;;
              3) clean_build "debug-coverage" ;;
              4) compile_build "debug" ;;
              5) compile_build "release" ;;
              6) compile_build "debug-coverage" ;;
              7)
                  clean_all
                  build_all
                  ;;
              8) install_build "debug" ;;
              9) install_build "release" ;;
              10) install_build "debug-coverage" ;;
              11) install_all ;;
              12) initialize ;;
              *)
                  echo "Invalid choice. Exiting."
                  exit 1
                  ;;
          esac
      }

      # Parse command-line options
      if [ "$#" -eq 0 ]; then
          # No command-line options provided; enter interactive mode
          interactive_mode
      else
          while [[ $# -gt 0 ]]; do
              case $1 in
                  -c|--clean)
                      clean_build "$2"
                      shift 2
                      ;;
                  -b|--build)
                      compile_build "$2"
                      shift 2
                      ;;
                  -i|--install)
                      install_build "$2"
                      shift 2
                      ;;
                  -a|--all)
                      clean_all
                      build_all
                      install_all
                      shift
                      ;;
                  -z|--initialize)
                      initialize
                      shift
                      ;;
                  -h|--help)
                      show_help
                      ;;
                  *)
                      echo "Unknown option $1"
                      show_help
                      ;;
              esac
          done
      fi

      exit 0