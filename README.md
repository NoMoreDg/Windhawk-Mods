# Dodi's Customizations

A Windows customization mod by [NoMoreDg](https://github.com/NoMoreDg), built for [Windhawk](https://windhawk.net/).

**Current source version: 1.21.1 — work in progress.**

I started this because I wanted more control over how my desktop looks: animated borders, icons that follow the same colors, cursor glow, trails, particles, and enough customization to make something of my own. It kept growing from there. There is now quite a lot packed into one mod, including custom effect formulas and experimental features.

Some combinations look really good, especially the cursor effects together with the borders. There are also bugs, unfinished parts, and combinations that need more testing. I'm sharing the project here while I keep working on it.

## Contents

- [How this project started](#how-this-project-started)
- [Why it isn't on Windhawk yet](#why-it-isnt-on-windhawk-yet)
- [What is included](#what-is-included)
- [Requirements and installation](#requirements-and-installation)
- [How it works](#how-it-works)
- [Start menu and Explorer icons](#icons-inside-start-and-file-explorer)
- [Borders and sampled colors](#borders-cinema-ambient-and-wallpaper-colors)
- [Music, day/night, and weather](#music-daynight-and-weather)
- [Cursor effects](#cursor-effects)
- [Experimental effects](#experimental-effects)
- [Custom effects](#writing-custom-effects)
- [CPU, GPU, smoothing, and caching](#cpu-gpu-smoothing-and-caching)
- [Known issues and testing](#known-issues-and-current-testing-status)
- [Troubleshooting](#troubleshooting)
- [Capture and network access](#screenshots-audio-levels-and-network-access)
- [What still needs work](#what-still-needs-work)
- [Credits and license](#credits-and-license)

## How this project started

This originally started as multiple standalone mods. They were very buggy, and getting them to behave properly together became a project of its own. Icons would refuse to update, effects would get stuck, startup behavior was inconsistent, and fixing one thing could break something else.

I ended up switching to AI assistance to help work through those problems. What started as debugging and fixing issues turned into adding more effects, combining the mods, and building most of the features that are here now.

**Most of the current features were implemented with AI assistance, mainly ChatGPT.** I've been coming up with ideas, testing the results, reporting what breaks, and going back through the code and behavior with it. ChatGPT has helped with implementation, research, debugging, optimization, and documentation. A big thank you to ChatGPT for helping this project get this far. ❤️

That also means there is still plenty to review and improve. Generated code needs real testing, and a successful compile or a simulated test does not tell me that everything will behave correctly on someone else's desktop.

## Why it isn't on Windhawk yet

I want to get this into a much better state before submitting it to Windhawk's mod collection.

There are working features here, but the whole project needs more polishing: consistent startup and cleanup, fewer rendering glitches, better performance, proper testing of the newer integrations, and clearer behavior when something is unsupported. I don't want to submit a huge feature list while people still have to fight the mod to use it.

For now, GitHub is where I'm sharing the source and continuing development. **The goal is to improve and stabilize it first, then submit it to Windhawk.** There is no release date for that, and this repository is not a claim that the mod has passed Windhawk's review.

The current settings also target Windhawk 2.0's newer interface features. Getting the mod ready involves more than waiting for a Windhawk release; the mod itself still needs work.

## What is included

| Area | Features |
| --- | --- |
| Desktop icons | Grayscale, colorize, transparency, shadow, glow, animated/custom colors, label hiding, and shortcut-arrow hiding. |
| Taskbar icons | Independent color, transparency, shadow, glow, and animation settings for app icons. Shared coordinates let effects travel across a group of icons. |
| Start menu icons | Separate settings for supported pinned-app, All apps, and folder-preview bitmap icons inside Start. Newer integration; live testing is still needed. |
| File Explorer icons | Separate effects for supported file/folder image-list icons, with a toggle to include the left navigation pane. Newer integration; live testing is still needed. |
| Selection styling | Desktop/Explorer selection fill, border, opacity, background blur, and selected-item styling on supported list views. |
| App borders | Width, radius, colors, opacity, built-in/custom effects, inactive-window behavior, per-app rules, targeting, and exclusions. |
| Screen borders and corners | Animated monitor borders, custom formulas, corner fill, corner size/shape, opacity, and fullscreen hiding. |
| Sampled-color effects | Cinema, timed Ambient, and Wallpaper Ambient. |
| Music, time, and weather | Audio-level response, day/night coloring, and optional weather-driven visuals. |
| Cursor effects | Silhouette glow, halo, trails, cursor copies, particles, image particles, fluid, custom colors, and experimental window interactions. |
| Experimental borders | Procedural ribbons and flame-like effects, mouse bending, sparks, and separate custom body/layout/particle formulas. |
| Extra experimental effects | Focus fades, click shockwaves, comet cursor copies, glass shimmer, magnetic border particles, and linked palettes. |
| Rendering and optimization | CPU rendering, partial Direct3D 11 acceleration, per-stage GPU switches, adaptive timing, smoothing, caching, and diagnostic controls. |

There is also a window motion-blur/trail option still present in the settings. **It has been reported not working, and that work is currently parked.** Leave it off for now.

Available presets vary by category. The border effects include things like Mercury V3/V6, breathing, neon, wave, rainbow, aurora, plasma, fire, stars, and shimmer. Icons have their own smaller preset selection. An icon's separate Glow setting is different from the Glow border preset.

## Requirements and installation

- Windows 11, x64. Other Windows versions and replacement shells are not covered by the current testing.
- Windhawk 2.0 alpha 7 or newer is the intended baseline for the collapsible settings, color pickers, sliders, and conditional controls. Older versions can reject the settings annotations.
- GPU mode requires a hardware Direct3D 11 device with feature level 11.0. CPU rendering remains available.

### Install from source

1. Download [dodis-customizations-combined.wh.cpp](dodis-customizations-combined.wh.cpp).
2. Open Windhawk and choose **Create a new mod**.
3. Replace the generated code with the complete contents of that file.
4. Compile the mod, leave editing mode, and open its settings.
5. Enable the features you want and save.

If you're updating an existing installation, back up your **Textual settings** and keep a copy of the previous source first. Update that existing local mod rather than running two copies of the same customization at once.

The official [Windhawk development guide](https://github.com/ramensoftware/windhawk/wiki/Creating-a-new-mod) explains the editor and source format. Official Windhawk builds are available from its [releases page](https://github.com/ramensoftware/windhawk/releases).

### Start small

I would start with one border effect or cursor glow, check that it behaves correctly, and then add the other features. Enabling everything immediately makes it much harder to work out which setting is causing a problem.

Live weather updates are enabled in the current defaults. Check that setting if you want to disable network requests; the initial coordinates are 0, 0, not an automatically detected location.

Start menu and File Explorer icon effects are separate categories and start disabled. Cursor effects have a master switch plus individual switches for their layers. A screen-border width of zero disables that border.

## How it works

The mod runs different parts in different processes:

- **Explorer** handles desktop/taskbar/Explorer icon integrations, supported selection styling, app-border overlays, and the cursor effects worker.
- **StartMenuExperienceHost** handles the Start menu icon integration.
- An interactive **Windhawk process** hosts the monitor-border and corner overlays.

Borders and cursor visuals use transparent overlay windows. They do not replace an application's real frame or turn the native Windows cursor into a different cursor pack. The app border follows its target window, and the cursor glow follows a sampled silhouette of the real pointer.

Icon effects process private copies of icon pixels. The Explorer integration does not edit the system image list or change files on disk. Start menu and taskbar paths can temporarily supply processed image sources, then restore the native source when it is still safe to do so.

There are startup/recovery paths, cached artwork and frames, visibility checks, and event-based window tracking to reduce repeated work. Those help, but an overlay is still a separate window. Very fast movement, a busy UI thread, or a slow capture/render can make an effect fall behind.

## Icons inside Start and File Explorer

### Start menu

This targets supported bitmap app logos in pinned apps, All apps, and known folder-preview containers. Visual-tree notifications discover XAML roots and realized elements; a bounded scan provides a fallback. Loaded writable bitmaps can be read directly. Other supported sources use asynchronous visual snapshots.

The integration preserves the native icon rectangle. Shadow/glow extending outside that rectangle is clipped. Snapshots are capped at 96 × 96 pixels, so large icons can lose detail.

Profile pictures, recommended content, font/vector glyph controls, search results, third-party Start menus, and unknown templates are excluded. Unsupported or empty captures keep the native icon and retry.

**This was recently reworked after the previous path was reported not working. The new code still needs live verification on the target Windows builds.** A log saying that a tree was discovered is not proof that an icon was successfully processed.

### File Explorer and the sidebar toggle

Explorer has a dedicated image-list drawing integration for supported modern `DirectUIHWND` file views and legacy lists. It captures a private copy of the native draw, including its overlay and draw-state flags, then applies the selected effects. Buffered painting is tracked so a memory DC can still be associated with its view.

Enable **Include navigation-pane icons (pinned folders and other sidebar entries)** to apply the same Explorer icon settings to the left navigation tree. It starts off.

That toggle covers the navigation tree as a whole, including pinned folders, drives, and other sidebar entries. It is not a pinned-items-only filter. Toolbar, address-bar, and tab icons are excluded, as are dialogs outside Explorer. Thumbnails rendered through a different path can remain unchanged.

The new Explorer path also needs live testing. Unsupported drawing paths use native rendering, and the previous supported list-view path remains available if no new draw hook can be installed.

## Borders, Cinema, Ambient, and wallpaper colors

Normal app borders have their own appearance, timing, inactive behavior, and per-app rules. Monitor borders follow the configured screen corners. Experimental borders can replace the normal border's appearance while keeping its targeting and exclusion rules.

The sampled-color effects have different jobs:

| Mode | What it does |
| --- | --- |
| Cinema | Repeatedly samples visible pixels near the relevant edges so the border follows changing content. |
| Ambient | Samples colors at a selected interval and holds them between updates. App-border Ambient samples outside the app edges; monitor Ambient samples screen edges. |
| Wallpaper Ambient | Tries to sample desktop wallpaper renderers, including Windows wallpaper, Lively Wallpaper, and Wallpaper Engine, with the Windows wallpaper image as fallback. |

Wallpaper applications can render through surfaces that do not expose usable capture pixels. In that case, the static wallpaper fallback may be what you see.

For app-border Cinema, **Sample Cinema outside the window** switches between inside-edge probes and the area outside the app. Outside probes can pick up the desktop and neighboring windows. Content covered by another window is sampled as it appears on screen; this does not read an app's private video data.

Ambient intervals range from 2 to 240 seconds. Cinema has fixed sampling targets plus Auto, Adaptive, and Adaptive Lite. Sampling speed is separate from animation FPS, and the selected rates are targets rather than guaranteed delivery rates.

### Per-app rules and exclusions

App-border rules can choose a different effect, palette, width, radius, opacity, or inactive appearance for an executable. For example:

```text
notepad.exe: effect=wave, color=#00FFAA, color2=#0066FF, width=4, count=3
```

Rules match executable names. The manual exclusion list also accepts paths, wildcard patterns, and window classes. Exclusions take priority over appearance rules.

**Honor Windhawk default and Advanced exclusions** combines the manual list with the supported Windhawk exclusion settings and a bundled snapshot of critical/incompatible/game patterns. It reads those settings without overwriting them. The bundled snapshot is dated 2026-10-15; future Windhawk list changes need an updated snapshot here.

These exclusions affect app borders and window motion trails. They do not automatically hide screen borders, monitor corners, or cursor effects. Unknown games and launchers may need manual exclusions. This does not guarantee anti-cheat compatibility.

## Music, day/night, and weather

Select the corresponding effect from an app-border or screen-border effect dropdown. The shared settings control its response and palette.

**Music Reactive** uses the selected Windows audio endpoint's peak level. Leave the device name empty for the default playback output, or enter a device name/unique part of it. Sensitivity, gain, attack/release smoothing, brightness pulse, speed response, color response, and a directional opacity mask have separate controls. This is volume-level response, not a frequency-spectrum visualizer.

**Day/night effects** calculate the Sun's position from the clock and configured latitude/longitude, then apply day, twilight, and night brightness/colors. Location is entered manually. The default 0, 0 points to the equator/prime meridian intersection, so change it if you want local sunrise/sunset timing.

**Weather effects** use current weather codes to drive the configured rain, cloud, sun, and snow appearance. Updates use Open-Meteo and require internet access. Failed requests keep the previous condition. Weather updates start enabled in the current defaults; disable **Enable live weather updates** if you don't want those requests. See the network section below.

## Cursor effects

Glow, trails, particles, and fluid have independent controls. You can combine them, and cursor effects have their own timing and optimization settings.

- **Glow and halo** follow the pointer silhouette. The native cursor artwork remains in control.
- **Trails** include ribbons, dots, afterimages, and cursor-copy styles, with distance-based sampling, lifetime, width, and spacing controls.
- **Particles** have shape, emission, gravity, drag, turbulence, and optional bouncing settings. Local PNG/BMP image particles are supported within the size limits in the settings/guide.
- **Fluid** uses a CPU simulation for velocity, pressure, and dye. Resolution, solver iterations, and coverage can get expensive quickly.
- **Custom formulas** can recolor glow, trail, and particle effects independently.
- **Experimental window interactions** affect the visuals; they do not confine the actual mouse pointer.

The shrinking comet copies are tinted silhouettes, so they do not preserve every original RGB color in a multicolor cursor pack. Cursor artwork changes and DPI are checked periodically, which can introduce a small update delay.

Cursor smoothing and capture visibility have their own switches. The global smoothing and capture options do not replace those controls. Saving cursor settings restarts its worker, which clears the current particles, trail history, and fluid field.

## Experimental effects

The experimental border controls live below Cursor effects. Their window and screen switches are independent. The corresponding normal border must be enabled, with a nonzero width.

These are **procedural 2D overlays with simulated depth**: ribbons, flame-like patterns, soft particles, and pointer bending. They are not a full 3D scene attached to the application.

The custom system has three separate fields:

| Field | Purpose |
| --- | --- |
| Border field | Defines body color and alpha per border pixel. |
| Scene/layout | Sets particle count and the body opacity multiplier once per target draw. |
| Particle instances | Sets each particle's contour position, depth, offsets, size, stretch, rotation, color, and alpha. |

The other experimental groups add:

- **Focus transitions:** fade between focused and unfocused app-border appearances. They do not merge neighboring windows or connect their borders.
- **Click shockwaves:** expanding rings from mouse-button presses.
- **Comet cursor trail:** shrinking, fading pointer silhouettes.
- **Glass shimmer:** traveling reflections and a mouse highlight on visible border pixels.
- **Magnetic particles:** gather border particles toward the nearby mouse position.
- **Linked palettes:** share two colors across selected app-border, monitor-border, and cursor renderers. This does not recolor icons or synchronize particle positions.

Each group has its own switch. Enabling every group adds work even if each individual effect looks simple.

## Writing custom effects

The detailed variable tables, limits, and recipes are included in the mod's **Details** page and in the README comment near the top of the [source file](dodis-customizations-combined.wh.cpp). This is a quick starting point.

Select **Custom** in the relevant category, then enter assignments separated by semicolons:

```text
r=1; g=0.3; b=0.05; a=1
```

`r`, `g`, `b`, and `a` use values from 0 to 1. The color pickers provide `c1r/c1g/c1b` and `c2r/c2g/c2b`. Temporary variables are allowed, and math functions include `sin`, `cos`, `pow`, `mix`, `clamp`, `smoothstep`, noise, and rainbow helpers.

A traveling two-color wave for a normal border or icon formula:

```text
k=0.5+0.5*sin(tau*(p*n-t*speed*0.1));
r=mix(c1r,c2r,k);
g=mix(c1g,c2g,k);
b=mix(c1b,c2b,k);
a=1
```

A rainbow:

```text
r=hr(p-t*0.1); g=hg(p-t*0.1); b=hb(p-t*0.1); a=1
```

For a normal border, `p` travels around the perimeter. On icons, `p` is the horizontal normalized coordinate. Icon groups use shared coordinates rather than restarting an identical wave at every icon.

For cursor fields, the meaning changes again: trail/particle `p` is normalized age, while glow `p` follows the silhouette angle. Check the cursor table in Details before moving a recipe between categories.

### Experimental particle example

Select the experimental **Custom** effect. To draw only a ring of traveling soft particles, put this in the scene/layout field:

```text
count=10; body=0
```

Then put this in the particle-instance field:

```text
pos=home-t*0.04;
depth=0.4;
size=1.3;
stretch=4;
rotation=0;
r=mix(c1r,c2r,seed);
g=mix(c1g,c2g,seed);
b=mix(c1b,c2b,seed);
a=0.95
```

`home` spaces the particles around the contour, `pos` moves them, and `depth` positions them across the available Reach. Offsets and size do not expand the overlay's drawing area. Keep the soft edges inside that area to avoid clipping.

### Limits to keep in mind

- Formulas are a small assignment language. They do not execute C++, JavaScript, external scripts, or arbitrary HLSL. There are no loops, persistent state, arrays, or strings.
- Use `if(condition,a,b)` for conditional choices. Do not paste Markdown fences into a setting.
- Sliders such as speed/count/intensity only affect a custom formula if it reads the relevant variables.
- Ambient/Cinema sample variables have no sampled colors on icons. Cursor aliases have different meanings; they are not screen probes.
- Cursor color formulas do not change trail geometry, particle physics, or fluid simulation.
- Normal formulas cannot expand a border's drawing area. Experimental formulas stay inside Reach.
- Selection styling, corner fill, motion trails, icon labels/arrows, and the independent icon shadow/glow controls do not have custom formula fields.
- Long per-pixel formulas can still be slow. The optimization paths have limits and fall back when necessary.

## CPU, GPU, smoothing, and caching

### GPU support is partial

CPU/GDI is the default. Direct3D 11 is an experimental compute backend for specific stages:

| GPU switches | Work moved to the GPU |
| --- | --- |
| App-border and screen-border formulas | Eligible normal border formula evaluation. |
| Experimental custom border fields | Eligible custom body-field formula evaluation. |
| Desktop, taskbar, Start menu, and Explorer icon formulas | Icon formula evaluation, with a separate switch for each category. |
| Cursor trail, particle, glow, and fluid blending | Interpolation and source-over composition of those layers, with a separate switch for each. |

**Selecting CPU hides and ignores the GPU controls. Selecting GPU shows them and restores their saved choices.** Enabled stages can use the GPU with CPU fallback; disabled stages stay on CPU.

There is still CPU work in GPU mode: icon non-formula processing, cursor simulation and source drawing, built-in experimental border presets, particle/layout formulas, capture, geometry, and other post-processing. Layered-window presentation still uses GDI, with GPU results read back before presentation. DirectComposition presentation without readback is not implemented.

There is no 50/50 CPU/GPU setting and no promise of a particular resource split. Readback and dispatch overhead can make GPU mode slower, especially for small or simple effects. A device-ready log only proves device creation; look for a successful stage/frame log to confirm actual GPU work.

### Smoothing

Smoothing blends intermediate frames for eligible lower-FPS animations. An effect calculated at 30 FPS can be presented with blended steps toward a 60 FPS target. It adds work, can add roughly an effect-frame of latency, and can leave ghosting during fast motion.

It does not create new motion detail or guarantee the appearance of a native 60 FPS animation. Higher-rate effects bypass unnecessary blending. Cursor layers have their own smoothing controls and stop requesting extra blend presentations once settled. Experimental borders keep their separate behavior.

### Caching and optimization

The Optimization category includes formula optimizations, experimental animation playback, memory budgets, cursor-history/artwork reuse, fluid limits, duplicate-frame checks, animation ceilings, and diagnostics.

Experimental **cached loop playback** stores a bounded set of rendered edge frames and keeps animation moving. It is different from **Freeze experimental borders while idle**, which intentionally holds a frame. Interactive work can bypass replay, and arbitrary custom effects may visibly repeat at a loop boundary.

If things start stuttering, reduce particle count, fluid resolution/iterations, border width/Reach, active-window count, and calculation FPS first. Compare the same effect with individual GPU switches on and off. A higher FPS number can increase cost without improving what actually reaches the screen.

## Known issues and current testing status

| Area | Current situation |
| --- | --- |
| Mercury effect switching | An intermittent app-border issue has required a disable/refocus/change/re-enable sequence. It does not happen every time and is not considered fully resolved. |
| Window motion blur | Reported not working; the option remains present, and repairs are parked. |
| Start menu and Explorer icons | Previous paths were reported not working. The integrations were reworked recently, but the new paths still need live Windows verification. |
| GPU acceleration | Implemented for the stages listed above. Simulated backend/pixel comparisons and generated-shader checks have passed; Windows shader compilation, real device execution, appearance, and performance still need verification. |
| Fast window movement | Lag, apparent slow motion, and imperfect attachment have been reported during development. Event tracking helps, but a separate overlay can still fall behind under load. |
| Experimental edges and corners | Seams, clipping, and bending problems have been reported. Closed-contour math and later fixes help, but custom formulas and unusual geometry can still expose problems. |
| Experimental app-border presentation tiles | Optional and still experimental. If the narrower tiles look wrong or behave worse, turn that option off. |

**v1.21.1 fixes the reported ambiguous `hresult`/`HRESULT` compile expression in the new diagnostics code. A fix for that error is not confirmation that the entire version has passed a full Windows build and runtime test.**

The older core effects have had hands-on development testing. Native tests with mocked Win32/WinRT/Direct3D APIs also exercise parts of the pixel pipelines, discovery, fallback, and cleanup logic. Those checks are useful, but they do not reproduce the Windows shell or every driver. There are no reliable whole-project performance percentages or universal compatibility claims here.

### Other problems you might run into

- A Windows update can change a taskbar symbol, Start menu template, shell image source, or drawing path and leave that integration unchanged or unable to attach.
- Explorer startup/restarts, virtualized items, recycled controls, and delayed icon loading can expose recovery bugs. Older versions sometimes needed the mod toggled again for icons to update; those scenarios remain important regression tests.
- Different monitor refresh rates, mixed DPI, HDR/color handling, and unusual scaling can expose alignment, sampling, or appearance differences that have not been fully tested.
- Other mods changing the same images, XAML properties, window subclasses, or overlay behavior can interfere. Trying this with fewer overlapping customizations helps narrow that down.
- Unsupported captures can return black/empty pixels or use a fallback. Wallpaper effects and some icon sources are particularly dependent on the renderer.
- Fast movement, complex formulas, many windows, large fluid fields, and layered effects can increase CPU use, memory use, or stutter.
- Saving settings can reset transient animation state. On an incomplete cleanup, the mod can retain inert callback code until the host exits rather than freeing code Windows might still call.

These are compatibility and testing concerns, not a statement that every one happens on every system.

## Troubleshooting

For an effect that is stuck or refuses to switch, use this exact order:

1. Disable the affected feature.
2. Click outside the app.
3. Click back inside the app.
4. Change the effect.
5. Re-enable the feature.

For the screen border, use width zero as the disable step. This is a workaround, not a fix for the Mercury switching bug.

For other issues:

- Check the category's master switch, opacity/transparency, inactive/fullscreen behavior, and exclusions. A screen border also needs nonzero width.
- Select **Custom** before expecting formula text to apply. Check the debug log for parser errors and fallbacks.
- If a custom effect is static, make sure it references `t`. Check which variables it uses before expecting a slider to change it.
- For Start, open the menu after saving and give discovery/capture time to finish. Look for **first processed bitmap applied**, not just a root-discovery message.
- For Explorer, check the hook/attached-view messages and **first processed file view/navigation pane icon drawn**. Enable the sidebar toggle separately if that is the area you want.
- If GPU mode behaves worse, select CPU or disable only the problematic GPU stage. Keep all other comparison settings the same.
- If an icon stays dark, remember that custom tint preserves source brightness. A fully transparent source pixel cannot be brought back by setting `a=1`.

If you report a bug, include the mod version, Windows build, Windhawk version, the affected category, relevant settings/formula, exact reproduction steps, and a short log. For rendering/performance issues, include GPU/driver details, monitor refresh/scaling, and whether the same problem happens on CPU. Remove personal paths or anything else you do not want to share from logs/settings before posting them.

## Screenshots, audio levels, and network access

The final **Show overlays in screenshots and screen sharing** switch controls supported capture exclusion for app borders, screen borders, monitor corners, and motion trails. It starts off. Cursor effects have a separate capture setting. Icon modifications are part of the shell's normal content and are not hidden by those overlay switches.

Capture behavior depends on the recording method. Window-only sharing may omit separate overlays even when capture visibility is on. Making overlays captureable can also let Cinema or screen-edge Ambient sample the overlays themselves.

Cinema/Ambient read visible screen pixels locally to choose colors. Music response reads audio levels from the configured Windows audio device.

When live weather is enabled, the mod requests current weather codes from [Open-Meteo](https://open-meteo.com/) using the configured latitude and longitude. Those coordinates are sent to that service. Disable live weather if you do not want those requests. Custom particle images are loaded from the local path selected in the settings.

## What still needs work

The priorities are to validate the new shell integrations on real Windows builds, improve rendering consistency and performance, get effect switching and startup/recovery into a more dependable state, and keep checking shutdown/cleanup behavior as the project grows.

More GPU work is possible, especially reducing readback and eventually changing presentation. That needs its own implementation and testing; the current GPU switches do not make the entire mod GPU-rendered.

Caption-button overlays and lock-screen customization are not included in this version. Ideas and future integrations are not a promise that they are already implemented.

I want to keep the customization and the fun parts while making the whole thing less fragile. Once it is stable enough for a wider release, the plan is to submit it to Windhawk.

## Credits and license

- **NoMoreDg** — project direction, customization ideas, hands-on testing, and ongoing development.
- **ChatGPT / OpenAI** — substantial AI assistance with implementation, debugging, research, optimization, and documentation.
- **m417z and Windhawk contributors** — Windhawk and upstream techniques referenced in the source, including taskbar integration and Start menu XAML diagnostics patterns.
- **Microsoft documentation** — the Windows, XAML, rendering, and Direct3D API references used during development.
- **Open-Meteo** — weather data for the optional weather effects.

The mod source declares **GPL-3.0-only**. Keep the source attribution and follow that license when redistributing or modifying the code. Individual upstream credits and references are retained in the source.
