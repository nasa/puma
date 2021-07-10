FIND_PACKAGE(DoxyGen REQUIRED)
ADD_CUSTOM_TARGET(Documentation "${DOXYGEN}" "${TexGen_BINARY_DIR}/Docs/Doxyfile")
CONFIGURE_FILE(${TexGen_SOURCE_DIR}/Docs/Doxyfile.in
                ${TexGen_BINARY_DIR}/Docs/Doxyfile)


