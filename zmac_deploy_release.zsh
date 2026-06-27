#!/bin/zsh

set -euo pipefail

INSTALL_DIR=${INSTALL_DIR:-/usr/local/procmt}
ASSEMBLY_ROOT=${ASSEMBLY_ROOT:-$HOME/procmt_install_mac}
ASSEMBLY_DIR=${ASSEMBLY_DIR:-$ASSEMBLY_ROOT/procmt}
ARCHIVE_FILE=${ARCHIVE_FILE:-$ASSEMBLY_ROOT/procmt_macos_install.zip}
MACDEPLOYQT_BIN=${MACDEPLOYQT_BIN:-$(command -v macdeployqt || command -v macdeployqt6 || true)}
CODESIGN_IDENTITY=${CODESIGN_IDENTITY:--}

function info() {
  print -- "$*"
}

function fail() {
  print -u2 -- "$*"
  exit 1
}

function add_rpath_if_missing() {
  local binary_path="$1"
  local rpath_value="$2"

  if ! otool -l "$binary_path" | grep -Fq "$rpath_value"; then
    install_name_tool -add_rpath "$rpath_value" "$binary_path"
  fi
}

function codesign_path() {
  local target_path="$1"

  codesign --force --sign "$CODESIGN_IDENTITY" --timestamp=none --deep "$target_path"
}

function verify_codesign() {
  local target_path="$1"

  codesign --verify --deep --strict --verbose=2 "$target_path"
}

if [[ "$(uname -s)" != "Darwin" ]]; then
  fail "This deploy script must be run on macOS."
fi

if [[ ! -d "$INSTALL_DIR/bin" ]]; then
  fail "Install directory '$INSTALL_DIR/bin' not found. Run the build/install first."
fi

if [[ -z "$MACDEPLOYQT_BIN" ]]; then
  fail "macdeployqt not found. Install Qt tools or set MACDEPLOYQT_BIN explicitly."
fi

apps=("$INSTALL_DIR"/bin/*.app(N))
if (( ${#apps[@]} == 0 )); then
  fail "No .app bundles found in '$INSTALL_DIR/bin'."
fi

info "Using install dir: $INSTALL_DIR"
info "Using assembly dir: $ASSEMBLY_DIR"
info "Using macdeployqt: $MACDEPLOYQT_BIN"

rm -rf "$ASSEMBLY_DIR"
mkdir -p "$ASSEMBLY_ROOT"
ditto "$INSTALL_DIR" "$ASSEMBLY_DIR"

staged_apps=("$ASSEMBLY_DIR"/bin/*.app(N))
for app in "${staged_apps[@]}"; do
  info "Deploying Qt frameworks for ${app:t}"
  "$MACDEPLOYQT_BIN" "$app" -always-overwrite

  # If macdeployqt misses optional Qt components, copy them manually here.
  # Qt on macOS usually ships as frameworks rather than plain .dylib files.
  # Example for QtSvg and QtSvgWidgets, uncomment if one of your apps needs it:
  # framework_dir="$app/Contents/Frameworks"
  # mkdir -p "$framework_dir"
  # ditto /opt/homebrew/opt/qtsvg/lib/QtSvg.framework "$framework_dir/QtSvg.framework"
  # ditto /opt/homebrew/opt/qtsvg/lib/QtSvgWidgets.framework "$framework_dir/QtSvgWidgets.framework"
done

staged_libs=("$ASSEMBLY_DIR"/lib/*.dylib(N))
for dylib_path in "${staged_libs[@]}"; do
  add_rpath_if_missing "$dylib_path" "@loader_path"
  info "Signing ${dylib_path:t}"
  codesign_path "$dylib_path"
  verify_codesign "$dylib_path"
done

for app in "${staged_apps[@]}"; do
  executable_path="$app/Contents/MacOS/${app:t:r}"
  if [[ -x "$executable_path" ]]; then
    add_rpath_if_missing "$executable_path" "@executable_path/../../../../lib"
  fi

  info "Signing ${app:t}"
  codesign_path "$app"
  verify_codesign "$app"
done

find "$ASSEMBLY_DIR" -name '.DS_Store' -delete
rm -f "$ARCHIVE_FILE"
ditto -c -k --sequesterRsrc --keepParent "$ASSEMBLY_DIR" "$ARCHIVE_FILE"

info "Assembly completed successfully."
info "Staged tree: $ASSEMBLY_DIR"
info "Archive: $ARCHIVE_FILE"