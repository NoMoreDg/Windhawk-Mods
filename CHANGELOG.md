# Changelog

Changes to Dodi's Customizations, newest first.

This project is still a work in progress. An entry here describes what changed in the source; it does not mean every feature has been verified on Windows or every configuration is stable. See the [README](README.md) for setup, limitations, and troubleshooting.

## 1.21.1

### Fixed

- Fixed the ambiguous `hresult` / `HRESULT` return expression in the Start menu XAML diagnostics integration that caused the reported compilation error.
- Kept the existing settings and effects unchanged for this fix.

### Testing notes

- A full Windows build and live runtime test of this version still need confirmation. Fixing the reported expression does not establish that the entire mod compiles and runs correctly.

## 1.21.0

### Added

- Start menu icon discovery through XAML diagnostics, including scanning XAML island roots when the older `Window::Current` route cannot find them.
- File Explorer icon interception through image-list drawing, with targeting for supported icon views inside Explorer windows.
- An optional **Include navigation pane** setting for File Explorer icons. This covers the supported left navigation tree, including pinned folders and drives; it is not limited to pinned items.
- Bounded private image caches and capture limits for the new icon integrations.

### Changed

- Reworked the Start menu and File Explorer icon implementations beyond the earlier taskbar-derived settings.
- Renamed copied taskbar labels so the settings describe Start menu and File Explorer icons correctly.
- Preserved native icon dimensions and added restoration of owned Start menu image changes during cleanup.
- Kept native drawing as the fallback for unsupported, blank, or oversized Explorer icon draws.

### Testing notes

- Mock-based checks exercised targeting, image handling, restoration, cache limits, fallback paths, and lifecycle wiring.
- Those checks do not replace testing in the actual Start menu and File Explorer. Compatibility and visible behavior still need confirmation across Windows versions and Explorer views.

## 1.20.0

### Added

- Separate GPU switches for custom icon formula evaluation in desktop, taskbar, Start menu, and File Explorer icon categories.
- Separate GPU switches for cursor trail, particle, glow, and fluid interpolation/composition stages.
- CPU fallback when a supported GPU stage is unavailable, busy, unsuitable for the workload, or fails.

### Changed

- Expanded the optional Direct3D 11 backend beyond the existing border/custom-field work.
- Used shared, bounded GPU resources with serialized access and a CPU fallback when access would block.

### Limitations

- GPU acceleration covers specific processing stages. Icon capture, non-formula icon work, cursor source drawing, and fluid simulation still involve the CPU.
- Results are read back for the existing CPU/GDI presentation path. This is not a full GPU presentation rewrite.
- Regression checks included simulated GPU calculations and shader syntax checks; live Direct3D execution and overall performance gains still require Windows testing.

## 1.19.1

### Changed

- Made the per-category GPU controls conditional on selecting the Direct3D 11 backend.
- CPU mode hides and ignores those GPU switches while preserving their saved values for switching back later.
- Kept CPU/GDI as the default backend.

## Earlier development

The project grew through many iterations, including a rollback and reused version numbers. I am keeping the detailed numbered history above to changes that can be identified reliably rather than assigning features to uncertain releases.

Earlier development brought together:

- Screen and app borders, rounded-corner overlays, animated presets, and custom formulas.
- Cinema sampling, timed ambient colors, and wallpaper ambient options.
- Mercury effects, music-reactive behavior, and day/night controls.
- Desktop and taskbar icon customization and shared animation behavior.
- Cursor glow, trails, cursor duplication, particles, fluid effects, and custom formulas.
- Experimental borders, interactive deformation, particles, linked palettes, and focus transitions.
- Smoothing, adaptive update behavior, caching, optimization controls, and event-driven overlay positioning.
- Exclusion handling and a switch controlling whether supported overlays appear in captures.
- An optional Direct3D 11 backend with CPU fallback for supported stages.

These features have different levels of maturity. Motion blur troubleshooting is currently paused, and intermittent effect switching and compatibility issues remain documented in the README.

## Reporting a regression

If an update breaks something, include the mod version, Windows build, selected CPU/GPU backend, affected feature, relevant settings or custom formula, and the steps needed to reproduce it. Mention whether the problem also happens with other visual mods disabled, and attach relevant Windhawk logs when available.
