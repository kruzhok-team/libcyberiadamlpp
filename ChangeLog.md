# libcyberiadamlpp project changelog

## Version 1.0.7 (unreleased)

Fixed:
- the transition color was saved only together with the transition geometry.

## Version 1.0.6

The version compatible with `libcyberiadaml` 1.0.6 and the CyberiadaML-GraphML 1.0
(PNST_1044-2025) standard.

Added:
- the strict standard checks support;
- the export options and the yEd dialects on saving;
- the event propagation keywords in actions;
- the comment subjects on transitions;
- the declared geometry format of the document (`none` / `short` / `full`);
- the ctest-based test suite with the valgrind checks.

Fixed:
- the geometry reconstruction and the bounding rect calculation;
- the CMake package configuration is installed into its own directory;
- minor fixes.

## Version 1.0

Stable version of the C++ binding library for the `libcyberiadaml` project.

Known issues:
- limited standard API;
- bounding rect issues.

