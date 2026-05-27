#!/bin/bash

# ============================================================================
# Stages Management and Unified Compiling Script
# Usage: ./export.sh {x|export|c|compile|a|all|p|print|h|help}
# ============================================================================

# ANSI color codes for terminal logging
print_error() {
    local message="$1"
    echo -e "\033[31mERROR:\033[0m $message" >&2
    return 0
}

print_warning() {
    local message="$1"
    echo -e "\033[33mWARNING:\033[0m $message" >&2
    return 0
}

print_info() {
    local message="$1"
    echo -e "\033[32mINFO:\033[0m $message"
    return 0
}

check_git_repo() {
    if ! git rev-parse --git-dir > /dev/null 2>&1; then
        print_error "Not inside a git repository!"
        exit 1
    fi
    return 0
}

check_gitignore() {
    if [[ ! -f .gitignore ]]; then
        print_error ".gitignore does not exist!"
        exit 1
    fi

    if ! grep -q "^FCG_Stages$" .gitignore; then
        print_warning "FCG_Stages not found in .gitignore, adding it..."
        echo "FCG_Stages" >> .gitignore
    fi
    return 0
}

generate_wrapper_cmake() {
    local target_dir="FCG_Stages"
    local cmake_template="CMakeLists.stages.template"
    local cmake_file="$target_dir/CMakeLists.txt"
    
    if [[ ! -f "$cmake_template" ]]; then
        print_error "CMake template '$cmake_template' not found in project root!"
        exit 1
    fi

    print_info "Generating collective CMakeLists.txt inside $target_dir using template..."
    cp "$cmake_template" "$cmake_file"

    # Find and sort all Stage_XX subdirectories
    local stages=$(find "$target_dir" -maxdepth 1 -type d -name "Stage_*" | sort -V)
    
    echo "" >> "$cmake_file"
    echo "# ============================================================================" >> "$cmake_file"
    echo "# DYNAMICALLY APPENDED SUBDIRECTORIES (STAGES)" >> "$cmake_file"
    echo "# ============================================================================" >> "$cmake_file"

    for stage in $stages; do
        local stage_name=$(basename "$stage")
        echo "message(STATUS \"[Wrapper] Adding subdirectory module: $stage_name\")" >> "$cmake_file"
        echo "add_subdirectory($stage_name \${CMAKE_BINARY_DIR}/build_$stage_name)" >> "$cmake_file"
    done

    return 0
}

export_command() {
    check_git_repo
    check_gitignore

    local current_branch=$(git branch --show-current)

    # Guard against detached HEAD
    if [[ -z "$current_branch" ]]; then
        print_error "Repository is in detached HEAD state. Switch back to a branch first."
        exit 1
    fi

    # Ensure local HEAD is fully pushed and matches origin tip
    local branch_tip=$(git rev-parse "$current_branch")
    local current_head=$(git rev-parse HEAD)

    if [[ "$branch_tip" != "$current_head" ]]; then
        print_error "Local HEAD differs from branch tip '$current_branch'. Push all commits before exporting."
        exit 1
    fi

    # Fetch standard SemVer tags (vX.Y.Z)
    local all_tags=$(git tag -l "v[0-9]*.[0-9]*.[0-9]*" 2>/dev/null)

    if [[ -z "$all_tags" ]]; then
        print_error "No SemVer tags found matching vX.Y.Z (required for staging)"
        exit 1
    fi

    # Filter tags: keep only the latest patch version for each minor version release
    local tags=$(echo "$all_tags" | sort -V | awk -F. '{key=$1"."$2; arr[key]=$0} END {for (k in arr) print arr[k]}' | sort -V)

    # Check for untracked unignored files
    local untracked=$(git ls-files --others --exclude-standard)
    if [[ -n "$untracked" ]]; then
        print_error "Repository has untracked, unignored files. Remove or commit them:"
        echo "$untracked" | sed 's/^/  /'
        exit 1
    fi

    # Ensure repository status is fully clean
    if ! git diff-index --quiet HEAD --; then
        print_error "Repository has uncommitted changes. Commit or stash them."
        exit 1
    fi

    if [[ ! -d FCG_Stages ]]; then
        mkdir -p FCG_Stages
    fi

    print_info "Beginning stages export cycle..."

    local count=1
    for tag in $tags; do
        local stage_num=$(printf "%02d" "$count")
        local stage_dir="FCG_Stages/Stage_$stage_num"

        print_info "Checking out $tag -> exporting to $stage_dir"

        # Checkout target version tag
        git checkout "$tag" > /dev/null 2>&1
        if [[ $? -ne 0 ]]; then
            print_error "Could not checkout tag: $tag"
            git checkout "$current_branch" > /dev/null 2>&1
            exit 1
        fi

        # Safely remove old staging folder
        if [[ -d "$stage_dir" ]]; then
            rm -rf "$stage_dir"
        fi
        mkdir -p "$stage_dir"

        # Copy only Git tracked files
        git ls-files | while read -r file; do
            # Filter out git configuration, templates, scripts and pipeline definitions
            if [[ $(basename "$file") == .git* ]] || \
               [[ "$file" == .github* ]] || \
               [[ "$file" == .clang-format ]] || \
               [[ "$file" == release-please-config.json ]] || \
               [[ "$file" == LICENSE ]] || \
               [[ $(basename "$file") == export.sh ]] || \
               [[ $(basename "$file") == CMakeLists.stages.template ]]; then
                continue
            fi
            mkdir -p "$(dirname "$stage_dir/$file")"
            cp "$file" "$stage_dir/$file"
        done

        count=$((count + 1))
    done

    # Revert workspace to main/original development branch
    print_info "Returning workspace to branch '$current_branch'..."
    git checkout "$current_branch" > /dev/null 2>&1

    print_info "Copying documentation and assets to FCG_Stages root..."

    # Copy the main README.md if exists
    if [[ -f README.md ]]; then
        cp README.md FCG_Stages/
        print_info "  -> README.md copied successfully."
    fi

    # Copy screenshot folder recursively
    if [[ -d resources/screenshots ]]; then
        mkdir -p FCG_Stages/resources
        cp -r resources/screenshots FCG_Stages/resources/
        print_info "  -> Screenshots folder copied successfully."
    else
        print_warning "No 'resources/screenshots' folder found to copy."
    fi

    # Generate the root wrapper CMakeLists.txt using the template
    generate_wrapper_cmake

    print_info "Staging export process completed successfully!"

    return 0
}

