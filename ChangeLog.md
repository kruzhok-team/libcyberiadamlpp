# libcyberiadamlpp project changelog

## Version 1.0.8

Added:
- `Document::update_geometry`: the geometry update keeping the drawn elements
  (libhtgeom 1.2); the packages require libcyberiadaml 1.0.8 and libhtreegeom 1.2;
- the test of the wide transition labels inside the reconstructed machine
  border (the point-sourced label rect of libhtgeom 1.1.1).
- `State::set_actions`, `State::clear_actions` and `State::remove_action`: the
  actions of a state replaced, cleared or removed one by one;
- `formatLapki`: the Lapki IDE dialect of Cyberiada 1.0 written for lapki-client and
  lapki-compiler (read as Cyberiada 1.0).

Fixed:
- the detected Arena and Lapki documents are Cyberiada, not legacy yEd.
- `Document::reset` clears the document name together with the metainformation (a
  document opened after a named one kept the old name).

## Version 1.0.7

Changed:
- the full geometry reconstruction follows the libhtgeom 1.1 layered layout.

Fixed:
- minor fixes.

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

