find_package(PkgConfig REQUIRED)

pkg_check_modules(SQLCIPHER REQUIRED sqlcipher)

mark_as_advanced(SQLCIPHER_INCLUDE_DIRS SQLCIPHER_LIBRARIES SQLCIPHER_LIBRARY_DIRS)