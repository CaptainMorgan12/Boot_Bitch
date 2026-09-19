# Packaged desktop entries point AppStream at the installed icon path.
#
# Discover 6.7.x built against KIconThemes 6.28 fails to resolve stock theme
# icons: AppStreamUtils::kIconLoaderHasIcon() compares bare icon names against
# KIconLoader::queryIcons(), which returns file paths, so stock-only components
# fall back to the package-x-generic placeholder. AppStream's desktop-entry
# importer turns an absolute Icon= value into a local icon, which Discover
# loads directly. The source desktop entry and the AppImage keep the themed
# icon name; only the packaged copy is rewritten.

if(NOT CPACK_PACKAGING_INSTALL_PREFIX)
    set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
endif()

set(_icon_path "${CPACK_PACKAGING_INSTALL_PREFIX}/share/icons/hicolor/512x512/apps/org.bootrepair.BootRepair.png")
set(_desktop_file "${CPACK_TEMPORARY_DIRECTORY}${CPACK_PACKAGING_INSTALL_PREFIX}/share/applications/org.bootrepair.BootRepair.desktop")

if(NOT EXISTS "${_desktop_file}")
    message(WARNING "Desktop entry not found for packaged icon rewrite: ${_desktop_file}")
    return()
endif()

file(READ "${_desktop_file}" _desktop)
string(REPLACE
    "Icon=org.bootrepair.BootRepair\n"
    "Icon=${_icon_path}\n"
    _desktop "${_desktop}")
file(WRITE "${_desktop_file}" "${_desktop}")
