
To adapt this template to your own project, follow these steps:


In the root directory adapt/change/do the following:
--------------------------------------------------------------------

* [x] Edit AUTHORS
* [x] Edit LICENSE
* [x] Edit README.md
* [x] Rename ./template-config.cmake -> ./\<project>-config.cmake

CMakeLists.txt:
* [x]  Set META_PROJECT_*
* [x]  Set META_VERSION_*
* [x]  Set META_AUTHOR_*
* [x]  Set META_CMAKE_INIT_SHA (to the commit hash of the applied cmake-init template, e.g., to 83d7cbc29a6fcb74a98498e5b0fcebd953d9d5cc)
* [x]  Adjust INSTALL_* to the desired install locations for all systems (defaults should be fine for a start)


In subdirectory "./deploy/" do:
--------------------------------------------------------------------

deploy/CMakeLists.txt:
* [x] Rename deploy/packages/pack-template.cmake -> pack-\<project>.cmake

deploy/packages/pack-\<project>.cmake:
* [x] Adjust OPTION_PACK_GENERATOR to your liking for all systems
* [ ] Adjust package options, e.g., CPACK_DEBIAN_PACKAGE_DEPENDS, CPACK_DEBIAN_PACKAGE_SECTION, CPACK_DEBIAN_PACKAGE_PRIORITY, CPACK_RPM_PACKAGE_LICENSE, CPACK_RPM_PACKAGE_GROUP, ...




In subdirectory "./docs/api-docs/" do:
--------------------------------------------------------------------

docs/api-docs/doxyfile.in:
* [ ] Adjust INPUT tag (list of doxygen annotated sources)

docs/api-docs/CMakeLists.txt
* [ ] Adjust DEPENDS parameter to include all targets of project


In subdirectory "./docs/manual/" do:
--------------------------------------------------------------------

docs/manual/cmake-init.tex:
* [ ] Rename to match own project name

docs/manual/CMakeLists.txt
* [ ] Adjust source and pdf file name


In subdirectory "./source/tests/" do:
--------------------------------------------------------------------

source/tests/CMakeLists.txt:
* [x]  Set META_PROJECT_NAME


General stuff left to do:
--------------------------------------------------------------------

* [x] Rename and adjust targets in source/
* [x] Add new targets to source/CMakeLists.txt
* [x] Add new targets to ./{project}-config.cmake
* [ ] Add new targets to the INPUT tag in docs/api-docs/doxyfile.in
* [x] Remove data/DATA_FOLDER.txt
* [x] Populate data/
* [ ] Remove ADAPT.md