compile_command() {
    local original_dir=$(pwd)

    if [[ ! -d FCG_Stages ]] || [[ ! -f FCG_Stages/CMakeLists.txt ]]; then
        print_error "FCG_Stages workspace is not initialized. Run export first."
        exit 1
    fi

    print_info "Starting centralized build of all stages..."
    cd FCG_Stages || exit 1

    # Configure CMake build tree
    print_info "Running: cmake -B build -D CMAKE_BUILD_TYPE=Release"
    cmake -B build -D CMAKE_BUILD_TYPE=Release
    if [[ $? -ne 0 ]]; then
        print_error "CMake configuration failed for FCG_Stages."
        cd "$original_dir"
        exit 1
    fi

    # Execute build step
    print_info "Running: cmake --build build"
    cmake --build build
    if [[ $? -ne 0 ]]; then
        print_error "Build compilation cycle failed!"
        cd "$original_dir"
        exit 1
    fi

    cd "$original_dir"
    print_info "All Stages successfully built!"
    print_info "Stage executables are available inside:"
    print_info "  FCG_Stages/build/bin/Stage_XX/"

    return 0
}

# ============================================================================
# COMMAND DISPATCHER
# ============================================================================

usage() {
    echo ""
    echo "Usage: $0 {x|c|a|p|h}"
    echo "Commands:"
    echo "  x|export      Filter SemVer tags, clean copy project files, generate Stages + wrapper CMake"
    echo "  c|compile     Compile all exported stages simultaneously in parallel"
    echo "  a|all         Sequence both export and compile pipelines"
    echo "  p|print       Print the filtered list of SemVer tags mapped to output stages"
    echo "  h|help        Print this assistance dialogue"

    return 0
}

main() {
    if [[ $# -eq 0 ]]; then
        print_error "No instruction provided"
        usage
        exit 1
    fi

    local command="$1"
    shift

    case "$command" in
        x|"export")
            export_command
            ;;
        c|compile)
            compile_command
            ;;
        a|all)
            export_command && compile_command
            ;;
        p|print)
            check_git_repo
            all_tags=$(git tag -l "v[0-9]*.[0-9]*.[0-9]*" 2>/dev/null)
            if [[ -n "$all_tags" ]]; then
                echo "Detected SemVer tags selected for Stage mapping:"
                echo "$all_tags" | sort -V | awk -F. '{key=$1"."$2; arr[key]=$0} END {for (k in arr) print arr[k]}' | sort -V | sed 's/^/  - /'
            else
                print_warning "No valid SemVer tags found in git history."
            fi
            ;;
        h|help)
            usage
            ;;
        *)
            print_error "Command unrecognized: $command"
            usage
            exit 1
            ;;
    esac

    return 0
}

main "$@"
