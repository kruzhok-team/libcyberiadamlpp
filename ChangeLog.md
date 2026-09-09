# libcyberiadamlpp project changelog

## Unreleased

Fixed:
- the comment subjects of a copied element tree point into the copy;
- the top-level coordinates of a state machine without a rect stay global
  through the Qt geometry format (7.2.1), so a saved document keeps its
  placement instead of shifting by the content centre.

Added:
- `LocalDocument::set_file()` sets the file identity without a write.

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
- minor fixes.

## Version 1.0

Stable version of the C++ binding library for the `libcyberiadaml` project.

Known issues:
- limited standard API;
- bounding rect issues.

