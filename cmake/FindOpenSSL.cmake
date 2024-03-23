find_package(PkgConfig REQUIRED)

set(OPENSSL_USE_STATIC_LIBS TRUE)

pkg_check_modules(OPENSSL REQUIRED openssl>=3.3.0)

list(REMOVE_ITEM OPENSSL_LIBRARIES ssl)

if(CMAKE_TARGET_PLATFORM STREQUAL "Window")
	list(APPEND OPENSSL_LIBRARIES "crypt32")
	list(APPEND OPENSSL_LIBRARIES "ws2_32")
endif()

mark_as_advanced(OPENSSL_INCLUDE_DIRS OPENSSL_LIBRARIES OPENSSL_LIBRARY_DIRS)