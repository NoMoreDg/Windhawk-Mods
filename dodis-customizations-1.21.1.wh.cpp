// ==WindhawkMod==
// @id              dodis-customizations
// @name            Dodi's Customizations
// @description     Customize icons, borders, corners and cursor effects, with custom formulas and optimization controls. If an effect gets stuck: disable its feature, click outside, click inside the app, change the effect, then re-enable.
// @version         1.21.1
// @license         GPL-3.0-only
// @author          NoMoreDg
// @github          https://github.com/NoMoreDg
// @include         explorer.exe
// @include         windhawk.exe
// @include         StartMenuExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lgdi32 -lmsimg32 -luser32 -ldwmapi -lwinmm -ladvapi32 -lshlwapi -lole32 -loleaut32 -lruntimeobject -lshcore -lwindowscodecs -luuid -lshell32 -lmmdevapi -lwinhttp
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Dodi's Customizations

By **NoMoreDg** — https://github.com/NoMoreDg

Customize desktop, taskbar, Start menu and File Explorer icons, selection boxes, app borders, screen borders,
rounded monitor corners, window motion trails, and cursor effects. Choose built-in
animations or write custom color and opacity formulas. Mercury, audio response,
day/night coloring and live weather are included.

Cursor effects include silhouette glow, trails, particles, image particles and a
bounded 2D fluid simulation. Experimental borders add electric ribbons, flame-like
edges, traveling sparks and mouse bending. Both experimental border switches start off.
Six optional experimental subcategories add focus fades, click rings, comet cursor copies,
glass reflections, magnetic border particles and linked palettes; each starts disabled.

**Effect troubleshooting:** If an effect does not apply, stops updating, or keeps showing
an earlier selection, follow this order: disable the affected feature, click outside the
app, click back inside the app, change the effect, then re-enable the feature. This is a
general troubleshooting step for all effect categories and a temporary workaround for
the reported Mercury app-border switching issue; it is not a guaranteed fix.

## Setup

Use Windhawk 2.0 alpha 7 or later for collapsible categories, color pickers and sliders.
Save a copy of your Textual settings before updating the source. Existing icon and
border setting paths are retained. Expand a category and enable only the features you
want. Cursor effects have their own master switch; screen border width 0 disables it.
Desktop/taskbar discovery and recovery run automatically as Explorer creates its UI.

The cursor worker runs in Explorer and shares the mod's start and stop lifecycle.
Saving settings safely restarts that worker; its fluid ink and particle history reset.
Icon hooks are not unloaded for this cursor restart. Shutdown signals render workers
before UI restoration; weather waits are interruptible between bounded network operations.
Monitor effects render in the
Windhawk host. A failure to start cursor effects leaves the other features available.

## GPU rendering (experimental)

The default **CPU / GDI** backend remains available. The Direct3D 11 backend moves
eligible **border/icon formula evaluation and cursor-layer interpolation/composition** onto a hardware GPU.
Formula stages use compute shaders generated from the existing parsed formula; cursor blending
uses a fixed integer compute kernel. The built-in effects
and Custom text boxes keep their existing syntax. It does not accept arbitrary HLSL,
shader files, includes or downloads. Only Windows system copies of d3d11.dll and
d3dcompiler_47.dll are loaded. Hardware feature level 11.0 is required; unsupported hardware
uses the existing CPU renderer rather than software-emulated Direct3D.

The backend dropdown is the master control. **CPU / GDI** hides all GPU target switches
and GPU tuning controls and ignores them at runtime. **Direct3D 11 compute + GDI** shows
those controls: enabled targets may use the GPU with CPU fallback; disabled targets use
CPU. Switching back to GPU restores the saved choices rather than resetting them.

To try it:

1. Open **GPU rendering (experimental)** and select **Direct3D 11 compute + GDI**.
2. Leave the desired category switches on. The instructions below test border formulas;
   for icons, choose an animated formula and look for the category stage log; cursor switches
   accelerate blending of their enabled layers.
3. Disable that target's experimental border field and choose an animated normal effect,
   such as Mercury or a custom formula using `t`. To test experimental fields instead, select
   Experimental → Custom and enable **Use GPU for experimental custom border fields**. Keep the FPS control at a reasonable
   rate; increasing FPS does not prove GPU work is faster.
4. In Windhawk debug logging, look for **hardware compute device ready**, followed by
   **first border formula frame evaluated on GPU**. Device creation alone does not confirm
   a GPU-rendered frame. Background shader compilation initially leaves frames on CPU.
5. Compare the same effect, dimensions and FPS against CPU / GDI. Select CPU again if the
   transfer overhead or contention with a game makes it worse.

Each implemented stage has its own switch:

| GPU switch | Stage accelerated | Remaining CPU work |
| --- | --- | --- |
| App / Screen border formulas | Normal border formulas | Geometry and final presentation |
| Experimental custom border fields | Custom field formulas, with the matching border target enabled | Built-in presets, layout and particles |
| Desktop / Taskbar / Start menu / Explorer icon formulas | Animated and custom icon formula evaluation | Colorize/grayscale, shading, alpha, shadows, glow and presentation |
| Cursor trail blending | Interpolation and source-over blending, including cursor copies | Trail generation and custom colors |
| Cursor particle blending | Interpolation and source-over blending, including click rings | Particle simulation and drawing |
| Cursor glow blending | Interpolation and source-over blending, including the separate glow window | Glow generation and custom colors |
| Cursor fluid blending | Interpolation and source-over blending | Fluid simulation and source drawing |

These switches are independent of one another; cursor and icon switches do not depend on
app/screen border switches. The backend dropdown gates every GPU stage. Icons using only
grayscale, colorize, transparency, shadows or glow have no formula work to offload. The
Start menu and Explorer switches apply when their icon integration supplies bitmap pixels;
unsupported source/draw paths retain native rendering.

Icon formulas retain local/shared coordinates, animation time, source brightness and alpha.
Plan generation is cached with loaded icon settings; GPU work never writes the original
icon bitmap. First successful dispatches log the category name and **stage evaluated on GPU**.
The icon pool serializes its immediate context; a busy pool falls back to CPU without waiting.
Cursor layers use a separate pool. GPU cursor math uses the same integer rounding as the CPU
interpolation/composition. A layer above 131072 clipped pixels stays on CPU; it is not
partially blended. Source layers remain cached by the existing renderer.

The new pools each keep at most 131072 input records (6 MiB), plus compute/output/staging
buffers and driver/shader overhead. Icon formula result copies add at most 2 MiB per active
call. Small work, pending compilation, contention, shutdown and failures preserve the current
CPU frame. Pools release resources and join shader compilation on settings reload/unload.
Readback can outweigh the arithmetic saved, especially for simple cursor blending; disable
an individual switch if it performs worse. This version makes no measured speedup claim.

**This is a hybrid backend.** Geometry preparation/upload, sampled-color capture,
music/solar/weather post-processing, shimmer/focus/palette passes, border/icon smoothing, and GDI
layered-window presentation remain on their current paths. GPU colors are read back before
those CPU steps. Built-in experimental presets, particle/layout formulas, cursor source
drawing/simulation, icon non-formula processing, corner fill, selection and motion trails
remain on CPU. DirectComposition presentation without readback is not implemented yet.
Consequently GPU mode can be slower, especially for short or simple formulas and small
bands. There is no measured FPS or CPU reduction claim for this version.

The **minimum pixels** setting keeps small workloads on CPU. A dispatch supports at most
131072 normal edge pixels, 1024 parsed expression nodes and 128 assignments. Experimental
custom fields retain their existing 256-node limit and run in chunks of at most 65536 pixels.
A small final chunk below the minimum edge-pixel threshold stays on CPU. Larger valid formulas
keep working on CPU; their normal parser limit is unchanged. At most eight distinct shader
attempts are cached across normal and custom-field formulas per render worker per settings epoch;
the icon pool has a separate eight-shader cache, and the cursor pool uses one fixed kernel.
Extra formulas use CPU. Static
borders and icons can finish their initial CPU frame before shader compilation completes and remain
on CPU until another redraw. GPU floating-point functions can differ slightly from CPU
results, especially noise/trigonometry and formulas near thresholds.

Experimental custom-field acceleration preserves unassigned channels from the original
fallback strand, per-pixel `prox`/`ab`, pointer coordinates, and the field common variables
(`reach`, `radius`, `perimeter`, `clock`, `enter`, `inside`, `mx`, `my`). Layout formulas,
particle generation, magnetic positioning and composition retain their CPU implementations.
Uniform fields stay on the existing once-per-frame CPU optimization when enabled. Existing
animation replay caching remains available and can reuse already rendered frames. First-use
shader compilation or any GPU failure keeps the current CPU field result; no blank field is
substituted. A successful field dispatch logs **first experimental custom field evaluated
on GPU**. Switching back to CPU or unloading also releases the field scratch records.

The **GPU completion polling budget** is 1–4 ms per attempted dispatch. Results not ready
within it use the current CPU frame, never a stale GPU frame. While an old dispatch is
pending the worker skips new dispatches. Three consecutive completion/readback misses
suspend GPU work in that worker until settings reload; device/resource failure does the
same. Rejected shaders fall back per formula. Selecting CPU or saving settings releases
worker-owned GPU resources on that worker; shutdown joins pending shader compilation before
unloading the compiler. Driver calls and shader compilation themselves have no hard time
limit, so this polling budget is not a guarantee against a driver stall.

Reusable GPU/staging buffers account for roughly 10 MiB at the maximum normal band size,
plus a 2 MiB CPU result buffer per rendering worker. Experimental custom-field batching adds
at most 4 MiB of reusable pixel records per worker. Shader/source caches, driver allocations,
and existing mod buffers are additional. Icon/cursor pools are created on demand only when
their switches and the GPU backend are enabled. Existing capture-visibility and exclusion
policies stay with the current overlays.

Validation so far covers CPU regression comparisons, generated HLSL compilation with
Microsoft DXC, and native tests with simulated Direct3D APIs. DXC validates Shader Model 6
syntax here; Windows d3dcompiler_47 Shader Model 5 compilation, device execution, visual
matching and performance still require Windows testing. A fallback log is evidence of a
CPU fallback, not a successful GPU render.

Implementation references:
[DirectCompute setup](https://learn.microsoft.com/en-us/windows/win32/direct3d11/direct3d-11-advanced-stages-compute-create),
[shader compilation](https://learn.microsoft.com/en-us/windows/win32/api/d3dcompiler/nf-d3dcompiler-d3dcompile),
[CPU access restrictions](https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_cpu_access_flag),
and [nonblocking mapping](https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_map_flag).

## Start menu and File Explorer icons

These independent categories sit directly below Taskbar icons and start **disabled**.
Enable **Start menu icon effects** or **File Explorer icon effects**, then select grayscale,
colorize, transparency, shadow, glow, or an animated/custom formula. Existing setting keys
and saved values are retained; the switches and descriptions now name their own category.

The formula guide below applies to both. `u`, `v`, `x`, `y`, `w`, `h`, `t`, `speed`,
`power`, `n`, and the two colors use the same syntax. Coordinates span the Start menu's
XAML root or each Explorer view/navigation-tree client area. A wave travels across the
icons in that area. Different Explorer windows have separate canvases and share the effect
clock. Screen samples, border depth, live audio/weather inputs are unavailable on these icons.

**Start menu:** visual-tree notifications discover realized Windows XAML roots and icon
hosts, including roots unavailable through `Window::Current`. A bounded scan remains as
fallback and handles layout changes. Pinned apps, All apps bitmap logos, and bitmap folder
preview icons in known logo containers are targeted. Profile pictures, recommended content,
font/vector glyphs, search results, third-party menus and unknown templates are excluded.
The diagnostics tap is loaded inside StartMenuExperienceHost using the public Windows
XAML diagnostics interfaces; no registry or system-file changes are made. No cursor,
border, audio or weather workers are started there.

Open Start after saving. Loaded writable bitmap sources can be copied directly; other
supported image sources use asynchronous visual snapshots. The first effect can take a
few seconds while discovery and snapshots complete. Logs distinguish **visual-tree notifications ready**,
**visual-tree root discovered**, **first processed bitmap applied**, and failed/blank
captures. Unsupported image sources retain the native icon and retry; successful discovery
alone does not prove that a bitmap was processed. This integration still needs live testing
on the target Windows build.

Source snapshots are capped at 96 x 96 pixels, so enlarged icons may lose detail. The
native icon rectangle and layout size are preserved. Shadow/glow outside that rectangle
are clipped; they no longer shrink the icon to fit a padded bitmap. Sources are recaptured
when Windows replaces them or changes icon dimensions. Disable/reload/unload restores a
source only if this mod's bitmap still owns it. Discovery watches weak references to
virtualized templates; rendering pauses while Start is hidden. Each UI context tracks at
most 64 icons and 24 MiB of accounted pixel buffers, with at most two contexts. Windows
textures, containers and allocator overhead are additional.

**File Explorer:** file-view icon drawing now has its own integration. It intercepts the
public shell `IImageList::Draw` implementations and `ImageList_DrawIndirect`, including
supported `DirectUIHWND` and legacy list views. Draw interception is restricted to views
inside Explorer windows. A scoped paint target identifies buffered/memory-DC drawing.
The native icon, overlay and draw-state flags are first rendered into a private transparent
bitmap, then processed using this category's settings. The original image list and caller's
draw parameters are unchanged. A failed capture or upload uses the native draw. If no
draw hook can be installed, the previous supported list-view path remains available.

**Include navigation-pane icons (pinned folders and other sidebar entries)** starts off.
Enable it to apply the same Explorer settings to the left navigation tree. This covers the
whole sidebar tree, including pinned folders, drives and other navigation entries; it is
not a filter for pinned items alone. Toolbar, address-bar and tab icons, other applications,
desktop icons and file dialogs outside Explorer are excluded. Thumbnails are processed
only when supplied through a supported image-list draw; separately rendered thumbnails and
unsupported drawing paths and destination-dependent raster operations keep their native appearance.

Existing and newly opened Explorer views are discovered automatically while the category
is enabled. Hidden/minimized views do not receive effect animation redraws. Per-view paint
targets are tracked on the owning UI thread. A shared cache has at most 256 entries and
24 MiB of accounted pixel buffers; two reusable drawing surfaces and container overhead
are extra. Native source captures are refreshed on the next draw after 500 ms, and settings
changes clear the cache. The log reports installed draw hooks, attached view classes and
the first processed file-view/navigation-pane icon. A hook/attachment log alone does not
prove a processed icon appeared. Live Windows verification remains necessary.

The global smoothing switch and the matching per-category GPU formula switches apply to
both integrations. Their existing CPU paths and GPU fallback remain available. Disabling a
category stops custom drawing and requests a native repaint. On cleanup failure, an inert
module lease is retained rather than freeing code still registered with Windows.

## Settings guide

| Category | Controls |
|---|---|
| Desktop icons | Grayscale; colorize with saturation and contrast; transparency; label/shortcut-arrow hiding; refresh delay; shadow; glow; animated or custom tint. |
| Taskbar icons | Independent enable switch and icon color, transparency, shadow, glow, and animation controls. |
| Start menu icons | Independent, opt-in bitmap icon effects for Windows 11 pinned apps and All apps. |
| File Explorer icons | Independent file/folder icon effects, with a separate toggle for navigation-pane icons. |
| GPU rendering (experimental) | Per-category hardware D3D11 border/icon formulas and cursor-layer blending, with CPU fallback and existing GDI presentation. |
| Selection box | Desktop/File Explorer switches; fill and outline colors/opacities; outline width; background blur; selected-item styling. |
| App window | Enable; width; corner radius; maximized/fullscreen hiding; colors; effect; opacity; speed/intensity/count; unfocused appearance; timing; per-app rules and exclusions; nested motion blur (off by default). |
| Monitor corners | Fill color; opacity; corner size; roundness; hide on fullscreen. |
| Screen border | Width; two colors; opacity; effect; speed/intensity/count; custom formula. Width 0 disables the screen border. |
| Music Reactive | Enter an enabled Windows input/output device name (or leave blank for default output); select shared music style, sensitivity, pulse strength, and idle brightness. |
| Screen border → Timing and sampling | Monitor-overlay FPS, Ambient/Wallpaper Ambient interval, and Cinema sampling rate. |
| Location settings and effects | Latitude/longitude, solar transitions, day/night palette and optional live weather. |
| Cursor effects | Independent glow, trails, particles, fluid, frame budgets, smoothing, capture and custom formulas. |
| Experimental | Independent window/screen switches, two animated border presets, custom field, colors, reach and mouse response. |

Opacity is visibility: lower values are more transparent. **Icon transparency** is the reverse:
0 means fully visible and 100 means invisible. An icon's transparency-amount slider needs
its transparency switch enabled. Shadow/glow controls likewise need their respective switches.

Icon processing follows grayscale, colorize, custom/preset tint, then transparency; shadow
and glow are composed separately. Custom tint retains the image's brightness and existing
transparent pixels. Dark source pixels therefore remain dark, even with a bright formula.
Desktop hover temporarily shows the normal icon. Label hiding also affects type-to-select
and screen-reader labels while enabled.

App borders straddle the window edge: part of their width is behind the window. Screen
borders follow the monitor corners. Normal border formulas cannot enlarge their drawing
area; increase Width instead. Experimental borders use their own Reach. Taskbar glow/shadow padding uses the existing image box
and can make the visible icon smaller.

Both border effect dropdowns include **Music Reactive**, **Mercury V3 (Controlled Chaos)** and **Mercury V6 (Mercury Storm)**. These presets preserve their original silver formulas and can also respond to the optional Music Reactive opacity multiplier.

## Creating a custom effect

For icons and normal borders, choose **Custom** in the effect selector and paste a formula.
Cursor formula fields are optional overrides; experimental fields have their own Custom selector.
Each field keeps its own formula and palette. A formula can be copied between fields;
there is no need to learn a separate language for each feature.

| Location | Formula field |
|---|---|
| Desktop icons → Animated effects | Custom effect formula (`Icons.Animation.IconCustomEffect`) |
| Taskbar icons → Animated effects | Custom effect formula (`TaskbarIcons.Animation.IconCustomEffect`) |
| Start menu icons → Animated effects | Custom effect formula (`StartMenuIcons.Animation.IconCustomEffect`) |
| File Explorer icons → Animated effects | Custom effect formula (`ExplorerIcons.Animation.IconCustomEffect`) |
| App window → Effects and colors | Custom effect (`Borders.Effects.BorderCustom`) |
| Screen border | Custom effect (`ScreenBorder.CustomEffect`) |
| Cursor effects → Custom effects | Glow, Trail, Particles; optional formulas layered over the selected appearance. |
| Experimental | Select Custom and edit Custom border field formula. |

**No formula field:** selection styling, window motion trails, cursor fluid simulation, monitor-corner fill, icon labels,
shortcut arrows, and the independent shadow/glow controls. Their settings are adjusted
through their own controls. Normal border/icon/cursor formulas cannot enlarge the drawing
area or alter native window geometry. Experimental alpha fields can shape the visible
border inside their configured reach. Targeting, FPS and sampling remain settings.

Start with this formula, save the settings, then adjust one expression at a time:

```text
r=1; g=0.3; b=0.05; a=1
```

### Syntax and outputs

A formula is a sequence of assignments separated by semicolons. Spaces and line breaks
are allowed. Names ignore letter case. Use decimal numbers, not hex colors, inside formulas.
The color pickers supply palette variables automatically.

| Output | Meaning |
|---|---|
| `r`, `g`, `b` | Red, green, blue, each from 0 to 1. |
| `a` | Additional opacity, from 0 to 1. `0` hides the affected pixel. |

For normal icon/border formulas, unassigned colors start at color 1 and `a` starts at 1.
Cursor custom fields start from the current preset color and `a=1`. Experimental
fields start from the procedural plasma result, including its opacity. Outputs are clamped
to 0–1. Border opacity is multiplied afterward; do not multiply `a` by `op` unless an
extra fade is intended. Icon alpha is multiplied by the source image's alpha and the enabled
transparency setting. A formula cannot restore pixels that were already transparent.

Create temporary variables by assigning a new name before reading it. Up to 29 are available;
they reset for each pixel evaluation. Names start with a letter or `_`, followed by letters,
digits, or `_`. Built-in input variables are read-only. There are no persistent variables,
loops, arrays, strings, comments, C++ statements, or `?:` expressions. Use `if(c,a,b)` for choices.
Do not paste Markdown fences into a formula field. Curly braces are reserved for formulas
inside per-app rules, explained later.

| Operators | Meaning |
|---|---|
| `+ - * /` | Arithmetic; unary `-` negates a value. |
| `%`, `^` | Remainder and exponentiation. `^` means power, not XOR. |
| `< > <= >= == !=` | Comparisons return 1 or 0. |
| `&&`, `||`, `!` | Logical AND, OR, NOT. |
| `( )` | Group expressions; commas separate function arguments. |

Write `a=0.5`, not `a=50`. A color such as `#FF8000` becomes approximately
`r=1; g=0.502; b=0`. Read palette values instead when the effect should follow color pickers.

### Variables

| Variable | Meaning |
|---|---|
| `x`, `y` | Pixel coordinates in the current effect canvas. |
| `u`, `v` | Normalized horizontal/vertical coordinates, approximately 0–1. Border padding can extend slightly outside that range. |
| `w`, `h` | Canvas width and height in pixels. |
| `p` | Continuous clockwise position around the rounded border, starting at its top-left top tangent, 0–1. |
| `d` | Normalized depth across the border band, from its outer side toward its inner side. |
| `ang` | Angle around the canvas center, measured in turns, 0–1; 0 points right and angles increase clockwise. |
| `t` | Time in seconds. Any reference to `t` marks the formula as animated, including `t*0`. |
| `mon` | Monitor index, starting at 0. |
| `speed` | Effect speed setting divided by 20; setting 20 gives 1. |
| `power` | Effect intensity setting divided by 100. |
| `n` | Effect count setting. |
| `bw` | Border width in pixels. |
| `rad` | Border corner radius. |
| `op` | Opacity multiplier, 0–1. |
| `c1r`, `c1g`, `c1b` | Color 1 channels, 0–1. For icons, this is the Colorizer color picker, even when Colorizer itself is disabled. |
| `c2r`, `c2g`, `c2b` | Color 2 channels, 0–1. For icons, this is Effect second color. |
| `ar`, `ag`, `ab` | Timed Ambient color samples. |
| `kr`, `kg`, `kb` | Cinema color samples. |
| `pi`, `tau` | Approximately 3.14159 and 6.28318. |

**Icon exceptions:** `p` is the horizontal coordinate `u`, not an edge perimeter;
`d`, `rad`, and `mon` are 0; `bw` is the source-image width; `ang` is in radians
from approximately `-pi` to `pi`. Ambient/Cinema variables have no samples and remain 0.
`op` contains the configured opacity amount, but its automatic application needs the
transparency switch enabled. An expression such as `a=1-d` therefore produces no depth
fade on icons. Use `u` or `v` for an icon gradient instead.

Desktop icon coordinates span the virtual desktop. Taskbar icon coordinates span each
available taskbar XAML root, with a horizontal fallback during layout. The shared canvas
keeps waves aligned across icons. Separate taskbar roots do not form one continuous canvas.
An app border has its own window canvas; the screen overlay has one canvas per monitor.
For identical angle math across fields, use `atan2(v-0.5,u-0.5)/tau` instead of `ang`.

Speed, intensity, and count change a custom formula only if it references `speed`, `power`,
or `n`. Merely selecting a faster speed cannot animate a formula that never reads time.
A formula without `t` can still update when sampled colors, geometry, or settings change.

### Functions

| Function | Result |
|---|---|
| `sin(x)`, `cos(x)`, `tan(x)`, `atan(x)`, `atan2(y,x)` | Trigonometry in radians. |
| `abs(x)`, `sqrt(x)`, `exp(x)`, `log(x)`, `sign(x)` | Basic math; `sign` returns -1, 0, or 1. |
| `floor(x)`, `ceil(x)`, `round(x)`, `fract(x)` | Rounding; `fract` repeats the fractional part from 0 to below 1. |
| `pow(a,b)`, `min(a,b)`, `max(a,b)`, `mod(a,b)` | Power, smaller value, larger value, and remainder. |
| `clamp(x,lo,hi)` | Limit a value to a range. |
| `mix(a,b,k)` | Linear interpolation: `a+(b-a)*k`. Use `k` from 0–1 for an ordinary blend. |
| `step(edge,x)` | 0 below the edge, otherwise 1. |
| `smoothstep(lo,hi,x)` | Smooth transition from 0 to 1. |
| `saw(x)`, `tri(x)`, `sq(x)` | Repeating sawtooth, triangle, and square waves with period 1. |
| `hr(h)`, `hg(h)`, `hb(h)` | RGB channels for a repeating rainbow hue; hue repeats every 1. |
| `if(condition,a,b)` | Choose `a` for a nonzero condition, otherwise `b`. |
| `noise(x)`, `noise2(x,y)` | Smooth pseudorandom values, approximately 0–1. |
| `hash(x)`, `hash2(x,y)` | Pseudorandom values that repeat for the same inputs. |
| `bayer(x,y)` | Ordered-dither threshold, approximately 0–1. |

### Formula recipes

Paste one complete recipe into a formula field. The accompanying notes identify exceptions.

**Two-color horizontal wave.** `n` controls repetitions, `speed` controls travel, and `power`
controls how far the blend reaches toward color 2.

```text
k=(0.5+0.5*sin(tau*(u*n-t*speed*0.1)))*power; r=mix(c1r,c2r,k); g=mix(c1g,c2g,k); b=mix(c1b,c2b,k); a=1
```

**Traveling rainbow.** Replace `u` with `p` for perimeter travel; on icons `p` still means `u`.

```text
hu=u*n-t*speed*0.1; r=hr(hu); g=hg(hu); b=hb(hu); a=1
```

**Rotating color wheel with consistent angle math.** Rotation is centered on the effect canvas.

```text
hu=atan2(v-0.5,u-0.5)/tau-t*speed*0.1; r=hr(hu); g=hg(hu); b=hb(hu); a=1
```

**Breathing color 1.** Intensity controls the amount of fading.

```text
k=0.5+0.5*sin(tau*t*speed*0.5); a=mix(1-power,1,k)
```

**Soft moving pearls.** The minimum opacity keeps a faint background between the bright peaks.

```text
q=fract(p*n-t*speed*0.1); k=exp(-pow((q-0.5)*9,2)); r=mix(c1r,c2r,k); g=mix(c1g,c2g,k); b=mix(c1b,c2b,k); a=0.15+0.85*k
```

**Smooth cloudy color.** Increase the spatial multipliers for finer detail.

```text
k=noise2(u*n*4+t*speed*0.1,v*n*4-t*speed*0.07); r=mix(c1r,c2r,k); g=mix(c1g,c2g,k); b=mix(c1b,c2b,k); a=1
```

**Edge fade.** No depth fade on icons because their `d` is 0.

```text
a=pow(1-d,2)
```

**Dithered four-step fade.** No depth fade on icons; replace `1-d` with `u` for a horizontal fade.

```text
a=clamp(floor((1-d)*4+bayer(x,y))/4,0,1)
```

**Blend Ambient and Cinema samples.** No sampled colors on icons.

```text
r=mix(ar,kr,0.5); g=mix(ag,kg,0.5); b=mix(ab,kb,0.5); a=1
```

## Presets and sampled colors

The built-in presets use the same formula language. Choose a preset for a ready-made look,
or choose Custom to edit a recipe. Formula text is ignored while another preset is selected.

| Preset | Appearance |
|---|---|
| Default / solid | Color 1 without animation. |
| Breathing / Heartbeat | Smooth pulsing / a double pulse. |
| Glow / Neon | Depth fade / a bright core with a halo-like border pattern. |
| Wave / Gradient | Traveling two-color wave / slow two-color gradient. |
| Rainbow / Color wheel | Traveling hue / rotating hue around the center. |
| Aurora / Silk / Plasma | Shifting rainbow tones / flowing bands / layered color waves. |
| Comet / Smooth pearls | Traveling tails / soft repeated bright peaks. |
| Fire / Stars | Dithered flame-like noise / twinkling particles. |
| Shimmer | Narrow bright highlights. |
| Ambient / Wallpaper Ambient / Cinema | Sampled colors, described below. |

Default, Glow, Neon, Heartbeat, Comet, Fire, Stars, Ambient, Wallpaper Ambient, and Cinema
are not offered in the icon preset selectors. **Off** disables the icon formula/preset layer.
Glow as an icon setting is a separate image effect, not the Glow border preset.

App-border Ambient samples only outside the app edges and holds those colors between
updates. Monitor Ambient keeps its screen-edge sampling. Wallpaper Ambient samples desktop wallpaper renderers, with the Windows wallpaper
image as fallback. Cinema samples screen pixels near the relevant edges at its sampling rate;
it does not read an application's private video/image data.

A custom formula that reads `ar/ag/ab` requests timed screen samples; one that reads
`kr/kg/kb` requests Cinema samples. Border samples fall back to color 1 when unavailable.
Custom formulas cannot select the wallpaper sampling source: Wallpaper Ambient is a preset.
GPU-rendered Lively/Wallpaper Engine content may not be captured; the static wallpaper may
be used instead. Ambient/Wallpaper intervals are 2, 5, 10, 20, 30, 60, 120, or 240 seconds.
Cinema sampling has fixed targets of 1, 2, 5, 10, 15, 20, 24, 30, 45, 60, 90, 120,
144, 165, 180, 200, or 240 captures per second, independent of animation FPS. Auto follows
the foreground monitor's refresh rate, capped at 240/s. Adaptive polls at 5/s while stable
and uses that display rate during window activity or for 0.5 seconds after sampled colors
change. Adaptive Lite uses 2/s idle and up to 60/s active. An idle adaptive mode can take
up to its next poll to notice new content. Custom formulas reading `kr/kg/kb` use the same
sampling policy. Sampling deadlines can wake the worker independently of animation FPS;
unchanged samples do not force a redraw. These rates are targets: capture/rendering cost
and Windows scheduling can limit them. Higher rates and multiple visible monitors use
more CPU. Ambient and Wallpaper Ambient keep their separate seconds-based intervals.

## Per-app border rules

Add one entry per executable name. Matching is case-insensitive; the name is exact,
not a path or wildcard. Unspecified values inherit the global app-border settings.
A formula must be wrapped in `{ }` so its function-argument commas stay inside the formula.

```text
notepad.exe: effect=custom, custom={k=0.5+0.5*sin(tau*(p*n-t*speed*0.1)); r=mix(c1r,c2r,k); g=mix(c1g,c2g,k); b=mix(c1b,c2b,k); a=1-d*0.4}, color=#00FFAA, color2=#0066FF, width=4, count=3
```

| Rule key | Value |
|---|---|
| `effect` | Preset key in lowercase: `static`, `breathing`, `glow`, `neon`, `wave`, `rainbow`, `comet`, `gradient`, `aurora`, `heartbeat`, `fire`, `stars`, `wheel`, `silk`, `plasma`, `pearls`, `shimmer`, `ambient`, `wallpaper`, `cinema`, `custom`. |
| `custom` | Formula inside `{ }`; implies Custom unless `effect` is also specified. |
| `color`, `color2` | Hex palette colors, such as `#00BFFF`. |
| `width`, `radius` | Border width and corner radius in pixels. |
| `opacity` | 1–100. |
| `speed`, `intensity`, `count` | 1–100, 0–100, and 1–16 respectively. |
| `inactive` | `off`, `same`, or a hex color. |
| `hide` | `yes` hides that app's border. |

Per-app rules do not customize screen borders, monitor corners, icons, selection styling,
or motion-trail appearance. A `hide=yes` rule is border-only; use Excluded apps to suppress
both the app border and motion trails. Rules do not override exclusions.

Unfocused app borders can be hidden, solid, or use the same formula. Time is frozen at 0
for that last mode except in app-border **Adaptive**, which also animates unfocused borders.
It is not a frozen snapshot of the moment focus was lost.

## Exclusions

Excluded apps accepts executable names, full paths, wildcard patterns, or window classes.
Add one entry per item. For example, `game.exe`, `E:\Games\*`, or `*\steamapps\common\*`.
Exclusions affect app borders and motion trails, including a trail already in progress.
They do not hide monitor corners or the screen border.

**Smooth animated effects** is an optional global switch for animated desktop/taskbar
icons and app/screen borders, including Mercury, music, solar/weather effects, custom
animations and Cinema. Below 60 effect FPS it presents blended intermediate frames at
a 60 FPS target while retaining the effect calculation and capture rates. It uses linear
frame blending, adds roughly one effect-frame of delay, extra memory and presentation
work, and can show fading trails on fast motion; it does not reconstruct object motion.
It defaults to Off. Effects already at 60 FPS or above keep native presentation. Static
settings, selection and window/trail geometry, and held Ambient/Wallpaper Ambient colors
are unchanged. Oversized optional buffers fall back to native rendering.

The final **Show overlays in screenshots and screen sharing** switch is outside all
categories. Off (the default) excludes app borders, screen borders, monitor corners and
motion trails from supported Windows captures. On removes that restriction. Saving
updates existing overlays without restarting Explorer. Desktop and taskbar icons remain
part of Explorer's normal captured content. With capture visibility On, Cinema and
screen-edge Ambient may sample the overlays themselves. Capture support depends on the
recording application's capture method.

**Honor Windhawk default and Advanced exclusions**, beneath the manual list, is on by default.
It combines the manual list with Windhawk's global process exclusions, this mod's Advanced
exclusions, and a bundled snapshot of Windhawk's critical-process/incompatible-program/
known-game patterns (2026-10-15), respecting the associated Windhawk switches. The lists
are read from registry or portable INI storage and refreshed every two seconds. The Advanced
"Ignore mod inclusion/exclusion lists" switch removes the metadata exclusions from this
filter but retains custom exclusions. Neither stored list is overwritten.

An exclusion of `*` hides every app border/trail; process inclusion lists do not whitelist
overlay targets. Disabling inheritance leaves manual exclusions and normal window-eligibility
checks in place. Unreadable/protected processes are left alone. Other mods' private lists
do not control this mod. Games outside the listed patterns need manual exclusions. The
switch does not change Windhawk injection settings or guarantee anti-cheat compatibility.
Future changes to Windhawk's built-in lists require an updated snapshot in this mod.


## Cursor effects

Enable **Cursor effects → General → Basic controls → Enable cursor effects**, then
enable Glow, Trails, Particles or Fluid independently. The normal Windows pointer and
installed cursor pack remain in control: this mod does not replace, recolor or hide the
native pointer body. Glow follows a sampled silhouette. Manual silhouette scale is used
unless experimental auto-match is enabled.

- **Glow:** choose an outline preset, colors, thickness and softness. Enable Halo for
  the wider soft light. Glow calculation FPS is independent of particle physics FPS.
- **Trails:** ribbon, dots and afterimages use travel-distance sampling. Length, width
  and spacing have separate controls. Old trail points expire automatically.
- **Particles:** choose a shape and emission limit. Optional physics controls gravity,
  drag, turbulence and bouncing. Image particles accept an explicitly selected local
  absolute `.png` or `.bmp` path; images are limited to 128×128 pixels and 12 MiB files.
- **Fluid:** a CPU velocity/pressure/dye grid produces swirls. Resolution, pressure
  iterations and coverage have a larger cost than simple glow. Start at 96 resolution
  and 30 simulation FPS. Radius 0 uses whole-monitor coverage. Fluid FPS can run
  independently, follow rendering, or adapt to motion; visible-ink inactivity suspends
  solver calculations. Moving between monitors starts a fresh field.
- **Window collisions:** visual effects can react to nearby window frames. This never
  confines the actual mouse or changes app windows. This option is experimental.

Cursor rendering has its own Fixed/Auto/Adaptive modes, maximum FPS, idle FPS, trail
sampling, particle simulation and glow clocks. Its smoothing switch blends layers below
60 FPS toward 60 presentations; layers at 60 FPS or more bypass blending. Settled layers
stop requesting extra blend presentations. It adds latency and possible ghosting.
The combined cursor scene presents at the selected render rate, raised to at least
60 only during active low-rate blending. Pointer polling no longer bypasses this
rate. Idle input polling falls to 30 Hz, so wake-up can take about 33 ms. No cursor
worker starts with all effects off. Identical native artwork reuses its outline
distance field; source artwork and DPI are still checked periodically.
The root smoothing switch does not control cursor effects or experimental borders.

Cursor **Exclude overlay from supported screen captures** is independent of the final
global capture switch. Cursor fullscreen hiding is independent as well. App-border
exclusions do not automatically hide cursor effects or screen borders.

### Custom glow, trail and particle formulas

Leave a field blank to preserve its preset. Otherwise enter assignments such as:

```text
r=hr(p-t*0.1); g=hg(p-t*0.1); b=hb(p-t*0.1); a=0.8
```

These fields use the same assignment syntax and math functions as the main guide.
They change color and multiply the existing opacity, retaining native fades and soft
edges. Custom formulas run after the rainbow override. Their `c1` palette is the
current preset color; `c2` uses the secondary glow color. `t` is elapsed seconds,
wrapped every 4096 seconds. No screen or wallpaper capture is performed for formulas.

| Input | Cursor interpretation |
|---|---|
| `p` | Glow: angle around the silhouette, 0–1. Trail: normalized age, newest 0. Particle: normalized age, newborn 0. |
| `d` | Glow: distance from silhouette divided by glow radius. Trail: 0. Particle: stable random seed. |
| `x`, `y` | Glow: silhouette pixels. Trail/particle: absolute screen coordinates. |
| `u`, `v` | Glow: normalized silhouette position. Ribbon trail: position within its segment; dots/afterimages: position in the trail history, with `v=0`. Particles: 0. |
| `ar`, `ag`, `ab` | Aliases for `u`, `v`, `p`; there are no Ambient samples here. |

`w`, `h`, `speed`, `power`, `n` and `op` are 1 in cursor fields. `bw`, `rad`,
`mon`, `ang` and Cinema samples are 0. Use explicit constants and the inputs above.
Glow formulas do not change the native cursor body or expand the silhouette envelope.
Trail formulas do not change trail geometry. Particle formulas do not change physics,
emission or shapes; custom images receive a partial color tint rather than replacement.
Cursor formulas do not control fluid simulation.

For particle fading with a repeating color gradient:

```text
r=mix(c1r,c2r,p); g=mix(c1g,c2g,p); b=mix(c1b,c2b,p); a=1-p
```

## Experimental borders

The category below Cursor effects contains independent window/screen switches. Enable
the corresponding normal border and keep its width above zero. Experimental drawing
replaces its appearance; switching it off restores the saved normal effect. App
targeting, exclusions, opacity and visibility policies still apply. Rounded monitor
corner fill stays active. These are procedural 2D overlays with simulated depth.

Plasma and Ember retain their existing behavior. Select **Custom** for the programmable
field and particle system below. Reach is the drawing envelope, Width is the fallback
plasma width, and FPS caps inherited timing at 15–60. Existing Speed and palette controls
remain. No extra collection of preset-tweaking sliders is required.

Turn **Freeze experimental borders while idle (live mode only)** off for continuous live animation,
or enable cached loop playback in Optimization. The optional freeze holds a frame after pointer interaction settles and resumes
on interaction, target/focus or settings changes. It samples the current animation
clock when resuming. Inactive Hide stays hidden; Solid uses the saved normal border;
Same effect freezes unless app-border Adaptive animates unfocused borders. Global
smoothing does not interpolate experimental geometry. The global capture switch applies.

### Three formula fields, three jobs

| Field | Runs | Outputs |
|---|---|---|
| Custom border field formula | Per border pixel | `r`, `g`, `b`, `a`: body color and shape. |
| Custom scene / particle count formula | Once per target draw | `count`: particle instances; `body`: body opacity multiplier. |
| Custom particle instance formula | Once per instance per target draw | Position, geometry and `r/g/b/a` for that particular particle. |

Screen tiles can evaluate the same target draw separately. Particle formulas do not
run for every pixel. Instances are indexed afresh each draw; this is deterministic
procedural animation, without persistent velocity, collision or physics state.

Blank layout means `count=0; body=1`, preserving existing Custom fields without
extra particles. Blank particle code uses soft Color 2 dots orbiting at 75 px/sec,
with depth 0.35 and size 2.24. The layout must create particles first. Existing Custom
field recipes still work. To show particles alone, set `body=0` in the layout.

### Shared inputs

All three fields use the existing assignment language, functions, palette and constants.
Additional names below are initialized for each evaluation. Shared aliases can be
reassigned locally, but doing so does not change settings or another formula field.
Original built-in inputs such as `x`, `t` and `p` remain read-only.

| Input | Meaning |
|---|---|
| `t` | Animation seconds multiplied by the existing Speed control. |
| `clock` | Animation seconds independent of Speed. Both clocks wrap after 4096 raw seconds; design short loops. |
| `reach` | Configured envelope thickness in pixels. |
| `radius` | Configured corner radius; actual geometry caps it to fit the frame. |
| `perimeter` | Rounded-frame length used for normalized travel. Superellipse corner length is approximated by circular arcs. |
| `w`, `h` | Target frame dimensions in pixels. |
| `mx`, `my` | Smoothed pointer position normalized to frame dimensions; aliases of `ar`, `ag`. Can be outside 0–1. |
| `prox` | Alias of `ab`: soft pointer proximity. Zero when mouse interaction is disabled or unavailable. |
| `enter` | Seconds since the last frame entry/exit, capped at 60. |
| `inside` | 1 when the pointer is inside the target rectangle, otherwise 0. Independent of the mouse interaction switch. |
| `c1r/g/b`, `c2r/g/b` | Existing experimental palette channels, 0–1. |
| `pi`, `tau` | Circle constants. |

### Body field coordinates and outputs

`x/y` are frame-local pixel positions, `u/v` their normalized equivalents, and `p`
is continuous position around the closed contour. It starts at the top-left upper
tangent and increases clockwise. `d` is signed depth divided by Reach: positive points
outward for app borders and inward for screen borders. `prox` is sampled at the
pixel's contour anchor. Layout coordinates `x/y/u/v/p/d` are zero; its proximity is
measured from the pointer to the frame contour.

Set `a` explicitly to define the body's shape. Otherwise the fallback plasma alpha
remains. Use `r/g/b` for color. Channels are clamped to 0–1; layout `body` and normal
border opacity apply afterward. Custom alpha does not automatically inherit preset
bending, width or glow. For bending, include `prox` in the shape formula.

Use integer perimeter cycles such as `sin(p*tau*12-t*2)` to close the wrap. A raw
`sin(p*80)` generally gives a seam. Keep shape peaks away from the Reach boundary.

### Layout outputs

| Output | Default and limits | Meaning |
|---|---|---|
| `count` | 0; truncated to an integer in 0–128 | Number of particle instances. May depend on time, pointer or dimensions. |
| `body` | 1; clamped to 0–1 | Multiplies the pixel field's alpha. Zero skips body formula evaluation. |

Layout `r/g/b/a` assignments do not color the field or particles. Put those assignments
in their respective fields. Temporary variables do not carry between fields or frames.

### Per-particle inputs

| Input | Meaning |
|---|---|
| `id` | Read-only integer index, from 0 to count−1. |
| `seed` | Read-only stable 0–1 pseudo-random value for this ID. |
| `home` | Read-only evenly spaced starting position, id/count. |
| `count` | Read-only particle count from the layout. |
| `p`, `d` | Home position and default depth 0.35. They do not update when outputs change. |
| `x/y`, `u/v` | Home contour anchor in pixels and normalized coordinates. |
| `nx`, `ny` | Read-only outward unit normal at the home anchor. |
| `ang` | Home outward normal angle in radians. |
| `prox` | Pointer proximity at the home anchor; output offsets do not resample it. |

Changing count changes home spacing. IDs and seeds stay stable, but count-dependent
layouts can rearrange immediately. Use fixed counts for stable identities.

### Per-particle outputs

| Output | Default and limits | Meaning |
|---|---|---|
| `pos` | Default orbit; wraps into 0–1 | Final position along the contour, independent of the body. |
| `depth` | 0.35; −0.9 to 0.9 | Radial position as a fraction of Reach. Positive is outward for apps, inward for screens. |
| `offsetx`, `offsety` | 0; −96 to 96 px | Extra frame-local XY displacement after contour/depth placement. |
| `size` | 2.24; 0.5–24 px | Soft particle Gaussian scale, not a hard radius. |
| `stretch` | 1; 0.25–4 | Multiplies one axis to form ellipses or short streaks. |
| `rotation` | 0; wraps in turns | Rotation relative to the final contour tangent. 0 follows the border; 0.25 points across it. |
| `r/g/b`, `a` | Color 2 and alpha 1; clamped to 0–1 | Independent particle color and opacity. |

Use a=0 to hide selected IDs; size=0 is clamped to 0.5 and does not hide a particle.
Output assignment order matters: later expressions can use the current value of an
earlier output. Use `pos=home` for static tangential placement. Use `clock` for travel
independent of body Speed, or `t` to follow that control. Size/stretched tails fade
smoothly and are cut off at three Gaussian scales.

Reach and the existing border geometry mask remain hard limits. XY offsets do not
expand the overlay or allow drawing across the app interior. Negative app depths can
be occluded by the app; negative monitor depths can lie off-screen. Place particles
inside the envelope to keep their soft edges visible. Particles blend over the body;
lower IDs blend later where instances overlap.

### Working themes

**Independent rainbow particles.** Body field:

```text
q=0.25+0.08*sin(p*tau*8-t);
a=exp(-pow((d-q)*22,2))*0.5;
r=c1r;g=c1g;b=c1b
```

Layout:

```text
count=18;body=1
```

Particle instances:

```text
pos=home-clock*(0.015+seed*0.02);
depth=0.55+0.12*sin(clock*2+seed*tau);
size=1+seed*2;stretch=1;
r=hr(seed+clock*0.05);g=hg(seed+clock*0.05);b=hb(seed+clock*0.05);a=0.9
```

**Comets only, following the corners.** Keep any body formula, then use this layout:

```text
count=10;body=0
```

Particle instances:

```text
pos=home-t*0.04;depth=0.4;
size=1.3;stretch=4;rotation=0;
r=mix(c1r,c2r,seed);g=mix(c1g,c2g,seed);b=mix(c1b,c2b,seed);a=0.95
```

**Embers with independent radial travel.** Use the rainbow theme's body or your own
flame field. Layout:

```text
count=24;body=1
```

Particle instances:

```text
age=fract(clock*(0.2+seed*0.2)+seed);
pos=home-clock*0.01;depth=0.1+age*0.65;
size=0.7+(1-age)*1.8;stretch=1+age;rotation=0.25;
offsetx=sin(clock*3+seed*tau)*2;
r=1;g=0.3+0.6*(1-age);b=0.05;a=(1-age)*(1-age)
```

**Pointer-reactive markers.** Layout:

```text
count=20;body=0
```

Particle instances:

```text
pos=home;depth=0.3+prox*0.25;
size=1+prox*3;stretch=1;offsetx=nx*sin(clock*2)*2;offsety=ny*sin(clock*2)*2;
r=mix(c1r,c2r,prox);g=mix(c1g,c2g,prox);b=mix(c1b,c2b,prox);a=0.2+prox*0.8
```

### Authoring, limits and troubleshooting

Use semicolon-separated assignments, temporary variables and the shared math functions
listed earlier in this guide. There is no C++, JavaScript, HLSL, texture import or
file/network access in formulas. Each field has a 256-node and 16384-character limit
plus shared parser depth limits. Particle fields reserve input/output slots, leaving
seven user temporary names; layout fields leave eighteen and body fields twenty.
Common aliases may be reassigned locally; particle ID/seed/home/count/normals cannot.

Body syntax errors retain the plasma fallback. Invalid layout means zero particles
and normal body visibility. Invalid particle code suppresses the custom particles.
The debug log identifies the failing field. Leaving a field blank uses its defaults.
Nonfinite assignment results become zero before output bounds are applied.

Start with a fixed count and size. Check static positions, animate pos, vary depth,
then add color/stretch/XY motion. Test the wrap, all corners, resize, different corner
roundness, inactive modes and both targets. For missing particles, check layout count,
alpha, depth and Reach. For held animation, disable idle caching. For clipped tails,
reduce size/stretch/offsets or move the particle lane inward within the envelope.

Custom layout/particle fields do not modify Plasma or Ember presets, classic borders,
desktop/taskbar icons, or cursor glow/trails/particles/fluid. Experimental fields do
not receive main music, weather, day/night, Ambient or Cinema modulation. Cursor fields
retain their existing color/alpha-only capabilities described above.

This remains CPU rendering. Instance count, large soft particles, overlapping sprites
and complex per-pixel body formulas increase work. Start around 12–24 instances at
30 FPS. Cell lists restrict per-pixel particle checks to nearby instances, buffers
are reused, and instance formulas run once per instance. App borders reuse geometry
across focus changes and clear only edge pixels; layered-window uploads still cover
the full app surface. An FPS setting is a target, not a guaranteed rate.


## Optimization

The separate **Optimization** category adds controls alongside the existing effect
settings. Its master switch bypasses the optional paths below. Exclusions, allocation
checks, native cursor masking and safe shutdown remain mandatory. Existing effect FPS,
resolution, smoothing and adaptive controls keep their own roles.

The default keeps live animation timing, full-density fluid sampling and existing FPS
limits. Lean cursor history is enabled because unblended layers do not need a second
frame. The old experimental idle hold is now explicitly named **Freeze experimental
borders while idle (live mode only)** and defaults to Off. A saved On value is retained;
cached loop playback overrides it so cached animation keeps moving.

### Faster live experimental formulas

**Compile experimental formulas for faster live rendering** is enabled by default.
It compiles experimental field, layout and particle formulas when settings load,
reuses identical expressions and calculates field values that are constant across
pixels once per frame. Time-only RGBA fields use a uniform-color shortcut. When a
custom field supplies every color channel without reading the built-in colors, the
renderer also skips the unused built-in background shader, provided built-in
particles are not layered over the custom field.

This keeps the same formula math, assignment order and lazy `if`, `&&` and `||`
behavior. It does not lower resolution or FPS, freeze animation, or wrap time.
The compiler has bounded instruction/register storage; formulas outside those
limits use the original evaluator. Disable this switch to compare the original
path. Cursor formulas retain their existing evaluator. Classic border shaders have a separate
compiled-formula option under Shared project limits.

This works with live rendering and cached loop playback. It reduces the cost of
rendering new frames, including during pointer interaction and cache warm-up.
Complex expressions that vary at every pixel still require computation; large
border areas and window uploads can remain expensive. Use shared intermediate
variables for clarity, and use loop playback when a repeating animation is suitable.

### Cinema probe direction

**App borders: Sample Cinema outside the window**, directly below the app effect
selector, defaults to Off. Off keeps the existing inside-edge sampling. On samples
strips outside the app's four edges, using visible desktop and neighboring windows.
The strip depth scales with window size from 8 to 96 pixels. The setting also applies
to custom app-border formulas reading `kr`, `kg`, or `kb`. Ambient, Wallpaper Ambient,
monitor borders and experimental rendering keep their own behavior. Off-desktop
segments can appear black; an entirely unavailable capture keeps the last valid table
or the configured palette fallback.
The source is the visible desktop; other windows can cover the sampled area.

## Experimental visual additions

Six collapsible groups sit under **Experimental**, below the existing border controls.
Each has an independent **Enable** switch, initially Off. The existing experimental
window/screen border switches are not master switches for these six additions.
Save settings after enabling a group. Existing targeting, exclusions, fullscreen hiding,
capture visibility and cursor master settings still apply. These are experimental paths;
Windows visual behavior and performance need testing with each user's configuration.

| Group | Starting controls | Scope and requirements |
|---|---|---|
| Focus transitions | 220 ms; 65% unfocused brightness | App borders; blends between their focused/unfocused rendered appearances. |
| Click shockwaves | 550 ms; 80 px radius; 3 px thickness; 75% opacity | Cursor effects master must be enabled; normal particles may stay off. |
| Comet cursor trail | Up to 16 copies; 25% tail size; 65% opacity | Cursor effects master; overrides normal trail style and activates its sampler. |
| Glass shimmer borders | App/monitor targets; 55% strength; 15% travel speed; 8% width | Adds traveling reflections and a mouse highlight to existing visible border pixels. |
| Magnetic border particles | 60% attraction; 120 px interaction radius | Requires experimental window/screen borders with procedural or custom particles. |
| Linked palettes | Two shared colors; separate app/monitor/cursor targets | Remaps rendered hue while retaining alpha and brightness. |

### Focus transitions

The duration controls the blend after a window gains or loses focus. Unfocused
brightness multiplies its final RGB brightness; opacity is retained. Existing inactive
color/same-effect settings still determine the destination appearance. New focus changes
start from the currently displayed edge pixels, so rapid switching can reverse a fade.
Snapshots contain edge pixels only, with a maximum of 65,536 pixels per transition and
a shared 4 MiB process limit. Exceeding either limit applies the focus change immediately.
Snapshots are released after completion or geometry reallocation. Hidden/unfocused-off,
excluded and fullscreen-hidden borders remain hidden. Width and geometry do not animate.

### Click shockwaves

A new button press creates an expanding ring at the captured pointer position. Set
separate left/right colors, radius, thickness, lifetime and opacity. At most eight rings
exist at once; a new ring replaces the oldest when full. Ring positions remain in screen
coordinates while the pointer moves. The existing cursor particle layer draws the rings,
so no extra overlay window or screen capture is created. They stop and clear on cursor
worker shutdown/reload or hiding. The existing cursor overlay radius and optional visual
window walls can clip them. Click detection polls current button state; holding a button
does not repeat a ring. Very short press/release pairs between polls may be missed.
Click rings do not consume or forward mouse clicks and do not change application input.
Technical reference: [GetAsyncKeyState](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getasynckeystate).

### Comet cursor trail

Enabling Comet activates the existing trail sampler and replaces its visual style with
shrinking, fading cursor silhouettes. Normal trail lifetime, spacing, FPS, color/rainbow
and custom trail color formula remain available. Maximum copies bounds the drawing work;
Tail scale sets the smallest silhouette relative to the current cursor artwork. Recent
copies are larger and brighter. The cursor's normal glow remains independent; enable
that glow for a bright leading outline. Disabling Comet restores the configured trail
style and enabled state after settings reload. Artwork changes follow the existing cursor
capture/cache behavior. These are tinted silhouettes, matching the existing duplication
style; they do not retain the artwork's original multicolor RGB pixels.

### Glass shimmer borders

Reflection strength blends border RGB toward white. Width controls the fraction of the
perimeter occupied by the traveling highlight; Speed sets travel rate. Mouse reflection
adds a second highlight near the pointer's closest border position. Use the independent
app/monitor target switches to choose where it appears. It preserves border alpha, shape,
opacity and custom formula output coverage; completely transparent pixels stay invisible.
It works after classic or experimental shading. Inactive app policies still govern
unfocused animation. Enabling shimmer on a static border makes that border animate at
its existing FPS limit, so it adds work. Experimental screen shimmer keeps updating even
if the underlying border's idle-freeze option is enabled.

### Magnetic border particles

Move the pointer near a border to gather particles along its closed contour; move away
to relax the attraction. The existing mouse-position smoothing provides gradual movement.
Strength controls contour compression; Radius controls how far the pointer can influence
it. Custom particle `pos` output is displaced after its formula runs. Custom offsets,
size, rotation, color and count remain unchanged. Attraction affects particles only;
the body field and mouse-bending setting remain independent. It does not add particles
when a custom layout returns zero count. It does not affect classic borders, cursor
particles, or the pointer itself. Animation replay is bypassed while the option is enabled
and a valid mouse position exists, so cached loops cannot replay stale magnetic positions.

### Linked palettes and combining additions

Choose two shared colors and enable the desired renderer targets. The palette remaps
visible pixels while retaining their source brightness and alpha, including cursor glow,
trails, particles and fluid. It does not recolor desktop/taskbar icons, monitor corner fill,
or caption buttons. This is shared color selection, not synchronized particle positions
or shared animation clocks. It takes precedence over original/custom/rainbow hue; palettes
are applied before glass highlights and before the final focus blend on app borders.
Cursor rings' individual colors are also remapped when cursor linking is enabled.
Disable the corresponding target or the palette group to restore original colors.
All six groups may be enabled together; cost grows with visible pixels, copy count, ring
count and active renderers. Lower those controls first if a combination stutters. The
existing smoothness/performance settings remain independent. No motion-blur fixes are
included in this update.

## Movement and sampling diagnostics

App Ambient samples only strips outside the app's own frame, independently of the Cinema
probe-direction switch. It holds the sampled colors for the selected Ambient interval.
A failed first capture keeps the
configured fallback color; it does not borrow monitor-edge samples. Wallpaper Ambient
keeps its separate wallpaper source. Cinema also requests the app frame rectangle.
Enable **Log render timing diagnostics** while reproducing an issue: app sampling messages
show the target, effect, frame rectangle and sample validity. The settings-load message
shows whether experimental borders replace the selected classic effect. For motion-trail
problems, keep Motion Blur and timing diagnostics enabled together during window movement.
Movement updates reuse the current border pixels; experimental animation continues at
its selected cadence. A fresh position is checked again immediately before presentation.
Heavy shading can still delay updates because window events and shading share a worker.

## Cinema, frame pacing and classic formulas

Classic app Cinema now samples every visible border whose current appearance uses
Cinema, including an unfocused window set to the same appearance. Experimental
window borders still replace the classic border appearance; they do not layer Cinema
under the experimental shader.

App and monitor Cinema screen reads run on one separate capture worker per engine.
Capture workers use the same per-monitor DPI awareness and optional worker priority
as the overlay workers. Rendering consumes the latest completed color table and wakes
on a completion event.
A slow read therefore delays new sampled colors rather than blocking effect rendering.
The queue admits at most 128 target records, coalesces outstanding requests and expires
unused records after 15 seconds on later requests. Geometry/version checks reject
obsolete results after movement or target reuse. Failed reads preserve the last valid
table and log a throttled failure; recovery is logged. Before the first valid capture,
the existing palette fallback remains. Worker startup/allocation failure or queue saturation uses the
previous synchronous capture path. Reload/shutdown stop and join the worker and release
its GDI resources. A GDI call already in progress must finish before it can be joined.
Ambient sampling retains its existing timed path.

Animated app and monitor waits now account for time already spent rendering. Static
monitor effects retain a bounded idle wait, so an expired animation timestamp cannot
create a busy loop. Settings FPS limits retain their meaning; expensive formulas can
still exceed the frame budget. Experimental monitor tiles share scene layout, custom
particles and frame-wide preparation for matching frame inputs. Pixel shading remains
at full density and replay-cache behavior is retained.

**Compile classic border formulas for faster rendering** defaults to On under
Optimization → Shared project limits. It compiles classic app/screen formulas once,
reuses expressions and prepares uniform values per rendered band. Cinema/Ambient
samples remain spatial inputs. Assignment order and lazy conditionals are preserved.
Plans exceeding the compiler's register/instruction limits use the original evaluator.
The original parser limits and formula syntax are unchanged. Off, or the Optimization
master switch, restores original classic evaluation. Icons and cursor formulas retain
their own evaluation paths.

Monitor effects now require a nonzero user session and a visible window station.
The mod does not switch desktops, change permissions or modify a service. Host candidate
logs include process/session IDs. This prevents the mod from starting monitor overlays
in service/non-interactive contexts; it does not assume every Windhawk process is an
interactive host.

For window motion blur, enable **Log render timing diagnostics** when investigating
missing trails. Logs distinguish movement-event counts, start attempts, upload attempts
and failures, and report excluded targets, resize/maximize skips, capture/size limits,
blending and presentation failures. Existing trail length, strength, copies, exclusions
and capture visibility are retained. These diagnostics do not establish that a live
Windows capture problem has been solved. Disable diagnostics after collecting a sample.

### Responsive window and cursor tracking

The separate **Responsive window and cursor effect tracking** switch is On by default.
App borders follow DWM extended frame bounds (GetWindowRect fallback), rather than
caption-button bounds: the effect surrounds the whole frame. Location events reposition
already rendered pixels with SetWindowPos and SWP_NOACTIVATE. A temporary 16 ms timer
checks the target during interactive movement/resizing; it stops on move-end, hide,
minimization/destruction, reload, shutdown or ten seconds without a location event.
Resizing still requires new geometry and an effect frame. Foreground changes request
stacking synchronization. Exclusions and inactive/maximized-window rules still apply.

The original adjacent-behind-target stacking lets the app cover inner border pixels.
Borders do not become globally topmost or activate the app. Out-of-context events are
queued: this reduces avoidable delay but cannot guarantee perfect lockstep with DWM
under heavy rendering or system load.

Cursor-local glow uses a separate small overlay and position checks at least 60 times
per second, subject to scheduling. Effect calculations retain their own FPS. Smoothing
frames share the current hotspot; native-pointer masking and cursor capture visibility
apply to both cursor windows. If the extra window cannot be created or an upload fails, the previous
combined cursor presenter is used. Trails, emitted particles and fluid retain their
world positions and intentional trailing behavior. Screen borders/corners remain tied
to monitors; icons retain their native layout tracking. Off restores previous tracking
and the combined cursor presenter. Faster position checks can increase idle wakeups.

### Frame preparation, app tiles and diagnostics

**Prepare weather and music values once per frame** defaults to On. It snapshots
music gradient/audio tint and weather settings, moving lane/flake/cloud motion out of
the pixel loop. Full-resolution math is retained. Off uses the original path. Day/night
calculation already runs outside the pixel loop. Experimental custom fields also skip
unused proximity/ripple work when the compiled-formula optimization is active.

**Use narrow app border presentation tiles (experimental)** defaults to Off pending
live Windows validation. Four non-overlapping top/bottom/side windows submit portions
of the same source DIB, including rounded corners and experimental Reach. Coordinates,
shading, capture visibility and target-relative stacking are retained. This reduces
submitted area, not the full-size source DIB allocation. It adds three windows and up
to four presentation calls per target; small windows may benefit less. Allocation or
upload failure releases extra windows and restores the full surface for that border.
Movement, hiding, reload and shutdown handle every tile together.

Desktop animation repaints visible item bounds for lists up to 256 items. Larger lists
retain the whole-list repaint to avoid expensive per-item scanning on every tick.
Startup recovery and source/cache invalidation are preserved. Undecorated taskbar icons
are processed directly into their output buffer, removing one extra copy.

**Log render timing diagnostics** defaults to Off. The mod log reports per-worker
count, mean and maximum milliseconds approximately every ten seconds while rendering
continues. Stages cover experimental/classic shading, app/screen uploads, cursor source,
composition/upload, color sampling, wallpaper acquisition and icon processing. Nested
stages overlap: cursor composition includes upload, so do not add their totals. These
are elapsed stage times, not total CPU percentages or FPS promises. Standard log
thread/process fields distinguish workers. Disable diagnostics after comparison; it
adds clock reads and occasional logs, with no trace files or new profiling threads.

Compare identical effects, sizes and FPS. Test responsive tracking first; enable app
tiles separately. Check resize, focus, minimization, DPI/monitor changes, exclusions
and capture visibility. Keep tiles Off if presentation regresses on a Windows build.

References: [SetWinEventHook](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwineventhook),
[SetWindowPos](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowpos),
[DWM attributes](https://learn.microsoft.com/en-us/windows/win32/api/dwmapi/ne-dwmapi-dwmwindowattribute).

### Experimental animation playback

Choose **Experimental animation cache → Cached loop playback**. This supports Plasma,
Ember and custom field/layout/particle formulas. It caches animated frames, rather than
stopping at one frame. Classic app/screen border shaders keep their live render paths.

1. Start with 16 MiB, a four-second loop, 30 source FPS and eight cached geometries.
2. The first cycle renders and records visited phases. Later cycles reuse them.
3. Optional **Gradually pre-render the next cache frame** builds at most one extra
   future phase after a sufficiently cheap draw. It does not pause startup to build
   the whole movie. Its scheduling budget is an estimate, not a hard time deadline.
4. Interpolation blends two already available neighboring phases. If the next phase
   is not available yet, the renderer uses the current phase. Disable interpolation
   if fast particles show ghosts or decoding becomes the bottleneck.

Cached time wraps at the selected loop duration. Arbitrary formulas do not have an
automatically detectable period. Author a loop with `sin(t*tau/4)` or
`pos=fract(home-clock/4)` for a four-second cycle. Both t and clock receive replay
time; existing Speed still multiplies t. With Speed 2, that t-based example repeats
twice within the four-second cache. All animated inputs must close for a seamless
join. Interpolation can morph the join, but cannot make unrelated endpoints identical.
Built-in effects can also show a repeat at the selected join.

Mouse activity bypasses replay and uses live time for responsive bending/ripples.
After it settles, the cached idle state uses rounded pointer coordinates and an expired
entry ripple (`enter=60`). Mouse state, geometry and settings changes invalidate the
relevant cache. Custom effects that depend continuously on enter are therefore different
in idle replay mode. Switching between live interaction and loop playback can change
phase; choose Live rendering to preserve uninterrupted original timing.

The cache stores only the edge-band results, using lossless 32-bit transparent-run
and literal encoding. It does not keep a full-window bitmap for every animation frame
and writes no disk files. A base edge snapshot supports safe ahead rendering. Retained
base/frame vector capacities share the selected 8–64 MiB budget per render worker;
bounded entry/phase metadata and allocator overhead are additional. Explorer app borders
and the screen overlay have separate budgets. Existing DIBs and cursor caches are
outside this animation-cache budget.

At most 240 phases and the selected number of geometries are retained. Eight screen
tiles count as eight geometries. When full, the cache keeps its admitted geometries
until expiry; additional targets render without admission. Frames that cannot fit are
rendered live at the loop phase, so a partial cache still animates. Lower cache FPS,
shorter loops, smaller Reach and fewer targets improve the chance of fitting the whole
cycle. Increasing the budget is optional. Unused entries expire on later renderer calls;
there is no extra cleanup polling thread. Settings reload and worker shutdown release
their caches. Warm-up/packing can temporarily increase CPU usage.

### Cursor rendering and memory

| Option | What it saves | Tradeoff |
|---|---|---|
| Store one cursor frame when interpolation is bypassed | Previous-frame storage and copies on unblended layers | Blending resumes with freshly initialized history when needed. |
| Skip identical cursor uploads | Unchanged layered-window uploads | Requires a full-scene comparison scan and retained comparison frame. Turn Off to avoid that cost for constantly changing large fluid scenes. |
| Reuse unchanged native cursor artwork | Rescaling and outline distance-field rebuilding | Source pixels are still checked periodically; no native cursor replacement. |
| Idle cursor input polling | Input wake-ups | 15/30/60 Hz means up to about 67/33/17 ms input detection delay; active effect deadlines may wake sooner. |
| Native artwork refresh policy | Repeated native bitmap reads | Economy can delay same-handle animated cursor artwork updates. |
| Cursor collision window scan policy | Window enumeration calls | Economy delays reaction to changed window boundaries. Only relevant when collisions are enabled. |
| Cursor fluid raster sampling | Full fluid-grid samples per pixel | Spacing 2/4 uses small reusable rows and bilinear reconstruction; thin ink details can soften. Full preserves original sampling. |
| Extra fluid solver FPS ceiling | Simulation steps | Lower rates can change motion; existing resolution and timing mode remain separate. |

One 1080×1080 32-bit history frame is about 4.45 MiB. Avoiding the unused previous
frame can save that amount for such a layer. Disabling final comparison can avoid
another scene-sized buffer. This does not remove active canvases or required smoothing
frames. Coarse fluid sampling uses two rows, not another full-scene texture: about
17 KiB for a 1080-pixel-wide region at spacing 2, or 8.5 KiB at spacing 4.

### Shared project limits

The extra calculation ceiling limits desktop/taskbar icon, cursor and border animation
source rates. Existing smoothing can still present at 60 FPS, and Cinema sampling keeps
its separate policy. It does not limit other applications. A slower app-border fallback
rescan reduces enumeration/exclusion refresh work; window events remain active, but
missed-event recovery and external exclusion changes can take longer. The desktop icon
entry limit trades retained entries against rebuild frequency; taskbar caches stay
separate. Below-normal render worker priority can help foreground responsiveness under
load, but does not reduce the amount of rendering work and may reduce effect FPS.

### Analysis and measured limits

| Area | Finding and implemented control | Remaining cost |
|---|---|---|
| Experimental formulas | Repeated per-pixel evaluation can dominate; compressed replay skips the shader on hits. | Packing during warm-up, decoding/blending and window presentation. |
| Custom particles | Existing instance formulas already run once per instance, with spatial cell lists. | Large overlapping particles and complex body fields remain expensive. |
| Cursor | Large fluid layers amplify history copies and final comparison scans; lean storage and comparison toggle address these. | Live pointer masking, active scene composition and uploads remain. |
| Fluid | Grid interpolation previously ran for every raster pixel; optional two-row sampling lowers full-grid queries. | Reconstruction, opacity conversion and blending still run per pixel. |
| Other visuals | Icons already cache processed images; borders reuse geometry. Limits reduce calculation/rescan frequency. | More cache misses at low entry limits; classic animated shaders still calculate live. |

An isolated Linux native CPU test used 164864 experimental edge pixels over 30 frames.
One custom shader took about 1482 ms live versus 13.7 ms for warmed replay without
interpolation in that scenario. A sample compressed edge-frame capacity was 256 KiB,
versus about 7.9 MiB for a full 1920×1080 raw frame. Compression varies by effect.
These are shader/cache timings, not Windows frame rates or whole-mod speedups.

In a separate dense 1080×1080 fluid sampling-stage test, four frames took approximately
68.0 ms at full density, 51.8 ms at spacing 2 and 41.5 ms at spacing 4. That is about
24%/39% less sampling-stage time in this test, with reconstruction quality tradeoffs.
It does not measure the complete cursor renderer or the solver. Measure actual benefit
with the same effects, target sizes and FPS on Windows.

The default app-border presenter submits a full layered surface: shader caching does
not eliminate that upload. The optional tiled presenter submits four smaller edge
rectangles from the same source DIB. Microsoft documents that UpdateLayeredWindow
updates the entire destination window. GDI layered-window uploads remain the presentation path even in GPU mode.
The optional worker priority uses only thread CPU priority; it does not change process
priority, Windows settings, services or protected files.

Technical references: [UpdateLayeredWindow](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-updatelayeredwindow),
[SetThreadPriority](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setthreadpriority).


## Timing, performance, and troubleshooting

If any effect appears missing or stuck after a settings change:

1. Disable the affected feature using its own switch (or set screen-border width to 0).
2. Click outside the app.
3. Click back inside the app.
4. Change the effect.
5. Re-enable the feature (or restore the screen-border width).

This applies as general self-troubleshooting across effect categories. In particular,
Mercury app borders can require the exact sequence above before another effect appears. The switching issue remains unresolved; this workaround
may help and does not mean the underlying problem has been fixed.

Icons have independent 1–30 FPS controls. App borders and the monitor overlay have separate
FPS settings, including fixed rates through 720, Auto, Adaptive, and Adaptive Lite. A selected
rate is a target, not a guaranteed frame rate. Auto follows the foreground monitor's refresh
rate. Adaptive modes raise the rate during detected activity. For app borders, Adaptive uses
60 FPS while idle and Adaptive Lite uses 30; for the monitor overlay both idle at 30, and
Adaptive Lite limits its active rate to 60. Lower FPS, narrower borders, fewer trail copies,
and simpler formulas reduce rendering cost. Animated formulas are evaluated per pixel.

- **No custom effect:** choose Custom, save, and confirm the feature is enabled. A screen border needs nonzero width. A setting hidden by an exclusion or fullscreen policy cannot be restored by a formula.
- **Formula error:** check Windhawk's debug log for the message and character position. Unknown names, missing semicolons, wrong argument counts, or assignments to read-only inputs cause a fallback to the solid default formula. The setting text is not automatically corrected.
- **Unexpected black:** verify palette colors and channel values. `ar/ag/ab` and `kr/kg/kb` are zero on icons; a tint also retains the icon's original brightness.
- **No animation or no slider response:** reference `t` for animation, and reference `speed`, `power`, or `n` for the corresponding sliders. Replacing a recipe's `u` with `p` gives horizontal travel on icons.
- **Faint or invisible output:** check `a`, opacity/transparency, border width, icon source alpha, and inactive-window settings. Multiplying by `op` inside a border formula applies opacity an extra time.
- **Wallpaper samples show a static image:** the renderer may not expose captureable pixels; the Windows wallpaper fallback may be used.

The mod uses separate Explorer and Windhawk-hosted rendering workers with independent state. This guide describes the current source;
Windows rendering, capture availability, and interactions with other mods can vary.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Icons:
  - Appearance:
    - IconHideNames: false
      $name: 'Icons: Hide icon names'
      $description: Shows only the icons, without their text labels. Also affects type-to-select and screen readers while
        enabled.
    - IconHideArrow: false
      $name: 'Icons: Hide shortcut arrow'
      $description: Removes the small arrow at the bottom left of shortcut icons
    - IconRefreshDelay: s2
      $name: 'Icons: Delay before re-applying after resize / refresh'
      $options:
      - s05: 0.5 seconds
      - s1: 1 second
      - s2: 2 seconds
      - s3: 3 seconds
      - s5: 5 seconds
    $name: Appearance and labels
  - Color:
    - IconGrayscale: false
      $name: 'Icons: 1. Black-white colorspace'
    - IconColorize: false
      $name: 'Icons: 2. Colorizer'
      $description: Tints icons with the color below (runs after black-white)
    - IconColorizeColor: '#0078D7'
      $name: 'Icons: Colorizer color (hex)'
      $description: Only the hue of this color is used
      $format: colorRgb
    - IconColorizeSaturation: 100
      $name: 'Icons: Colorizer saturation (0-100)'
      $min: 0
      $max: 100
      $format: slider
    - IconColorizeContrast: 100
      $name: 'Icons: Colorizer contrast (0-200)'
      $description: 100 = unchanged
      $min: 0
      $max: 200
      $format: slider
    - IconTransparency: false
      $name: 'Icons: 3. Transparency'
    - IconTransparencyAmount: 50
      $name: 'Icons: Transparency amount (0-100)'
      $description: 0 = fully visible, 100 = invisible
      $min: 0
      $max: 100
      $format: slider
    $name: Color and transparency
  - Shadow:
    - IconShadow: false
      $name: 'Icons: Shadow: Enable'
    - IconShadowColor: '#000000'
      $name: 'Icons: Shadow: Color (hex)'
      $format: colorRgb
    - IconShadowOpacity: 55
      $name: 'Icons: Shadow: Opacity (0-100)'
      $min: 0
      $max: 100
      $format: slider
    - IconShadowBlur: 5
      $name: 'Icons: Shadow: Blur (0-16 px)'
      $min: 0
      $max: 16
      $format: slider
    - IconShadowOffsetX: 2
      $name: 'Icons: Shadow: Horizontal offset (-16 to 16 px)'
      $min: -16
      $max: 16
      $format: slider
    - IconShadowOffsetY: 3
      $name: 'Icons: Shadow: Vertical offset (-16 to 16 px)'
      $min: -16
      $max: 16
      $format: slider
    $name: Shadow
  - Glow:
    - IconGlow: false
      $name: 'Icons: Glow: Enable'
    - IconGlowColor: '#00BFFF'
      $name: 'Icons: Glow: Color (hex)'
      $format: colorRgb
    - IconGlowIntensity: 70
      $name: 'Icons: Glow: Intensity (0-100)'
      $min: 0
      $max: 100
      $format: slider
    - IconGlowBlur: 6
      $name: 'Icons: Glow: Blur (0-16 px)'
      $min: 0
      $max: 16
      $format: slider
    - IconGlowOutline: 1
      $name: 'Icons: Glow: Outline spread (0-8 px)'
      $min: 0
      $max: 8
      $format: slider
    $name: Glow
  - Animation:
    - IconEffect: none
      $name: 'Icons: Animated effect'
      $description: Tints icon pixels while preserving alpha, shading and labels.
      $options:
      - none: 'Off'
      - wave: Wave
      - wheel: Color wheel
      - rainbow: Rainbow
      - breathing: Breathing
      - gradient: Gradient
      - aurora: Aurora
      - silk: Silk
      - plasma: Plasma
      - pearls: Smooth pearls
      - shimmer: Shimmer
      - custom: Custom
    - IconEffectColor2: '#FF00FF'
      $name: 'Icons: Effect second color'
      $format: colorRgb
    - IconEffectSpeed: 20
      $name: 'Icons: Effect speed (1-100)'
      $min: 1
      $max: 100
      $format: slider
    - IconEffectIntensity: 70
      $name: 'Icons: Effect intensity (0-100)'
      $min: 0
      $max: 100
      $format: slider
    - IconEffectCount: 2
      $name: 'Icons: Effect count (1-16)'
      $min: 1
      $max: 16
      $format: slider
    - IconEffectFPS: 15
      $name: 'Icons: Animation FPS (1-30)'
      $min: 1
      $max: 30
      $format: slider
    - IconCustomEffect: r=hr(u-t*0.1); g=hg(u-t*0.1); b=hb(u-t*0.1); a=1
      $name: 'Icons: Custom effect formula'
      $description: Select Custom above. Assign r, g, b and a from 0 to 1; separate assignments with semicolons. See the shared Custom effects guide in Details. Screen samples and border depth are unavailable on icons.
    $name: Animated effects
  $name: Desktop icons
- $name: Taskbar icons
  TaskbarIcons:
  - Enabled: true
    $name: Enable taskbar icon effects
    $description: Customize taskbar app images independently of desktop icons.
  - Color:
    - IconGrayscale: true
      $name: Black-white colorspace
    - IconColorize: false
      $name: Colorizer
      $description: Tints icons with the color below (runs after black-white)
    - IconColorizeColor: '#0078D7'
      $name: Colorizer color (hex)
      $description: Only the hue of this color is used
      $format: colorRgb
    - IconColorizeSaturation: 100
      $name: Colorizer saturation (0-100)
      $min: 0
      $max: 100
      $format: slider
    - IconColorizeContrast: 100
      $name: Colorizer contrast (0-200)
      $description: 100 = unchanged
      $min: 0
      $max: 200
      $format: slider
    - IconTransparency: false
      $name: Transparency
    - IconTransparencyAmount: 50
      $name: Transparency amount (0-100)
      $description: 0 = fully visible, 100 = invisible
      $min: 0
      $max: 100
      $format: slider
    $name: Color and transparency
  - Shadow:
    - IconShadow: false
      $name: 'Shadow: Enable'
    - IconShadowColor: '#000000'
      $name: 'Shadow: Color (hex)'
      $format: colorRgb
    - IconShadowOpacity: 55
      $name: 'Shadow: Opacity (0-100)'
      $min: 0
      $max: 100
      $format: slider
    - IconShadowBlur: 5
      $name: 'Shadow: Blur (0-16 px)'
      $min: 0
      $max: 16
      $format: slider
    - IconShadowOffsetX: 2
      $name: 'Shadow: Horizontal offset (-16 to 16 px)'
      $min: -16
      $max: 16
      $format: slider
    - IconShadowOffsetY: 3
      $name: 'Shadow: Vertical offset (-16 to 16 px)'
      $min: -16
      $max: 16
      $format: slider
    $name: Shadow
  - Glow:
    - IconGlow: false
      $name: 'Glow: Enable'
    - IconGlowColor: '#00BFFF'
      $name: 'Glow: Color (hex)'
      $format: colorRgb
    - IconGlowIntensity: 70
      $name: 'Glow: Intensity (0-100)'
      $min: 0
      $max: 100
      $format: slider
    - IconGlowBlur: 6
      $name: 'Glow: Blur (0-16 px)'
      $min: 0
      $max: 16
      $format: slider
    - IconGlowOutline: 1
      $name: 'Glow: Outline spread (0-8 px)'
      $min: 0
      $max: 8
      $format: slider
    $name: Glow
  - Animation:
    - IconEffect: none
      $name: Animated effect
      $description: Applies to taskbar app images only. Custom formulas use the shared formula language documented in Details.
      $options:
      - none: 'Off'
      - wave: Wave
      - wheel: Color wheel
      - rainbow: Rainbow
      - breathing: Breathing
      - gradient: Gradient
      - aurora: Aurora
      - silk: Silk
      - plasma: Plasma
      - pearls: Smooth pearls
      - shimmer: Shimmer
      - custom: Custom
    - IconEffectColor2: '#FF00FF'
      $name: Effect second color
      $format: colorRgb
    - IconEffectSpeed: 20
      $name: Effect speed (1-100)
      $min: 1
      $max: 100
      $format: slider
    - IconEffectIntensity: 70
      $name: Effect intensity (0-100)
      $min: 0
      $max: 100
      $format: slider
    - IconEffectCount: 2
      $name: Effect count (1-16)
      $min: 1
      $max: 16
      $format: slider
    - IconEffectFPS: 10
      $name: Animation FPS (1-30)
      $min: 1
      $max: 30
      $format: slider
    - IconCustomEffect: r=hr(u-t*0.1); g=hg(u-t*0.1); b=hb(u-t*0.1); a=1
      $name: Custom effect formula
      $description: Select Custom above. Uses the shared formula language documented in Details; coordinates span the taskbar canvas. Screen samples and border depth are unavailable on icons.
    $name: Animated effects
- $name: Start menu icons
  StartMenuIcons:
  - Enabled: false
    $name: Enable Start menu icon effects
    $description: Pinned and All apps bitmap icons in the Windows 11 Start menu. Experimental; vector glyphs, profile pictures, search results and third-party Start menus are excluded.
  - Color:
    - IconGrayscale: true
      $name: Black-white colorspace
    - IconColorize: false
      $name: Colorizer
      $description: Tints icons with the color below (runs after black-white)
    - IconColorizeColor: '#0078D7'
      $name: Colorizer color (hex)
      $description: Only the hue of this color is used
      $format: colorRgb
    - IconColorizeSaturation: 100
      $name: Colorizer saturation (0-100)
      $min: 0
      $max: 100
      $format: slider
    - IconColorizeContrast: 100
      $name: Colorizer contrast (0-200)
      $description: 100 = unchanged
      $min: 0
      $max: 200
      $format: slider
    - IconTransparency: false
      $name: Transparency
    - IconTransparencyAmount: 50
      $name: Transparency amount (0-100)
      $description: 0 = fully visible, 100 = invisible
      $min: 0
      $max: 100
      $format: slider
    $name: Color and transparency
  - Shadow:
    - IconShadow: false
      $name: 'Shadow: Enable'
    - IconShadowColor: '#000000'
      $name: 'Shadow: Color (hex)'
      $format: colorRgb
    - IconShadowOpacity: 55
      $name: 'Shadow: Opacity (0-100)'
      $min: 0
      $max: 100
      $format: slider
    - IconShadowBlur: 5
      $name: 'Shadow: Blur (0-16 px)'
      $min: 0
      $max: 16
      $format: slider
    - IconShadowOffsetX: 2
      $name: 'Shadow: Horizontal offset (-16 to 16 px)'
      $min: -16
      $max: 16
      $format: slider
    - IconShadowOffsetY: 3
      $name: 'Shadow: Vertical offset (-16 to 16 px)'
      $min: -16
      $max: 16
      $format: slider
    $name: Shadow
  - Glow:
    - IconGlow: false
      $name: 'Glow: Enable'
    - IconGlowColor: '#00BFFF'
      $name: 'Glow: Color (hex)'
      $format: colorRgb
    - IconGlowIntensity: 70
      $name: 'Glow: Intensity (0-100)'
      $min: 0
      $max: 100
      $format: slider
    - IconGlowBlur: 6
      $name: 'Glow: Blur (0-16 px)'
      $min: 0
      $max: 16
      $format: slider
    - IconGlowOutline: 1
      $name: 'Glow: Outline spread (0-8 px)'
      $min: 0
      $max: 8
      $format: slider
    $name: Glow
  - Animation:
    - IconEffect: none
      $name: Animated effect
      $description: Applies to app icons inside the Start menu. Custom formulas use the shared formula language documented in Details.
      $options:
      - none: 'Off'
      - wave: Wave
      - wheel: Color wheel
      - rainbow: Rainbow
      - breathing: Breathing
      - gradient: Gradient
      - aurora: Aurora
      - silk: Silk
      - plasma: Plasma
      - pearls: Smooth pearls
      - shimmer: Shimmer
      - custom: Custom
    - IconEffectColor2: '#FF00FF'
      $name: Effect second color
      $format: colorRgb
    - IconEffectSpeed: 20
      $name: Effect speed (1-100)
      $min: 1
      $max: 100
      $format: slider
    - IconEffectIntensity: 70
      $name: Effect intensity (0-100)
      $min: 0
      $max: 100
      $format: slider
    - IconEffectCount: 2
      $name: Effect count (1-16)
      $min: 1
      $max: 16
      $format: slider
    - IconEffectFPS: 10
      $name: Animation FPS (1-30)
      $min: 1
      $max: 30
      $format: slider
    - IconCustomEffect: r=hr(u-t*0.1); g=hg(u-t*0.1); b=hb(u-t*0.1); a=1
      $name: Custom effect formula
      $description: Select Custom above. Uses the shared formula language documented in Details; coordinates span the Start menu canvas. Screen samples and border depth are unavailable on icons.
    $name: Animated effects
- $name: File Explorer icons
  ExplorerIcons:
  - Enabled: false
    $name: Enable File Explorer icon effects
    $description: File and folder image-list icons inside File Explorer, including supported DirectUI file views. Independent of desktop and taskbar icons. Native draw flags and overlays are captured before processing; unsupported draws keep native rendering.
  - IncludeNavigationPane: false
    $name: Include navigation-pane icons (pinned folders and other sidebar entries)
    $description: Applies the File Explorer icon settings to the left navigation tree as well. Covers the sidebar tree as a whole, including pinned folders and drives; does not change toolbar, address bar or tab icons.
    $showIf: {Enabled: true}
  - Color:
    - IconGrayscale: true
      $name: Black-white colorspace
    - IconColorize: false
      $name: Colorizer
      $description: Tints icons with the color below (runs after black-white)
    - IconColorizeColor: '#0078D7'
      $name: Colorizer color (hex)
      $description: Only the hue of this color is used
      $format: colorRgb
    - IconColorizeSaturation: 100
      $name: Colorizer saturation (0-100)
      $min: 0
      $max: 100
      $format: slider
    - IconColorizeContrast: 100
      $name: Colorizer contrast (0-200)
      $description: 100 = unchanged
      $min: 0
      $max: 200
      $format: slider
    - IconTransparency: false
      $name: Transparency
    - IconTransparencyAmount: 50
      $name: Transparency amount (0-100)
      $description: 0 = fully visible, 100 = invisible
      $min: 0
      $max: 100
      $format: slider
    $name: Color and transparency
  - Shadow:
    - IconShadow: false
      $name: 'Shadow: Enable'
    - IconShadowColor: '#000000'
      $name: 'Shadow: Color (hex)'
      $format: colorRgb
    - IconShadowOpacity: 55
      $name: 'Shadow: Opacity (0-100)'
      $min: 0
      $max: 100
      $format: slider
    - IconShadowBlur: 5
      $name: 'Shadow: Blur (0-16 px)'
      $min: 0
      $max: 16
      $format: slider
    - IconShadowOffsetX: 2
      $name: 'Shadow: Horizontal offset (-16 to 16 px)'
      $min: -16
      $max: 16
      $format: slider
    - IconShadowOffsetY: 3
      $name: 'Shadow: Vertical offset (-16 to 16 px)'
      $min: -16
      $max: 16
      $format: slider
    $name: Shadow
  - Glow:
    - IconGlow: false
      $name: 'Glow: Enable'
    - IconGlowColor: '#00BFFF'
      $name: 'Glow: Color (hex)'
      $format: colorRgb
    - IconGlowIntensity: 70
      $name: 'Glow: Intensity (0-100)'
      $min: 0
      $max: 100
      $format: slider
    - IconGlowBlur: 6
      $name: 'Glow: Blur (0-16 px)'
      $min: 0
      $max: 16
      $format: slider
    - IconGlowOutline: 1
      $name: 'Glow: Outline spread (0-8 px)'
      $min: 0
      $max: 8
      $format: slider
    $name: Glow
  - Animation:
    - IconEffect: none
      $name: Animated effect
      $description: Applies to file/folder icons and, when enabled above, navigation-pane icons. Custom formulas use the shared formula language documented in Details.
      $options:
      - none: 'Off'
      - wave: Wave
      - wheel: Color wheel
      - rainbow: Rainbow
      - breathing: Breathing
      - gradient: Gradient
      - aurora: Aurora
      - silk: Silk
      - plasma: Plasma
      - pearls: Smooth pearls
      - shimmer: Shimmer
      - custom: Custom
    - IconEffectColor2: '#FF00FF'
      $name: Effect second color
      $format: colorRgb
    - IconEffectSpeed: 20
      $name: Effect speed (1-100)
      $min: 1
      $max: 100
      $format: slider
    - IconEffectIntensity: 70
      $name: Effect intensity (0-100)
      $min: 0
      $max: 100
      $format: slider
    - IconEffectCount: 2
      $name: Effect count (1-16)
      $min: 1
      $max: 16
      $format: slider
    - IconEffectFPS: 10
      $name: Animation FPS (1-30)
      $min: 1
      $max: 30
      $format: slider
    - IconCustomEffect: r=hr(u-t*0.1); g=hg(u-t*0.1); b=hb(u-t*0.1); a=1
      $name: Custom effect formula
      $description: Select Custom above. Uses the shared formula language documented in Details; coordinates span the Explorer file-view canvas. Screen samples and border depth are unavailable on icons.
    $name: Animated effects
- Selection:
  - SelDesktop: true
    $name: 'Selection box: Desktop'
  - SelExplorer: true
    $name: 'Selection box: File Explorer'
  - SelFillColor: '#0078D7'
    $name: 'Selection box: Box color (hex)'
    $format: colorRgb
  - SelFillOpacity: 25
    $name: 'Selection box: Box opacity (0-100)'
    $min: 0
    $max: 100
    $format: slider
  - SelBorderColor: '#0078D7'
    $name: 'Selection box: Border color (hex)'
    $format: colorRgb
  - SelBorderOpacity: 100
    $name: 'Selection box: Border opacity (0-100)'
    $min: 0
    $max: 100
    $format: slider
  - SelBorderWidth: 1
    $name: 'Selection box: Border width in pixels (0-8)'
    $min: 0
    $max: 8
    $format: slider
  - SelBlur: false
    $name: 'Selection box: Blur what is under the box'
  - SelBlurStrength: 10
    $name: 'Selection box: Blur strength (1-30)'
    $min: 1
    $max: 30
    $format: slider
  - SelStyleItems: true
    $name: 'Selection box: Same box and border on selected items'
  $name: Selection box
- Borders:
  - Geometry:
    - BorderEnable: true
      $name: 'App borders: Enable'
    - BorderWidth: 3
      $name: 'App borders: Border width (px)'
      $description: Total thickness, half of it stays behind the window. 0 = off
      $min: 0
      $max: 40
      $format: slider
    - BorderRadius: 8
      $name: 'App borders: Corner radius (px)'
      $description: Windows 11 windows use about 8
      $min: 0
      $max: 60
      $format: slider
    - HideMaximized: true
      $name: 'App borders: Hide on maximized and fullscreen windows'
    $name: Shape and appearance
  - Effects:
    - BorderEffect: static
      $name: 'App borders: Effect'
      $description: See the README for what every effect does
      $options:
      - static: Default (solid color)
      - breathing: Breathing
      - glow: Glow
      - neon: Neon
      - wave: Wave
      - rainbow: Rainbow
      - comet: Comet
      - gradient: Gradient
      - aurora: Aurora
      - heartbeat: Heartbeat
      - fire: Fire
      - stars: Stars
      - ambient: Ambient (outside app edges, timed)
      - wheel: Color wheel
      - silk: Silk
      - plasma: Plasma
      - pearls: Smooth pearls
      - shimmer: Shimmer
      - mercury-v3: Mercury V3 (Controlled Chaos)
      - mercury-v6: Mercury V6 (Mercury Storm)
      - music-reactive: Music Reactive
      - timecycle: 24-Hour Cycle
      - weather: Live Weather
      - wallpaper: Wallpaper Ambient
      - cinema: Cinema (window edge colors)
      - custom: Custom (use the formula below)
    - CinemaOutside: false
      $name: 'App borders: Sample Cinema outside the window'
      $description: Off samples just inside the app frame. On samples the surrounding desktop or visible apps outside its four edges. Applies to app-border Cinema and custom formulas using kr/kg/kb. Monitor Cinema, Ambient and Wallpaper Ambient keep their own sampling behavior. Areas outside the desktop have no colors to sample and may appear black.
    - BorderColor1: '#00BFFF'
      $name: 'App borders: Color 1 (hex)'
      $format: colorRgb
    - BorderColor2: '#FF00FF'
      $name: 'App borders: Color 2 (hex)'
      $format: colorRgb
    - BorderOpacity: 100
      $name: 'App borders: Opacity (%)'
      $min: 1
      $max: 100
      $format: slider
    - EffectSpeed: 20
      $name: 'App borders: Effect speed (1-100)'
      $min: 1
      $max: 100
      $format: slider
    - EffectIntensity: 70
      $name: 'App borders: Effect intensity (0-100)'
      $min: 0
      $max: 100
      $format: slider
    - EffectCount: 2
      $name: 'App borders: Effect count (1-16)'
      $min: 1
      $max: 16
      $format: slider
    - BorderCustom: hu=p*n-t*speed*0.1; r=hr(hu); g=hg(hu); b=hb(hu); a=1-d*0.5
      $name: 'App borders: Custom effect'
      $description: Select Custom above. Uses the shared Custom effects guide in Details, including variables, recipes and per-app rules. Border geometry is controlled separately.
    $name: Effects and colors
  - Inactive:
    - InactiveMode: solid
      $name: 'App borders: Unfocused windows'
      $options:
      - solid: Solid color
      - same: Same look, frozen
      - false: No border
    - InactiveColor: '#555555'
      $name: 'App borders: Unfocused color (hex)'
      $format: colorRgb
    $name: Unfocused windows
  - Timing:
    - AnimationFPS: f30
      $name: 'App borders: Animation FPS'
      $description: Auto follows the foreground monitor refresh rate. Adaptive modes use 30 FPS while idle and boost
        during detected movement / changing Cinema samples.
      $options:
      - auto: Auto (monitor refresh rate)
      - adaptive: Adaptive (30 idle, refresh rate on activity)
      - adaptiveLite: Adaptive Lite (30 idle, refresh rate on activity)
      - f10: '10'
      - f15: '15'
      - f30: '30'
      - f60: '60'
      - f75: '75'
      - f100: '100'
      - f120: '120'
      - f144: '144'
      - f200: '200'
      - f240: '240'
      - f360: '360'
      - f480: '480'
      - f500: '500'
      - f720: '720'
    - AmbientInterval: s5
      $name: Ambient / Wallpaper Ambient update interval
      $description: Colors remain unchanged between samples. Cinema uses its separate sampling rate.
      $options:
      - s2: 2 seconds
      - s5: 5 seconds
      - s10: 10 seconds
      - s20: 20 seconds
      - s30: 30 seconds
      - s60: 60 seconds
      - s120: 120 seconds
      - s240: 240 seconds
    - SampleRate: r10
      $name: 'App borders: Color sampling rate (Cinema)'
      $description: 'Screen captures per second, independent of animation FPS. Auto follows the foreground monitor up to 240/s. Adaptive uses 5/s while stable and boosts on color changes or window activity; Lite uses 2/s idle and caps active sampling at 60/s. Higher rates use more CPU.'
      $options:
      - r1: 1 per second
      - r2: 2 per second
      - r5: 5 per second
      - r10: 10 per second
      - r15: 15 per second
      - r20: 20 per second
      - r24: 24 per second
      - r30: 30 per second
      - r45: 45 per second
      - r60: 60 per second
      - r90: 90 per second
      - r120: 120 per second
      - r144: 144 per second
      - r165: 165 per second
      - r180: 180 per second
      - r200: 200 per second
      - r240: 240 per second
      - auto: Auto (foreground display refresh, up to 240/s)
      - adaptive: Adaptive (5/s idle, display refresh when active)
      - adaptiveLite: Adaptive Lite (2/s idle, up to 60/s active)
    $name: Timing and sampling
  - Rules:
    - AppRules:
      - 'notepad.exe: effect=neon, color=#FF3030, width=3'
      $name: 'App borders: Per-app border rules'
      $description: 'One rule per entry, press Add new for more. Format: app.exe: key=value, key=value (see the README
        for all keys and examples)'
    - AppExclude:
      - searchhost.exe
      - startmenuexperiencehost.exe
      - shellexperiencehost.exe
      $name: 'App borders: Excluded apps'
      $description: Process names/paths (wildcards supported) or window classes excluded from borders and motion trails
    - InheritWindhawkExclusions: true
      $name: 'App borders: Honor Windhawk default and Advanced exclusions'
      $description: Combine your list above with Windhawk's built-in critical/incompatible/game exclusions, global process exclusions, and this mod's Advanced exclusions. Applies to app borders and motion trails. Refreshed every 2 seconds. Does not change injection settings or detect every game; add custom game paths manually. An exclusion of '*' hides all app borders.
    $name: Per-app rules and exclusions
  - Motion:
    - MotionBlur: false
      $name: 'Motion blur: Enable'
    - TrailLength: 250
      $name: 'Motion blur: Trail length (ms)'
      $min: 50
      $max: 1500
      $format: slider
    - TrailStrength: 50
      $name: 'Motion blur: Trail strength (1-100)'
      $min: 1
      $max: 100
      $format: slider
    - TrailBlur: 12
      $name: 'Motion blur: Trail blur (px)'
      $min: 1
      $max: 60
      $format: slider
    - TrailCopies: 6
      $name: 'Motion blur: Trail copies (2-12)'
      $min: 2
      $max: 12
      $format: slider
    $name: Window motion blur
  $name: App window
- Corners:
  - Color: '#000000'
    $name: Corner color
    $description: Color of the rounded corners
    $format: colorRgb
  - Opacity: 100
    $name: Corner opacity (%)
    $description: 1-100
    $min: 1
    $max: 100
    $format: slider
  - Size: 30
    $name: Corner size (px)
    $description: 0-500. 0 = square corners
    $min: 0
    $max: 500
    $format: slider
  - Roundness: 100
    $name: Corner roundness
    $description: 100 = perfect circular curve, lower = squarer / tighter corner (1-100)
    $min: 1
    $max: 100
    $format: slider
  - HideOnFullscreen: true
    $name: Hide on fullscreen applications
  $name: Monitor corners
- ScreenBorder:
  - BorderWidth: 0
    $name: Screen border width (px)
    $description: 0 = no border (0-200). The border follows the rounded corners. Glow-style effects look best at 8-30.
    $min: 0
    $max: 200
    $format: slider
  - BorderColor: '#00BFFF'
    $name: Border color 1
    $description: Main border color (hex)
    $format: colorRgb
  - BorderColor2: '#FF00FF'
    $name: Border color 2
    $description: Second color, used by Wave, Gradient, Comet, Fire and Stars
    $format: colorRgb
  - BorderOpacity: 100
    $name: Border opacity (%)
    $description: 1-100
    $min: 1
    $max: 100
    $format: slider
  - Effect: static
    $name: Border effect
    $description: Animation of the border. See the README for what every effect does and how Custom works.
    $options:
    - static: Default (solid color)
    - breathing: Breathing
    - glow: Glow
    - neon: Neon
    - wave: Wave
    - rainbow: Rainbow
    - comet: Comet
    - gradient: Gradient
    - aurora: Aurora
    - heartbeat: Heartbeat
    - fire: Fire
    - stars: Stars
    - ambient: Ambient (screen edges, timed)
    - wheel: Color wheel
    - silk: Silk
    - plasma: Plasma
    - pearls: Smooth pearls
    - shimmer: Shimmer
    - mercury-v3: Mercury V3 (Controlled Chaos)
    - mercury-v6: Mercury V6 (Mercury Storm)
    - music-reactive: Music Reactive
    - timecycle: 24-Hour Cycle
    - weather: Live Weather
    - wallpaper: Wallpaper Ambient
    - cinema: Cinema (screen edge colors)
    - custom: Custom (use the formula below)
  - EffectSpeed: 20
    $name: Effect speed (1-100)
    $min: 1
    $max: 100
    $format: slider
  - EffectIntensity: 70
    $name: Effect intensity (0-100)
    $description: Strength of the effect, meaning depends on the effect (see README)
    $min: 0
    $max: 100
    $format: slider
  - EffectCount: 2
    $name: Effect count (1-16)
    $description: Number of waves / comets / flames / stars / color cycles
    $min: 1
    $max: 16
    $format: slider
  - CustomEffect: hu=p*n-t*speed*0.1; r=hr(hu); g=hg(hu); b=hb(hu); a=1-d*0.5
    $name: Custom effect
    $description: Select Custom above. Uses the shared Custom effects guide in Details. The canvas is one monitor; corner-fill color is configured separately. Wallpaper sampling is available through its preset, not a custom formula.
  - Timing:
    - AnimationFPS: f30
      $name: Animation FPS
      $description: Auto follows the foreground monitor refresh rate. Adaptive modes use 30 FPS while idle and boost during
        detected window movement / changing Cinema samples. Static effects never animate.
      $options:
      - auto: Auto (monitor refresh rate)
      - adaptive: Adaptive (30 idle, refresh rate on activity)
      - adaptiveLite: Adaptive Lite (30 idle, refresh rate on activity)
      - f10: '10'
      - f15: '15'
      - f30: '30'
      - f60: '60'
      - f75: '75'
      - f100: '100'
      - f120: '120'
      - f144: '144'
      - f200: '200'
      - f240: '240'
      - f360: '360'
      - f480: '480'
      - f500: '500'
      - f720: '720'
    - AmbientInterval: s5
      $name: Ambient / Wallpaper Ambient update interval
      $description: Colors remain unchanged between samples. Cinema uses its separate sampling rate.
      $options:
      - s2: 2 seconds
      - s5: 5 seconds
      - s10: 10 seconds
      - s20: 20 seconds
      - s30: 30 seconds
      - s60: 60 seconds
      - s120: 120 seconds
      - s240: 240 seconds
    - SampleRate: r10
      $name: Color sampling rate (Cinema)
      $description: 'Screen captures per second, independent of animation FPS. Auto follows the foreground monitor up to 240/s. Adaptive uses 5/s while stable and boosts on color changes or window activity; Lite uses 2/s idle and caps active sampling at 60/s. Higher rates use more CPU.'
      $options:
      - r1: 1 per second
      - r2: 2 per second
      - r5: 5 per second
      - r10: 10 per second
      - r15: 15 per second
      - r20: 20 per second
      - r24: 24 per second
      - r30: 30 per second
      - r45: 45 per second
      - r60: 60 per second
      - r90: 90 per second
      - r120: 120 per second
      - r144: 144 per second
      - r165: 165 per second
      - r180: 180 per second
      - r200: 200 per second
      - r240: 240 per second
      - auto: Auto (foreground display refresh, up to 240/s)
      - adaptive: Adaptive (5/s idle, display refresh when active)
      - adaptiveLite: Adaptive Lite (2/s idle, up to 60/s active)
    $name: Timing and sampling
  $name: Screen border
- MusicReactive:
  - AudioDeviceName: ""
    $name: Audio device name (input or output)
    $description: Enter the Windows audio device name or a unique part of it (case-insensitive). Leave empty for the default playback output. If multiple enabled devices match, the mod reports ambiguity in its log and does not pick one arbitrarily.
  - Style: mercury-v3
    $name: Music effect style (shared by both borders)
    $description: Select Music Reactive in either border effect dropdown to activate this style. Uses the selected Windows audio endpoint peak level, not frequency analysis.
    $options:
    - mercury-v3: Reactive Mercury V3
    - mercury-v6: Reactive Mercury V6
    - wave: Wave
    - neon: Neon
    - rainbow: Rainbow
    - glow: Glow
    - breathing: Breathing
  - Enable: true
    $name: Enable audio response for Music Reactive effect
    $description: Master switch. Selecting Music Reactive on an app or screen border activates its response independently. Uses the named audio endpoint, or default playback output when the name is empty.
  - Sensitivity: 180
    $name: Sensitivity (50-400)
    $min: 50
    $max: 400
    $format: slider
  - SmoothingEnabled: true
    $name: Smooth audio response
    $description: Smooth sudden volume changes without changing the audio device. Turn off for immediate response.
  - Attack: 55
    $name: Rise speed (1-100)
    $description: How quickly effects respond when music gets louder. Higher is faster.
    $min: 1
    $max: 100
    $format: slider
  - Release: 13
    $name: Fall speed (1-100)
    $description: How quickly effects fade when music gets quieter. Higher is faster.
    $min: 1
    $max: 100
    $format: slider
  - NoiseGate: 0
    $name: Ignore low audio levels (0-30%)
    $description: Ignore background noise below this peak threshold. Zero disables the gate.
    $min: 0
    $max: 30
    $format: slider
  - ResponseCurve: linear
    $name: Audio response curve
    $description: Soft emphasizes quiet audio, punchy emphasizes loud peaks. Affects all enabled music responses.
    $options:
    - linear: Linear
    - soft: Soft (more visible at low volume)
    - punchy: Punchy (emphasize peaks)
  - Gain: 100
    $name: Additional audio gain (25-400%)
    $description: Multiplies the detected peak before applying sensitivity. High values can cause clipping at maximum brightness.
    $min: 25
    $max: 400
    $format: slider
  - Strength: 65
    $name: Pulse strength (0-100)
    $min: 0
    $max: 100
    $format: slider
  - IdleBrightness: 65
    $name: Brightness when silent (0-100)
    $min: 0
    $max: 100
    $format: slider
  - PulseEnabled: true
    $name: Pulse brightness with music
    $description: Original audio-driven opacity pulse. Turn off to keep steady brightness while using speed, color or directional mask responses.
  - SpeedEnabled: false
    $name: Change animation speed with music
  - SpeedBoost: 150
    $name: Extra speed at full audio level (0-400%)
    $min: 0
    $max: 400
    $format: slider
  - ColorEnabled: false
    $name: Shift border color with music
  - PulseColor: "#00D9FF"
    $name: Color at full audio level
    $description: Hex RGB color used when music is loud; original effect colors return during silence.
  - GradientEnabled: false
    $name: Directional opacity mask reacts to music
    $description: Audio reveals more of the border along the selected direction. At 0 degrees, low audio shows the bottom portion first, and higher audio reveals toward the top.
  - GradientDegrees: 0
    $name: Mask direction (0-360 degrees)
    $min: 0
    $max: 360
    $format: slider
  - GradientSoftness: 15
    $name: Mask edge softness (0-100)
    $min: 0
    $max: 100
    $format: slider
  $name: Music Reactive
- TimeCycle:
  - Latitude: 0
    $name: Latitude (degrees, -90 to 90)
    $description: Set your approximate latitude for real sunrise and sunset. Zero is the equator, not automatic location detection.
    $min: -90
    $max: 90
  - Longitude: 0
    $name: Longitude (degrees, -180 to 180)
    $description: Set your approximate longitude (east positive, west negative). Used with UTC to calculate the Sun position; Windows handles the current date and clock.
    $min: -180
    $max: 180
  - DayBrightness: 100
    $name: Daytime brightness (0-100)
    $min: 0
    $max: 100
    $format: slider
  - NightBrightness: 20
    $name: Nighttime brightness (0-100)
    $min: 0
    $max: 100
    $format: slider
  - TwilightRange: 8
    $name: Sunrise and sunset transition range (degrees)
    $description: Smooth transition around the horizon. Larger values mean longer sunrise/sunset fades.
    $min: 1
    $max: 20
    $format: slider
  - TintEnabled: true
    $name: Adjust colors for time of day
  - NightColor: '#648ACD'
    $name: Night color
    $format: colorRgb
  - TwilightColor: '#FFAD61'
    $name: Sunrise and sunset color
    $format: colorRgb
  - ColorStrength: 35
    $name: Time-based color tint strength (0-100)
    $min: 0
    $max: 100
    $format: slider
  - WeatherEnabled: true
    $name: Enable live weather updates
    $description: Weather data is requested from Open-Meteo using the latitude and longitude above. Internet access is required; if unavailable, the last successful weather is kept.
  - WeatherIntensity: 80
    $name: Weather animation intensity
    $min: 0
    $max: 100
    $format: slider
  - WeatherSpeed: 50
    $name: Weather animation speed
    $min: 0
    $max: 100
    $format: slider
  - WeatherSoftness: 65
    $name: Weather particle and cloud softness
    $min: 0
    $max: 100
    $format: slider
  - RainColor: '#258DD9'
    $name: Rain color
    $format: colorRgb
  - CloudColor: '#F0F5FF'
    $name: Cloud color
    $format: colorRgb
  - SunnyColor: '#FFD36A'
    $name: Sunny color
    $format: colorRgb
  - SnowColor: '#D5EFFF'
    $name: Snow color
    $format: colorRgb
  - WeatherRefresh: 15
    $name: Weather refresh interval (minutes)
    $min: 5
    $max: 60
  $name: Location settings and effects
- CursorEffects:
  - General:
    - Controls:
      - Enabled: false
        $name: Enable cursor effects
      - WindowCollisions: false
        $name: Bounce effects off window borders (experimental)
        $description: Focused window contains particle and fluid effects; other nearby windows repel them. Trails are clipped at frames. Never confines the actual mouse. Disabled by default.
      - HideFullscreen: true
        $name: Hide on fullscreen / borderless fullscreen
      - HideFromCapture: false
        $name: Exclude overlay from supported screen captures
      $name: Basic controls
    - RGB:
      - AllEffects: false
        $name: Rainbow colors for ALL visual effects
        $description: Off uses selected palette colors (and separate explicit rainbow presets). On overrides colors for outlines, trails and particles with animated RGB, without changing their motion or geometry. Never recolors the actual Windows cursor.
      $name: Global rainbow override
    $name: General
  - Cursor:
    - Shape:
      - Scale: 100
        $showIf: {AutoSize: false}
        $name: Manual silhouette scale (only when Auto-match is off)
        $min: 50
        $max: 250
        $format: slider
      - AutoSize: false
        $name: Auto-match Windows cursor size (experimental)
        $description: Off by default because automatic DPI compensation may oversize some custom pointers; enable only when needed. Manual scale is used while disabled.
      $name: Cursor tracking and size
    $name: Cursor
  - Rendering:
    - FrameRate:
      - Mode: adaptive
        $name: FPS mode
        $options:
        - fixed: Fixed
        - auto: Auto (monitor Hz)
        - adaptive: Adaptive (idle, half or full monitor Hz)
      - FixedFPS: 120
        $showIf: {Mode: fixed}
        $name: Fixed FPS
        $min: 15
        $max: 360
        $format: slider
      - IdleFPS: 20
        $showIf: {Mode: adaptive}
        $name: Adaptive idle FPS
        $min: 15
        $max: 30
        $format: slider
      - HalfSpeedThreshold: 1000
        $showIf: {Mode: adaptive}
        $name: Adaptive fast-motion threshold (px/s)
        $min: 300
        $max: 3000
        $format: slider
      - MaximumFPS: 360
        $name: FPS safety ceiling
        $min: 30
        $max: 360
        $format: slider
      $name: FPS mode
    - Interpolation:
      - Smoothing: false
        $name: Smooth low-FPS effects with 60 FPS interpolation
        $description: Optional per-effect pixel interpolation. Calculations stay at the chosen FPS; effects already at 60 FPS or higher bypass blending. Default off for unchanged visuals.
      $name: Smooth interpolation
    - Rates:
      - ParticleSimulationFPS: 60
        $name: Particle physics FPS
        $min: 15
        $max: 240
        $format: slider
      - TrailSampleFPS: 120
        $name: Trail sampler FPS
        $min: 15
        $max: 240
        $format: slider
      - GlowAnimationFPS: 60
        $name: Glow animation FPS
        $min: 15
        $max: 240
        $format: slider
      $name: Update rates
    - Limits:
      - AdaptiveQuality: true
        $name: Reduce particle draw density automatically when frames are expensive
      - OverlayRadius: 440
        $name: Overlay radius around cursor (px)
        $min: 160
        $max: 480
        $format: slider
      $name: Performance limits
    $name: Rendering
  - Glow:
    - Switches:
      - Enabled: false
        $name: Enable outline
      - HaloEnabled: false
        $showIf: {Enabled: true}
        $name: Add soft glow outside the outline
        $description: Off keeps the smooth colored outline but removes the surrounding glow.
      $name: Enable / glow
    - OutlineStyle:
      - Mode: neon
        $name: Outline effect
        $options:
        - solid: Static outline
        - neon: Neon
        - breathing: Breathing
        - rainbow: Rainbow cycling
        - spatial: Rainbow outline
        - aurora: Aurora
        - plasma: Plasma
        - shimmer: Shimmer
        - mercury-v3: Mercury V3 (cursor adaptation)
        - mercury-v6: Mercury V6 (cursor adaptation)
        - liquid: Liquid outline
        - comet: Comet / traveling highlights
      - Speed: 50
        $name: Effect animation speed
        $min: 0
        $max: 100
        $format: slider
      - RainbowSpeed: 45
        $name: Hue cycle speed (degrees/sec)
        $min: 0
        $max: 360
        $format: slider
      $name: Outline style
    - OutlineColors:
      - Color: '#AA55FF'
        $name: Main color
        $format: colorRgb
      - Color2: '#00EEFF'
        $name: Second color
        $format: colorRgb
      $name: Outline colors
    - OutlineGeometry:
      - Thickness: 2
        $name: Outline thickness (px)
        $min: 0
        $max: 16
        $format: slider
      - Strength: 75
        $name: Outline brightness (%)
        $min: 0
        $max: 100
        $format: slider
      - Radius: 10
        $name: Soft glow radius (px)
        $min: 1
        $max: 30
        $format: slider
      - Softness: 65
        $name: Soft glow feather (%)
        $min: 0
        $max: 100
        $format: slider
      $name: Outline geometry
    $name: Outline
  - Fluid:
    - Switches:
      - Enabled: false
        $name: Enable real 2D liquid ink simulation
        $description: Moving the mouse injects coloured dye and velocity; swirling ink fades in screen space. Can also be enabled by selecting the Liquid outline preset.
      - Rainbow: false
        $name: Rainbow dye colours
      $name: Fluid controls
    - Colors:
      - Color1: '#AA55FF'
        $name: First fluid colour
        $format: colorRgb
      - Color2: '#00EEFF'
        $name: Second fluid colour
        $format: colorRgb
      - Opacity: 72
        $name: Fluid opacity (%)
        $min: 1
        $max: 100
        $format: slider
      $name: Fluid dye colours
    - Simulation:
      - Resolution: r96
        $name: Fluid grid resolution (higher costs CPU)
        $options:
        - r64: '64 x 64 (performance)'
        - r96: '96 x 96 (balanced)'
        - r128: '128 x 128 (detail)'
        - r192: '192 x 192 (high detail / full monitor)'
      - FPS: 30
        $name: Fluid simulation FPS limit (15-60)
        $description: In Independent mode this is the fixed rate; otherwise a maximum limit. The existing value is preserved.
        $min: 15
        $max: 60
        $format: slider
      - FPSMode: independent
        $name: Fluid simulation FPS mode
        $description: Independent keeps original timing; Follow rendering caps fluid by overlay FPS; Adaptive uses idle/normal/fast rates (capped by the Fluid FPS slider). Simulation and presentation remain separate.
        $options:
        - independent: Independent (original fixed FPS)
        - follow: Follow rendering FPS (capped)
        - adaptive: Adaptive (idle / normal / fast)
      - IdleFPS: 15
        $showIf: {FPSMode: adaptive}
        $name: Adaptive fluid idle FPS
        $description: Only used in Adaptive fluid FPS mode; ink keeps drifting while visible.
        $min: 15
        $max: 30
        $format: slider
      - Radius: 185
        $name: Fluid coverage radius in pixels (0 = entire monitor)
        $description: Controls the simulated region separately from the overlay radius. Larger values keep ink around longer without edge cutoff. 0 covers the monitor currently under the cursor, with a fixed simulation area. Higher coverage makes grid cells larger at the same resolution; try 128 or 192 detail for full monitor, and lower FPS if needed. Supports 0-1600.
        $min: 0
        $max: 1600
        $format: slider
      - SplatSize: 15
        $name: Ink injection size (px)
        $min: 4
        $max: 46
        $format: slider
      - Force: 65
        $name: Mouse-motion force (%)
        $min: 0
        $max: 200
        $format: slider
      - Vorticity: 35
        $name: Swirl / vorticity strength
        $min: 0
        $max: 100
        $format: slider
      - DyeRetention: 96
        $name: Dye persistence (percent retained per 1/60 sec)
        $min: 85
        $max: 99
        $format: slider
      - VelocityRetention: 98
        $name: Flow inertia (percent retained per 1/60 sec)
        $min: 85
        $max: 100
        $format: slider
      - PressureIterations: 12
        $name: Pressure solver iterations
        $min: 4
        $max: 24
        $format: slider
      $name: Fluid physics and performance
    $name: Fluid simulation
  - Trails:
    - TrailOptions:
      - Enabled: false
        $name: Enable trails
      - Style: ribbon
        $name: Trail style
        $options:
        - ribbon: Smooth ribbon
        - dots: Glowing dots
        - afterimage: Cursor shape afterimages
        - soft: Blurred / soft ribbon
        - fluid: Liquid-like ribbon
      - Rainbow: false
        $name: Rainbow trail (independent of global RGB)
      $name: Trail type
    - Colors:
      - Color: '#8B6DFF'
        $name: Trail color
        $format: colorRgb
      $name: Trail colors
    - Dimensions:
      - LengthMS: 420
        $name: Trail lifetime (ms)
        $min: 100
        $max: 1500
        $format: slider
      - Width: 5
        $name: Trail width (px)
        $min: 1
        $max: 22
        $format: slider
      - MinimumSpacing: 3
        $name: Distance between trail samples (px)
        $min: 1
        $max: 16
        $format: slider
      $name: Trail dimensions
    $name: Trails
  - Particles:
    - Appearance:
      - Enabled: false
        $name: Particles enabled
      - Shape: spark
        $name: Particle shape
        $options:
        - spark: Smooth glowing spark
        - soft: Soft / smoke
        - dot: Circle
        - star: Star
        - diamond: Diamond
        - square: Square
        - ring: Ring
        - bubble: Bubble
        - snow: Snowflake
        - heart: Heart
        - image: Local image (PNG/BMP)
      - ImagePath: ''
        $showIf: {Shape: image}
        $name: Image file path for image particle shape
        $description: Absolute local PNG or BMP file path (max 128x128 pixels / 12 MiB)
      - ImageScale: 100
        $showIf: {Shape: image}
        $name: Particle image size scale (%)
        $min: 25
        $max: 200
        $format: slider
      $name: Particle appearance
    - Colors:
      - Color: '#C3A0FF'
        $name: Particle color
        $format: colorRgb
      - Rainbow: false
        $name: Rainbow particles (independent of global RGB)
      $name: Particle colors
    - Emission:
      - Maximum: 90
        $name: Max live particles
        $min: 0
        $max: 300
        $format: slider
      - Per100Pixels: 18
        $name: Particles / 100px traveled
        $min: 0
        $max: 100
        $format: slider
      - LifetimeMS: 700
        $name: Lifetime (ms)
        $min: 150
        $max: 2500
        $format: slider
      - Size: 3
        $name: Size (px)
        $min: 1
        $max: 16
        $format: slider
      $name: Particle counts and size
    - MotionOptions:
      - UsePhysics: false
        $name: Enable particle physics (Off means particles remain in place and fade)
      - Physics: zero
        $showIf: {UsePhysics: true}
        $name: Physics behavior
        $options:
        - gravity: Downward gravity
        - zero: Zero gravity outward
        - upward: Float upward
        - orbit: Orbit / spiral
        - swirl: Wind / turbulence
        - inertia: Momentum / drift
      - PhysicsStrength: 100
        $showIf: {UsePhysics: true}
        $name: Physics strength (0-200%)
        $min: 0
        $max: 200
        $format: slider
      - Drag: 35
        $showIf: {UsePhysics: true}
        $name: Drag / air resistance (0-100)
        $min: 0
        $max: 100
        $format: slider
      - Turbulence: 20
        $showIf: {UsePhysics: true}
        $name: Particle turbulence (0-100)
        $min: 0
        $max: 100
        $format: slider
      - Bounce: false
        $showIf: {UsePhysics: true}
        $name: Bounce off edges of monitors
      - Speed: 115
        $showIf: {UsePhysics: true}
        $name: Initial speed (px/s)
        $min: 0
        $max: 500
        $format: slider
      - Gravity: 220
        $showIf: {UsePhysics: true}
        $name: Gravity acceleration (px/s2)
        $min: 0
        $max: 800
        $format: slider
      $name: Particle physics
    $name: Particles
  - Custom:
    - Glow: ''
      $name: Custom glow formula
      $description: Optional color and opacity formula over the selected glow preset; blank preserves preset. See the custom effects guide.
    - Trail: ''
      $name: Custom trail formula
      $description: Optional color and opacity formula over the selected trail style; blank preserves preset.
    - Particles: ''
      $name: Custom particle formula
      $description: Optional color and opacity formula per particle; blank preserves preset. Custom image colors receive a partial tint.
    $name: Custom effects
  $name: Cursor effects
- Experimental:
  - WindowBorders: false
    $name: Experimental window borders
    $description: Replaces normal app border drawing while enabled. Existing app targeting, exclusions, visibility and opacity still apply. Requires app borders enabled and a nonzero width.
  - ScreenBorders: false
    $name: Experimental screen borders
    $description: Replaces normal screen border drawing while enabled. Rounded corner fill and fullscreen hiding remain active. Requires a nonzero screen border width.
  - Effect: plasma
    $name: Experimental effect
    $options:
    - plasma: Plasma ribbon (electric aurora)
    - ember: Ember frame (fire and sparks)
    - custom: Custom
  - Custom: 'q=0.3+0.12*sin(p*tau*12-t*2)+ab*0.2; a=exp(-pow((d-q)*18,2)); r=hr(p+t*0.05); g=hg(p+t*0.05); b=hb(p+t*0.05)'
    $name: Custom border field formula
    $description: Defines color and opacity across the wider edge envelope, allowing curves and displaced silhouettes. d is signed depth divided by reach; ar/ag are normalized mouse position and ab is proximity. See guide.
  - MouseInteraction: true
    $name: Mouse bending and entry ripples
  - PauseWhenIdle: false
    $name: Freeze experimental borders while idle (live mode only)
    $description: Optional still-frame hold, not animation caching. Cached loop playback in Optimization overrides this hold and keeps animating. Off renders continuously. Targeting and exclusions remain unchanged.
  - Reach: 28
    $name: Maximum effect reach (pixels)
    $min: 8
    $max: 48
    $format: slider
  - Width: 3
    $name: Plasma strand width
    $min: 1
    $max: 8
    $format: slider
  - Speed: 100
    $name: Animation speed
    $min: 0
    $max: 200
    $format: slider
  - FPS: 30
    $name: Experimental FPS ceiling
    $description: Caps the inherited Fixed/Auto/Adaptive timing at 15-60 FPS. Normal inactive hide/solid/frozen behavior is retained. Global frame blending does not apply to experimental geometry.
    $min: 15
    $max: 60
    $format: slider
  - Color1: '#AA55FF'
    $name: Color 1
    $format: colorRgb
  - Color2: '#00EEFF'
    $name: Color 2
    $format: colorRgb
  - CustomLayout: ''
    $name: Custom scene / particle count formula
    $description: Custom mode only. Runs once per target frame. Assign count (0-128) and body (0-1); use time and mouse inputs. Blank preserves the existing custom field. See the full guide in Details.
  - CustomParticles: ''
    $name: Custom particle instance formula
    $description: Custom mode only. Runs per particle. Assign pos, depth, size, stretch, rotation, offsetx, offsety and r/g/b/a using id, seed, home and count. The layout formula creates particles. Blank uses default orbiting dots. See Details.
  - FocusTransitions:
    - Enabled: false
      $name: Enable focus transitions
      $description: App borders only. Blend edge pixels when focus changes. Existing hide rules win; geometry and width stay unchanged. Large edge bands fall back to an immediate change.
    - DurationMS: 220
      $name: Transition duration (ms)
      $min: 50
      $max: 800
      $format: slider
    - InactiveBrightness: 65
      $name: Unfocused brightness (%)
      $min: 10
      $max: 100
      $format: slider
    $name: Focus transitions
  - ClickShockwaves:
    - Enabled: false
      $name: Enable click shockwaves
      $description: Expanding left/right click rings using the cursor particle layer. Requires Cursor effects master enabled. No mouse hook; very short clicks between polls can be missed.
    - DurationMS: 550
      $name: Ring lifetime (ms)
      $min: 150
      $max: 1500
      $format: slider
    - Radius: 80
      $name: Maximum ring radius (px)
      $min: 20
      $max: 160
      $format: slider
    - Width: 3
      $name: Ring thickness (px)
      $min: 1
      $max: 8
      $format: slider
    - Opacity: 75
      $name: Ring opacity (%)
      $min: 1
      $max: 100
      $format: slider
    - LeftColor: '#AA55FF'
      $name: Left button color
      $format: colorRgb
    - RightColor: '#00EEFF'
      $name: Right button color
      $format: colorRgb
    $name: Click shockwaves
  - CometTrail:
    - Enabled: false
      $name: Enable comet cursor trail
      $description: Shrinking tinted cursor silhouettes. Overrides the normal trail style while enabled and enables its sampler. Requires Cursor effects master enabled. Existing lifetime and spacing still apply.
    - MaximumCopies: 16
      $name: Maximum cursor copies
      $min: 4
      $max: 32
      $format: slider
    - TailScale: 25
      $name: Oldest copy size (%)
      $min: 10
      $max: 90
      $format: slider
    - Opacity: 65
      $name: Comet opacity (%)
      $min: 1
      $max: 100
      $format: slider
    $name: Comet cursor trail
  - GlassShimmer:
    - Enabled: false
      $name: Enable glass shimmer borders
      $description: Moving white reflections and a highlight near the mouse. Adds to existing classic or experimental border pixels; does not turn an invisible or excluded border on.
    - AppBorders: true
      $name: Apply to app borders
    - ScreenBorders: true
      $name: Apply to monitor borders
    - Strength: 55
      $name: Reflection strength (%)
      $min: 1
      $max: 100
      $format: slider
    - Speed: 15
      $name: Travel speed (%)
      $min: 1
      $max: 100
      $format: slider
    - Width: 8
      $name: Highlight width (% of perimeter)
      $min: 2
      $max: 25
      $format: slider
    - MouseHighlight: 50
      $name: Mouse reflection strength (%)
      $min: 0
      $max: 100
      $format: slider
    $name: Glass shimmer borders
  - MagneticParticles:
    - Enabled: false
      $name: Enable magnetic border particles
      $description: Requires experimental window or screen borders with particles. Pulls procedural/custom particle positions along the contour near the mouse. Does not move windows or the pointer.
    - Strength: 60
      $name: Attraction strength (%)
      $min: 0
      $max: 90
      $format: slider
    - Radius: 120
      $name: Attraction radius (px)
      $min: 32
      $max: 240
      $format: slider
    $name: Magnetic border particles
  - LinkedPalettes:
    - Enabled: false
      $name: Enable linked palettes
      $description: Share these two colors across selected renderers. Keeps existing alpha and brightness. Overrides rendered hue including custom/rainbow colors; icons and caption buttons are not included.
    - Color1: '#AA55FF'
      $name: Shared color 1
      $format: colorRgb
    - Color2: '#00EEFF'
      $name: Shared color 2
      $format: colorRgb
    - AppBorders: true
      $name: Link app borders
    - ScreenBorders: true
      $name: Link monitor borders
    - CursorEffects: true
      $name: Link cursor effects
    $name: Linked palettes
  $name: Experimental
- Rendering:
  - Backend: cpu
    $name: Rendering backend
    $description: Experimental Direct3D 11 evaluates eligible border/icon formulas and cursor-layer blending on the hardware GPU. Geometry, music/weather post-processing and GDI presentation remain CPU-based. Readback overhead can outweigh gains. CPU mode hides and ignores all GPU controls, preserving their saved values. Errors and unsupported work fall back to CPU; see Details.
    $options:
    - cpu: CPU / GDI (default)
    - d3d11: Direct3D 11 compute + GDI (experimental)
  - AppBorders: true
    $showIf: {Backend: d3d11}
    $name: Use GPU for app border formulas
    $description: Applies only when Direct3D 11 is selected above. Normal formulas and eligible experimental custom fields can use GPU; particle layout/composition retain their existing renderer.
  - ScreenBorders: true
    $showIf: {Backend: d3d11}
    $name: Use GPU for screen border formulas
  - ExperimentalFields: true
    $showIf: {Backend: d3d11}
    $name: Use GPU for experimental custom border fields
    $description: Custom field formulas only. Preserves fallback colors, pointer inputs and particle composition. Uniform formulas stay on the cheaper CPU path. Built-in experimental presets and particle/layout formulas remain CPU-based.
  - DesktopIcons: true
    $showIf: {Backend: d3d11}
    $name: Use GPU for desktop icon formulas
    $description: Formula evaluation only. Grayscale, colorize, alpha, shadows, glow and icon presentation retain their existing CPU paths.
  - TaskbarIcons: true
    $showIf: {Backend: d3d11}
    $name: Use GPU for taskbar icon formulas
    $description: Formula evaluation only; retains shared icon coordinates and animation timing.
  - StartMenuIcons: true
    $showIf: {Backend: d3d11}
    $name: Use GPU for Start menu icon formulas
    $description: Formula evaluation for bitmap app icons discovered inside the Start menu. Unsupported image sources retain native rendering.
  - ExplorerIcons: true
    $showIf: {Backend: d3d11}
    $name: Use GPU for Explorer icon formulas
    $description: Formula evaluation for captured Explorer file/folder icons and, when enabled, navigation-pane icons. Unsupported draw paths retain native rendering.
  - CursorTrail: true
    $showIf: {Backend: d3d11}
    $name: Use GPU for cursor trail blending
    $description: Interpolates and composites the trail layer on GPU, including cursor copies. Trail generation and custom colors remain CPU-based.
  - CursorParticles: true
    $showIf: {Backend: d3d11}
    $name: Use GPU for cursor particle blending
    $description: Interpolates and composites the particle layer and click rings on GPU. Particle physics, drawing and custom colors remain CPU-based.
  - CursorGlow: true
    $showIf: {Backend: d3d11}
    $name: Use GPU for cursor glow blending
    $description: Interpolates and composites the glow layer on GPU, including the separate glow window. Glow drawing and custom colors remain CPU-based.
  - CursorFluid: true
    $showIf: {Backend: d3d11}
    $name: Use GPU for cursor fluid blending
    $description: Interpolates and composites the fluid layer on GPU. Fluid simulation and source drawing remain CPU-based.
  - MinimumPixels: 1024
    $showIf: {Backend: d3d11}
    $name: Minimum pixels per GPU dispatch
    $description: Smaller workloads stay on CPU to avoid transfer overhead. Counts border edge pixels, source icon pixels or the clipped cursor layer rectangle, depending on the stage.
    $min: 256
    $max: 8192
    $format: slider
  - ReadbackBudgetMS: 2
    $showIf: {Backend: d3d11}
    $name: GPU completion polling budget (ms)
    $description: CPU fallback if results are not ready within this polling budget. Three consecutive readback misses suspend GPU attempts in that worker until settings reload. Driver calls themselves are not time-bounded.
    $min: 1
    $max: 4
    $format: slider
  $name: GPU rendering (experimental)
- Optimization:
  - Enabled: true
    $name: Enable optional optimizations
    $description: Master switch for the options below. Off uses the existing live render paths and existing timing settings. Safety checks, targeting and exclusions remain mandatory.
  - Experimental:
    - FastFormulas: true
      $name: Compile experimental formulas for faster live rendering
      $description: Reuse identical expressions and calculate frame-wide values once. Preserves the math and lazy conditionals; no resolution/FPS reduction. Off uses the original evaluator.
    - AnimationCache: live
      $name: Experimental animation cache
      $description: Loop playback stores compressed edge frames and keeps animation moving. Live preserves original timing. Replay works with Plasma, Ember and custom layout/particle/field formulas; interactive frames bypass it.
      $options:
      - live: Live rendering (original timing)
      - loop: Cached loop playback
    - MemoryMiB: m16
      $name: Animation cache memory budget per render worker
      $description: Hard budget for frame/base pixel vector capacities; metadata adds a small bounded overhead. No disk cache. Explorer and screen overlay workers have separate budgets. Uncached phases render live.
      $options:
      - m8: 8 MiB
      - m16: 16 MiB
      - m32: 32 MiB
      - m64: 64 MiB
    - LoopSeconds: s4
      $name: Cached animation loop length
      $description: Replay time wraps at this duration. Author custom formulas to close their loop. Arbitrary effects may visibly repeat or morph at the join.
      $options:
      - s2: 2 seconds
      - s4: 4 seconds
      - s8: 8 seconds
    - CacheFPS: f30
      $name: Cached animation source FPS
      $description: Maximum 240 phases per loop. Does not raise the existing render FPS. Lower rates reduce cache size; optional interpolation blends intermediate frames.
      $options:
      - f15: 15 FPS
      - f30: 30 FPS
      - f60: 60 FPS
    - Interpolate: true
      $name: Interpolate cached frames
      $description: Blend two available adjacent cached phases, including the loop join. Uses extra CPU and can ghost fast particles. Does not calculate missing future frames automatically.
    - PreRenderAhead: false
      $name: Gradually pre-render the next cache frame
      $description: Optional extra work during warm-up. Builds at most one future phase after a cheap draw, then restores the current frame. Never blocks to build the entire loop.
    - WarmBudgetMS: b2
      $name: Pre-render scheduling budget
      $description: Starts an ahead frame only when measured draw cost suggests it fits this budget. Cooperative estimate, not a hard execution-time deadline.
      $options:
      - b1: 1 ms
      - b2: 2 ms
      - b4: 4 ms
    - Targets: t8
      $name: Maximum cached geometries per worker
      $description: App sizes and individual screen tiles count separately. Existing entries are retained until expiry when full; increase carefully for multi-monitor screen borders.
      $options:
      - t1: 1 geometry
      - t4: 4 geometries
      - t8: 8 geometries
      - t16: 16 geometries
    - ExpirySeconds: e15
      $name: Release unused animation caches after
      $description: Pruned on subsequent renderer calls. Hiding all targets does not start a separate cleanup timer; worker shutdown releases the entire cache.
      $options:
      - e5: 5 seconds
      - e15: 15 seconds
      - e30: 30 seconds
    $name: Experimental animation playback
  - Cursor:
    - LeanHistory: true
      $name: Store one cursor frame when interpolation is bypassed
      $description: Avoid previous-frame buffers and copies for layers that currently do not blend. Keeps two frames where cursor smoothing needs them.
    - SkipDuplicateUploads: true
      $name: Skip identical cursor uploads
      $description: Compare final pixels before uploading. Saves unchanged uploads but requires a comparison buffer and scan; Off can help constantly changing large fluid scenes.
    - CacheArtwork: true
      $name: Reuse unchanged native cursor artwork
      $description: Compare source pixels before rebuilding the outline distance field. Periodic source checks still detect animated artwork and DPI changes.
    - IdlePollingHz: h30
      $name: Idle cursor input polling
      $description: Lower values save wake-ups but delay motion detection. Active simulation and interpolation deadlines can wake the worker sooner.
      $options:
      - h15: 15 Hz (up to about 67 ms wake delay)
      - h30: 30 Hz
      - h60: 60 Hz
    - ArtworkPolling: balanced
      $name: Native artwork refresh policy
      $options:
      - balanced: Balanced (0.33 seconds moving / 0.85 idle)
      - economy: Economy (0.5 seconds moving / 1.5 idle)
      - responsive: Responsive (0.1 seconds moving / 0.5 idle)
    - WallPolling: balanced
      $name: Cursor collision window scan policy
      $description: Only applies when cursor window collisions are enabled. Slower scans can delay reaction to window changes.
      $options:
      - balanced: Balanced (75 ms moving / 250 idle)
      - economy: Economy (150 ms moving / 500 idle)
      - responsive: Responsive (33 ms moving / 100 idle)
    - FluidSampling: q1
      $name: Cursor fluid raster sampling
      $description: Lower sampling density uses two small reusable rows and bilinear interpolation. Full preserves original pixels; reduced modes may soften thin ink details.
      $options:
      - q1: Full density
      - q2: Every 2 pixels
      - q4: Every 4 pixels
    - SolverCeiling: keep
      $name: Extra fluid solver FPS ceiling
      $description: Caps the existing fluid solver cadence; does not change grid resolution or the independent/follow/adaptive policy.
      $options:
      - keep: Use existing settings
      - c30: Maximum 30 FPS
      - c45: Maximum 45 FPS
      - c60: Maximum 60 FPS
    $name: Cursor rendering and memory
  - General:
    - AnimationCeiling: keep
      $name: Extra effect calculation FPS ceiling
      $description: Caps icon, cursor, classic and experimental border source calculations. Existing smoothing may still present at 60 FPS. Does not cap games or other apps.
      $options:
      - keep: Use existing settings
      - c15: Maximum 15 FPS
      - c30: Maximum 30 FPS
      - c60: Maximum 60 FPS
      - c120: Maximum 120 FPS
    - MaintenanceSeconds: s2
      $name: App-border fallback rescan interval
      $description: Window events still react promptly; slower fallback scans delay missed-event recovery and exclusion-list refresh.
      $options:
      - s2: 2 seconds
      - s5: 5 seconds
      - s10: 10 seconds
    - IconCacheEntries: n1024
      $name: Desktop icon cache entry limit
      $description: Lower limits reduce retained icon cache entries but can cause more cache rebuilds. This is an entry count, not a byte limit. Taskbar caches retain their separate bounded implementation.
      $options:
      - n128: 128 entries
      - n256: 256 entries
      - n512: 512 entries
      - n1024: 1024 entries
    - FastClassicFormulas: true
      $name: Compile classic border formulas for faster rendering
      $description: Reuse expression results and frame-wide values in classic app and screen borders. Preserves lazy conditionals and sampled Cinema/Ambient inputs. Off uses the original evaluator; oversized compiled plans fall back automatically. Does not change icon/cursor formulas.
    - FramePreparation: true
      $name: Prepare weather and music values once per frame
      $description: Reuses frame-wide weather motion and music gradient/tint values. Keeps full-resolution rendering and the existing formulas. Off uses the original per-pixel preparation.
    - TiledAppBorders: false
      $name: Use narrow app border presentation tiles (experimental)
      $description: Submit four non-overlapping edge surfaces instead of the transparent full-window rectangle. Keeps the same rendered pixels and capture setting. More overlay windows and API calls; small windows may not benefit. Falls back to the original surface if tile creation or presentation fails. Off by default pending Windows testing.
    - TimingDiagnostics: false
      $name: Log render timing diagnostics
      $description: Optional per-worker averages and maximum milliseconds over ten-second windows in the mod log. Includes separate border shading, presentation, cursor source and composition stages. Nested stages overlap and must not be added. No disk trace files; disabled by default.
    - BelowNormalWorkers: false
      $name: Lower background render worker priority
      $description: Runs app-border, screen-overlay and cursor render workers below normal priority. Can improve app responsiveness under CPU load but may lower effect FPS. Does not change Explorer UI thread priority.
    $name: Shared project limits
  $name: Optimization
- ResponsiveEffectTracking: true
  $name: Responsive window and cursor effect tracking
  $description: Reposition cached app borders on window events, with temporary 16 ms move/resize checks. Cursor-local glow follows at at least 60 position checks per second without increasing effect calculation FPS. Preserves intentional trails, fluid motion, exclusions and existing border stacking. Separate overlay windows cannot guarantee perfect lockstep under load.
- SmoothAnimatedEffects: false
  $name: Smooth animated effects (60 FPS presentation)
  $description: Blend intermediate frames for animated desktop/taskbar/Start menu/File Explorer icons and app/screen border effects, including Cinema, Mercury, music and weather. Effects below 60 FPS keep their calculation rate and present at 60 FPS. Adds up to one effect-frame of delay, extra memory and presentation work; fast motion can show blending trails. Static settings, selection/motion geometry and timed Ambient holds are unchanged. Cursor and experimental effects have separate timing controls.
- ShowOverlaysInCapture: false
  $name: Show overlays in screenshots and screen sharing
  $description: 'Off: hide app borders, screen borders, monitor corners and motion trails from supported captures. On: allow them to appear in display captures; window-only sharing may still omit them. Does not hide desktop or taskbar icons. Cursor capture visibility is controlled in Cursor effects. Cinema and screen-edge Ambient may sample the overlays when On.'
*/
// ==/WindhawkModSettings==

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <winhttp.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include <mmsystem.h>
#include <shlwapi.h>
#include <windowsx.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <windhawk_utils.h>
#include <shcore.h>
#include <wincodec.h>
#include <shlobj.h>
#include <commoncontrols.h>
#include <xamlom.h>
#include <ocidl.h>
#ifdef GetCurrentTime
#undef GetCurrentTime
#endif
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <atomic>
#include <deque>
#include <cstdint>
#include <utility>
#include <objbase.h>
#ifndef WDA_EXCLUDEFROMCAPTURE
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
#endif
#include <memory>
#include <new>
#include <mutex>
#include <vector>
#include <map>
#include <set>
#include <tuple>
#include <string>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <shellapi.h>

// System output peak meter: no loopback capture, recording, or audio buffer access.
// This runs once in each supported host; renderers only read atomic values.
// Minimal IAudioMeterInformation ABI declaration. Some Windhawk SDK headers
// only forward-declare this interface; the four methods follow IUnknown.
struct DodiAudioMeter : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetPeakValue(float* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMeteringChannelCount(UINT* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelsPeakValues(UINT count, float* values) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryHardwareSupport(DWORD* mask) = 0;
};

// IAudioMeterInformation IID, passed explicitly because MinGW Clang does not
// provide __mingw_uuidof for this locally declared COM interface.
static const GUID kDodiAudioMeterIID = {0xC02216F6,0x8C67,0x4B5B,{0x9D,0x00,0xD0,0x08,0xE7,0x3E,0x00,0x64}};

// Optional frame blending. Shader/sampling rates remain independent of presentation.
namespace EffectSmoothing {
static std::atomic<bool> enabled{false};
static void Settings() { enabled.store(Wh_GetIntSetting(L"SmoothAnimatedEffects") != 0); }
static bool Use(bool animated, int rate) { return enabled.load() && animated && rate < 60; }
struct Frame {
    std::vector<BYTE> previous, current;
    double stamp = 0, period = 1.0 / 30.0;
    bool ready = false, pending = false;
    void Reset() {
        std::vector<BYTE>().swap(previous); std::vector<BYTE>().swap(current);
        ready = pending = false;
    }
    size_t Bytes() const { return previous.capacity() + current.capacity(); }
    bool Prepare(size_t bytes, double now, double interval, bool& first) {
        // Bound optional buffers; oversized effects retain their native rendering.
        if (!bytes || bytes > 8u * 1024u * 1024u) { Reset(); return false; }
        first = !ready || current.size() != bytes || now < stamp;
        try {
            if (first) { previous.resize(bytes); current.resize(bytes); }
            else { previous.swap(current); current.resize(bytes); }
        } catch (const std::bad_alloc&) { Reset(); return false; }
        stamp = now; period = std::max(1.0 / 1000.0, std::min(1.0, interval));
        ready = true; pending = !first;
        return true;
    }
    bool Push(const BYTE* pixels, size_t bytes, double now, double interval) {
        bool first;
        if (!Prepare(bytes, now, interval, first)) return false;
        memcpy(current.data(), pixels, bytes);
        if (first) memcpy(previous.data(), pixels, bytes);
        return true;
    }
    template<class Pixel> bool PushBand(const DWORD* pixels, const std::vector<Pixel>& band,
                                        double now, double interval) {
        bool first;
        if (!Prepare(band.size() * 4, now, interval, first)) return false;
        for (size_t i = 0; i < band.size(); ++i)
            memcpy(current.data() + i * 4, pixels + band[i].idx, 4);
        if (first) memcpy(previous.data(), current.data(), current.size());
        return true;
    }
    int Weight(double now) {
        double alpha = pending ? std::max(0.0, std::min(1.0, (now - stamp) / period)) : 1.0;
        if (alpha >= 1) pending = false;
        return (int)(alpha * 256 + 0.5);
    }
    void Blend(BYTE* pixels, size_t stride, size_t rowBytes, int rows, double now) {
        int weight = Weight(now);
        for (int y = 0; y < rows; ++y) for (size_t x = 0; x < rowBytes; ++x) {
            size_t i = (size_t)y * rowBytes + x;
            pixels[(size_t)y * stride + x] = (BYTE)((previous[i] * (256 - weight) + current[i] * weight + 128) >> 8);
        }
    }
    template<class Pixel> void BlendBand(DWORD* pixels, const std::vector<Pixel>& band, double now) {
        int weight = Weight(now);
        for (size_t i = 0; i < band.size(); ++i) {
            BYTE* dst = reinterpret_cast<BYTE*>(pixels + band[i].idx);
            for (size_t k = 0; k < 4; ++k) {
                size_t j = i * 4 + k;
                dst[k] = (BYTE)((previous[j] * (256 - weight) + current[j] * weight + 128) >> 8);
            }
        }
    }
};
}

// Shared Cinema sampling policy. All clocks are owned by their overlay worker.
namespace CinemaSampling {
static int FixedRate(const std::wstring& mode) {
    static const int rates[] = {1,2,5,10,15,20,24,30,45,60,90,120,144,165,180,200,240};
    for (int rate : rates) if (mode == L"r" + std::to_wstring(rate)) return rate;
    return 10;
}
static int Rate(const std::wstring& mode, int fixed, int refresh, bool active) {
    int display = std::max(1, std::min(240, refresh));
    if (mode == L"auto") return display;
    if (mode == L"adaptive") return active ? display : 5;
    if (mode == L"adaptiveLite") return active ? std::min(60, display) : 2;
    return std::max(1, std::min(240, fixed));
}
struct Clock {
    double last = -1, next = 0, activeUntil = 0;
    int scheduledRate = 0;
    bool Active(double now, bool windowActivity) const {
        return windowActivity || now < activeUntil;
    }
    double Deadline(double now, int rate) const {
        if (last < 0) return now;
        if (rate != scheduledRate) return last + 1.0 / std::max(1, rate);
        return next;
    }
    bool Due(double now, int rate) const { return now >= Deadline(now, rate); }
    void Sampled(double now, int rate, bool changed) {
        double period = 1.0 / std::max(1, rate);
        double due = Deadline(now, rate);
        last = now; scheduledRate = rate;
        // Preserve fractional deadlines; skip missed intervals without catch-up bursts.
        next = (due > now - period ? due : now) + period;
        if (changed) activeUntil = now + 0.5;
    }
};
static DWORD WaitMs(double delay) {
    if (delay <= 0) return 0;
    return (DWORD)std::min(1000.0, ceil(delay * 1000.0));
}
}

// One root-level setting shared by both hosts; apply only to our own overlay windows.
namespace CaptureVisibility {
static std::atomic<bool> visible{false};
static void Settings() {
    visible.store(Wh_GetIntSetting(L"ShowOverlaysInCapture") != 0);
}
static void Apply(HWND window) {
    if (!window) return;
    // WDA_NONE = 0; WDA_EXCLUDEFROMCAPTURE = 0x11 (Windows 10 2004+).
    DWORD affinity = visible.load() ? 0u : 0x11u;
    if (!SetWindowDisplayAffinity(window, affinity))
        Wh_Log(L"Overlay capture visibility could not be applied (error %lu)", GetLastError());
}
}

namespace TimeCycle {
// Approximate solar elevation using the NOAA fractional-year model. UTC + longitude,
// not the local clock hour, keeps the cycle aligned with real sunrise and sunset.
static std::atomic<int> latitude{0}, longitude{0}, dayBrightness{100}, nightBrightness{20};
static std::atomic<int> twilightRange{8}, tintStrength{35};
static std::atomic<bool> tintEnabled{true};
static std::atomic<unsigned int> nightRgb{0x648ACD}, twilightRgb{0xFFAD61};
static unsigned int ParseRgb(PCWSTR text, unsigned int fallback) {
    if (!text) return fallback;
    if (*text==L'#') ++text;
    if (wcslen(text)!=6) return fallback;
    unsigned int result=0;
    for(int i=0;i<6;i++) {
        wchar_t c=text[i];
        unsigned int d=c>=L'0'&&c<=L'9'?c-L'0':c>=L'a'&&c<=L'f'?c-L'a'+10:c>=L'A'&&c<=L'F'?c-L'A'+10:16;
        if(d>15) return fallback;
        result=(result<<4)|d;
    }
    return result;
}
static void Settings() {
    latitude.store(Wh_GetIntSetting(L"TimeCycle.Latitude"));
    longitude.store(Wh_GetIntSetting(L"TimeCycle.Longitude"));
    dayBrightness.store(Wh_GetIntSetting(L"TimeCycle.DayBrightness"));
    nightBrightness.store(Wh_GetIntSetting(L"TimeCycle.NightBrightness"));
    twilightRange.store(Wh_GetIntSetting(L"TimeCycle.TwilightRange"));
    tintEnabled.store(Wh_GetIntSetting(L"TimeCycle.TintEnabled")!=0);
    tintStrength.store(Wh_GetIntSetting(L"TimeCycle.ColorStrength"));
    PCWSTR c=Wh_GetStringSetting(L"TimeCycle.NightColor");
    nightRgb.store(ParseRgb(c,0x648ACD));Wh_FreeStringSetting(c);
    c=Wh_GetStringSetting(L"TimeCycle.TwilightColor");
    twilightRgb.store(ParseRgb(c,0xFFAD61));Wh_FreeStringSetting(c);
}
struct State {float brightness, night, twilight;};
static State Get() {
    SYSTEMTIME utc;GetSystemTime(&utc);
    // Gregorian day of year, leap-year aware.
    static const int monthDays[12]={31,28,31,30,31,30,31,31,30,31,30,31};
    int day=utc.wDay;
    for(int m=0;m<(int)utc.wMonth-1 && m<12;m++) day+=monthDays[m];
    if(utc.wMonth>2 && (utc.wYear%4==0 && (utc.wYear%100!=0 || utc.wYear%400==0)))day++;
    const double pi=3.14159265358979323846;
    double hour=utc.wHour+utc.wMinute/60.0+utc.wSecond/3600.0;
    double gamma=2*pi/365.0*(day-1+(hour-12)/24.0);
    double eq=229.18*(0.000075+0.001868*cos(gamma)-0.032077*sin(gamma)-0.014615*cos(2*gamma)-0.040849*sin(2*gamma));
    double decl=0.006918-0.399912*cos(gamma)+0.070257*sin(gamma)-0.006758*cos(2*gamma)+0.000907*sin(2*gamma)-0.002697*cos(3*gamma)+0.00148*sin(3*gamma);
    double lat=std::max(-90,std::min(90,latitude.load()))*pi/180.0;
    double minutes=hour*60+eq+4*std::max(-180,std::min(180,longitude.load()));
    double ha=(minutes/4.0-180)*pi/180.0;
    double sine=sin(lat)*sin(decl)+cos(lat)*cos(decl)*cos(ha);
    sine=std::max(-1.0,std::min(1.0,sine));
    float elevation=(float)(asin(sine)*180/pi);
    float range=(float)std::max(1,std::min(20,twilightRange.load()));
    float x=std::max(0.f,std::min(1.f,(elevation+range)/(2*range)));
    float daylight=x*x*(3-2*x);
    float dusk=1.f-fabsf(elevation)/range;
    dusk=std::max(0.f,std::min(1.f,dusk));
    float brightness=(nightBrightness.load()+(dayBrightness.load()-nightBrightness.load())*daylight)/100.f;
    return {std::max(0.f,std::min(1.f,brightness)),1.f-daylight,dusk};
}
static void Tint(float& r,float& g,float& b,const State& s) {
    if(!tintEnabled.load())return;
    unsigned int n=nightRgb.load(),t=twilightRgb.load();
    float strength=std::max(0.f,std::min(1.f,tintStrength.load()/100.f));
    float nw=s.night*(1-s.twilight)*strength,tw=s.twilight*strength;
    float keep=std::max(0.f,1.f-nw-tw);
    r=r*keep+nw*((n>>16)&255)/255.f+tw*((t>>16)&255)/255.f;
    g=g*keep+nw*((n>>8)&255)/255.f+tw*((t>>8)&255)/255.f;
    b=b*keep+nw*(n&255)/255.f+tw*(t&255)/255.f;
}
}

namespace RenderShutdown { static std::atomic<bool> requested{false}; }

namespace LiveWeather {
// Open-Meteo current weather codes (WMO); only weather code is requested.
// Never perform network I/O on the border render thread.
static std::atomic<int> code{-1}, intensity{80}, speed{50}, softness{65}, refresh{15};
static std::atomic<unsigned> rain{0x258DD9}, cloud{0xF0F5FF}, sun{0xFFD36A}, snow{0xD5EFFF};
static std::atomic<bool> enabled{true}, stop{false};
static HANDLE thread=nullptr, wake=nullptr;
static void RequestStop(){stop.store(true);if(wake)SetEvent(wake);}
static void Settings(){
 enabled.store(Wh_GetIntSetting(L"TimeCycle.WeatherEnabled")!=0);
 intensity.store(Wh_GetIntSetting(L"TimeCycle.WeatherIntensity"));
 speed.store(Wh_GetIntSetting(L"TimeCycle.WeatherSpeed"));
 softness.store(Wh_GetIntSetting(L"TimeCycle.WeatherSoftness"));
 refresh.store(std::max(5,std::min(60,Wh_GetIntSetting(L"TimeCycle.WeatherRefresh"))));
 struct Color {const wchar_t* name;std::atomic<unsigned>* dst;unsigned fallback;};
 Color colors[]={{L"TimeCycle.RainColor",&rain,0x258DD9},{L"TimeCycle.CloudColor",&cloud,0xF0F5FF},{L"TimeCycle.SunnyColor",&sun,0xFFD36A},{L"TimeCycle.SnowColor",&snow,0xD5EFFF}};
 for(auto& c:colors){PCWSTR str=Wh_GetStringSetting(c.name);c.dst->store(TimeCycle::ParseRgb(str,c.fallback));Wh_FreeStringSetting(str);}
}
static bool Fetch(){
 if(stop.load())return false;
 int lat=std::max(-90,std::min(90,TimeCycle::latitude.load()));
 int lon=std::max(-180,std::min(180,TimeCycle::longitude.load()));
 wchar_t path[300];
 swprintf(path,300,L"/v1/forecast?latitude=%d&longitude=%d&current=weather_code&forecast_days=1",lat,lon);
 HINTERNET session=WinHttpOpen(L"DodiWindhawkWeather/1.0",WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,WINHTTP_NO_PROXY_NAME,WINHTTP_NO_PROXY_BYPASS,0);
 if(!session)return false;
 WinHttpSetTimeouts(session,2000,2000,2000,2000);
 HINTERNET conn=WinHttpConnect(session,L"api.open-meteo.com",INTERNET_DEFAULT_HTTPS_PORT,0);
 HINTERNET req=conn&&!stop.load()?WinHttpOpenRequest(conn,L"GET",path,nullptr,WINHTTP_NO_REFERER,WINHTTP_DEFAULT_ACCEPT_TYPES,WINHTTP_FLAG_SECURE):nullptr;
 bool ok=false;
 if(req&&!stop.load()&&WinHttpSendRequest(req,WINHTTP_NO_ADDITIONAL_HEADERS,0,WINHTTP_NO_REQUEST_DATA,0,0,0)&&!stop.load()&&WinHttpReceiveResponse(req,nullptr)&&!stop.load()){
  DWORD status=0,sz=sizeof(status);
  if(WinHttpQueryHeaders(req,WINHTTP_QUERY_STATUS_CODE|WINHTTP_QUERY_FLAG_NUMBER,WINHTTP_HEADER_NAME_BY_INDEX,&status,&sz,WINHTTP_NO_HEADER_INDEX)&&status==200){
   std::string data;char buf[2048];DWORD got=0;
   while(!stop.load() && data.size()<16384 && WinHttpReadData(req,buf,sizeof(buf),&got)&&got){data.append(buf,got);}
   size_t at=data.find("\"weather_code\"");
   if(at!=std::string::npos){at=data.find(':',at);if(at!=std::string::npos){int n=atoi(data.c_str()+at+1);if(n>=0&&n<=99){code.store(n);ok=true;Wh_Log(L"Live weather: updated WMO code %d",n);}}}
  }
 }
 if(req)WinHttpCloseHandle(req);if(conn)WinHttpCloseHandle(conn);WinHttpCloseHandle(session);
 return ok;
}
static DWORD WINAPI Worker(void*){
 // Wait a moment after initialization, then fetch and poll. Avoid network work if disabled.
 DWORD elapsed=3600000;
 while(!stop.load()){
  if(enabled.load() && elapsed>=DWORD(refresh.load())*60000){if(!Fetch())Wh_Log(L"Live weather: update failed; keeping previous condition");elapsed=0;}
  if(WaitForSingleObject(wake,1000)!=WAIT_TIMEOUT)break;elapsed+=1000;
 }
 return 0;
}
static void Start(){
 Settings();if(thread)return;stop.store(false);wake=CreateEventW(nullptr,TRUE,FALSE,nullptr);
 if(wake)thread=CreateThread(nullptr,0,Worker,nullptr,0,nullptr);
 if(!thread&&wake){CloseHandle(wake);wake=nullptr;}
}
static void Stop(){RequestStop();if(thread){WaitForSingleObject(thread,INFINITE);CloseHandle(thread);thread=nullptr;}if(wake){CloseHandle(wake);wake=nullptr;}}
static float Clamp(float x){return std::max(0.f,std::min(1.f,x));}
static float Blob(float x,float y,float cx,float cy,float sx,float sy){float a=(x-cx)/sx,b=(y-cy)/sy;return expf(-2.5f*(a*a+b*b));}
static float Hash(float n){return sinf(n*127.1f)*43758.5453f-floorf(sinf(n*127.1f)*43758.5453f);}
// Adds weather-specific animated color and opacity to the existing border shader.
// x/y are normalized screen/window coordinates; no network calls or allocations.
static void Apply(float x,float y,float t,float& r,float& g,float& b,float& a){
 if(!enabled.load())return;
 int w=code.load();if(w<0)return;
 float amp=Clamp(intensity.load()/100.f),sp=0.2f+speed.load()/35.f;
 float soft=0.04f+softness.load()/180.f;
 unsigned rgb=sun.load();float field=0;
 if(w>=51 && w<=67 || w>=80 && w<=82 || w>=95){
  rgb=rain.load();
  // Smooth falling rain streaks, multiple lanes and different blue shades.
  for(int i=0;i<8;i++){
   float lane=Hash(float(i)+1.2f),phase=Hash(float(i)+18.3f);
   float yy=fmodf(y*0.8f-t*sp*(0.12f+Hash(float(i)+9.f)*0.12f)+phase+1000.f,1.f);
   float d=fabsf(x-lane);float width=0.007f+soft*0.035f;
   float streak=expf(-d*d/(width*width))*expf(-powf((yy-0.5f)/(.10f+soft*.35f),2.f));
   field+=streak*(0.4f+0.6f*Hash(float(i)+4.1f));
  }
  field=Clamp(field*1.8f);
 }else if(w>=71&&w<=77 || w>=85&&w<=86){
  rgb=snow.load();
  for(int i=0;i<8;i++){
   float xx=Hash(float(i)+2.f)+0.03f*sinf(t*.5f+i);
   float yy=fmodf(Hash(float(i)+12.f)+t*sp*.025f*(.5f+Hash(float(i)+33.f)),1.f);
   field+=Blob(x,y,xx,yy,.016f+soft*.035f,.025f+soft*.04f);
  }field=Clamp(field);
 }else if(w==1||w==2||w==3||w==45||w==48){
  rgb=cloud.load();
  // Slow white cloud banks drift and change size near the window sides.
  for(int i=0;i<5;i++){
   float cy=.12f+.19f*i+.06f*sinf(t*.13f*sp+i);
   float cx=(i%2?1.f:0.f)+.05f*sinf(t*.18f*sp+i*2.f);
   float radius=.13f+soft*.5f+.045f*sinf(t*.21f*sp+i);
   field+=Blob(x,y,cx,cy,radius,.12f+soft*.25f);
  }field=Clamp(field*.75f);
 }else{
  rgb=sun.load();
  // Warm golden light moving slowly around the perimeter in clear weather.
  float phase=t*.08f*sp;
  field=.32f+.34f*(.5f+.5f*sinf(phase+x*6.28f+y*3.14f));
 }
 float blend=Clamp(field*amp);
 r=r*(1-blend)+((rgb>>16)&255)/255.f*blend;
 g=g*(1-blend)+((rgb>>8)&255)/255.f*blend;
 b=b*(1-blend)+(rgb&255)/255.f*blend;
 a*=Clamp(.35f+.65f*field*amp+(1-amp)*.65f);
}
struct Frame {
    bool active=false;int w=-1;float amp=0,sp=0,soft=0;unsigned rgb=0;
    float lane[8]{},phase[8]{},rate[8]{},weight[8]{},xx[8]{},yy[8]{},cx[5]{},cy[5]{},radius[5]{};
    explicit Frame(float t,bool wanted=true){
        active=wanted&&enabled.load();w=code.load();if(!active||w<0){active=false;return;}
        amp=Clamp(intensity.load()/100.f);sp=.2f+speed.load()/35.f;soft=.04f+softness.load()/180.f;
        rgb=sun.load();
        if(w>=51&&w<=67||w>=80&&w<=82||w>=95){rgb=rain.load();for(int i=0;i<8;i++){
            lane[i]=Hash(float(i)+1.2f);phase[i]=Hash(float(i)+18.3f);
            rate[i]=t*sp*(.12f+Hash(float(i)+9.f)*.12f);weight[i]=.4f+.6f*Hash(float(i)+4.1f);}}
        else if(w>=71&&w<=77||w>=85&&w<=86){rgb=snow.load();for(int i=0;i<8;i++){
            xx[i]=Hash(float(i)+2.f)+.03f*sinf(t*.5f+i);
            yy[i]=fmodf(Hash(float(i)+12.f)+t*sp*.025f*(.5f+Hash(float(i)+33.f)),1.f);}}
        else if(w==1||w==2||w==3||w==45||w==48){rgb=cloud.load();for(int i=0;i<5;i++){
            cy[i]=.12f+.19f*i+.06f*sinf(t*.13f*sp+i);
            cx[i]=(i%2?1.f:0.f)+.05f*sinf(t*.18f*sp+i*2.f);
            radius[i]=.13f+soft*.5f+.045f*sinf(t*.21f*sp+i);}}
        else phase[0]=t*.08f*sp;
    }
    void Apply(float x,float y,float& r,float& g,float& b,float& a)const{
        if(!active)return;float field=0;
        if(w>=51&&w<=67||w>=80&&w<=82||w>=95){for(int i=0;i<8;i++){
            float yy=fmodf(y*.8f-rate[i]+phase[i]+1000.f,1.f);
            float d=fabsf(x-lane[i]),width=.007f+soft*.035f;
            float streak=expf(-d*d/(width*width))*expf(-powf((yy-.5f)/(.10f+soft*.35f),2.f));
            field+=streak*weight[i];}field=Clamp(field*1.8f);}
        else if(w>=71&&w<=77||w>=85&&w<=86){for(int i=0;i<8;i++)field+=Blob(x,y,xx[i],yy[i],.016f+soft*.035f,.025f+soft*.04f);field=Clamp(field);}
        else if(w==1||w==2||w==3||w==45||w==48){for(int i=0;i<5;i++)field+=Blob(x,y,cx[i],cy[i],radius[i],.12f+soft*.25f);field=Clamp(field*.75f);}
        else field=.32f+.34f*(.5f+.5f*sinf(phase[0]+x*6.28f+y*3.14f));
        float blend=Clamp(field*amp);
        r=r*(1-blend)+((rgb>>16)&255)/255.f*blend;
        g=g*(1-blend)+((rgb>>8)&255)/255.f*blend;
        b=b*(1-blend)+(rgb&255)/255.f*blend;
        a*=Clamp(.35f+.65f*field*amp+(1-amp)*.65f);
    }
};

}

namespace MusicReactive {
static std::atomic<float> level{0.f};
static std::atomic<bool> enabled{false}, app{false}, screen{false};
static std::atomic<int> sensitivity{180}, strength{65}, idle{65};
static std::atomic<bool> smoothingEnabled{true};
static std::atomic<int> attack{55}, release{13}, noiseGate{0}, responseCurve{0}, gain{100};
static std::atomic<bool> pulseEnabled{true}, speedEnabled{false}, colorEnabled{false}, gradientEnabled{false};
static std::atomic<int> speedBoost{150}, gradientDegrees{0}, gradientSoftness{15};
static std::atomic<unsigned int> pulseRgb{0x00D9FF};
static HANDLE stopEvent=nullptr, worker=nullptr;
static std::mutex sourceMutex;
static std::wstring sourceName;
static std::atomic<int> sourceRevision{0};
static std::wstring Lower(std::wstring s) {
    std::transform(s.begin(),s.end(),s.begin(),[](wchar_t c){return (wchar_t)towlower(c);});
    return s;
}
static std::wstring SelectedDeviceId(IMMDeviceEnumerator* en, const std::wstring& requested) {
    std::wstring query=Lower(requested), exactId, partialId;
    int exactCount=0, partialCount=0;
    for(EDataFlow flow : {eRender,eCapture}) {
        IMMDeviceCollection* collection=nullptr;
        if(FAILED(en->EnumAudioEndpoints(flow,DEVICE_STATE_ACTIVE,&collection)))continue;
        UINT count=0;collection->GetCount(&count);
        for(UINT i=0;i<count;i++) {
            IMMDevice* dev=nullptr;
            if(FAILED(collection->Item(i,&dev)))continue;
            IPropertyStore* props=nullptr;
            std::wstring name,id;
            if(SUCCEEDED(dev->OpenPropertyStore(STGM_READ,&props))) {
                PROPVARIANT pv;PropVariantInit(&pv);
                if(SUCCEEDED(props->GetValue(PKEY_Device_FriendlyName,&pv)) && pv.vt==VT_LPWSTR && pv.pwszVal)name=pv.pwszVal;
                PropVariantClear(&pv);props->Release();
            }
            LPWSTR rawId=nullptr;
            if(SUCCEEDED(dev->GetId(&rawId)) && rawId){id=rawId;CoTaskMemFree(rawId);}
            if(!name.empty() && !id.empty()) {
                std::wstring normalized=Lower(name);
                if(normalized==query){exactId=id;++exactCount;}
                if(normalized.find(query)!=std::wstring::npos){partialId=id;++partialCount;}
            }
            dev->Release();
        }
        collection->Release();
    }
    if(exactCount==1)return exactId;
    if(exactCount>1){Wh_Log(L"Music audio device name matches multiple enabled devices exactly: %s",requested.c_str());return L"";}
    if(partialCount==1)return partialId;
    if(partialCount>1)Wh_Log(L"Music audio device name matches multiple enabled devices: %s",requested.c_str());
    else Wh_Log(L"Music audio device not found among enabled endpoints: %s",requested.c_str());
    return L"";
}

static DWORD WINAPI Run(LPVOID) {
    HRESULT hr=CoInitializeEx(nullptr,COINIT_MULTITHREADED);
    bool com=SUCCEEDED(hr);
    if(!com && hr!=RPC_E_CHANGED_MODE) {
        Wh_Log(L"Music Reactive: COM initialization failed (HRESULT 0x%08X)",(unsigned)hr);
        return 0;
    }
    IMMDeviceEnumerator* enumerator=nullptr;
    IMMDevice* device=nullptr;
    DodiAudioMeter* meter=nullptr;
    int lastRevision=-1;
    DWORD lastAttempt=0, lastActivityLog=0;
    bool wasActive=false, hadSignal=false;
    float maxPeak=0.f;
    unsigned int sampleCount=0;
    auto releaseMeter=[&]() {
        if(meter){meter->Release();meter=nullptr;}
        if(device){device->Release();device=nullptr;}
    };
    while(WaitForSingleObject(stopEvent,33)==WAIT_TIMEOUT) {
        bool active=enabled.load() && (app.load() || screen.load());
        if(!active) {
            if(wasActive)Wh_Log(L"Music Reactive: inactive; audio meter released");
            wasActive=false;
            releaseMeter();
            level.store(0.f);
            continue;
        }
        if(!wasActive) {
            Wh_Log(L"Music Reactive: enabled; starting audio monitoring");
            wasActive=true;
            lastRevision=-1;
        }
        int revision=sourceRevision.load();
        DWORD now=GetTickCount();
        if(revision!=lastRevision) {
            releaseMeter();
            level.store(0.f);
            lastRevision=revision;
            lastAttempt=now-2000;
            hadSignal=false;
            maxPeak=0.f;
            sampleCount=0;
            Wh_Log(L"Music Reactive: audio device setting changed; reconnecting");
        }
        if(!meter && (DWORD)(now-lastAttempt)>=2000) {
            lastAttempt=now;
            releaseMeter();
            if(!enumerator) {
                hr=CoCreateInstance(__uuidof(MMDeviceEnumerator),nullptr,CLSCTX_ALL,
                                    __uuidof(IMMDeviceEnumerator),(void**)&enumerator);
                if(FAILED(hr)) {
                    Wh_Log(L"Music Reactive: device enumerator unavailable (HRESULT 0x%08X); retrying",(unsigned)hr);
                    continue;
                }
            }
            std::wstring requested;
            {std::lock_guard<std::mutex> lock(sourceMutex);requested=sourceName;}
            if(requested.empty()) {
                hr=enumerator->GetDefaultAudioEndpoint(eRender,eConsole,&device);
                if(FAILED(hr))Wh_Log(L"Music Reactive: default playback device unavailable (HRESULT 0x%08X); retrying",(unsigned)hr);
            } else {
                std::wstring id=SelectedDeviceId(enumerator,requested);
                if(!id.empty()) {
                    hr=enumerator->GetDevice(id.c_str(),&device);
                    if(FAILED(hr))Wh_Log(L"Music Reactive: matched device could not be opened (HRESULT 0x%08X); retrying",(unsigned)hr);
                    else Wh_Log(L"Music Reactive: matched enabled audio device: %s",requested.c_str());
                }
            }
            if(device) {
                hr=device->Activate(kDodiAudioMeterIID,CLSCTX_ALL,nullptr,(void**)&meter);
                if(FAILED(hr) || !meter) {
                    Wh_Log(L"Music Reactive: audio peak meter activation failed (HRESULT 0x%08X); retrying",(unsigned)hr);
                    releaseMeter();
                } else {
                    Wh_Log(L"Music Reactive: audio peak meter initialized successfully");
                    lastActivityLog=now;
                    maxPeak=0.f;
                    sampleCount=0;
                    hadSignal=false;
                }
            }
        }
        float peak=0.f;
        if(meter) {
            hr=meter->GetPeakValue(&peak);
            if(FAILED(hr)) {
                Wh_Log(L"Music Reactive: GetPeakValue failed (HRESULT 0x%08X); reconnecting",(unsigned)hr);
                releaseMeter();
                peak=0.f;
                lastAttempt=now-2000;
            } else {
                if(!std::isfinite(peak) || peak<0.f)peak=0.f;
                if(peak>1.f)peak=1.f;
                maxPeak=std::max(maxPeak,peak);
                ++sampleCount;
                if(peak>0.002f && !hadSignal) {
                    Wh_Log(L"Music Reactive: audio signal detected (peak %.3f)",(double)peak);
                    hadSignal=true;
                }
                if((DWORD)(now-lastActivityLog)>=5000) {
                    Wh_Log(L"Music Reactive: peak diagnostics: max=%.3f, samples=%u",(double)maxPeak,sampleCount);
                    maxPeak=0.f;
                    sampleCount=0;
                    lastActivityLog=now;
                }
            }
        }
        float old=level.load();
        float threshold=(float)noiseGate.load()/100.f;
        if(peak<threshold)peak=0.f;
        if(smoothingEnabled.load()) {
            float rate=(float)(peak>old?attack.load():release.load())/100.f;
            if(rate<0.01f)rate=0.01f;
            level.store(old+rate*(peak-old));
        } else level.store(peak);
    }
    releaseMeter();
    if(enumerator)enumerator->Release();
    level.store(0.f);
    if(com)CoUninitialize();
    return 0;
}
static void Settings() {
    PCWSTR src=Wh_GetStringSetting(L"MusicReactive.AudioDeviceName");
    {std::lock_guard<std::mutex> lock(sourceMutex);sourceName=src?src:L"";}
    Wh_FreeStringSetting(src);
    sourceRevision.fetch_add(1);
    enabled.store(Wh_GetIntSetting(L"MusicReactive.Enable")!=0);
    PCWSTR ae=Wh_GetStringSetting(L"Borders.Effects.BorderEffect");
    app.store(ae && _wcsicmp(ae,L"music-reactive")==0);
    Wh_FreeStringSetting(ae);
    PCWSTR se=Wh_GetStringSetting(L"ScreenBorder.Effect");
    screen.store(se && _wcsicmp(se,L"music-reactive")==0);
    Wh_FreeStringSetting(se);
    sensitivity.store(Wh_GetIntSetting(L"MusicReactive.Sensitivity"));
    smoothingEnabled.store(Wh_GetIntSetting(L"MusicReactive.SmoothingEnabled")!=0);
    attack.store(Wh_GetIntSetting(L"MusicReactive.Attack"));
    release.store(Wh_GetIntSetting(L"MusicReactive.Release"));
    noiseGate.store(Wh_GetIntSetting(L"MusicReactive.NoiseGate"));
    gain.store(Wh_GetIntSetting(L"MusicReactive.Gain"));
    PCWSTR curve=Wh_GetStringSetting(L"MusicReactive.ResponseCurve");
    responseCurve.store(curve && _wcsicmp(curve,L"soft")==0 ? 1 : curve && _wcsicmp(curve,L"punchy")==0 ? 2 : 0);
    Wh_FreeStringSetting(curve);
    strength.store(Wh_GetIntSetting(L"MusicReactive.Strength"));
    idle.store(Wh_GetIntSetting(L"MusicReactive.IdleBrightness"));
    pulseEnabled.store(Wh_GetIntSetting(L"MusicReactive.PulseEnabled")!=0);
    speedEnabled.store(Wh_GetIntSetting(L"MusicReactive.SpeedEnabled")!=0);
    colorEnabled.store(Wh_GetIntSetting(L"MusicReactive.ColorEnabled")!=0);
    gradientEnabled.store(Wh_GetIntSetting(L"MusicReactive.GradientEnabled")!=0);
    speedBoost.store(Wh_GetIntSetting(L"MusicReactive.SpeedBoost"));
    gradientDegrees.store(Wh_GetIntSetting(L"MusicReactive.GradientDegrees"));
    gradientSoftness.store(Wh_GetIntSetting(L"MusicReactive.GradientSoftness"));
    PCWSTR hex=Wh_GetStringSetting(L"MusicReactive.PulseColor");
    if(hex) {
        const wchar_t* h=hex;
        if(*h==L'#')++h;
        if(wcslen(h)==6) {
            unsigned int rgb=0;
            bool valid=true;
            for(int i=0;i<6;i++) {
                wchar_t c=h[i];
                unsigned int n=(c>=L'0'&&c<=L'9')?(unsigned)(c-L'0'):
                    (c>=L'a'&&c<=L'f')?(unsigned)(c-L'a'+10):
                    (c>=L'A'&&c<=L'F')?(unsigned)(c-L'A'+10):16u;
                if(n>15){valid=false;break;}
                rgb=(rgb<<4)|n;
            }
            if(valid)pulseRgb.store(rgb);
        }
    }
    Wh_FreeStringSetting(hex);
}
static void Start() {
    Settings();
    Wh_Log(L"Music Reactive: worker starting");
    stopEvent=CreateEventW(nullptr,TRUE,FALSE,nullptr);
    if(stopEvent)worker=CreateThread(nullptr,0,Run,nullptr,0,nullptr);
    if(!worker && stopEvent){CloseHandle(stopEvent);stopEvent=nullptr;}
}
static void Stop() {
    Wh_Log(L"Music Reactive: worker stopping");
    if(stopEvent)SetEvent(stopEvent);
    if(worker){WaitForSingleObject(worker,INFINITE);CloseHandle(worker);worker=nullptr;}
    if(stopEvent){CloseHandle(stopEvent);stopEvent=nullptr;}
}
static float Audio() {
    float x=level.load()*(float)sensitivity.load()/100.f*(float)gain.load()/100.f;
    x=x<0.f?0.f:(x>1.f?1.f:x);
    int curve=responseCurve.load();
    if(curve==1)x=sqrtf(x);
    else if(curve==2)x=x*x;
    return x;
}
static bool Active(bool isApp) {
    return enabled.load() && (isApp?app.load():screen.load());
}
static float Speed(bool isApp) {
    return Active(isApp) && speedEnabled.load() ? 1.f+Audio()*(float)speedBoost.load()/100.f : 1.f;
}
static float Mask(bool isApp, float x, float y, float w, float h) {
    if(!Active(isApp) || !gradientEnabled.load())return 1.f;
    float a=Audio();
    if(a<=0.0001f)return 0.f;
    float radians=(float)gradientDegrees.load()*0.017453292519943295f;
    // 0 degrees: reveal from bottom toward top; 90: left toward right.
    float dx=sinf(radians),dy=-cosf(radians);
    float nx=w>0.f?x/w:0.5f,ny=h>0.f?y/h:0.5f;
    float extent=(fabsf(dx)+fabsf(dy))*0.5f;
    float p=extent>0.f?0.5f+(dx*(nx-0.5f)+dy*(ny-0.5f))/(2.f*extent):0.5f;
    float softness=(float)gradientSoftness.load()/100.f*0.35f;
    if(softness<0.001f)return p<=a?1.f:0.f;
    float v=(a-p)/softness+0.5f;
    v=v<0.f?0.f:(v>1.f?1.f:v);
    return v*v*(3.f-2.f*v);
}
static void Tint(bool isApp, float& r, float& g, float& b) {
    if(!Active(isApp) || !colorEnabled.load())return;
    float k=Audio();
    unsigned int rgb=pulseRgb.load();
    r=r*(1.f-k)+((rgb>>16)&255)/255.f*k;
    g=g*(1.f-k)+((rgb>>8)&255)/255.f*k;
    b=b*(1.f-k)+(rgb&255)/255.f*k;
}
static float Multiplier(bool isApp) {
    if(!Active(isApp) || !pulseEnabled.load())return 1.f;
    float x=Audio();
    float baseline=(float)idle.load()/100.f;
    float pulse=(float)strength.load()/100.f;
    float result=baseline+(1.f-baseline)*x*pulse;
    return result<0.f?0.f:(result>1.f?1.f:result);
}
struct Frame {
    bool gradient=false,tint=false;float audio=0,dx=0,dy=0,extent=0,softness=0;
    unsigned rgb=0;
    explicit Frame(bool isApp,bool wanted=true){
        bool active=wanted&&Active(isApp);gradient=active&&gradientEnabled.load();tint=active&&colorEnabled.load();
        if(!gradient&&!tint)return;audio=Audio();rgb=pulseRgb.load();
        if(gradient){float radians=(float)gradientDegrees.load()*0.017453292519943295f;
            dx=sinf(radians);dy=-cosf(radians);extent=(fabsf(dx)+fabsf(dy))*.5f;
            softness=(float)gradientSoftness.load()/100.f*.35f;}
    }
    float Mask(float x,float y,float w,float h)const{
        if(!gradient)return 1.f;if(audio<=.0001f)return 0.f;
        float nx=w>0?x/w:.5f,ny=h>0?y/h:.5f;
        float p=extent>0?.5f+(dx*(nx-.5f)+dy*(ny-.5f))/(2.f*extent):.5f;
        if(softness<.001f)return p<=audio?1.f:0.f;
        float v=(audio-p)/softness+.5f;v=v<0?0:(v>1?1:v);return v*v*(3.f-2.f*v);
    }
    void Tint(float& r,float& g,float& b)const{
        if(!tint)return;
        r=r*(1.f-audio)+((rgb>>16)&255)/255.f*audio;
        g=g*(1.f-audio)+((rgb>>8)&255)/255.f*audio;
        b=b*(1.f-audio)+(rgb&255)/255.f*audio;
    }
};
}

// One bounded capture queue per overlay engine. GDI capture resources stay on
// the capture thread; render threads only exchange small color tables.
namespace CinemaCapture {
template<class Table> class Worker {
    struct Slot {RECT rect{};float depth=0;unsigned long long version=0;bool queued=false,busy=false,ready=false;Table table{};double touched=0,lastLog=0;DWORD error=0;};
    std::mutex mutex;std::map<UINT_PTR,Slot> slots;std::deque<UINT_PTR> queue;
    HANDLE thread=nullptr,stop=nullptr,wake=nullptr,ready=nullptr;
    unsigned long long revision=0;double lastStartAttempt=-100,lastStartLog=-100;
    bool (*sample)(const RECT&,Table&,float,bool*)=nullptr;void (*cleanup)()=nullptr;void (*initialize)()=nullptr;
    static bool SameRect(const RECT& a,const RECT& b){return a.left==b.left&&a.top==b.top&&a.right==b.right&&a.bottom==b.bottom;}
    static DWORD WINAPI Entry(void* self){return static_cast<Worker*>(self)->Run();}
    DWORD Run(){
        if(initialize)initialize();
        HANDLE events[2]={stop,wake};
        while(!RenderShutdown::requested.load()){
            DWORD wait=WaitForMultipleObjects(2,events,FALSE,INFINITE);if(wait!=WAIT_OBJECT_0+1)break;
            for(;;){
                if(RenderShutdown::requested.load()||WaitForSingleObject(stop,0)==WAIT_OBJECT_0)break;
                UINT_PTR key=0;RECT rect{};float depth=0;unsigned long long version=0;
                {std::lock_guard<std::mutex> lock(mutex);if(queue.empty())break;
                    key=queue.front();queue.pop_front();auto it=slots.find(key);if(it==slots.end())continue;
                    auto& slot=it->second;slot.queued=false;slot.busy=true;rect=slot.rect;depth=slot.depth;version=slot.version;}
                Table value;bool ignored=false;SetLastError(ERROR_SUCCESS);
                bool ok=sample(rect,value,depth,&ignored);DWORD error=ok?0:GetLastError();if(!ok&&!error)error=ERROR_GEN_FAILURE;
                {std::lock_guard<std::mutex> lock(mutex);auto it=slots.find(key);if(it==slots.end())continue;
                    auto& slot=it->second;slot.busy=false;if(slot.version!=version)continue;
                    double now=Clock();
                    if(ok){slot.table=value;slot.ready=true;if(slot.error)Wh_Log(L"Cinema capture recovered: target=0x%llX",(unsigned long long)key);slot.error=0;}
                    else {if(slot.error!=error||now-slot.lastLog>=10){Wh_Log(L"Cinema capture failed: target=0x%llX error=%lu; keeping last valid colors",(unsigned long long)key,error);slot.lastLog=now;}slot.error=error;}}
                if(ready)SetEvent(ready);
            }
        }
        if(cleanup)cleanup();return 0;
    }
    static double Clock(){static LARGE_INTEGER f=[](){LARGE_INTEGER n;QueryPerformanceFrequency(&n);return n;}();LARGE_INTEGER n;QueryPerformanceCounter(&n);return double(n.QuadPart)/f.QuadPart;}
    bool Start(){
        if(thread)return true;
        double now=Clock();if(now-lastStartAttempt<2)return false;lastStartAttempt=now;
        stop=CreateEventW(nullptr,TRUE,FALSE,nullptr);wake=CreateEventW(nullptr,FALSE,FALSE,nullptr);
        if(stop&&wake)thread=CreateThread(nullptr,0,Entry,this,0,nullptr);
        if(thread)return true;
        if(stop)CloseHandle(stop);if(wake)CloseHandle(wake);stop=wake=nullptr;
        if(now-lastStartLog>=10){Wh_Log(L"Cinema capture worker unavailable; using synchronous capture fallback");lastStartLog=now;}return false;
    }
public:
    void Configure(bool (*fn)(const RECT&,Table&,float,bool*),void (*end)(),void (*begin)()=nullptr){sample=fn;cleanup=end;initialize=begin;}
    HANDLE ReadyEvent(){if(!ready)ready=CreateEventW(nullptr,FALSE,FALSE,nullptr);return ready;}
    bool Request(UINT_PTR key,const RECT& rect,float depth)try{
        if(!sample||!Start())return false;
        double now=Clock();
        std::lock_guard<std::mutex> lock(mutex);
        for(auto it=slots.begin();it!=slots.end();)if(!it->second.busy&&!it->second.queued&&now-it->second.touched>15)it=slots.erase(it);else ++it;
        if(!slots.count(key)&&slots.size()>=128)return false; // bounded queue; caller uses original capture fallback
        auto& slot=slots[key];slot.touched=now;
        if(!slot.version||!SameRect(slot.rect,rect)||slot.depth!=depth){slot.rect=rect;slot.depth=depth;slot.version=++revision;slot.ready=false;}
        if(!slot.busy&&!slot.queued){queue.push_back(key);slot.queued=true;SetEvent(wake);}
        return true;
    }catch(const std::bad_alloc&){return false;}
    bool Read(UINT_PTR key,const RECT& rect,float depth,Table& out){
        std::lock_guard<std::mutex> lock(mutex);auto it=slots.find(key);
        if(it==slots.end()||!it->second.ready||!SameRect(it->second.rect,rect)||it->second.depth!=depth)return false;
        out=it->second.table;return true;
    }
    void Forget(UINT_PTR key){std::lock_guard<std::mutex> lock(mutex);slots.erase(key);queue.erase(std::remove(queue.begin(),queue.end(),key),queue.end());}
    void Stop(){
        if(stop)SetEvent(stop);if(thread){WaitForSingleObject(thread,INFINITE);CloseHandle(thread);thread=nullptr;}
        if(stop)CloseHandle(stop);if(wake)CloseHandle(wake);if(ready)CloseHandle(ready);stop=wake=ready=nullptr;lastStartAttempt=-100;
        std::lock_guard<std::mutex> lock(mutex);queue.clear();slots.clear();
    }
};
}

// Experimental geometry is worker-owned; disabled paths use the original renderer.
namespace PerformanceOptions {
static std::atomic<unsigned> epoch{1};
static std::atomic<bool> enabled{true},replay{false},interpolate{true},ahead{false},
    leanHistory{true},skipDuplicate{true},artworkCache{true},lowPriority{false},fastFormulas{true},
    follow{true},framePreparation{true},tiledBorders{false},timing{false},fastClassic{true};
static std::atomic<int> cacheMiB{16},loopSeconds{4},cacheFPS{30},targets{8},ttl{15},warmMs{2},
    ceiling{0},idlePoll{30},cursorPoll{0},wallPolicy{0},fluidStep{1},solverCap{0},maintenance{2},iconEntries{1024};
static std::wstring Value(PCWSTR key){
    PCWSTR p=Wh_GetStringSetting(key);std::wstring s=p?p:L"";if(p)Wh_FreeStringSetting(p);return s;
}
static int Choice(PCWSTR key,int fallback){
    auto s=Value(key);const wchar_t* start=s.c_str();while(*start&&(*start<L'0'||*start>L'9'))start++;int v=(int)wcstol(start,nullptr,10);return v>0?v:fallback;
}
static bool On(PCWSTR key){return Wh_GetIntSetting(key)!=0;}
static void Load(){
    enabled=On(L"Optimization.Enabled");
    fastFormulas=On(L"Optimization.Experimental.FastFormulas");
    follow=On(L"ResponsiveEffectTracking");
    framePreparation=On(L"Optimization.General.FramePreparation");
    fastClassic=On(L"Optimization.General.FastClassicFormulas");
    tiledBorders=On(L"Optimization.General.TiledAppBorders");
    timing=On(L"Optimization.General.TimingDiagnostics");
    replay=Value(L"Optimization.Experimental.AnimationCache")==L"loop";
    cacheMiB=std::clamp(Choice(L"Optimization.Experimental.MemoryMiB",16),8,64);
    loopSeconds=std::clamp(Choice(L"Optimization.Experimental.LoopSeconds",4),2,8);
    cacheFPS=std::clamp(Choice(L"Optimization.Experimental.CacheFPS",30),15,60);
    targets=std::clamp(Choice(L"Optimization.Experimental.Targets",8),1,16);
    ttl=std::clamp(Choice(L"Optimization.Experimental.ExpirySeconds",15),5,60);
    warmMs=std::clamp(Choice(L"Optimization.Experimental.WarmBudgetMS",2),1,4);
    interpolate=On(L"Optimization.Experimental.Interpolate");
    ahead=On(L"Optimization.Experimental.PreRenderAhead");
    leanHistory=On(L"Optimization.Cursor.LeanHistory");
    skipDuplicate=On(L"Optimization.Cursor.SkipDuplicateUploads");
    artworkCache=On(L"Optimization.Cursor.CacheArtwork");
    idlePoll=std::clamp(Choice(L"Optimization.Cursor.IdlePollingHz",30),15,60);
    auto poll=Value(L"Optimization.Cursor.ArtworkPolling");cursorPoll=poll==L"economy"?1:poll==L"responsive"?2:0;
    auto walls=Value(L"Optimization.Cursor.WallPolling");wallPolicy=walls==L"economy"?1:walls==L"responsive"?2:0;
    fluidStep=std::clamp(Choice(L"Optimization.Cursor.FluidSampling",1),1,4);
    solverCap=std::clamp(Choice(L"Optimization.Cursor.SolverCeiling",0),0,60);
    ceiling=std::clamp(Choice(L"Optimization.General.AnimationCeiling",0),0,240);
    maintenance=std::clamp(Choice(L"Optimization.General.MaintenanceSeconds",2),2,10);
    iconEntries=std::clamp(Choice(L"Optimization.General.IconCacheEntries",1024),128,1024);
    lowPriority=On(L"Optimization.General.BelowNormalWorkers");
    epoch.fetch_add(1);
}
static bool Replay(){return enabled.load()&&replay.load();}
static int Cap(int rate){int n=enabled.load()?ceiling.load():0;return n>0?std::min(rate,n):rate;}
static double Clock(){
    static LARGE_INTEGER frequency=[](){LARGE_INTEGER q;QueryPerformanceFrequency(&q);return q;}();
    LARGE_INTEGER q;QueryPerformanceCounter(&q);return (double)q.QuadPart/frequency.QuadPart;
}
static void WorkerPriority(){
    SetThreadPriority(GetCurrentThread(),enabled.load()&&lowPriority.load()?THREAD_PRIORITY_BELOW_NORMAL:THREAD_PRIORITY_NORMAL);
}
}

namespace RenderTiming {
enum Stage {Experimental,Classic,AppUpload,ScreenUpload,CursorSource,CursorCompose,CursorUpload,Capture,Wallpaper,Icons,Count};
static const wchar_t* const names[]={L"experimental live shader",L"classic border shader",L"app border upload",L"screen tile upload",L"cursor source",L"cursor composition",L"cursor upload",L"screen color sampling",L"wallpaper acquisition",L"icon pixel processing"};
struct Sample {double total=0,maximum=0;unsigned count=0;};
struct Recorder {Sample samples[Count];double since=0;unsigned epoch=0;};
static thread_local Recorder recorder;
struct Scope {
    Stage stage;double start=0;bool enabled=false;
    explicit Scope(Stage value):stage(value){
        enabled=PerformanceOptions::timing.load();
        if(enabled)start=PerformanceOptions::Clock();
    }
    ~Scope(){
        if(!enabled)return;
        double end=PerformanceOptions::Clock(),ms=(end-start)*1000;
        auto& r=recorder;unsigned epoch=PerformanceOptions::epoch.load();
        if(r.epoch!=epoch){r={};r.epoch=epoch;r.since=start;}
        auto& x=r.samples[stage];x.total+=ms;x.maximum=std::max(x.maximum,ms);++x.count;
        if(end-r.since<10)return;
        for(int i=0;i<Count;i++)if(r.samples[i].count){const auto& a=r.samples[i];
            Wh_Log(L"Render timing: %s: calls=%u average=%.3f ms max=%.3f ms",names[i],a.count,a.total/a.count,a.maximum);}
        r={};r.epoch=epoch;r.since=end;
    }
};
}
namespace BorderPresentation {
struct Rect {int x=0,y=0,w=0,h=0;};
// Matches the conservative BuildBand traversal, including curved corners.
static void Tiles(int w,int h,int margin,Rect (&out)[4]){
    for(auto& r:out)r={};if(w<=0||h<=0)return;
    int x=std::min(std::max(1,margin),(w+1)/2),y=std::min(std::max(1,margin),(h+1)/2);
    out[0]={0,0,w,y};out[1]={0,y,w,h-y};
    if(h>2*y){out[1]={0,h-y,w,y};out[2]={0,y,x,h-2*y};int right=std::max(x,w-x);out[3]={right,y,w-right,h-2*y};}
}
}
// Optional visual additions. Configuration is copied by each owning worker on reload.
namespace ExperimentalExtras {
struct Config {
    bool focus=false,shock=false,comet=false,glass=false,magnet=false,palette=false;
    bool glassApp=true,glassScreen=true,paletteApp=true,paletteScreen=true,paletteCursor=true;
    int focusMS=220,shockMS=550,shockRadius=80,shockWidth=3,cometCopies=16;
    float inactive=.65f,shockOpacity=.75f,cometScale=.25f,cometOpacity=.65f;
    float glassStrength=.55f,glassSpeed=.15f,glassWidth=.08f,glassMouse=.5f,magnetStrength=.6f,magnetRadius=120;
    float colors[2][3]={{.67f,.33f,1},{0,.93f,1}},clickColors[2][3]={{.67f,.33f,1},{0,.93f,1}};
};
static float Unit(float x){return std::clamp(x,0.f,1.f);}
static int Int(PCWSTR key,int lo,int hi){return std::clamp(Wh_GetIntSetting((std::wstring(L"Experimental.")+key).c_str()),lo,hi);}
static bool On(PCWSTR key){return Int(key,0,1)!=0;}
static void Color(PCWSTR key,float (&out)[3]){
    PCWSTR p=Wh_GetStringSetting((std::wstring(L"Experimental.")+key).c_str());
    std::wstring s=p?p:L"";if(p)Wh_FreeStringSetting(p);
    if(!s.empty()&&s[0]==L'#')s.erase(0,1);
    if(s.size()!=6)return;unsigned rgb=0;
    for(wchar_t c:s){unsigned v=c>=L'0'&&c<=L'9'?c-L'0':c>=L'a'&&c<=L'f'?c-L'a'+10:c>=L'A'&&c<=L'F'?c-L'A'+10:99;if(v>15)return;rgb=rgb*16+v;}
    out[0]=((rgb>>16)&255)/255.f;out[1]=((rgb>>8)&255)/255.f;out[2]=(rgb&255)/255.f;
}
static Config Load(){
    Config c;c.focus=On(L"FocusTransitions.Enabled");c.focusMS=Int(L"FocusTransitions.DurationMS",50,800);c.inactive=Int(L"FocusTransitions.InactiveBrightness",10,100)/100.f;
    c.shock=On(L"ClickShockwaves.Enabled");c.shockMS=Int(L"ClickShockwaves.DurationMS",150,1500);c.shockRadius=Int(L"ClickShockwaves.Radius",20,160);c.shockWidth=Int(L"ClickShockwaves.Width",1,8);c.shockOpacity=Int(L"ClickShockwaves.Opacity",1,100)/100.f;
    Color(L"ClickShockwaves.LeftColor",c.clickColors[0]);Color(L"ClickShockwaves.RightColor",c.clickColors[1]);
    c.comet=On(L"CometTrail.Enabled");c.cometCopies=Int(L"CometTrail.MaximumCopies",4,32);c.cometScale=Int(L"CometTrail.TailScale",10,90)/100.f;c.cometOpacity=Int(L"CometTrail.Opacity",1,100)/100.f;
    c.glass=On(L"GlassShimmer.Enabled");c.glassApp=On(L"GlassShimmer.AppBorders");c.glassScreen=On(L"GlassShimmer.ScreenBorders");c.glassStrength=Int(L"GlassShimmer.Strength",1,100)/100.f;c.glassSpeed=Int(L"GlassShimmer.Speed",1,100)/100.f;c.glassWidth=Int(L"GlassShimmer.Width",2,25)/100.f;c.glassMouse=Int(L"GlassShimmer.MouseHighlight",0,100)/100.f;
    c.magnet=On(L"MagneticParticles.Enabled");c.magnetStrength=Int(L"MagneticParticles.Strength",0,90)/100.f;c.magnetRadius=(float)Int(L"MagneticParticles.Radius",32,240);
    c.palette=On(L"LinkedPalettes.Enabled");c.paletteApp=On(L"LinkedPalettes.AppBorders");c.paletteScreen=On(L"LinkedPalettes.ScreenBorders");c.paletteCursor=On(L"LinkedPalettes.CursorEffects");
    Color(L"LinkedPalettes.Color1",c.colors[0]);Color(L"LinkedPalettes.Color2",c.colors[1]);return c;
}
static DWORD Palette(DWORD pixel,const Config& c,float phase){
    unsigned a=pixel>>24;if(!a)return pixel;
    float r=float((pixel>>16)&255)/a,g=float((pixel>>8)&255)/a,b=float(pixel&255)/a;
    float brightness=std::max(r,std::max(g,b));float k=Unit(phase);
    unsigned out[3];for(int i=0;i<3;i++)out[i]=(unsigned)std::min(float(a),(c.colors[0][i]*(1-k)+c.colors[1][i]*k)*brightness*a+.5f);
    return (a<<24)|(out[0]<<16)|(out[1]<<8)|out[2];
}
static float WrappedDelta(float a,float b){float d=a-b;return d-floorf(d+.5f);}
static float MagneticPosition(float position,float target,float strength,float proximity){
    return position+WrappedDelta(target,position)*Unit(strength)*Unit(proximity);
}
template<class Pixel> static void BorderPass(DWORD* bits,const std::vector<Pixel>& band,const Config& c,
    double time,bool screen,float mousePhase,float mouseNear,float brightness=1){
    bool glass=c.glass&&(screen?c.glassScreen:c.glassApp),palette=c.palette&&(screen?c.paletteScreen:c.paletteApp);
    if(!glass&&!palette&&brightness==1)return;
    float head=(float)fmod(time*c.glassSpeed,1.);
    for(const auto& p:band){
        DWORD pixel=bits[p.idx];unsigned a=pixel>>24;if(!a)continue;
        if(palette)pixel=Palette(pixel,c,.5f+.5f*sinf(p.p*6.2831853f));
        float highlight=0;
        if(glass){float d=WrappedDelta(p.p,head)/c.glassWidth;float m=WrappedDelta(p.p,mousePhase)/std::max(.02f,c.glassWidth);
            highlight=c.glassStrength*Unit(expf(-d*d*2)+c.glassMouse*mouseNear*expf(-m*m*2));}
        unsigned channels[3];int shifts[3]={16,8,0};
        for(int i=0;i<3;i++){float v=float((pixel>>shifts[i])&255);channels[i]=(unsigned)std::min(float(a),(v+(a-v)*highlight)*brightness+.5f);}
        bits[p.idx]=(a<<24)|(channels[0]<<16)|(channels[1]<<8)|channels[2];
    }
}
// Edge-only transition snapshots share a hard 4 MiB process budget.
static std::atomic<size_t> transitionBytes{0};
struct Transition {
    std::vector<DWORD> from;double started=0,duration=0;size_t charged=0;
    ~Transition(){Reset();}
    void Reset(){if(charged)transitionBytes.fetch_sub(charged);charged=0;std::vector<DWORD>().swap(from);duration=0;}
    bool Pending(double now)const{return duration>0&&now<started+duration;}
    template<class Pixel> void Begin(const DWORD* bits,const std::vector<Pixel>& band,double now,int ms){
        Reset();if(!bits||band.empty()||band.size()>65536)return;
        try{std::vector<DWORD> next(band.size());size_t bytes=next.capacity()*sizeof(DWORD);
            if(transitionBytes.fetch_add(bytes)+bytes>4u*1024u*1024u){transitionBytes.fetch_sub(bytes);return;}
            charged=bytes;for(size_t i=0;i<band.size();i++)next[i]=bits[band[i].idx];from.swap(next);started=now;duration=ms/1000.;
        }catch(const std::bad_alloc&){Reset();}
    }
    template<class Pixel> void Apply(DWORD* bits,const std::vector<Pixel>& band,double now){
        if(!Pending(now)||from.size()!=band.size()){Reset();return;}
        float u=Unit((float)((now-started)/duration));u=u*u*(3-2*u);unsigned mix=(unsigned)(u*255+.5f);
        for(size_t i=0;i<band.size();i++){DWORD to=bits[band[i].idx],result=0;
            for(int shift=0;shift<32;shift+=8){unsigned a=(from[i]>>shift)&255,b=(to>>shift)&255;result|=((a*(255-mix)+b*mix+127)/255)<<shift;}bits[band[i].idx]=result;}
    }
};
struct Click {float x=0,y=0;double born=0;int button=0;};
struct ClickState {
    std::vector<Click> rings;bool ready=false,left=false,right=false;
    void Clear(){rings.clear();ready=false;left=right=false;}
    void Update(bool l,bool r,float x,float y,double now,int ms){
        rings.erase(std::remove_if(rings.begin(),rings.end(),[&](const Click& c){return now-c.born>=ms/1000.;}),rings.end());
        if(ready){for(int button=0;button<2;button++)if((button?r:l)&&!(button?right:left)){
            if(rings.size()==8)rings.erase(rings.begin());rings.push_back({x,y,now,button});}}
        ready=true;left=l;right=r;
    }
};
}

namespace ExperimentalBorders {
struct Inputs {
    float x,y,u,v,p,d,t,w,h,mx,my,proximity;
    float reach=0,radius=0,perimeter=0,clock=0,entered=60,inside=0;
    float angle=0,nx=0,ny=0;
    unsigned frameToken=0;
};
struct Color { float r,g,b,a; };
static std::atomic<unsigned> configSerial{1};
struct Config {
    unsigned serial=0;
    bool enabled=false, mouse=true, pauseIdle=true;
    bool magnet=false;float magnetStrength=.6f,magnetRadius=120;
    int effect=0,reach=28,width=3,fps=30;
    float speed=1;
    bool body=true,particles=true,particlesOnCustom=false;
    float bodyDepth=.24f,wave=1,bodyOpacity=1,halo=1;
    float mouseRadius=72,bend=.25f,ripple=.18f;
    float particleDepth=.35f,particleWave=.15f,particleSpeed=75,particlePhase=0;
    float particleSize=2.236068f,particleOpacity=.85f,particleBend=0;
    int particleSpacing=64;
    float c1[3]={.35f,.12f,1},c2[3]={0,1,.8f},particleColor[3]={1,1,1};
    std::wstring code;
    std::shared_ptr<void> program,layoutProgram,particleProgram;
    std::wstring layoutCode,particleCode;
};
void Compile(Config& c);
void Custom(const Config& c,const Inputs& in,Color& color);
struct FieldSample {float x,y,u,v,p,d,angle,proximity;Color color;};
struct PendingFieldPixel {FieldSample sample;float spark,edgeFade,cov;int index;};
static_assert(sizeof(FieldSample)==48&&sizeof(PendingFieldPixel)==64,"Field scratch records must remain bounded");
static constexpr size_t kFieldChunk=65536;
static thread_local std::vector<PendingFieldPixel> fieldWork;
static void ClearFieldWork(){std::vector<PendingFieldPixel>().swap(fieldWork);}
bool PrepareGpuField(const Config& c,bool screen,size_t pixels);
void RunGpuField(const Config& c,const Inputs& frame,std::vector<PendingFieldPixel>& work,bool screen);

unsigned PrepareCustomFrame(const Config& c,const Inputs& in);
bool CustomReplacesFallback(const Config& c);
bool CustomNeedsProximity(const Config& c);
bool CustomFrameCurrent(const Config& c,unsigned token);
struct Layout {int count=0;float body=1;};
struct ParticleSpec {float pos=0,depth=.35f,size=2.24f,stretch=1,rotation=0,offsetx=0,offsety=0;Color color{1,1,1,1};};
Layout CustomLayout(const Config& c,const Inputs& in);
bool CustomParticle(const Config& c,const Inputs& in,int id,int count,ParticleSpec& out);
static float Unit(float x) { return std::max(0.f,std::min(1.f,x)); }
static std::wstring String(PCWSTR key) {
    PCWSTR value=Wh_GetStringSetting(key);std::wstring s=value?value:L"";
    if(value)Wh_FreeStringSetting(value);return s;
}
static void ResetReplayCache();
static Config Load(bool screen) {
    ResetReplayCache();
    Config c;c.serial=configSerial.fetch_add(1);
    c.enabled=Wh_GetIntSetting(screen?L"Experimental.ScreenBorders":L"Experimental.WindowBorders")!=0;
    c.mouse=Wh_GetIntSetting(L"Experimental.MouseInteraction")!=0;
    c.magnet=ExperimentalExtras::On(L"MagneticParticles.Enabled");
    c.magnetStrength=ExperimentalExtras::Int(L"MagneticParticles.Strength",0,90)/100.f;
    c.magnetRadius=(float)ExperimentalExtras::Int(L"MagneticParticles.Radius",32,240);
    c.pauseIdle=Wh_GetIntSetting(L"Experimental.PauseWhenIdle")!=0;
    const auto effect=String(L"Experimental.Effect");
    c.effect=effect==L"ember"?1:effect==L"custom"?2:0;
    c.reach=std::clamp(Wh_GetIntSetting(L"Experimental.Reach"),8,48);
    c.width=std::clamp(Wh_GetIntSetting(L"Experimental.Width"),1,8);
    c.fps=std::clamp(Wh_GetIntSetting(L"Experimental.FPS"),15,60);
    c.speed=std::clamp(Wh_GetIntSetting(L"Experimental.Speed"),0,200)/100.f;
    for(int k=0;k<2;k++){
        auto s=String(k?L"Experimental.Color2":L"Experimental.Color1");
        unsigned rgb=wcstoul(s.c_str()+(s.size()&&s[0]==L'#'),nullptr,16);
        float* out=k?c.c2:c.c1;
        out[0]=((rgb>>16)&255)/255.f;out[1]=((rgb>>8)&255)/255.f;out[2]=(rgb&255)/255.f;
    }
    c.code=String(L"Experimental.Custom");
    // Upgrade only the exact shipped recipe; user-written formulas are untouched.
    if(c.code==L"q=0.3+0.12*sin(p*80-t*2)+ab*0.2; a=exp(-pow((d-q)*18,2)); r=hr(p+t*0.05); g=hg(p+t*0.05); b=hb(p+t*0.05)")
        c.code=L"q=0.3+0.12*sin(p*tau*12-t*2)+ab*0.2; a=exp(-pow((d-q)*18,2)); r=hr(p+t*0.05); g=hg(p+t*0.05); b=hb(p+t*0.05)";
    c.layoutCode=String(L"Experimental.CustomLayout");
    c.particleCode=String(L"Experimental.CustomParticles");
    if(c.enabled&&c.effect==2)Compile(c);
    return c;
}
struct Motion {
    float x=0,y=0,rawX=0,rawY=0;double last=-1,entered=-100,activeUntil=0;
    bool inside=false,valid=false;
    void Update(double now,int left,int top,int width,int height) {
        POINT p{};valid=GetCursorPos(&p)!=FALSE;
        if(!valid)return;
        float nx=(float)(p.x-left),ny=(float)(p.y-top);
        bool nearby=nx>=-96&&ny>=-96&&nx<width+96&&ny<height+96;
        if(nearby&&(last<0||fabsf(nx-rawX)>.25f||fabsf(ny-rawY)>.25f))activeUntil=now+.8;
        rawX=nx;rawY=ny;
        float blend=last<0?1.f:1.f-expf(-(float)std::clamp(now-last,0.,.1)*22.f);
        x+=(nx-x)*blend;y+=(ny-y)*blend;last=now;
        bool hit=nx>=0&&ny>=0&&nx<width&&ny<height;
        if(hit!=inside){entered=now;inside=hit;activeUntil=now+1.5;}
    }
    bool Active(double now)const{return valid&&now<activeUntil;}
};
static bool NeedsFrame(const Config& c,const Motion& motion,double now){
    bool visible=(c.body&&c.bodyOpacity>0)||
        (c.particles&&c.particleOpacity>0&&(c.effect!=2||c.particlesOnCustom));
    return !RenderShutdown::requested.load()&&visible&&(!c.pauseIdle||motion.Active(now)||PerformanceOptions::Replay());
}
static bool ShouldAnimate(const Config& c,const Motion& motion,double now,
    bool active,int inactiveKind,const std::wstring& fpsMode,bool normalAnimated){
    if(!c.enabled)return normalAnimated;
    if(!active&&(inactiveKind!=1||fpsMode!=L"adaptive"))return false;
    return NeedsFrame(c,motion,now);
}

static float SoftFalloff(float q) {
    if(q>=16)return 0;
    if(q<=0)return 1;
    struct Table{float value[1025];Table(){for(int i=0;i<=1024;i++)value[i]=expf(-i/64.f);}};
    static const Table table;
    float x=q*64;int i=(int)x;float f=x-i;
    return table.value[i]+(table.value[i+1]-table.value[i])*f;
}
static float FastSine(float angle){
    struct Table{float value[1025];Table(){for(int i=0;i<=1024;i++)value[i]=sinf(i*(6.28318530718f/1024));}};
    static const Table table;
    float q=angle*(1024/6.28318530718f);q-=floorf(q/1024)*1024;
    int i=std::min(1023,(int)q);float f=q-i;
    return table.value[i]+(table.value[i+1]-table.value[i])*f;
}

static float ContourLength(float W,float H,float radius){
    float r=std::clamp(radius,0.f,std::min(W,H)*.5f);
    return 2*(W+H-4*r)+6.28318530718f*r;
}
struct Projection{float p,x,y;};
// Project a ray from the frame centre to its rounded outline. Unlike choosing
// the nearest straight edge, this map is continuous through corner diagonals
// and remains defined across the entire inward/outward drawing envelope.
static Projection Project(float x,float y,float W,float H,float radius,float exponent=2){
    const double pi=3.14159265358979323846;
    double r=std::clamp((double)radius,0.,(double)std::min(W,H)*.5);
    double hx=W*.5,hy=H*.5,dx=x-hx,dy=y-hy;
    if(fabs(dx)+fabs(dy)<1e-9){dx=0;dy=-1;}
    double scale=std::min(hx/std::max(1e-12,fabs(dx)),hy/std::max(1e-12,fabs(dy)));
    double bx=dx*scale,by=dy*scale;
    if(r>0&&fabs(bx)>hx-r&&fabs(by)>hy-r){
        if(exponent==2){
            double cx=std::copysign(hx-r,dx),cy=std::copysign(hy-r,dy);
            double A=dx*dx+dy*dy,B=dx*cx+dy*cy,C=cx*cx+cy*cy-r*r;
            scale=(B+sqrt(std::max(0.,B*B-A*C)))/A;
        }else{
            double lo=0,hi=scale;
            for(int i=0;i<22;i++){
                double mid=(lo+hi)*.5;
                double u=std::max(0.,fabs(dx*mid)-(hx-r))/r;
                double v=std::max(0.,fabs(dy*mid)-(hy-r))/r;
                if(pow(u,exponent)+pow(v,exponent)>1)hi=mid;else lo=mid;
            }
            scale=(lo+hi)*.5;
        }
        bx=dx*scale;by=dy*scale;
    }
    bx=std::clamp(bx+hx,0.,(double)W);by=std::clamp(by+hy,0.,(double)H);
    double Lh=W-2*r,Lv=H-2*r,arc=pi*r*.5,s=0;
    if(r>0&&bx<r&&by<r){
        double a=atan2(by-r,bx-r);if(a<0)a+=2*pi;
        s=2*Lh+2*Lv+3*arc+r*(a-pi);
    }else if(r>0&&bx>W-r&&by<r)s=Lh+r*(atan2(by-r,bx-(W-r))+pi*.5);
    else if(r>0&&bx>W-r&&by>H-r)s=Lh+Lv+arc+r*atan2(by-(H-r),bx-(W-r));
    else if(r>0&&bx<r&&by>H-r)s=2*Lh+Lv+2*arc+r*(atan2(by-(H-r),bx-r)-pi*.5);
    else if(by<=r+1e-6&&bx>=r-1e-6&&bx<=W-r+1e-6)s=bx-r;
    else if(bx>=W-r-1e-6&&by>=r-1e-6&&by<=H-r+1e-6)s=Lh+arc+by-r;
    else if(by>=H-r-1e-6&&bx>=r-1e-6&&bx<=W-r+1e-6)s=Lh+Lv+2*arc+(W-r-bx);
    else s=2*Lh+Lv+3*arc+(H-r-by);
    double length=2*Lh+2*Lv+4*arc;
    float p=(float)(s/std::max(1.,length));p-=floorf(p);
    return {p,(float)bx,(float)by};
}
static float ClosedWaveCount(float length,float frequency){
    return std::max(1.f,roundf(length*frequency/6.28318530718f));
}
static float LimitDisplacement(float value,float maximum){
    if(value<0)return -LimitDisplacement(-value,maximum);
    float knee=maximum*.75f;
    if(value<=knee)return value;
    return knee+(maximum-knee)*(1-SoftFalloff((value-knee)/(maximum-knee)));
}

struct EdgeProfile {float ribbon,flame,sparkDepth;};
struct ProfileCache {
    std::vector<EdgeProfile> entries;
    float time=-1,particleClock=-1,W=0,H=0,corner=-1;int reach=0;
    bool valid=false;
    bool Prepare(float t,float w,float h,int radius,float roundness=0,float particlesTime=-1){
        if(particlesTime<0)particlesTime=t;
        if(valid&&time==t&&particleClock==particlesTime&&W==w&&H==h&&reach==radius&&corner==roundness)return true;
        valid=false;time=t;particleClock=particlesTime;W=w;H=h;reach=radius;corner=roundness;
        float length=ContourLength(W,H,corner);
        int cells=std::max(4,(int)ceilf(length*.5f));
        entries.resize((size_t)cells+1);
        const float tau=6.28318530718f;
        float n1=ClosedWaveCount(length,.025f),n2=ClosedWaveCount(length,.061f);
        float n3=ClosedWaveCount(length,.07f),n4=ClosedWaveCount(length,.133f);
        for(int i=0;i<cells;i++){
            if((i&255)==0&&RenderShutdown::requested.load())return false;
            float p=(float)i/cells,angle=tau*p;
            entries[i]={radius*(.24f+.12f*sinf(angle*n1-t*2)+.07f*sinf(angle*n2+t*3)),
                radius*(.22f+.13f*sinf(angle*n3+t*5)+.1f*sinf(angle*n4-t*7)),
                sinf(angle*3+particlesTime*2)};
        }
        entries[cells]=entries[0]; // exact closed-loop endpoint, including interpolation
        valid=true;return true;
    }
    EdgeProfile At(float p)const{
        p-=floorf(p);float q=p*(entries.size()-1);
        int i=std::min((int)entries.size()-2,(int)q);float f=q-i;
        const auto& a=entries[i];const auto& b=entries[i+1];
        return {a.ribbon+(b.ribbon-a.ribbon)*f,a.flame+(b.flame-a.flame)*f,
            a.sparkDepth+(b.sparkDepth-a.sparkDepth)*f};
    }
};

// A bounded sparse cell list. Instance formulas run once per instance, never per
// border pixel. Reused vectors avoid per-frame map/node allocations.
struct ParticleGrid {
    struct Spot {float x,y,rx,ry,cosine,sine;Color color;};
    struct Link {int particle,next;};
    std::vector<Spot> spots;
    std::vector<int> heads;
    std::vector<Link> links;
    float left=0,top=0;int columns=0,rows=0;
    bool Begin(float W,float H,float reach){
        spots.clear();links.clear();columns=rows=0;
        left=top=-reach-2;
        double cols=ceil((W+2*reach+4)/32),lines=ceil((H+2*reach+4)/32);
        if(cols<1||lines<1||cols*lines>262144)return false;
        columns=(int)cols;rows=(int)lines;
        heads.assign((size_t)columns*rows,-1);
        spots.reserve(128);links.reserve(8192);
        return true;
    }
    // Position along a closed rounded contour plus outward unit normal.
    static void Point(float p,float W,float H,float radius,float exponent,
                      float& x,float& y,float& nx,float& ny){
        const float pi=3.14159265358979323846f;
        float r=std::clamp(radius,0.f,std::min(W,H)*.5f);
        float lh=W-2*r,lv=H-2*r,arc=pi*r*.5f;
        float z=(p-floorf(p))*ContourLength(W,H,r);
        float angle=0,cx=0,cy=0;
        if(z<lh){x=r+z;y=0;nx=0;ny=-1;return;}
        z-=lh;
        if(z<arc){angle=-pi*.5f+z/std::max(.001f,r);cx=W-r;cy=r;}
        else if((z-=arc)<lv){x=W;y=r+z;nx=1;ny=0;return;}
        else if((z-=lv)<arc){angle=z/std::max(.001f,r);cx=W-r;cy=H-r;}
        else if((z-=arc)<lh){x=W-r-z;y=H;nx=0;ny=1;return;}
        else if((z-=lh)<arc){angle=pi*.5f+z/std::max(.001f,r);cx=r;cy=H-r;}
        else if((z-=arc)<lv){x=0;y=H-r-z;nx=-1;ny=0;return;}
        else {z-=lv;angle=pi+z/std::max(.001f,r);cx=r;cy=r;}
        float c=cosf(angle),s=sinf(angle);
        float scale=1.f/powf(std::max(.000001f,powf(fabsf(c),exponent)+powf(fabsf(s),exponent)),1.f/exponent);
        x=cx+r*c*scale;y=cy+r*s*scale;
        nx=std::copysign(powf(fabsf(c*scale),exponent-1),c);
        ny=std::copysign(powf(fabsf(s*scale),exponent-1),s);
        float length=std::max(.000001f,sqrtf(nx*nx+ny*ny));nx/=length;ny/=length;
    }
    void Add(const ParticleSpec& spec,float W,float H,float reach,float radius,
             float exponent,bool screen){
        if(spec.color.a<=0||spots.size()>=128)return;
        float x,y,nx,ny;Point(spec.pos,W,H,radius,exponent,x,y,nx,ny);
        float sign=screen?-1.f:1.f;
        x+=nx*spec.depth*reach*sign+spec.offsetx;
        y+=ny*spec.depth*reach*sign+spec.offsety;
        float rx=spec.size*spec.stretch,ry=spec.size;
        float angle=atan2f(ny,nx)+1.57079632679f+spec.rotation*6.28318530718f,c=cosf(angle),s=sinf(angle);
        float ex=3*sqrtf(rx*rx*c*c+ry*ry*s*s);
        float ey=3*sqrtf(rx*rx*s*s+ry*ry*c*c);
        int x0=std::max(0,(int)floorf((x-ex-left)/32));
        int y0=std::max(0,(int)floorf((y-ey-top)/32));
        int x1=std::min(columns-1,(int)floorf((x+ex-left)/32));
        int y1=std::min(rows-1,(int)floorf((y+ey-top)/32));
        if(x0>x1||y0>y1)return;
        int index=(int)spots.size();spots.push_back({x,y,rx,ry,c,s,spec.color});
        for(int row=y0;row<=y1;row++)for(int col=x0;col<=x1;col++){
            if(links.size()>=65536||RenderShutdown::requested.load())return;
            int cell=row*columns+col;
            links.push_back({index,heads[cell]});heads[cell]=(int)links.size()-1;
        }
    }
    void Blend(float x,float y,Color& out)const{
        if(spots.empty())return;
        int col=(int)floorf((x-left)/32),row=(int)floorf((y-top)/32);
        if(col<0||row<0||col>=columns||row>=rows)return;
        for(int node=heads[row*columns+col];node>=0;node=links[node].next){
            const auto& spot=spots[links[node].particle];
            float dx=x-spot.x,dy=y-spot.y;
            float u=(dx*spot.cosine+dy*spot.sine)/spot.rx;
            float v=(-dx*spot.sine+dy*spot.cosine)/spot.ry;
            float q=u*u+v*v;
            if(q>=9)continue;
            float a=Unit(spot.color.a)*SoftFalloff(q);
            float total=a+Unit(out.a)*(1-a);
            if(total<=0)continue;
            out.r=(spot.color.r*a+Unit(out.r)*Unit(out.a)*(1-a))/total;
            out.g=(spot.color.g*a+Unit(out.g)*Unit(out.a)*(1-a))/total;
            out.b=(spot.color.b*a+Unit(out.b)*Unit(out.a)*(1-a))/total;
            out.a=total;
        }
    }
};

struct PreparedScene {
    bool batching=false,valid=false;unsigned serial=0,epoch=0;
    double now=0;float W=0,H=0,radius=0,exponent=0;bool screen=false;
    Motion mouse;Inputs frame{};Layout layout{};ParticleGrid particles;
    bool Matches(const Config& c,const Motion& m,double t,float w,float h,float r,float e,bool scr)const{
        return batching&&valid&&serial==c.serial&&epoch==PerformanceOptions::epoch.load()&&now==t&&W==w&&H==h&&radius==r&&exponent==e&&screen==scr&&
            mouse.x==m.x&&mouse.y==m.y&&mouse.valid==m.valid&&mouse.inside==m.inside&&mouse.entered==m.entered;
    }
};
static thread_local PreparedScene preparedScene;
struct FrameBatch {
    FrameBatch(){preparedScene.batching=true;preparedScene.valid=false;}
    ~FrameBatch(){preparedScene.batching=false;preparedScene.valid=false;}
};
// Pixel inputs are cached only in a narrow edge envelope. No full-scene particle
// pool or allocations in this loop. Traveling analytic sparks have bounded reach.
template<class Pixel> static void DrawLive(DWORD* bits,const std::vector<Pixel>& band,
    const Config& c,const Motion& mouse,double now,float W,float H,float opacity,
    bool screen,float originX=0,float originY=0,float radius=0,float exponent=2) {
    RenderTiming::Scope timing(RenderTiming::Experimental);
    if(!(c.body&&c.bodyOpacity>0)&&
       !(c.particles&&c.particleOpacity>0&&(c.effect!=2||c.particlesOnCustom)))return;
    float time=(float)fmod(now,4096.)*c.speed;
    float perimeter=ContourLength(W,H,radius);
    float magneticTarget=0,magneticPull=0;
    if(c.magnet&&mouse.valid){auto at=Project(mouse.x,mouse.y,W,H,radius,exponent);
        magneticTarget=at.p;float dx=at.x-mouse.x,dy=at.y-mouse.y;
        magneticPull=c.magnetStrength*SoftFalloff((dx*dx+dy*dy)/(c.magnetRadius*c.magnetRadius));}
    thread_local ProfileCache profile;
    const float particleTime=(float)fmod(now,4096.);

    const float width=std::min((float)c.width,std::max(.75f,c.reach*.2f));
    const float invWidth=1.f/(width*width);
    const float maxCenter=c.reach-std::min(c.reach*.4f,std::max(width*2,5.f));
    const float tau=6.28318530718f;
    const float hueCycles=ClosedWaveCount(perimeter,.008f),fireCycles=ClosedWaveCount(perimeter,.04f);
    const float sparkCount=std::max(1.f,roundf(perimeter/c.particleSpacing)),sparkSpacing=perimeter/sparkCount;
    const float particleTravel=particleTime*c.particleSpeed/sparkSpacing+c.particlePhase*sparkCount;
    const float invParticleSize=1.f/(c.particleSize*c.particleSize);
    auto& scene=preparedScene;
    bool reuse=scene.Matches(c,mouse,now,W,H,radius,exponent,screen);
    Inputs& frame=scene.frame;Layout& layout=scene.layout;ParticleGrid& customParticles=scene.particles;
    if(!reuse){
    scene.valid=false;layout=Layout{};
    frame=Inputs{0,0,0,0,0,0,time,W,H,
        mouse.x/std::max(1.f,W),mouse.y/std::max(1.f,H),0};
    frame.reach=(float)c.reach;frame.radius=radius;frame.perimeter=perimeter;
    frame.clock=particleTime;frame.entered=std::clamp((float)(now-mouse.entered),0.f,60.f);
    frame.inside=mouse.valid&&mouse.inside?1.f:0.f;
    if(c.mouse&&mouse.valid){
        auto point=Project(mouse.x,mouse.y,W,H,radius,exponent);
        float dx=point.x-mouse.x,dy=point.y-mouse.y;
        frame.proximity=SoftFalloff((dx*dx+dy*dy)/(72.f*72.f));
    }
    if(c.effect==2){
        layout=CustomLayout(c,frame);
        if(layout.count>0&&customParticles.Begin(W,H,(float)c.reach)){
            for(int id=0;id<layout.count;id++){
                if(RenderShutdown::requested.load())return;
                ParticleSpec spec;
                Inputs instance=frame;instance.p=(float)id/layout.count;instance.d=.35f;
                float nx,ny;
                ParticleGrid::Point(instance.p,W,H,radius,exponent,instance.x,instance.y,nx,ny);
                instance.u=instance.x/std::max(1.f,W);instance.v=instance.y/std::max(1.f,H);
                instance.nx=nx;instance.ny=ny;instance.angle=atan2f(ny,nx);
                float dx=instance.x-mouse.x,dy=instance.y-mouse.y;
                instance.proximity=c.mouse&&mouse.valid?SoftFalloff((dx*dx+dy*dy)/(72.f*72.f)):0;
                if(CustomParticle(c,instance,id,layout.count,spec)){
                    if(magneticPull>0)spec.pos=ExperimentalExtras::MagneticPosition(spec.pos,magneticTarget,magneticPull,1);
                    customParticles.Add(spec,W,H,(float)c.reach,radius,exponent,screen);
                }
            }
        }else customParticles.spots.clear();
    }
    scene.serial=c.serial;scene.epoch=PerformanceOptions::epoch.load();scene.now=now;
    scene.W=W;scene.H=H;scene.radius=radius;scene.exponent=exponent;scene.screen=screen;scene.mouse=mouse;scene.valid=true;
    }
    bool skipFallback=c.effect==2&&!(c.particles&&c.particlesOnCustom)&&
        (layout.body<=0||CustomReplacesFallback(c));
    if(!skipFallback&&!profile.Prepare(time,W,H,c.reach,radius,particleTime))return;
    if(c.effect==2&&layout.body>0&&(!reuse||!CustomFrameCurrent(c,frame.frameToken)))frame.frameToken=PrepareCustomFrame(c,frame);
    bool needProximity=!skipFallback||CustomNeedsProximity(c);
    auto finishPixel=[&](int index,float x,float y,float edgeFade,float spark,float coverage,Color out){
        if(c.effect==2){
            if(layout.body>0)out.a*=layout.body;else out.a=0;
            customParticles.Blend(x,y,out);
        }
        if(!c.body)out.a=0;
        else if(c.effect==2)out.a*=c.bodyOpacity;
        if(c.effect!=2||c.particlesOnCustom){
            float combined=spark+Unit(out.a)*(1-spark);
            if(combined>0){
                out.r=(c.particleColor[0]*spark+Unit(out.r)*Unit(out.a)*(1-spark))/combined;
                out.g=(c.particleColor[1]*spark+Unit(out.g)*Unit(out.a)*(1-spark))/combined;
                out.b=(c.particleColor[2]*spark+Unit(out.b)*Unit(out.a)*(1-spark))/combined;
            }
            out.a=combined;
        }
        float a=Unit(out.a)*Unit(opacity)*edgeFade*coverage;
        if(a<=0)return;
        DWORD dst=bits[index];float remain=1-a;
        unsigned A=(unsigned)std::min(255.f,a*255+(dst>>24)*remain+.5f);
        unsigned R=std::min(A,(unsigned)(Unit(out.r)*a*255+((dst>>16)&255)*remain+.5f));
        unsigned G=std::min(A,(unsigned)(Unit(out.g)*a*255+((dst>>8)&255)*remain+.5f));
        unsigned B=std::min(A,(unsigned)(Unit(out.b)*a*255+(dst&255)*remain+.5f));
        bits[index]=(A<<24)|(R<<16)|(G<<8)|B;
    };
    bool gpuField=c.effect==2&&layout.body>0&&c.body&&PrepareGpuField(c,screen,band.size());
    if(gpuField){
        try{fieldWork.clear();fieldWork.reserve(std::min(kFieldChunk,band.size()));}
        catch(const std::bad_alloc&){ClearFieldWork();gpuField=false;}
    }
    auto flushField=[&](){
        if(fieldWork.empty())return;
        RunGpuField(c,frame,fieldWork,screen);
        if(!RenderShutdown::requested.load())for(const auto& item:fieldWork)
            finishPixel(item.index,item.sample.x,item.sample.y,item.edgeFade,item.spark,item.cov,item.sample.color);
        fieldWork.clear();
    };
    size_t processed=0;
    for(const auto& b:band){
        if((processed++&511)==0&&RenderShutdown::requested.load())return;
        float x=b.x-originX,y=b.y-originY;
        float dx=b.u-mouse.x,dy=b.v-mouse.y;
        float distance2=dx*dx+dy*dy;
        float proximity=needProximity&&c.mouse&&mouse.valid?SoftFalloff(distance2/(c.mouseRadius*c.mouseRadius)):0;
        float age=(float)(now-mouse.entered);
        float ripple=!skipFallback&&c.mouse&&proximity>.0001f&&age>=0&&age<1.5f?
            sinf(sqrtf(distance2)*.055f-age*12)*SoftFalloff(age*3)*proximity:0;
        // Screen effects project inward. App effects live mostly outside the
        // frame because the target app correctly occludes their inner pixels.
        float depth=screen?-b.d:b.d;
        Color out{0,0,0,0};float spark=0;
        float edgeFade=Unit(((float)c.reach-fabsf(b.d))/4.f);
        if(!skipFallback){
        float bend=proximity*c.reach*c.bend+ripple*c.reach*c.ripple;
        float p=b.p-floorf(b.p);
        EdgeProfile edge=profile.At(p);
        float ribbon=LimitDisplacement(c.reach*c.bodyDepth+(edge.ribbon-c.reach*.24f)*c.wave+bend,maxCenter);
        float delta=depth-ribbon;
        float core=0;
        float hue=.5f+.5f*FastSine(tau*p*hueCycles-time+y*.012f);
        float alpha=0;
        if(c.effect==1){
            float flame=LimitDisplacement(c.reach*(c.bodyDepth-.02f)+(edge.flame-c.reach*.22f)*c.wave+bend,maxCenter);
            float f=Unit(1-depth/std::max(1.f,flame));
            alpha=depth>=0?f*f*(.65f+.35f*FastSine(tau*p*fireCycles-depth*.3f+time*6)):0;
            hue=Unit(depth/(float)c.reach*2);
        }else{core=SoftFalloff(delta*delta*invWidth);alpha=core+SoftFalloff(delta*delta/65.f)*.22f*c.halo;}
        // Particle lane/phase/clock are independent of the body strand.
        float particleP=p;bool magneticGap=false;
        if(magneticPull>.0001f){float delta=ExperimentalExtras::WrappedDelta(p,magneticTarget);
            magneticGap=fabsf(delta)>.5f*(1-magneticPull);
            particleP=magneticTarget+delta/std::max(.1f,1-magneticPull);}
        float phase=particleP*sparkCount+particleTravel;
        float along=(phase-floorf(phase)-.5f)*sparkSpacing;
        float lane=c.reach*(c.particleDepth+c.particleWave*edge.sparkDepth);
        float sparkDepth=LimitDisplacement(lane+bend*c.particleBend,maxCenter);
        spark=c.particles&&!magneticGap?SoftFalloff((along*along+(depth-sparkDepth)*(depth-sparkDepth))*invParticleSize)*c.particleOpacity:0;
        alpha=c.body?Unit(alpha*c.bodyOpacity):0;
        out={c.c1[0]*(1-hue)+c.c2[0]*hue,
                  c.c1[1]*(1-hue)+c.c2[1]*hue,c.c1[2]*(1-hue)+c.c2[2]*hue,alpha};
        // White specular core supplies simulated depth, not a real 3D mesh.
        if(c.effect==0){float light=core*.45f;out.r=out.r*(1-light)+light;
            out.g=out.g*(1-light)+light;out.b=out.b*(1-light)+light;}
        }
        if(c.effect==2&&layout.body>0){
            Inputs pixel=frame;pixel.x=x;pixel.y=y;
            pixel.u=x/std::max(1.f,W);pixel.v=y/std::max(1.f,H);
            pixel.p=b.p;pixel.d=depth/(float)c.reach;pixel.proximity=proximity;
            if(gpuField){
                fieldWork.push_back({{x,y,pixel.u,pixel.v,pixel.p,pixel.d,pixel.angle,proximity,out},spark,edgeFade,b.cov,b.idx});
                if(fieldWork.size()==kFieldChunk)flushField();
                continue;
            }
            Custom(c,pixel,out);
        }
        finishPixel(b.idx,x,y,edgeFade,spark,b.cov,out);
    }
    if(gpuField)flushField();
}
// Lossless transparent-run/literal encoding of edge-band pixels only.
// No full-window frame copies and no disk writes.
struct PackedFrame {
    std::vector<DWORD> words;
    bool blocked=false;
    size_t Bytes()const{return words.capacity()*sizeof(DWORD);}
    template<class Pixel> bool Pack(const DWORD* bits,const std::vector<Pixel>& band,size_t budget)try{
        size_t limit=budget/sizeof(DWORD);
        if(limit<2)return false;
        auto append=[&](DWORD value){
            if(words.size()>=limit)return false;
            if(words.size()==words.capacity()){
                size_t next=std::min(limit,std::max<size_t>(256,words.capacity()*2));
                if(next<=words.capacity())return false;words.reserve(next);
            }
            words.push_back(value);return true;
        };
        size_t i=0;
        while(i<band.size()){
            if((i&511)==0&&RenderShutdown::requested.load())return false;
            bool zero=bits[band[i].idx]==0;
            size_t end=i+1;
            while(end<band.size()&&(bits[band[end].idx]==0)==zero&&end-i<0x7fffffffu){
                if((end&1023)==0&&RenderShutdown::requested.load())return false;end++;
            }
            if(!append((DWORD)(end-i)|(zero?0x80000000u:0)))return false;
            if(!zero)for(size_t k=i;k<end;k++){
                if((k&511)==0&&RenderShutdown::requested.load())return false;
                if(!append(bits[band[k].idx]))return false;
            }
            i=end;
        }
        return true;
    }catch(const std::bad_alloc&){words.clear();return false;}
};
struct FrameReader {
    const PackedFrame& frame;size_t position=0,remaining=0;bool zero=false,valid=true;
    DWORD Next(){
        if(!remaining){
            if(position>=frame.words.size()){valid=false;return 0;}
            DWORD header=frame.words[position++];zero=(header&0x80000000u)!=0;
            remaining=header&0x7fffffffu;
            if(!remaining){valid=false;return 0;}
        }
        remaining--;
        if(zero)return 0;
        if(position>=frame.words.size()){valid=false;return 0;}
        return frame.words[position++];
    }
};
struct ReplayEntry {
    const void* address=nullptr;size_t count=0;int first=0,last=0;unsigned serial=0;
    float W=0,H=0,ox=0,oy=0,radius=0,exponent=0,opacity=0;
    int mx=0,my=0;bool validMouse=false,inside=false,screen=false;
    double touched=0,lastCost=0;
    std::vector<DWORD> base;
    std::vector<PackedFrame> frames;
    size_t Bytes()const{size_t n=base.capacity()*sizeof(DWORD);for(const auto& f:frames)n+=f.Bytes();return n;}
    template<class Pixel> bool Matches(const std::vector<Pixel>& band,float w,float h,float x,float y,
            float r,float e,float op,const Motion& mouse,bool isScreen,unsigned config)const{
        return serial==config&&address==band.data()&&count==band.size()&&first==band.front().idx&&last==band.back().idx&&
            W==w&&H==h&&ox==x&&oy==y&&radius==r&&exponent==e&&opacity==op&&screen==isScreen&&
            mx==(int)roundf(mouse.x)&&my==(int)roundf(mouse.y)&&validMouse==mouse.valid&&inside==mouse.inside;
    }
};
struct ReplayPool {
    unsigned epoch=0;size_t used=0;std::vector<ReplayEntry> entries;
    void Clear(){entries.clear();used=0;}
    void Prune(double now){
        if(epoch!=PerformanceOptions::epoch.load()){Clear();epoch=PerformanceOptions::epoch.load();}
        for(size_t i=0;i<entries.size();){
            if(now-entries[i].touched>PerformanceOptions::ttl.load()){
                used-=entries[i].Bytes();entries.erase(entries.begin()+i);
            }else i++;
        }
    }
    template<class Pixel> ReplayEntry* Find(const std::vector<Pixel>& band,float W,float H,
        float ox,float oy,float radius,float exponent,float opacity,const Motion& mouse,bool screen,double now,int phases,size_t budget,unsigned config)try{
        for(auto& entry:entries)if(entry.frames.size()==(size_t)phases&&entry.Matches(band,W,H,ox,oy,radius,exponent,opacity,mouse,screen,config)){
            entry.touched=now;return &entry;
        }
        // Remove superseded versions of this geometry (mouse/resize invalidation).
        for(size_t i=0;i<entries.size();){
            if(entries[i].address==band.data()){used-=entries[i].Bytes();entries.erase(entries.begin()+i);}
            else i++;
        }
        if(entries.size()>=(size_t)PerformanceOptions::targets.load()||band.size()>(budget-used)/sizeof(DWORD))return nullptr;
        ReplayEntry entry;entry.serial=config;entry.address=band.data();entry.count=band.size();
        entry.first=band.front().idx;entry.last=band.back().idx;
        entry.W=W;entry.H=H;entry.ox=ox;entry.oy=oy;entry.radius=radius;entry.exponent=exponent;entry.opacity=opacity;
        entry.mx=(int)roundf(mouse.x);entry.my=(int)roundf(mouse.y);
        entry.validMouse=mouse.valid;entry.inside=mouse.inside;entry.screen=screen;entry.touched=now;
        entry.base.resize(band.size());entry.frames.resize(phases);
        if(entry.Bytes()>budget-used)return nullptr;
        size_t bytes=entry.Bytes();entries.push_back(std::move(entry));used+=bytes;return &entries.back();
    }catch(const std::bad_alloc&){return nullptr;}
};
static ReplayPool& GetReplayPool(){thread_local ReplayPool pool;return pool;}
static void ResetReplayCache(){auto& pool=GetReplayPool();pool.Clear();pool.epoch=PerformanceOptions::epoch.load();}
static DWORD MixFramePixel(DWORD a,DWORD b,unsigned mix){
    DWORD result=0;
    for(int shift:{0,8,16,24})
        result|=((((a>>shift)&255)*(255-mix)+((b>>shift)&255)*mix+127)/255)<<shift;
    return result;
}
template<class Pixel> static bool RestoreFrame(DWORD* bits,const std::vector<Pixel>& band,
    const PackedFrame& a,const PackedFrame* b=nullptr,unsigned mix=0){
    FrameReader first{a},second{b?*b:a};
    for(size_t i=0;i<band.size();i++){
        if((i&511)==0&&RenderShutdown::requested.load())return false;
        DWORD value=first.Next();if(b)value=MixFramePixel(value,second.Next(),mix);
        bits[band[i].idx]=value;
    }
    return first.valid&&(!b||second.valid);
}
template<class Pixel> static void Draw(DWORD* bits,const std::vector<Pixel>& band,
    const Config& c,const Motion& mouse,double now,float W,float H,float opacity,
    bool screen,float originX=0,float originY=0,float radius=0,float exponent=2){
    ReplayPool& pool=GetReplayPool();
    double realNow=PerformanceOptions::Clock();pool.Prune(realNow);
    bool playback=PerformanceOptions::Replay()&&!band.empty()&&!mouse.Active(now)&&!(c.magnet&&mouse.valid);
    if(!playback){
        if(!PerformanceOptions::Replay())pool.Clear();
        DrawLive(bits,band,c,mouse,now,W,H,opacity,screen,originX,originY,radius,exponent);return;
    }
    int seconds=PerformanceOptions::loopSeconds.load();
    int fps=std::min(PerformanceOptions::cacheFPS.load(),c.fps);
    int phases=std::min(240,seconds*fps);
    double step=(double)seconds/phases;
    double phase=fmod(std::max(0.,now),(double)seconds)/step;
    int slot=std::min(phases-1,(int)phase),next=(slot+1)%phases;
    size_t budget=(size_t)PerformanceOptions::cacheMiB.load()*1024*1024;
    if(pool.used>budget)pool.Clear(); // safe even during a settings snapshot transition
    ReplayEntry* entry=pool.Find(band,W,H,originX,originY,radius,exponent,opacity,mouse,screen,realNow,phases,budget,c.serial);
    Motion frozen=mouse;frozen.x=roundf(mouse.x);frozen.y=roundf(mouse.y);
    // Replayed idle state has no live entry ripple; pointer interaction bypasses the cache.
    auto render=[&](int index){double t=index*step;frozen.entered=t-60;
        DrawLive(bits,band,c,frozen,t,W,H,opacity,screen,originX,originY,radius,exponent);};
    if(!entry){render(slot);return;}
    auto& frame=entry->frames[slot];
    bool hit=!frame.words.empty();
    if(!hit){
        for(size_t i=0;i<band.size();i++)entry->base[i]=bits[band[i].idx];
        double start=PerformanceOptions::Clock();render(slot);
        entry->lastCost=PerformanceOptions::Clock()-start;
        if(!frame.blocked){
            PackedFrame candidate;
            if(candidate.Pack(bits,band,budget-pool.used)&&candidate.Bytes()<=budget-pool.used){
                pool.used+=candidate.Bytes();frame=std::move(candidate);
            }else frame.blocked=true;
        }
    }
    if(RenderShutdown::requested.load())return;
    auto& future=entry->frames[next];
    if(!frame.words.empty()&&future.words.empty()&&!future.blocked&&PerformanceOptions::ahead.load()&&
       entry->lastCost>0&&entry->lastCost*2<PerformanceOptions::warmMs.load()/1000.){
        for(size_t i=0;i<band.size();i++)bits[band[i].idx]=entry->base[i];
        double start=PerformanceOptions::Clock();render(next);
        entry->lastCost=PerformanceOptions::Clock()-start;
        PackedFrame candidate;
        if(candidate.Pack(bits,band,budget-pool.used)&&candidate.Bytes()<=budget-pool.used){pool.used+=candidate.Bytes();future=std::move(candidate);}
        else future.blocked=true;
        hit=true; // restore the current phase after drawing ahead
    }
    if(hit){
        const PackedFrame* second=PerformanceOptions::interpolate.load()&&!future.words.empty()?&future:nullptr;
        unsigned mix=(unsigned)std::clamp((phase-slot)*255.,0.,255.);
        if(!RestoreFrame(bits,band,frame,second,mix)){
            for(size_t i=0;i<band.size();i++)bits[band[i].idx]=entry->base[i];
            render(slot);
        }
    }
}

}

// Opt-in D3D11 formula evaluation. CPU post-processing and GDI presentation remain.
namespace GpuFormula {
static std::atomic<bool> enabled{false},apps{true},screens{true},fields{true};
static std::atomic<unsigned> epoch{1};
static std::atomic<int> waitMS{2},minimumPixels{1024};
static void Settings(){
    PCWSTR value=Wh_GetStringSetting(L"Rendering.Backend");
    const bool useGpu=value&&wcscmp(value,L"d3d11")==0;Wh_FreeStringSetting(value);
    // Close the runtime gate before changing targets. Hidden controls retain their
    // stored values, but CPU mode must ignore them (including textual settings).
    enabled=false;
    fields=useGpu&&Wh_GetIntSetting(L"Rendering.ExperimentalFields")!=0;
    apps=useGpu&&Wh_GetIntSetting(L"Rendering.AppBorders")!=0;
    screens=useGpu&&Wh_GetIntSetting(L"Rendering.ScreenBorders")!=0;
    waitMS=std::clamp(Wh_GetIntSetting(L"Rendering.ReadbackBudgetMS"),1,4);
    minimumPixels=std::clamp(Wh_GetIntSetting(L"Rendering.MinimumPixels"),256,8192);++epoch;
    enabled=useGpu;
}
// Numeric layouts mirror both existing classic parsers; verified below each parser.
enum NodeOp {N_NUM,N_VAR,N_NEG,N_NOT,N_ADD,N_SUB,N_MUL,N_DIV,N_MOD,N_POW,N_LT,N_GT,N_LE,N_GE,N_EQ,N_NE,N_AND,N_OR,N_CALL};
enum Fn {F_SIN,F_COS,F_TAN,F_ATAN,F_ATAN2,F_ABS,F_SQRT,F_FLOOR,F_CEIL,F_FRACT,F_ROUND,F_SIGN,F_EXP,F_LOG,F_POW,F_MIN,F_MAX,F_CLAMP,F_MIX,F_STEP,F_SMOOTHSTEP,F_MOD,F_SAW,F_TRI,F_SQ,F_HR,F_HG,F_HB,F_IF,F_NOISE,F_HASH,F_HASH2,F_NOISE2,F_BAYER};
struct Plan {std::string source;bool valid=false;};
static const char kHelpers[]=R"HLSL(
float finite0(float x){return isfinite(x)?x:0.0;}
float div0(float x,float y){return y==0.0?0.0:x/y;}
float mod0(float x,float y){return y==0.0?0.0:x-y*floor(x/y);}
float power0(float x,float y){if(y==0.0)return 1.0;if(x==0.0)return 0.0;
 if(x<0.0){if(y!=trunc(y))return 0.0;return finite0(pow(-x,y)*(mod0(abs(y),2.0)==0.0?1.0:-1.0));}return finite0(pow(x,y));}
float clamp0(float x,float lo,float hi){return x<lo?lo:(x>hi?hi:x);}
float smooth0(float x,float y,float z){if(y==x)return z<x?0.0:1.0;float k=saturate((z-x)/(y-x));return k*k*(3.0-2.0*k);}
float hash0(float x){uint n=(uint)(int)floor(x);n=(n<<13)^n;n=n*(n*n*15731u+789221u)+1376312589u;return (float)(n&0x7fffffffu)/2147483648.0;}
float noise0(float x){float i=floor(x),f=x-i,u=f*f*(3.0-2.0*f);return hash0(i)+(hash0(i+1.0)-hash0(i))*u;}
float hash20(float x,float y){uint a=(uint)(int)floor(x),b=(uint)(int)floor(y);uint n=a*374761393u+b*668265263u;
 n=(n^(n>>13))*1274126177u;n^=n>>16;return (float)(n&0xffffffu)/16777216.0;}
float noise20(float x,float y){float fx=floor(x),fy=floor(y),tx=x-fx,ty=y-fy;float u=tx*tx*(3.0-2.0*tx),v=ty*ty*(3.0-2.0*ty);
 float a=hash20(fx,fy),b=hash20(fx+1.0,fy),c=hash20(fx,fy+1.0),d=hash20(fx+1.0,fy+1.0);
 float top=a+(b-a)*u,bot=c+(d-c)*u;return top+(bot-top)*v;}
float bayer0(float x,float y){const int m[16]={0,8,2,10,12,4,14,6,3,11,1,9,15,7,13,5};return ((float)m[(((int)floor(y))&3)*4+(((int)floor(x))&3)]+0.5)/16.0;}
cbuffer Params:register(b0){float4 initial[16];float4 ambient[96];float4 cinema[96];uint count;uint hasAmbient;uint hasCinema;uint fieldMode;};
struct Pixel{float x,y,u,v,p,d,ang,cov;};
Buffer<float4> inputPixels:register(t0);RWStructuredBuffer<float4> outputPixels:register(u0);
float3 sampleAmbient(float p){float f=frac(p)*96.0-0.5;int i=(int)floor(f);float k=f-floor(f);
 return ambient[(i+96)%96].rgb*(1.0-k)+ambient[(i+97)%96].rgb*k;}
float3 sampleCinema(float p){float f=frac(p)*96.0-0.5;int i=(int)floor(f);float k=f-floor(f);
 return cinema[(i+96)%96].rgb*(1.0-k)+cinema[(i+97)%96].rgb*k;}
[numthreads(64,1,1)]void main(uint3 id:SV_DispatchThreadID){if(id.x>=count)return;
 float4 first=inputPixels[id.x*3],second=inputPixels[id.x*3+1],fallback=inputPixels[id.x*3+2];
 Pixel b;b.x=first.x;b.y=first.y;b.u=first.z;b.v=first.w;b.p=second.x;b.d=second.y;b.ang=second.z;b.cov=second.w;float v[64];
 [unroll]for(int i=0;i<64;i++)v[i]=initial[i/4][i%4];
 v[0]=b.x;v[1]=b.y;v[2]=b.u;v[3]=b.v;v[4]=b.p;v[5]=b.d;v[6]=b.ang;
 if(fieldMode){v[31]=fallback.r;v[32]=fallback.g;v[33]=fallback.b;v[34]=fallback.a;v[27]=b.cov;v[43]=b.cov;}
 else{v[31]=v[17];v[32]=v[18];v[33]=v[19];v[34]=1.0;}
 if(hasAmbient){float3 a=sampleAmbient(b.p);v[25]=a.r;v[26]=a.g;v[27]=a.b;}
 if(hasCinema){float3 c=sampleCinema(b.p);v[28]=c.r;v[29]=c.g;v[30]=c.b;}
)HLSL";
static std::string FloatLiteral(float value){
    if(!std::isfinite(value))return "0.0";
    // Locale-independent exact bit representation of each parsed float constant.
    uint32_t bits=0;memcpy(&bits,&value,sizeof(bits));return "asfloat("+std::to_string(bits)+"u)";
}
template<class Program> static Plan Emit(const Program& p){
    Plan result;
    if(p.nodes.empty()||p.nodes.size()>1024||p.stmts.size()>128||p.tempCount<0||p.tempCount>29)return result;
    std::string body;unsigned serial=0;
    for(const auto& statement:p.stmts){
        if(statement.var<0||statement.var>=64||statement.root<0||statement.root>=(int)p.nodes.size())return result;
        std::vector<std::string> names(p.nodes.size());bool ok=true;
        // Parser trees are bounded to 128 levels. Emit each node once per statement;
        // temporaries must be reread after an earlier assignment changes them.
        auto visit=[&](auto&& self,int index,unsigned depth)->std::string{
            if(index<0||index>=(int)p.nodes.size()||depth>128){ok=false;return "0.0";}
            if(!names[index].empty())return names[index];
            const auto& n=p.nodes[index];std::string x,y,z,expression;
            if(n.op==N_NUM){if(!std::isfinite(n.num)){ok=false;return "0.0";}expression=FloatLiteral(n.num);}
            else if(n.op==N_VAR){if(n.var<0||n.var>=64){ok=false;return "0.0";}expression="v["+std::to_string(n.var)+"]";}
            else{
                x=self(self,n.a,depth+1);
                if(n.b>=0)y=self(self,n.b,depth+1);if(n.c>=0)z=self(self,n.c,depth+1);
                switch(n.op){
                    case N_NEG:expression="(-"+x+")";break;
                    case N_NOT:expression="("+x+"==0.0?1.0:0.0)";break;
                    case N_ADD:expression=x+"+"+y;break;case N_SUB:expression=x+"-"+y;break;case N_MUL:expression=x+"*"+y;break;
                    case N_DIV:expression="div0("+x+","+y+")";break;case N_MOD:expression="mod0("+x+","+y+")";break;case N_POW:expression="power0("+x+","+y+")";break;
                    case N_LT:case N_GT:case N_LE:case N_GE:case N_EQ:case N_NE:{const char* ops[]={"<",">","<=",">=","==","!="};expression="("+x+ops[n.op-N_LT]+y+"?1.0:0.0)";break;}
                    case N_AND:case N_OR:expression="(("+x+"!=0.0)"+(n.op==N_AND?"&&":"||")+"("+y+"!=0.0)?1.0:0.0)";break;
                    case N_CALL:{
                        const char* unary[]={"sin","cos","tan","atan"};
                        if(n.fn>=F_SIN&&n.fn<=F_ATAN)expression=std::string(unary[n.fn])+"("+x+")";
                        else switch(n.fn){
                            case F_ATAN2:expression="atan2("+x+","+y+")";break;
                            case F_ABS:expression="abs("+x+")";break;case F_SQRT:expression="("+x+">0.0?sqrt("+x+"):0.0)";break;
                            case F_FLOOR:expression="floor("+x+")";break;case F_CEIL:expression="ceil("+x+")";break;
                            case F_FRACT:case F_SAW:expression="frac("+x+")";break;
                            case F_ROUND:expression="sign("+x+")*floor(abs("+x+")+0.5)";break;
                            case F_SIGN:expression="("+x+">0.0?1.0:("+x+"<0.0?-1.0:0.0))";break;
                            case F_EXP:expression="exp(clamp0("+x+",-60.0,60.0))";break;
                            case F_LOG:expression="("+x+">0.0?log("+x+"):0.0)";break;
                            case F_POW:expression="power0("+x+","+y+")";break;
                            case F_MIN:expression="("+x+"<"+y+"?"+x+":"+y+")";break;
                            case F_MAX:expression="("+x+">"+y+"?"+x+":"+y+")";break;
                            case F_CLAMP:expression="clamp0("+x+","+y+","+z+")";break;
                            case F_MIX:expression=x+"+("+y+"-"+x+")*"+z;break;
                            case F_STEP:expression="("+y+"<"+x+"?0.0:1.0)";break;
                            case F_SMOOTHSTEP:expression="smooth0("+x+","+y+","+z+")";break;
                            case F_MOD:expression="mod0("+x+","+y+")";break;
                            case F_TRI:expression="1.0-abs(2.0*frac("+x+")-1.0)";break;
                            case F_SQ:expression="(frac("+x+")<0.5?1.0:0.0)";break;
                            case F_HR:expression="saturate(abs(6.0*frac("+x+")-3.0)-1.0)";break;
                            case F_HG:expression="saturate(2.0-abs(6.0*frac("+x+")-2.0))";break;
                            case F_HB:expression="saturate(2.0-abs(6.0*frac("+x+")-4.0))";break;
                            case F_IF:expression="("+x+"!=0.0?"+y+":"+z+")";break;
                            case F_NOISE:expression="noise0("+x+")";break;case F_HASH:expression="hash0("+x+")";break;
                            case F_HASH2:expression="hash20("+x+","+y+")";break;
                            case F_NOISE2:expression="noise20("+x+","+y+")";break;
                            case F_BAYER:expression="bayer0("+x+","+y+")";break;
                            default:ok=false;break;
                        }break;
                    }
                    default:ok=false;break;
                }
            }
            std::string name="z"+std::to_string(serial++);body+="float "+name+"="+expression+";\n";
            names[index]=name;return name;
        };
        auto value=visit(visit,statement.root,0);if(!ok||body.size()>256u*1024u)return result;
        body+="v["+std::to_string(statement.var)+"]=finite0("+value+");\n";
    }
    result.source=std::string(kHelpers)+body+"outputPixels[id.x]=float4(v[31],v[32],v[33],v[34]);}\n";
    result.valid=true;return result;
}

struct Geometry {float x,y,u,v,p,d,ang,cov;float r=0,g=0,b=0,a=1;};
template<class Pixel> static Geometry ToGeometry(const Pixel& v){return {v.x,v.y,v.u,v.v,v.p,v.d,v.ang,v.cov};}
static Geometry ToGeometry(const Geometry& v){return v;}
static Geometry ToGeometry(const ExperimentalBorders::PendingFieldPixel& point){const auto& v=point.sample;
    return {v.x,v.y,v.u,v.v,v.p,v.d,v.angle,v.proximity,v.color.r,v.color.g,v.color.b,v.color.a};}

struct alignas(16) Constants {float initial[64];float ambient[96][4],cinema[96][4];uint32_t count,hasAmbient,hasCinema,fieldMode;};
static_assert(sizeof(Geometry)==48&&sizeof(Constants)==3344,"GPU buffer packing must match HLSL");
static double Clock(){static const double frequency=[](){LARGE_INTEGER f{};QueryPerformanceFrequency(&f);return (double)f.QuadPart;}();LARGE_INTEGER n{};QueryPerformanceCounter(&n);return (double)n.QuadPart/frequency;}
class Engine {
    using CreateDevice=decltype(&D3D11CreateDevice);
    using Compile=decltype(&D3DCompile);
    HMODULE runtime=nullptr,compiler=nullptr;
    Compile compile=nullptr;
    winrt::com_ptr<ID3D11Device> device;
    winrt::com_ptr<ID3D11DeviceContext> context;
    winrt::com_ptr<ID3D11Buffer> geometry,output,staging,constants;
    winrt::com_ptr<ID3D11ShaderResourceView> inputView;
    winrt::com_ptr<ID3D11UnorderedAccessView> outputView;
    winrt::com_ptr<ID3D11Query> completion;
    struct Shader {std::string source;winrt::com_ptr<ID3D11ComputeShader> shader;};
    std::vector<Shader> shaders;
    struct Job {Compile compile=nullptr;std::string source;HRESULT result=E_FAIL;winrt::com_ptr<ID3DBlob> code;};
    std::unique_ptr<Job> job;
    HANDLE compilerThread=nullptr;
    size_t capacity=0;
    unsigned timeouts=0;
    bool attempted=false,disabled=false,pending=false,logged=false;
    std::vector<float> readback;
    static DWORD WINAPI CompileThread(LPVOID parameter){
        auto& j=*static_cast<Job*>(parameter);winrt::com_ptr<ID3DBlob> errors;
        j.result=j.compile(j.source.data(),j.source.size(),"DodiGeneratedBorder",nullptr,nullptr,"main","cs_5_0",
            D3DCOMPILE_ENABLE_STRICTNESS|D3DCOMPILE_IEEE_STRICTNESS|D3DCOMPILE_OPTIMIZATION_LEVEL1,0,j.code.put(),errors.put());
        return 0;
    }
    bool Fail(HRESULT hr,PCWSTR where){
        if(!disabled)Wh_Log(L"Direct3D 11: %s failed (0x%08X); CPU fallback until settings reload",where,(unsigned)hr);
        disabled=true;return false;
    }
    bool Initialize(){
        if(attempted)return !disabled&&static_cast<bool>(device);
        attempted=true;
        runtime=LoadLibraryExW(L"d3d11.dll",nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);
        compiler=LoadLibraryExW(L"d3dcompiler_47.dll",nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);
        auto create=runtime?reinterpret_cast<CreateDevice>(GetProcAddress(runtime,"D3D11CreateDevice")):nullptr;
        compile=compiler?reinterpret_cast<Compile>(GetProcAddress(compiler,"D3DCompile")):nullptr;
        if(!create||!compile)return Fail(HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND),L"system runtime/compiler load");
        D3D_FEATURE_LEVEL requested=D3D_FEATURE_LEVEL_11_0,actual{};
        HRESULT hr=create(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,0,&requested,1,D3D11_SDK_VERSION,device.put(),&actual,context.put());
        if(FAILED(hr)||actual<D3D_FEATURE_LEVEL_11_0)return Fail(FAILED(hr)?hr:E_NOINTERFACE,L"hardware compute device creation");
        D3D11_BUFFER_DESC cb{};cb.ByteWidth=sizeof(Constants);cb.Usage=D3D11_USAGE_DYNAMIC;
        cb.BindFlags=D3D11_BIND_CONSTANT_BUFFER;cb.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
        hr=device->CreateBuffer(&cb,nullptr,constants.put());if(FAILED(hr))return Fail(hr,L"constant buffer allocation");
        D3D11_QUERY_DESC query{D3D11_QUERY_EVENT,0};hr=device->CreateQuery(&query,completion.put());
        if(FAILED(hr))return Fail(hr,L"completion query allocation");
        Wh_Log(L"Direct3D 11: hardware compute device ready; GDI presentation retained");return true;
    }
    void FinishCompile(){
        if(!compilerThread||WaitForSingleObject(compilerThread,0)!=WAIT_OBJECT_0)return;
        CloseHandle(compilerThread);compilerThread=nullptr;
        Shader result;result.source=std::move(job->source);
        HRESULT hr=job->result;
        if(SUCCEEDED(hr)&&job->code)hr=device->CreateComputeShader(job->code->GetBufferPointer(),job->code->GetBufferSize(),nullptr,result.shader.put());
        if(FAILED(hr))Wh_Log(L"Direct3D 11: border shader rejected (0x%08X); this formula uses CPU",(unsigned)hr);
        shaders.push_back(std::move(result));job.reset();
    }
    ID3D11ComputeShader* ShaderFor(const std::string& source){
        FinishCompile();
        for(auto& s:shaders)if(s.source==source)return s.shader.get();
        if(compilerThread||shaders.size()>=8)return nullptr;
        job=std::make_unique<Job>();job->source=source;job->compile=compile;
        compilerThread=CreateThread(nullptr,0,CompileThread,job.get(),0,nullptr);
        if(!compilerThread){Shader failed;failed.source=source;shaders.push_back(std::move(failed));job.reset();
            Wh_Log(L"Direct3D 11: shader compiler thread unavailable; this formula uses CPU");}
        return nullptr;
    }
    bool Allocate(size_t count){
        if(count<=capacity)return true;
        geometry=nullptr;output=nullptr;staging=nullptr;inputView=nullptr;outputView=nullptr;capacity=0;
        size_t rounded=(count+255)&~size_t(255);
        D3D11_BUFFER_DESC d{};d.ByteWidth=(UINT)(rounded*sizeof(Geometry));d.Usage=D3D11_USAGE_DYNAMIC;
        d.BindFlags=D3D11_BIND_SHADER_RESOURCE;d.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
        // Typed float4 SRV permits dynamic CPU writes without structured-resource misc flags.
        d.MiscFlags=0;d.StructureByteStride=0;
        HRESULT hr=device->CreateBuffer(&d,nullptr,geometry.put());if(FAILED(hr))return Fail(hr,L"geometry allocation");
        D3D11_SHADER_RESOURCE_VIEW_DESC srv{};srv.Format=DXGI_FORMAT_R32G32B32A32_FLOAT;srv.ViewDimension=D3D11_SRV_DIMENSION_BUFFER;srv.Buffer.NumElements=(UINT)(rounded*3);
        hr=device->CreateShaderResourceView(geometry.get(),&srv,inputView.put());if(FAILED(hr))return Fail(hr,L"geometry view allocation");
        d.ByteWidth=(UINT)(rounded*sizeof(float)*4);d.Usage=D3D11_USAGE_DEFAULT;d.CPUAccessFlags=0;
        d.BindFlags=D3D11_BIND_UNORDERED_ACCESS;d.MiscFlags=D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;d.StructureByteStride=sizeof(float)*4;
        hr=device->CreateBuffer(&d,nullptr,output.put());if(FAILED(hr))return Fail(hr,L"output allocation");
        D3D11_UNORDERED_ACCESS_VIEW_DESC uav{};uav.Format=DXGI_FORMAT_UNKNOWN;uav.ViewDimension=D3D11_UAV_DIMENSION_BUFFER;uav.Buffer.NumElements=(UINT)rounded;
        hr=device->CreateUnorderedAccessView(output.get(),&uav,outputView.put());if(FAILED(hr))return Fail(hr,L"output view allocation");
        d.Usage=D3D11_USAGE_STAGING;d.CPUAccessFlags=D3D11_CPU_ACCESS_READ;d.BindFlags=0;d.MiscFlags=0;d.StructureByteStride=0;
        hr=device->CreateBuffer(&d,nullptr,staging.put());if(FAILED(hr))return Fail(hr,L"readback allocation");
        readback.resize(rounded*4);capacity=rounded;return true;
    }
    bool Ready(){
        if(!pending)return true;
        HRESULT hr=context->GetData(completion.get(),nullptr,0,D3D11_ASYNC_GETDATA_DONOTFLUSH);
        if(hr==S_OK){pending=false;return true;}if(FAILED(hr))Fail(hr,L"GPU completion");return false;
    }
public:
    unsigned generation=epoch.load();
    bool Prepare(const Plan& plan){return !disabled&&!RenderShutdown::requested.load()&&Initialize()&&Ready()&&ShaderFor(plan.source)!=nullptr;}

    ~Engine(){
        // No async DLL callbacks may survive teardown. Source generation is bounded.
        if(compilerThread){WaitForSingleObject(compilerThread,INFINITE);CloseHandle(compilerThread);compilerThread=nullptr;}
        job.reset();
        if(context){context->ClearState();context->Flush();}
        shaders.clear();inputView=nullptr;outputView=nullptr;completion=nullptr;
        geometry=nullptr;output=nullptr;staging=nullptr;constants=nullptr;context=nullptr;device=nullptr;
        if(compiler)FreeLibrary(compiler);if(runtime)FreeLibrary(runtime);
    }
    template<class Pixel> const float* Evaluate(const Plan& plan,const std::vector<Pixel>& band,const float* values,const float* ambient,const float* cinema,bool fieldMode=false,bool reportBorder=true){
        if(disabled||RenderShutdown::requested.load()||!Initialize()||!Ready())return nullptr;
        auto shader=ShaderFor(plan.source);if(!shader||!Allocate(band.size()))return nullptr;
        D3D11_MAPPED_SUBRESOURCE map{};
        HRESULT hr=context->Map(geometry.get(),0,D3D11_MAP_WRITE_DISCARD,0,&map);
        if(FAILED(hr)){Fail(hr,L"geometry upload");return nullptr;}
        auto target=static_cast<Geometry*>(map.pData);
        for(size_t i=0;i<band.size();i++)target[i]=ToGeometry(band[i]);
        context->Unmap(geometry.get(),0);
        Constants c{};memcpy(c.initial,values,sizeof(c.initial));c.count=(uint32_t)band.size();c.fieldMode=fieldMode;c.hasAmbient=ambient!=nullptr;c.hasCinema=cinema!=nullptr;
        for(int i=0;i<96;i++)for(int k=0;k<3;k++){if(ambient)c.ambient[i][k]=ambient[i*3+k];if(cinema)c.cinema[i][k]=cinema[i*3+k];}
        hr=context->Map(constants.get(),0,D3D11_MAP_WRITE_DISCARD,0,&map);
        if(FAILED(hr)){Fail(hr,L"frame constants upload");return nullptr;}
        memcpy(map.pData,&c,sizeof(c));context->Unmap(constants.get(),0);
        ID3D11ShaderResourceView* srv=inputView.get();ID3D11UnorderedAccessView* uav=outputView.get();ID3D11Buffer* cb=constants.get();
        context->CSSetShader(shader,nullptr,0);context->CSSetShaderResources(0,1,&srv);context->CSSetUnorderedAccessViews(0,1,&uav,nullptr);context->CSSetConstantBuffers(0,1,&cb);
        context->Dispatch((UINT)((band.size()+63)/64),1,1);
        srv=nullptr;uav=nullptr;cb=nullptr;context->CSSetShaderResources(0,1,&srv);context->CSSetUnorderedAccessViews(0,1,&uav,nullptr);context->CSSetConstantBuffers(0,1,&cb);context->CSSetShader(nullptr,nullptr,0);
        context->CopyResource(staging.get(),output.get());context->End(completion.get());context->Flush();pending=true;
        double deadline=Clock()+waitMS.load()/1000.0;
        while(!Ready()){
            if(disabled||RenderShutdown::requested.load()||Clock()>=deadline){
                if(!disabled&&++timeouts>=3)Fail(DXGI_ERROR_WAS_STILL_DRAWING,L"readback budget (three misses)");
                return nullptr;
            }
            SwitchToThread();
        }
        hr=context->Map(staging.get(),0,D3D11_MAP_READ,D3D11_MAP_FLAG_DO_NOT_WAIT,&map);
        if(hr==DXGI_ERROR_WAS_STILL_DRAWING){if(++timeouts>=3)Fail(hr,L"nonblocking readback");return nullptr;}
        if(FAILED(hr)){Fail(hr,L"readback map");return nullptr;}
        memcpy(readback.data(),map.pData,band.size()*4*sizeof(float));context->Unmap(staging.get(),0);timeouts=0;
        if(reportBorder&&!logged){logged=true;Wh_Log(L"Direct3D 11: first border formula frame evaluated on GPU (%zu edge pixels)",band.size());}
        return readback.data();
    }
};
static thread_local std::unique_ptr<Engine> g_engine;
static thread_local unsigned g_failedEpoch=0;
static void Reset(){g_engine.reset();ExperimentalBorders::ClearFieldWork();}
template<class Program> static const Plan* PrepareField(const Program& p,bool screen,size_t pixels){
    if(g_failedEpoch==epoch.load()||!enabled.load()||!fields.load()||!(screen?screens.load():apps.load())||
        pixels<(size_t)minimumPixels.load()||RenderShutdown::requested.load())return nullptr;
    try{
        if(!p.gpuPlan)p.gpuPlan=std::make_shared<Plan>(Emit(p));
        auto plan=static_cast<const Plan*>(p.gpuPlan.get());if(!plan->valid)return nullptr;
        if(g_engine&&g_engine->generation!=epoch.load())Reset();
        if(!g_engine)g_engine=std::make_unique<Engine>();
        return g_engine->Prepare(*plan)?plan:nullptr;
    }catch(...){g_failedEpoch=epoch.load();Reset();return nullptr;}
}
template<class Program,class Pixel> static const float* Evaluate(const Program& p,const std::vector<Pixel>& band,const float* values,const float* ambient,const float* cinema,bool screen){
    if(g_failedEpoch==epoch.load()||!enabled.load()||!(screen?screens.load():apps.load())||band.size()<(size_t)minimumPixels.load()||band.size()>131072||RenderShutdown::requested.load())return nullptr;
    try{
        if(!p.gpuPlan)p.gpuPlan=std::make_shared<Plan>(Emit(p));
        auto plan=static_cast<const Plan*>(p.gpuPlan.get());if(!plan->valid)return nullptr;
        if(g_engine&&g_engine->generation!=epoch.load())Reset();
        if(!g_engine)g_engine=std::make_unique<Engine>();
        return g_engine->Evaluate(*plan,band,values,ambient,cinema);
    }catch(...){g_failedEpoch=epoch.load();Wh_Log(L"Direct3D 11: allocation exception; CPU fallback until settings reload");Reset();return nullptr;}
}
} // namespace GpuFormula

// Additional GPU stages share bounded compute buffers, not UI-thread TLS resources.
// Each immediate context has one mutex owner; contention uses CPU rather than waiting.
namespace GpuStages {
enum Category {DesktopIcons,TaskbarIcons,StartMenuIcons,ExplorerIcons,CursorTrail,CursorParticles,CursorGlow,CursorFluid,CategoryCount};
static std::atomic<bool> targets[CategoryCount]{};
static const wchar_t* const names[CategoryCount]={L"DesktopIcons",L"TaskbarIcons",L"StartMenuIcons",L"ExplorerIcons",L"CursorTrail",L"CursorParticles",L"CursorGlow",L"CursorFluid"};
struct Pool {
    std::mutex mutex;
    std::unique_ptr<GpuFormula::Engine> engine;
    std::vector<GpuFormula::Geometry> work;
    unsigned failedEpoch=0;
    bool logged[CategoryCount]{};
    void Clear(){engine.reset();std::vector<GpuFormula::Geometry>().swap(work);for(bool& v:logged)v=false;}
};
static Pool icons,cursor;
static bool Allowed(Category category,size_t count){
    return category>=0&&category<CategoryCount&&GpuFormula::enabled.load()&&targets[category].load()&&
        !RenderShutdown::requested.load()&&count>0&&count<=131072&&count>=(size_t)GpuFormula::minimumPixels.load();
}
static void Reset(){for(Pool* pool:{&icons,&cursor}){std::lock_guard<std::mutex> lock(pool->mutex);pool->Clear();}}
static void Settings(){
    for(int i=0;i<CategoryCount;i++)targets[i]=GpuFormula::enabled.load()&&Wh_GetIntSetting((std::wstring(L"Rendering.")+names[i]).c_str())!=0;
    Reset();
}
template<class MakePlan,class Fill,class Consume>
static bool Compute(Category category,size_t count,const float* values,MakePlan makePlan,Fill fill,Consume consume){
    if(!Allowed(category,count))return false;
    Pool& pool=category<CursorTrail?icons:cursor;
    std::unique_lock<std::mutex> lock(pool.mutex,std::try_to_lock);
    if(!lock.owns_lock()||!Allowed(category,count)||pool.failedEpoch==GpuFormula::epoch.load())return false;
    try{
        if(pool.engine&&pool.engine->generation!=GpuFormula::epoch.load())pool.Clear();
        const auto* plan=makePlan();if(!plan||!plan->valid)return false;
        if(!pool.engine)pool.engine=std::make_unique<GpuFormula::Engine>();
        // Compilation is asynchronous. Do not build/upload pixels until ready.
        if(!pool.engine->Prepare(*plan))return false;
        pool.work.resize(count);fill(pool.work);
        const float* result=pool.engine->Evaluate(*plan,pool.work,values,nullptr,nullptr,false,false);
        if(!result||!Allowed(category,count))return false;
        // A failed operation never changes a destination. Validate before committing.
        for(size_t i=0;i<count*4;i++)if(!std::isfinite(result[i]))return false;
        consume(result);
        if(!pool.logged[category]){pool.logged[category]=true;Wh_Log(L"Direct3D 11: first %s stage evaluated on GPU (%zu pixels)",names[category],count);}
        return true;
    }catch(...){pool.failedEpoch=GpuFormula::epoch.load();pool.Clear();Wh_Log(L"Direct3D 11: stage allocation exception; CPU fallback until settings reload");return false;}
}
static const GpuFormula::Plan& CompositionPlan(){
    static const GpuFormula::Plan plan{R"HLSL(
cbuffer Params:register(b0){float4 initial[16];float4 ambient[96];float4 cinema[96];uint count;uint hasAmbient;uint hasCinema;uint fieldMode;};
Buffer<float4> inputPixels:register(t0);RWStructuredBuffer<float4> outputPixels:register(u0);
[numthreads(64,1,1)]void main(uint3 id:SV_DispatchThreadID){
 if(id.x>=count)return;
 uint4 previous=(uint4)inputPixels[id.x*3],current=(uint4)inputPixels[id.x*3+1],destination=(uint4)inputPixels[id.x*3+2];
 uint mixValue=(uint)initial[0].x;
 uint4 source=(previous*(255u-mixValue)+current*mixValue+127u)/255u;
 uint4 result=source.a==0u?destination:(source.a==255u?source:min(255u,source+(destination*(255u-source.a)+127u)/255u));
 outputPixels[id.x]=(float4)result;
}
)HLSL",true};return plan;
}
static GpuFormula::Geometry CompositionPixels(uint32_t previous,uint32_t current,uint32_t destination){
    auto byte=[](uint32_t p,int shift){return (float)((p>>shift)&255u);};
    return {byte(previous,0),byte(previous,8),byte(previous,16),byte(previous,24),
        byte(current,0),byte(current,8),byte(current,16),byte(current,24),
        byte(destination,0),byte(destination,8),byte(destination,16),byte(destination,24)};
}
static uint32_t Pack(const float* in){uint32_t pixel=0;for(int k=0;k<4;k++)pixel|=(uint32_t)std::clamp(in[k],0.f,255.f)<<(k*8);return pixel;}
} // namespace GpuStages

namespace DodiEngine {
// ====================================================================== shared helpers
static inline int Imin(int a, int b) { return a < b ? a : b; }
static inline int Imax(int a, int b) { return a > b ? a : b; }
static inline float Clamp01(float x) { return x < 0.f ? 0.f : (x > 1.f ? 1.f : x); }
static inline float Fract(float x) { return x - floorf(x); }

static std::wstring Lower(std::wstring s) {
    for (auto& ch : s)
        if (ch >= L'A' && ch <= L'Z') ch = (wchar_t)(ch - L'A' + L'a');
    return s;
}
static std::wstring Trim(const std::wstring& s) {
    size_t a = s.find_first_not_of(L" \t\r\n");
    if (a == std::wstring::npos) return L"";
    size_t b = s.find_last_not_of(L" \t\r\n");
    return s.substr(a, b - a + 1);
}
static void ParseRgb(PCWSTR c, const wchar_t* def, int* r, int* g, int* b) {
    const wchar_t* p = (c && *c) ? c : def;
    if (*p == L'#') p++;
    unsigned long v = wcstoul(p, nullptr, 16) & 0xFFFFFF;
    *r = (int)((v >> 16) & 0xFF);
    *g = (int)((v >> 8) & 0xFF);
    *b = (int)(v & 0xFF);
}

// ====================================================================== effect language
enum Var {
    V_X, V_Y, V_U, V_V, V_P, V_D, V_ANG, V_T, V_W, V_H, V_MON, V_SPEED, V_POWER, V_N, V_BW, V_RAD, V_OP,
    V_C1R, V_C1G, V_C1B, V_C2R, V_C2G, V_C2B, V_PI, V_TAU, V_AR, V_AG, V_AB, V_KR, V_KG, V_KB,
    V_R, V_G, V_B, V_A, V_FIRST_TEMP
};
static const int kMaxVars = 64;
static const wchar_t* kVarNames[V_FIRST_TEMP] = {
    L"x", L"y", L"u", L"v", L"p", L"d", L"ang", L"t", L"w", L"h", L"mon", L"speed", L"power", L"n", L"bw", L"rad", L"op",
    L"c1r", L"c1g", L"c1b", L"c2r", L"c2g", L"c2b", L"pi", L"tau", L"ar", L"ag", L"ab", L"kr", L"kg", L"kb",
    L"r", L"g", L"b", L"a"
};

enum NodeOp {
    N_NUM, N_VAR, N_NEG, N_NOT, N_ADD, N_SUB, N_MUL, N_DIV, N_MOD, N_POW,
    N_LT, N_GT, N_LE, N_GE, N_EQ, N_NE, N_AND, N_OR, N_CALL
};
enum Fn {
    F_SIN, F_COS, F_TAN, F_ATAN, F_ATAN2, F_ABS, F_SQRT, F_FLOOR, F_CEIL, F_FRACT, F_ROUND, F_SIGN,
    F_EXP, F_LOG, F_POW, F_MIN, F_MAX, F_CLAMP, F_MIX, F_STEP, F_SMOOTHSTEP, F_MOD, F_SAW, F_TRI, F_SQ,
    F_HR, F_HG, F_HB, F_IF, F_NOISE, F_HASH, F_HASH2, F_NOISE2, F_BAYER
};
struct FnDef { const wchar_t* name; int id; int argc; };
static const FnDef kFns[] = {
    {L"sin", F_SIN, 1}, {L"cos", F_COS, 1}, {L"tan", F_TAN, 1}, {L"atan", F_ATAN, 1}, {L"atan2", F_ATAN2, 2},
    {L"abs", F_ABS, 1}, {L"sqrt", F_SQRT, 1}, {L"floor", F_FLOOR, 1}, {L"ceil", F_CEIL, 1},
    {L"fract", F_FRACT, 1}, {L"round", F_ROUND, 1}, {L"sign", F_SIGN, 1}, {L"exp", F_EXP, 1},
    {L"log", F_LOG, 1}, {L"pow", F_POW, 2}, {L"min", F_MIN, 2}, {L"max", F_MAX, 2},
    {L"clamp", F_CLAMP, 3}, {L"mix", F_MIX, 3}, {L"step", F_STEP, 2}, {L"smoothstep", F_SMOOTHSTEP, 3},
    {L"mod", F_MOD, 2}, {L"saw", F_SAW, 1}, {L"tri", F_TRI, 1}, {L"sq", F_SQ, 1}, {L"hr", F_HR, 1},
    {L"hg", F_HG, 1}, {L"hb", F_HB, 1}, {L"if", F_IF, 3}, {L"noise", F_NOISE, 1},
    {L"hash", F_HASH, 1}, {L"hash2", F_HASH2, 2}, {L"noise2", F_NOISE2, 2}, {L"bayer", F_BAYER, 2},
};

struct Node {
    int op = 0;
    float num = 0;
    int var = 0;
    int fn = 0;
    int a = -1, b = -1, c = -1;
};
struct Stmt { int var; int root; };
struct Program {
    std::vector<Node> nodes;
    std::vector<Stmt> stmts;
    bool usesTime = false, usesAmb = false, usesCin = false;
    int tempCount = 0;
    mutable std::shared_ptr<void> classicPlan;
    mutable std::shared_ptr<void> gpuPlan;
    void Clear() { nodes.clear(); stmts.clear(); usesTime = usesAmb = usesCin = false; tempCount = 0; classicPlan.reset(); gpuPlan.reset(); }
};
static_assert(int(N_CALL)==int(GpuFormula::N_CALL)&&int(N_VAR)==int(GpuFormula::N_VAR)&&int(F_BAYER)==int(GpuFormula::F_BAYER)&&V_R==31&&V_A==34&&V_FIRST_TEMP==35&&kMaxVars==64,"GPU/classic formula layouts must match");
static const void* PrepareClassicBorder(const Program& program,const float* values);
static void RunClassicBorder(const void* prepared,float* values);

struct Parser {
    const wchar_t* s;
    size_t i = 0;
    Program* P;
    std::wstring err;
    std::vector<std::wstring> temps;
    std::vector<int> nodeDepth;
    int recursion = 0;
    struct RecursionScope {
        int& depth;
        explicit RecursionScope(int& value) : depth(value) { ++depth; }
        ~RecursionScope() { --depth; }
    };

    bool Fail(const std::wstring& m) {
        if (err.empty()) err = m + L" (at character " + std::to_wstring(i + 1) + L")";
        return false;
    }
    void Ws() { while (s[i] == L' ' || s[i] == L'\t' || s[i] == L'\r' || s[i] == L'\n') i++; }
    static bool IdStart(wchar_t c) { return (c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z') || c == L'_'; }
    static bool IdChar(wchar_t c) { return IdStart(c) || (c >= L'0' && c <= L'9'); }
    std::wstring Ident() {
        size_t st = i;
        while (IdChar(s[i])) i++;
        return Lower(std::wstring(s + st, i - st));
    }
    int Add(const Node& n) {
        if (!err.empty()) return -1;
        if (P->nodes.size() >= 4096) { Fail(L"Effect exceeds 4096 nodes"); return -1; }
        int depth = 1;
        for (int child : {n.a, n.b, n.c})
            if (child >= 0) depth = std::max(depth, nodeDepth[(size_t)child] + 1);
        if (depth > 128) { Fail(L"Expression exceeds 128 levels"); return -1; }
        P->nodes.push_back(n); nodeDepth.push_back(depth);
        return (int)P->nodes.size() - 1;
    }
    int Bin(int op, int a, int b) { Node n; n.op = op; n.a = a; n.b = b; return Add(n); }
    int Un(int op, int a) { Node n; n.op = op; n.a = a; return Add(n); }

    bool VarIndex(const std::wstring& id, bool assign, int* out) {
        for (int k = 0; k < V_FIRST_TEMP; k++) {
            if (id == kVarNames[k]) {
                if (assign && !(k == V_R || k == V_G || k == V_B || k == V_A))
                    return Fail(L"'" + id + L"' is read-only");
                *out = k;
                return true;
            }
        }
        for (size_t k = 0; k < temps.size(); k++)
            if (temps[k] == id) { *out = V_FIRST_TEMP + (int)k; return true; }
        if (!assign) return Fail(L"Unknown name '" + id + L"'");
        if (V_FIRST_TEMP + (int)temps.size() >= kMaxVars) return Fail(L"Too many variables");
        temps.push_back(id);
        *out = V_FIRST_TEMP + (int)temps.size() - 1;
        return true;
    }

    int Primary() {
        Ws();
        wchar_t c = s[i];
        if (c == L'(') {
            i++;
            int e = Or();
            if (e < 0) return -1;
            Ws();
            if (s[i] != L')') { Fail(L"Missing )"); return -1; }
            i++;
            return e;
        }
        if ((c >= L'0' && c <= L'9') || c == L'.') {
            wchar_t* end = nullptr;
            double v = wcstod(s + i, &end);
            if (end == s + i) { Fail(L"Bad number"); return -1; }
            i = (size_t)(end - s);
            Node n; n.op = N_NUM; n.num = (float)v;
            return Add(n);
        }
        if (IdStart(c)) {
            std::wstring id = Ident();
            Ws();
            if (s[i] == L'(') {
                const FnDef* fd = nullptr;
                for (const FnDef& f : kFns)
                    if (id == f.name) { fd = &f; break; }
                if (!fd) { Fail(L"Unknown function '" + id + L"'"); return -1; }
                i++;
                int args[3] = {-1, -1, -1};
                int argc = 0;
                Ws();
                if (s[i] != L')') {
                    for (;;) {
                        if (argc >= 3) { Fail(L"Too many arguments"); return -1; }
                        int e = Or();
                        if (e < 0) return -1;
                        args[argc++] = e;
                        Ws();
                        if (s[i] == L',') { i++; continue; }
                        break;
                    }
                }
                Ws();
                if (s[i] != L')') { Fail(L"Missing )"); return -1; }
                i++;
                if (argc != fd->argc) { Fail(L"Wrong number of arguments for '" + id + L"'"); return -1; }
                Node n; n.op = N_CALL; n.fn = fd->id; n.a = args[0]; n.b = args[1]; n.c = args[2];
                return Add(n);
            }
            int var;
            if (!VarIndex(id, false, &var)) return -1;
            if (var == V_T) P->usesTime = true;
            if (var >= V_AR && var <= V_AB) P->usesAmb = true;
            if (var >= V_KR && var <= V_KB) P->usesCin = true;
            Node n; n.op = N_VAR; n.var = var;
            return Add(n);
        }
        Fail(L"Unexpected character");
        return -1;
    }
    int Pow() {
        int l = Primary();
        if (l < 0) return -1;
        Ws();
        if (s[i] == L'^') {
            i++;
            int r = Unary();
            if (r < 0) return -1;
            return Bin(N_POW, l, r);
        }
        return l;
    }
    int Unary() {
        if (recursion >= 64) { Fail(L"Expression nesting exceeds 64 levels"); return -1; }
        RecursionScope scope(recursion);
        Ws();
        if (s[i] == L'-') { i++; int e = Unary(); if (e < 0) return -1; return Un(N_NEG, e); }
        if (s[i] == L'!' && s[i + 1] != L'=') { i++; int e = Unary(); if (e < 0) return -1; return Un(N_NOT, e); }
        return Pow();
    }
    int Mul() {
        int l = Unary();
        if (l < 0) return -1;
        for (;;) {
            Ws();
            int op = 0;
            if (s[i] == L'*') op = N_MUL;
            else if (s[i] == L'/') op = N_DIV;
            else if (s[i] == L'%') op = N_MOD;
            else break;
            i++;
            int r = Unary();
            if (r < 0) return -1;
            l = Bin(op, l, r);
        }
        return l;
    }
    int AddE() {
        int l = Mul();
        if (l < 0) return -1;
        for (;;) {
            Ws();
            int op = 0;
            if (s[i] == L'+') op = N_ADD;
            else if (s[i] == L'-') op = N_SUB;
            else break;
            i++;
            int r = Mul();
            if (r < 0) return -1;
            l = Bin(op, l, r);
        }
        return l;
    }
    int Cmp() {
        int l = AddE();
        if (l < 0) return -1;
        Ws();
        int op = 0, len = 0;
        if (s[i] == L'<' && s[i + 1] == L'=') { op = N_LE; len = 2; }
        else if (s[i] == L'>' && s[i + 1] == L'=') { op = N_GE; len = 2; }
        else if (s[i] == L'=' && s[i + 1] == L'=') { op = N_EQ; len = 2; }
        else if (s[i] == L'!' && s[i + 1] == L'=') { op = N_NE; len = 2; }
        else if (s[i] == L'<') { op = N_LT; len = 1; }
        else if (s[i] == L'>') { op = N_GT; len = 1; }
        if (!op) return l;
        i += len;
        int r = AddE();
        if (r < 0) return -1;
        return Bin(op, l, r);
    }
    int And() {
        int l = Cmp();
        if (l < 0) return -1;
        for (;;) {
            Ws();
            if (s[i] == L'&' && s[i + 1] == L'&') {
                i += 2;
                int r = Cmp();
                if (r < 0) return -1;
                l = Bin(N_AND, l, r);
            } else break;
        }
        return l;
    }
    int Or() {
        int l = And();
        if (l < 0) return -1;
        for (;;) {
            Ws();
            if (s[i] == L'|' && s[i + 1] == L'|') {
                i += 2;
                int r = And();
                if (r < 0) return -1;
                l = Bin(N_OR, l, r);
            } else break;
        }
        return l;
    }
    bool Statement() {
        Ws();
        if (!s[i] || s[i] == L';') return true;
        size_t save = i;
        if (IdStart(s[i])) {
            std::wstring id = Ident();
            Ws();
            if (s[i] == L'=' && s[i + 1] != L'=') {
                i++;
                int var;
                if (!VarIndex(id, true, &var)) return false;
                int e = Or();
                if (e < 0) return false;
                P->stmts.push_back({var, e});
                return true;
            }
            i = save;
        }
        return Or() >= 0;
    }
    bool ParseProgram() {
        for (;;) {
            if (!Statement()) return false;
            Ws();
            if (s[i] == L';') { i++; continue; }
            if (!s[i]) break;
            return Fail(L"Unexpected character");
        }
        return true;
    }
};

static bool CompileProgram(const std::wstring& src, Program* out, std::wstring* err) {
    out->Clear();
    if (src.size() > 16384) { *err = L"Effect exceeds 16384 characters"; return false; }
    Parser ps;
    ps.s = src.c_str();
    ps.P = out;
    bool ok = ps.ParseProgram();
    if (!ok) { *err = ps.err; out->Clear(); return false; }
    out->tempCount = (int)ps.temps.size();
    return true;
}

static float Hash01(int i) {
    unsigned n = (unsigned)i;
    n = (n << 13) ^ n;
    n = n * (n * n * 15731u + 789221u) + 1376312589u;
    return (float)(n & 0x7fffffffu) / 2147483648.0f;
}
static float Noise1(float x) {
    int i = (int)floorf(x);
    float f = x - (float)i;
    float u = f * f * (3.f - 2.f * f);
    float a = Hash01(i), b = Hash01(i + 1);
    return a + (b - a) * u;
}
static float Hash2(float x, float y) {
    unsigned a = (unsigned)(int)floorf(x), b = (unsigned)(int)floorf(y);
    unsigned n = a * 374761393u + b * 668265263u;
    n = (n ^ (n >> 13)) * 1274126177u;
    n ^= n >> 16;
    return (float)(n & 0xFFFFFFu) / 16777216.0f;
}
static float Noise2(float x, float y) {
    float fx = floorf(x), fy = floorf(y);
    float tx = x - fx, ty = y - fy;
    float u = tx * tx * (3.f - 2.f * tx), v = ty * ty * (3.f - 2.f * ty);
    float a = Hash2(fx, fy), b = Hash2(fx + 1, fy), c = Hash2(fx, fy + 1), d = Hash2(fx + 1, fy + 1);
    float top = a + (b - a) * u, bot = c + (d - c) * u;
    return top + (bot - top) * v;
}
static float Bayer(float x, float y) {
    static const int m[16] = {0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5};
    int ix = ((int)floorf(x)) & 3, iy = ((int)floorf(y)) & 3;
    return ((float)m[iy * 4 + ix] + 0.5f) / 16.0f;
}

static float Eval(const Program& P, int idx, const float* V);

static float CallFn(const Program& P, const Node& n, const float* V) {
    if (n.fn == F_IF) return Eval(P, n.a, V) != 0.f ? Eval(P, n.b, V) : Eval(P, n.c, V);
    float x = Eval(P, n.a, V);
    float y = n.b >= 0 ? Eval(P, n.b, V) : 0.f;
    float z = n.c >= 0 ? Eval(P, n.c, V) : 0.f;
    switch (n.fn) {
        case F_SIN: return sinf(x);
        case F_COS: return cosf(x);
        case F_TAN: return tanf(x);
        case F_ATAN: return atanf(x);
        case F_ATAN2: return atan2f(x, y);
        case F_ABS: return fabsf(x);
        case F_SQRT: return x > 0 ? sqrtf(x) : 0.f;
        case F_FLOOR: return floorf(x);
        case F_CEIL: return ceilf(x);
        case F_FRACT: return Fract(x);
        case F_ROUND: return roundf(x);
        case F_SIGN: return x > 0 ? 1.f : (x < 0 ? -1.f : 0.f);
        case F_EXP: return expf(x < -60.f ? -60.f : (x > 60.f ? 60.f : x));
        case F_LOG: return x > 0 ? logf(x) : 0.f;
        case F_POW: { float r = powf(x, y); return std::isfinite(r) ? r : 0.f; }
        case F_MIN: return x < y ? x : y;
        case F_MAX: return x > y ? x : y;
        case F_CLAMP: return x < y ? y : (x > z ? z : x);
        case F_MIX: return x + (y - x) * z;
        case F_STEP: return y < x ? 0.f : 1.f;   // step(edge, value)
        case F_SMOOTHSTEP: {
            if (y == x) return z < x ? 0.f : 1.f;
            float k = Clamp01((z - x) / (y - x));
            return k * k * (3.f - 2.f * k);
        }
        case F_MOD: return y == 0.f ? 0.f : x - y * floorf(x / y);
        case F_SAW: return Fract(x);
        case F_TRI: return 1.f - fabsf(2.f * Fract(x) - 1.f);
        case F_SQ: return Fract(x) < 0.5f ? 1.f : 0.f;
        case F_HR: return Clamp01(fabsf(6.f * Fract(x) - 3.f) - 1.f);
        case F_HG: return Clamp01(2.f - fabsf(6.f * Fract(x) - 2.f));
        case F_HB: return Clamp01(2.f - fabsf(6.f * Fract(x) - 4.f));
        case F_NOISE: return Noise1(x);
        case F_HASH: return Hash01((int)floorf(x));
        case F_HASH2: return Hash2(x, y);
        case F_NOISE2: return Noise2(x, y);
        case F_BAYER: return Bayer(x, y);
    }
    return 0.f;
}

static float Eval(const Program& P, int idx, const float* V) {
    const Node& n = P.nodes[idx];
    switch (n.op) {
        case N_NUM: return n.num;
        case N_VAR: return V[n.var];
        case N_NEG: return -Eval(P, n.a, V);
        case N_NOT: return Eval(P, n.a, V) == 0.f ? 1.f : 0.f;
        case N_ADD: return Eval(P, n.a, V) + Eval(P, n.b, V);
        case N_SUB: return Eval(P, n.a, V) - Eval(P, n.b, V);
        case N_MUL: return Eval(P, n.a, V) * Eval(P, n.b, V);
        case N_DIV: { float a = Eval(P, n.a, V), b = Eval(P, n.b, V); return b == 0.f ? 0.f : a / b; }
        case N_MOD: { float a = Eval(P, n.a, V), b = Eval(P, n.b, V); return b == 0.f ? 0.f : a - b * floorf(a / b); }
        case N_POW: { float r = powf(Eval(P, n.a, V), Eval(P, n.b, V)); return std::isfinite(r) ? r : 0.f; }
        case N_LT: return Eval(P, n.a, V) < Eval(P, n.b, V) ? 1.f : 0.f;
        case N_GT: return Eval(P, n.a, V) > Eval(P, n.b, V) ? 1.f : 0.f;
        case N_LE: return Eval(P, n.a, V) <= Eval(P, n.b, V) ? 1.f : 0.f;
        case N_GE: return Eval(P, n.a, V) >= Eval(P, n.b, V) ? 1.f : 0.f;
        case N_EQ: return Eval(P, n.a, V) == Eval(P, n.b, V) ? 1.f : 0.f;
        case N_NE: return Eval(P, n.a, V) != Eval(P, n.b, V) ? 1.f : 0.f;
        case N_AND: return (Eval(P, n.a, V) != 0.f && Eval(P, n.b, V) != 0.f) ? 1.f : 0.f;
        case N_OR: return (Eval(P, n.a, V) != 0.f || Eval(P, n.b, V) != 0.f) ? 1.f : 0.f;
        case N_CALL: return CallFn(P, n, V);
    }
    return 0.f;
}

// ====================================================================== built-in effects (written in the same language as Custom)
struct Preset { const wchar_t* key; const wchar_t* code; };
static const Preset kPresets[] = {
    {L"static", L"r=c1r; g=c1g; b=c1b; a=1"},
    {L"breathing", L"k=0.5+0.5*sin(tau*t*speed*0.5); r=c1r; g=c1g; b=c1b; a=mix(1-power,1,k)"},
    {L"glow", L"k=pow(1-d,1+3*(1-power)); pul=0.85+0.15*sin(tau*t*speed*0.5); r=c1r; g=c1g; b=c1b; a=k*pul"},
    {L"neon", L"core=1-smoothstep(0,2.5/bw,d); halo=pow(1-d,2.5)*power; fl=0.93+0.07*sin(tau*t*speed*2.5)+0.03*noise(t*speed*6); "
              L"r=mix(c1r,1,core*0.85); g=mix(c1g,1,core*0.85); b=mix(c1b,1,core*0.85); a=clamp(core+halo,0,1)*fl"},
    {L"wave", L"k=0.5+0.5*sin(tau*(p*n-t*speed*0.25)); r=mix(c1r,c2r,k); g=mix(c1g,c2g,k); b=mix(c1b,c2b,k); a=mix(1-power*0.8,1,k)"},
    {L"rainbow", L"hu=p*n-t*speed*0.1; r=hr(hu); g=hg(hu); b=hb(hu); a=1"},
    {L"comet", L"q=fract(t*speed*0.25-p*n); e=1+7*(1-power); "
               L"r=mix(c2r,c1r,q); g=mix(c2g,c1g,q); b=mix(c2b,c1b,q); a=pow(1-q,e)*smoothstep(0,0.08,q)*(1-0.55*d*d)"},
    {L"gradient", L"k=0.5+0.5*cos(tau*(p*n-t*speed*0.05)); r=mix(c1r,c2r,k); g=mix(c1g,c2g,k); b=mix(c1b,c2b,k); a=1"},
    {L"aurora", L"hu=0.5+0.18*sin(tau*(p*n*0.5-t*speed*0.05)); wob=0.5+0.5*sin(tau*(p*n-t*speed*0.1)); "
                L"r=hr(hu); g=hg(hu); b=hb(hu); a=mix(1-power*0.7,1,wob)*(1-0.5*d)"},
    {L"heartbeat", L"ph=fract(t*speed*0.4); b1=exp(-pow((ph-0.1)*14,2)); b2=exp(-pow((ph-0.3)*14,2)); k=clamp(b1+0.8*b2,0,1); "
                   L"r=c1r; g=c1g; b=c1b; a=mix(1-power,1,k)"},
    {L"fire", L"ht=1-v; n1=noise2(u*n*6,v*4+t*speed*1.2); n2=noise2(u*n*13+9,v*9+t*speed*2.1); fl=n1*0.62+n2*0.38; "
              L"k=clamp(fl*1.5-ht*(1.15-power*0.6)+0.1,0,1); lv=5; q=clamp(floor(k*lv+bayer(x,y))/lv,0,1); "
              L"r=mix(c1r,c2r,q); g=mix(c1g,c2g,q); b=mix(c1b,c2b,q); a=if(q>0,clamp(0.35+q,0,1),0)"},
    {L"stars", L"cs=max(8,bw*1.6); cx=floor(x/cs); cy=floor(y/cs); h1=hash2(cx,cy); h2=hash2(cx+11,cy+5); h3=hash2(cx+23,cy+41); "
               L"sx=(cx+0.25+0.5*h2)*cs; sy=(cy+0.25+0.5*h3)*cs; rr=bw*(0.1+0.4*h1)+0.6; dd=sqrt((x-sx)^2+(y-sy)^2); "
               L"on=if(h1<n*0.07,1,0); tw=0.45+0.55*sin(tau*(t*speed*0.3*(0.5+h2)+h3*7)); k=(1-smoothstep(0,rr,dd))*tw*on; "
               L"r=mix(c1r,c2r,h2); g=mix(c1g,c2g,h2); b=mix(c1b,c2b,h2); a=clamp(k*1.6,0,1)"},
    {L"wheel", L"hu=ang/tau-t*speed*0.1; r=hr(hu); g=hg(hu); b=hb(hu); a=1"},
    {L"silk", L"k=0.5+0.5*sin(tau*(p*n-t*speed*0.08)+2*sin(tau*(p-t*speed*0.03))); r=mix(c1r,c2r,k); g=mix(c1g,c2g,k); b=mix(c1b,c2b,k); a=0.65+0.35*k"},
    {L"plasma", L"k=0.5+0.25*sin(u*n*12+t*speed)+0.25*sin(v*n*12-t*speed*0.7); r=mix(c1r,c2r,k); g=mix(c1g,c2g,k); b=mix(c1b,c2b,k); a=1-d*0.35"},
    {L"pearls", L"q=fract(p*n-t*speed*0.1); k=exp(-pow((q-0.5)*9,2)); r=mix(c1r,c2r,k); g=mix(c1g,c2g,k); b=mix(c1b,c2b,k); a=(0.15+0.85*k)*(1-d*d*0.6)"},
    {L"mercury-v3", L"s=t*speed; q1=sin(tau*(p*3-s*0.06)+sin(tau*p*5+s*0.4)); q2=sin(tau*(p*4+s*0.045)); q3=sin(tau*(p*7-s*0.025)); k1=0.5+0.5*q1; k2=0.5+0.5*q2; k3=0.5+0.5*q3; k1=k1*k1*k1*k1; k2=k2*k2*k2*k2; k3=k3*k3*k3*k3; f1=0.5+0.5*sin(s*0.22+tau*p*2); f2=0.5+0.5*sin(s*0.17-tau*p*3); f3=0.5+0.5*sin(s*0.13+tau*p*5); k=clamp(k1*f1+k2*f2+k3*f3*0.7,0,1); r=(0.22+0.78*k)*k; g=(0.28+0.72*k)*k; b=(0.36+0.64*k)*k; a=k*(1-0.45*d)"},
    {L"mercury-v6", L"warp=0.055*sin(tau*p*8+t*speed*0.08)+0.025*sin(tau*p*13-t*speed*0.12); q1=sin(tau*((p+warp)*5-t*speed*0.085)); q2=sin(tau*((p-warp)*7+t*speed*0.065)); q3=sin(tau*(p*11-t*speed*0.11)); k1=pow(0.5+0.5*q1,10); k2=pow(0.5+0.5*q2,11); k3=pow(0.5+0.5*q3,16); pulse=pow(0.5+0.5*sin(t*speed*0.28+sin(tau*p*2)),12); k=clamp(k1*0.8+k2*0.8+k3*0.45+pulse*0.65,0,1); flicker=0.7+0.3*noise2(p*30,t*speed*0.5); k=k*flicker; r=(0.25+0.75*k)*k; g=(0.3+0.7*k)*k; b=(0.4+0.6*k)*k; a=k*(1-0.5*d)"},
    {L"shimmer", L"k=pow(0.5+0.5*sin(tau*(p*n-t*speed*0.15)),8); r=mix(c1r,1,k*power); g=mix(c1g,1,k*power); b=mix(c1b,1,k*power); a=0.55+0.45*k"},
    {L"wallpaper", L"r=ar; g=ag; b=ab; a=mix(1,1-d*0.85,power)"},
    {L"ambient", L"r=ar; g=ag; b=ab; a=mix(1,1-d*0.85,power)"},
    {L"cinema", L"r=kr; g=kg; b=kb; a=1-d*(0.2+0.65*power)"},
};

static const wchar_t* PresetCode(const std::wstring& key) {
    for (const Preset& p : kPresets)
        if (key == p.key) return p.code;
    return nullptr;
}

static bool CompileEffectCode(const std::wstring& effect, const std::wstring& custom, Program* out) {
    std::wstring code, err;
    if (effect == L"custom") code = custom;
    else {
        std::wstring selected = effect;
        if (effect == L"music-reactive" || effect == L"timecycle" || effect == L"weather") {
            PCWSTR style = Wh_GetStringSetting(L"MusicReactive.Style");
            selected = (effect == L"timecycle" || effect == L"weather") ? L"mercury-v3" : (style ? style : L"mercury-v3");
            Wh_FreeStringSetting(style);
            if (selected != L"mercury-v3" && selected != L"mercury-v6" &&
                selected != L"wave" && selected != L"neon" &&
                selected != L"rainbow" && selected != L"glow" &&
                selected != L"breathing") selected = L"mercury-v3";
        }
        const wchar_t* pc = PresetCode(selected);
        code = pc ? pc : kPresets[0].code;
    }
    if (!CompileProgram(code, out, &err)) {
        Wh_Log(L"Effect formula error: %s -- using the default effect instead", err.c_str());
        CompileProgram(kPresets[0].code, out, &err);
        return false;
    }
    return true;
}

// ====================================================================== color tables (ambient / cinema) and band renderer
static const int kSeg = 96;
struct ColorTable {
    float c[kSeg * 3];
    bool valid;
    ColorTable() : valid(false) { memset(c, 0, sizeof(c)); }
};

static void LookupTable(const ColorTable& t, float p, float* r, float* g, float* b) {
    p = p - floorf(p);
    float f = p * kSeg - 0.5f;
    int i0 = (int)floorf(f);
    float k = f - (float)i0;
    int i1 = i0 + 1;
    i0 = ((i0 % kSeg) + kSeg) % kSeg;
    i1 = ((i1 % kSeg) + kSeg) % kSeg;
    *r = t.c[i0 * 3] * (1.f - k) + t.c[i1 * 3] * k;
    *g = t.c[i0 * 3 + 1] * (1.f - k) + t.c[i1 * 3 + 1] * k;
    *b = t.c[i0 * 3 + 2] * (1.f - k) + t.c[i1 * 3 + 2] * k;
}

struct BandPx { int idx; float x, y, u, v, p, d, ang, cov; };
struct EvalParams {
    bool timecycle=false;
    bool weather=false;
    float W, H, mon, bw, rad, op, speed, power;
    int count;
    float c1[3], c2[3];
};

// Evaluates the effect for every border pixel and adds it (premultiplied) into bits.
static void RenderBand(DWORD* bits, const std::vector<BandPx>& band, const Program& P, const EvalParams& ep,
                       double t, const ColorTable* amb, const ColorTable* cin) {
    RenderTiming::Scope timing(RenderTiming::Classic);
    float V[kMaxVars];
    memset(V, 0, sizeof(V));
    V[V_T] = (float)t; V[V_W] = ep.W; V[V_H] = ep.H; V[V_MON] = ep.mon; V[V_SPEED] = ep.speed;
    V[V_POWER] = ep.power; V[V_N] = (float)ep.count; V[V_BW] = ep.bw; V[V_RAD] = ep.rad; V[V_OP] = ep.op;
    V[V_C1R] = ep.c1[0]; V[V_C1G] = ep.c1[1]; V[V_C1B] = ep.c1[2];
    V[V_C2R] = ep.c2[0]; V[V_C2G] = ep.c2[1]; V[V_C2B] = ep.c2[2];
    V[V_PI] = 3.14159265f; V[V_TAU] = 6.2831853f;
    bool useAmb = P.usesAmb && amb && amb->valid, useCin = P.usesCin && cin && cin->valid;
    if (P.usesAmb && !useAmb) { V[V_AR] = ep.c1[0]; V[V_AG] = ep.c1[1]; V[V_AB] = ep.c1[2]; }
    if (P.usesCin && !useCin) { V[V_KR] = ep.c1[0]; V[V_KG] = ep.c1[1]; V[V_KB] = ep.c1[2]; }

    const bool prepared=PerformanceOptions::enabled.load()&&PerformanceOptions::framePreparation.load();
    const MusicReactive::Frame musicFrame(true,prepared);
    const LiveWeather::Frame weatherFrame((float)t,ep.weather&&prepared);
    const float musicMultiplier = MusicReactive::Multiplier(true);
    const bool timeCycleActive = ep.timecycle;
    const TimeCycle::State solar = timeCycleActive ? TimeCycle::Get() : TimeCycle::State{1.f,0.f,0.f};
    const float musicSpeed = MusicReactive::Speed(true);
    V[V_T] = (float)(t * musicSpeed);
    const float* gpu=GpuFormula::Evaluate(P,band,V,useAmb?amb->c:nullptr,useCin?cin->c:nullptr,false);
    const void* compiled=gpu?nullptr:PrepareClassicBorder(P,V);
    size_t gpuIndex=0;
    for (const BandPx& b : band) {
        if(gpu){memcpy(&V[V_R],gpu+gpuIndex,4*sizeof(float));gpuIndex+=4;}
        else {
        V[V_X] = b.x; V[V_Y] = b.y; V[V_U] = b.u; V[V_V] = b.v;
        V[V_P] = b.p; V[V_D] = b.d; V[V_ANG] = b.ang;
        V[V_R] = V[V_C1R]; V[V_G] = V[V_C1G]; V[V_B] = V[V_C1B]; V[V_A] = 1.f;
        if (useAmb) LookupTable(*amb, b.p, &V[V_AR], &V[V_AG], &V[V_AB]);
        if (useCin) LookupTable(*cin, b.p, &V[V_KR], &V[V_KG], &V[V_KB]);
        for (int k = 0; k < P.tempCount; k++) V[V_FIRST_TEMP + k] = 0.f;
        if(compiled)RunClassicBorder(compiled,V);
        else for (const Stmt& st : P.stmts) {
            float v = Eval(P, st.root, V);
            if (!std::isfinite(v)) v = 0.f;
            V[st.var] = v;
        }
        }
        float a = Clamp01(V[V_A]) * ep.op * b.cov * musicMultiplier * (prepared?musicFrame.Mask(b.x,b.y,ep.W,ep.H):MusicReactive::Mask(true,b.x,b.y,ep.W,ep.H)) * solar.brightness;
        float r = Clamp01(V[V_R]), g = Clamp01(V[V_G]), bl = Clamp01(V[V_B]);
        if(prepared)musicFrame.Tint(r,g,bl);else MusicReactive::Tint(true,r,g,bl);
        if(timeCycleActive)TimeCycle::Tint(r,g,bl,solar);
        if(ep.weather){if(prepared)weatherFrame.Apply(b.x/std::max(1.f,ep.W),b.y/std::max(1.f,ep.H),r,g,bl,a);
            else LiveWeather::Apply(b.x/std::max(1.f,ep.W),b.y/std::max(1.f,ep.H),(float)t,r,g,bl,a);}
        if(a<=0.f)continue;
        DWORD d = bits[b.idx];
        int da = (int)(d >> 24), dr = (int)((d >> 16) & 255), dg = (int)((d >> 8) & 255), db = (int)(d & 255);
        int na = Imin(255, da + (int)(a * 255.f + 0.5f));
        int nr = Imin(na, dr + (int)(r * a * 255.f + 0.5f));
        int ng = Imin(na, dg + (int)(g * a * 255.f + 0.5f));
        int nb = Imin(na, db + (int)(bl * a * 255.f + 0.5f));
        bits[b.idx] = ((DWORD)na << 24) | ((DWORD)nr << 16) | ((DWORD)ng << 8) | (DWORD)nb;
    }
}
// ====================================================================== ambient (wallpaper) and cinema (screen) color sampling
static HBITMAP MakeDibSimple(HDC ref, int w, int h, void** bits) {
    if (!bits) return nullptr;
    *bits = nullptr;
    // Reject invalid geometry and oversized surfaces before calculating strides.
    if (w <= 0 || h <= 0 || w > 32768 || h > 32768 ||
        (size_t)w * (size_t)h > (256u * 1024u * 1024u) / 4u) return nullptr;
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    return CreateDIBSection(ref, &bi, DIB_RGB_COLORS, bits, nullptr, 0);
}

// Averages the pixels in strips along the edges of region (rx,ry,rw,rh) into kSeg colors around the perimeter
// (clockwise, starting at the top left).
static void BuildTable(ColorTable& out, const BYTE* src, int stride, int rx, int ry, int rw, int rh, float depthFrac) {
    float W = (float)rw, H = (float)rh;
    float perim = 2.f * (W + H);
    float depth = std::max(2.f, std::min(W, H) * depthFrac);
    float segLen = perim / (float)kSeg;
    for (int k = 0; k < kSeg; k++) {
        float s0 = ((float)k + 0.5f) * segLen;
        float cx, cy, hw, hh;
        if (s0 < W) { cx = s0; cy = depth * 0.5f; hw = segLen * 0.5f; hh = depth * 0.5f; }
        else if (s0 < W + H) { cx = W - depth * 0.5f; cy = s0 - W; hw = depth * 0.5f; hh = segLen * 0.5f; }
        else if (s0 < 2.f * W + H) { cx = W - (s0 - W - H); cy = H - depth * 0.5f; hw = segLen * 0.5f; hh = depth * 0.5f; }
        else { cx = depth * 0.5f; cy = H - (s0 - 2.f * W - H); hw = depth * 0.5f; hh = segLen * 0.5f; }
        double sr = 0, sg = 0, sb = 0;
        int cnt = 0;
        for (int iy = 0; iy < 5; iy++) {
            for (int ix = 0; ix < 5; ix++) {
                float fx = cx - hw + ((float)ix + 0.5f) * (2.f * hw) / 5.f;
                float fy = cy - hh + ((float)iy + 0.5f) * (2.f * hh) / 5.f;
                fx = std::max(0.f, std::min(fx, W - 1.f));
                fy = std::max(0.f, std::min(fy, H - 1.f));
                const BYTE* p = src + ((size_t)(ry + (int)fy) * (size_t)stride + (size_t)(rx + (int)fx)) * 4;
                sb += p[0]; sg += p[1]; sr += p[2];
                cnt++;
            }
        }
        out.c[k * 3] = (float)(sr / cnt / 255.0);
        out.c[k * 3 + 1] = (float)(sg / cnt / 255.0);
        out.c[k * 3 + 2] = (float)(sb / cnt / 255.0);
    }
    out.valid = true;
}

static void SmoothInto(ColorTable& dst, const ColorTable& src, float k) {
    if (!dst.valid) { dst = src; return; }
    for (int i = 0; i < kSeg * 3; i++) dst.c[i] += (src.c[i] - dst.c[i]) * k;
}

// ---- wallpaper source ------------------------------------------------------------------
struct WallpaperSrc {
    std::vector<BYTE> px;
    int w = 0, h = 0;
    bool captured = false, ok = false;
    RECT origin = {};
};

static BOOL CALLBACK EnumWallProc(HWND h, LPARAM lp) {
    wchar_t cls[32] = {};
    GetClassNameW(h, cls, 32);
    if (!wcscmp(cls, L"WorkerW") && !FindWindowExW(h, nullptr, L"SHELLDLL_DefView", nullptr)) {
        RECT r;
        if (GetWindowRect(h, &r) && r.right - r.left >= 640 && r.bottom - r.top >= 360) {
            *(HWND*)lp = h;
            return FALSE;
        }
    }
    return TRUE;
}

// PrintWindow capture. GPU-only renderers may not provide printable pixels.
static bool PrintToBuffer(HWND h, std::vector<BYTE>& px, int* w, int* hh, RECT* origin) {
    RECT r;
    if (!GetWindowRect(h, &r)) return false;
    int W = r.right - r.left, H = r.bottom - r.top;
    if (W <= 0 || H <= 0 || (long long)W * H > 40000000LL) return false;
    HDC scr = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(scr);
    void* bits = nullptr;
    HBITMAP bmp = MakeDibSimple(scr, W, H, &bits);
    bool ok = false;
    if (dc && bmp && bits) {
        HGDIOBJ old = SelectObject(dc, bmp);
        memset(bits, 0, (size_t)W * H * 4);
        ok = PrintWindow(h, dc, 2 /* PW_RENDERFULLCONTENT */) != 0;
        GdiFlush();
        if (ok) {
            px.assign((BYTE*)bits, (BYTE*)bits + (size_t)W * H * 4);
            *w = W; *hh = H; *origin = r;
        }
        SelectObject(dc, old);
    }
    if (bmp) DeleteObject(bmp);
    if (dc) DeleteDC(dc);
    ReleaseDC(nullptr, scr);
    return ok;
}

static bool MostlyBlack(const std::vector<BYTE>& px) {
    size_t n = px.size() / 4;
    if (!n) return true;
    unsigned long long sum = 0;
    size_t step = std::max<size_t>(1, n / 4000), cnt = 0;
    for (size_t i = 0; i < n; i += step) { sum += px[i * 4] + px[i * 4 + 1] + px[i * 4 + 2]; cnt++; }
    return (sum / (cnt * 3)) < 3;
}

// GDI+ (loaded dynamically so no extra libraries are needed)
struct GpStartupInput { UINT32 Version; void* DebugCb; BOOL NoThread; BOOL NoCodecs; };
struct GpRectI { INT X, Y, W, H; };
struct GpBmpData { UINT Width; UINT Height; INT Stride; INT Format; void* Scan0; UINT_PTR Reserved; };
typedef int (WINAPI* PGdiplusStartup)(ULONG_PTR*, const GpStartupInput*, void*);
typedef void (WINAPI* PGdiplusShutdown)(ULONG_PTR);
typedef int (WINAPI* PGdipCreateBitmapFromFile)(const WCHAR*, void**);
typedef int (WINAPI* PGdipGetImageDim)(void*, UINT*);
typedef int (WINAPI* PGdipBitmapLockBits)(void*, const GpRectI*, UINT, INT, GpBmpData*);
typedef int (WINAPI* PGdipBitmapUnlockBits)(void*, GpBmpData*);
typedef int (WINAPI* PGdipDisposeImage)(void*);

static HMODULE g_gdip = nullptr;
static ULONG_PTR g_gdipTok = 0;
static bool g_gdipTried = false;
static PGdiplusShutdown pGdipShutdown = nullptr;
static PGdipCreateBitmapFromFile pGdipCreate = nullptr;
static PGdipGetImageDim pGdipW = nullptr, pGdipH = nullptr;
static PGdipBitmapLockBits pGdipLock = nullptr;
static PGdipBitmapUnlockBits pGdipUnlock = nullptr;
static PGdipDisposeImage pGdipDispose = nullptr;

static bool GdipInit() {
    if (g_gdipTried) return g_gdip != nullptr;
    g_gdipTried = true;
    HMODULE m = LoadLibraryW(L"gdiplus.dll");
    if (!m) return false;
    PGdiplusStartup st = (PGdiplusStartup)GetProcAddress(m, "GdiplusStartup");
    pGdipShutdown = (PGdiplusShutdown)GetProcAddress(m, "GdiplusShutdown");
    pGdipCreate = (PGdipCreateBitmapFromFile)GetProcAddress(m, "GdipCreateBitmapFromFile");
    pGdipW = (PGdipGetImageDim)GetProcAddress(m, "GdipGetImageWidth");
    pGdipH = (PGdipGetImageDim)GetProcAddress(m, "GdipGetImageHeight");
    pGdipLock = (PGdipBitmapLockBits)GetProcAddress(m, "GdipBitmapLockBits");
    pGdipUnlock = (PGdipBitmapUnlockBits)GetProcAddress(m, "GdipBitmapUnlockBits");
    pGdipDispose = (PGdipDisposeImage)GetProcAddress(m, "GdipDisposeImage");
    GpStartupInput in = {1, nullptr, FALSE, FALSE};
    if (!st || !pGdipShutdown || !pGdipCreate || !pGdipW || !pGdipH || !pGdipLock || !pGdipUnlock || !pGdipDispose ||
        st(&g_gdipTok, &in, nullptr) != 0) {
        FreeLibrary(m);
        return false;
    }
    g_gdip = m;
    return true;
}

static void GdipShutdownAll() {
    if (g_gdip) {
        if (pGdipShutdown && g_gdipTok) pGdipShutdown(g_gdipTok);
        FreeLibrary(g_gdip);
        g_gdip = nullptr;
    }
    g_gdipTok = 0;
    g_gdipTried = false;
}

// Static wallpaper: read the image file (small thumbnail is enough for color averages).
static bool LoadWallpaperThumb(WallpaperSrc& wp) {
    wchar_t path[MAX_PATH] = {};
    SystemParametersInfoW(SPI_GETDESKWALLPAPER, MAX_PATH, path, 0);
    if (!path[0]) {   // solid color background
        COLORREF c = GetSysColor(COLOR_BACKGROUND);
        wp.w = wp.h = 1;
        wp.px.assign(4, 255);
        wp.px[0] = GetBValue(c); wp.px[1] = GetGValue(c); wp.px[2] = GetRValue(c);
        wp.captured = false;
        return true;
    }
    if (!GdipInit()) return false;
    void* bmp = nullptr;
    if (pGdipCreate(path, &bmp) != 0 || !bmp) return false;
    UINT w = 0, h = 0;
    pGdipW(bmp, &w);
    pGdipH(bmp, &h);
    bool ok = false;
    if (w > 0 && h > 0) {
        GpRectI r = {0, 0, (INT)w, (INT)h};
        GpBmpData bd = {};
        if (pGdipLock(bmp, &r, 1 /* read */, 0x26200A /* 32bppARGB */, &bd) == 0 && bd.Scan0) {
            int tw = Imin((int)w, 320);
            int th = Imax(1, (int)((long long)h * tw / (long long)w));
            wp.px.assign((size_t)tw * th * 4, 0);
            for (int ty = 0; ty < th; ty++) {
                int sy = (int)((long long)ty * h / th);
                for (int tx = 0; tx < tw; tx++) {
                    int sx = (int)((long long)tx * w / tw);
                    const BYTE* s = (const BYTE*)bd.Scan0 + (ptrdiff_t)sy * bd.Stride + (ptrdiff_t)sx * 4;
                    BYTE* d = &wp.px[((size_t)ty * tw + tx) * 4];
                    d[0] = s[0]; d[1] = s[1]; d[2] = s[2]; d[3] = 255;
                }
            }
            pGdipUnlock(bmp, &bd);
            wp.w = tw; wp.h = th; wp.captured = false;
            ok = true;
        }
    }
    pGdipDispose(bmp);
    return ok;
}

// Search only wallpaper renderer windows hosted under the desktop shell.
static BOOL CALLBACK EnumWallpaperRenderer(HWND h, LPARAM lp) {
    DWORD pid = 0; GetWindowThreadProcessId(h, &pid);
    HANDLE ph = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!ph) return TRUE;
    wchar_t path[32768] = {}; DWORD n = 32768;
    bool got = QueryFullProcessImageNameW(ph, 0, path, &n) != 0; CloseHandle(ph);
    if (!got) return TRUE;
    const wchar_t* base = wcsrchr(path, L'\\');
    std::wstring exe = Lower(base ? base+1 : path);
    if (exe == L"wallpaper32.exe" || exe == L"wallpaper64.exe" ||
        exe == L"lively.player.exe" || exe == L"lively.player.mpv.exe" ||
        exe == L"lively.player.cefsharp.exe" || exe == L"mpv.exe") {
        RECT r = {}; if (IsWindowVisible(h) && GetWindowRect(h, &r) &&
            r.right-r.left >= 160 && r.bottom-r.top >= 100)
            ((std::vector<HWND>*)lp)->push_back(h);
    }
    return TRUE;
}
static BOOL CALLBACK EnumWallpaperRoots(HWND h, LPARAM lp) {
    wchar_t cls[64] = {}; GetClassNameW(h, cls, 64);
    if (!wcscmp(cls,L"WorkerW") || !wcscmp(cls,L"Progman"))
        EnumChildWindows(h, EnumWallpaperRenderer, lp);
    return TRUE;
}
static bool GrabWallpaper(WallpaperSrc& wp) {
    RenderTiming::Scope timing(RenderTiming::Wallpaper);
    wp = WallpaperSrc();
    std::vector<HWND> renderers;
    EnumWindows(EnumWallpaperRoots, (LPARAM)&renderers);
    int x = GetSystemMetrics(SM_XVIRTUALSCREEN), y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int W = GetSystemMetrics(SM_CXVIRTUALSCREEN), H = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (!renderers.empty() && W > 0 && H > 0 && (long long)W*H <= 40000000LL) {
        // Initialize uncovered monitors from Windows wallpaper rather than sampling apps.
        WallpaperSrc fallback; LoadWallpaperThumb(fallback);
        wp.px.assign((size_t)W*H*4, 0);
        if (!fallback.px.empty()) for (int yy=0; yy<H; ++yy) for(int xx=0; xx<W; ++xx) {
            size_t a=((size_t)yy*W+xx)*4;
            size_t b=((size_t)(yy*fallback.h/H)*fallback.w+xx*fallback.w/W)*4;
            memcpy(&wp.px[a], &fallback.px[b], 4);
        }
        bool captured = false;
        for (HWND renderer : renderers) {
            std::vector<BYTE> px; int w=0,h=0; RECT r={};
            if (!PrintToBuffer(renderer,px,&w,&h,&r) || MostlyBlack(px)) continue;
            for (int yy=0; yy<h; ++yy) {
                int dy=r.top-y+yy; if (dy<0 || dy>=H) continue;
                int left=Imax(0,x-r.left), right=Imin(w,x+W-r.left);
                if(right>left) memcpy(&wp.px[((size_t)dy*W+r.left-x+left)*4],
                    &px[((size_t)yy*w+left)*4], (size_t)(right-left)*4);
            }
            captured = true;
        }
        if (captured) { wp.w=W; wp.h=H; wp.origin={x,y,x+W,y+H}; wp.captured=wp.ok=true; return true; }
    }
    wp = WallpaperSrc(); wp.ok = LoadWallpaperThumb(wp); return wp.ok;
}

static void MakeAmbientTable(const WallpaperSrc& wp, const RECT& mon, ColorTable& out) {
    if (!wp.ok || wp.px.empty() || wp.w < 2 || wp.h < 2) return;
    ColorTable tmp;
    if (wp.captured) {
        int rx = Imax(0, Imin((int)(mon.left - wp.origin.left), wp.w - 1));
        int ry = Imax(0, Imin((int)(mon.top - wp.origin.top), wp.h - 1));
        int rw = Imin((int)(mon.right - mon.left), wp.w - rx);
        int rh = Imin((int)(mon.bottom - mon.top), wp.h - ry);
        if (rw < 8 || rh < 8) return;
        BuildTable(tmp, wp.px.data(), wp.w, rx, ry, rw, rh, 0.14f);
    } else {
        BuildTable(tmp, wp.px.data(), wp.w, 0, 0, wp.w, wp.h, 0.14f);
    }
    out = tmp;
}

// Cinema: the colors on screen right next to the edge of region r.
struct ScreenSampleBuffer {
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ original = nullptr;
    void* bits = nullptr;
    int width = 0, height = 0;
    void Reset() {
        if (dc && original) SelectObject(dc, original);
        if (bitmap) DeleteObject(bitmap);
        if (dc) DeleteDC(dc);
        dc = nullptr; bitmap = nullptr; original = nullptr; bits = nullptr;
        width = height = 0;
    }
    bool Ensure(HDC screen, int w, int h) {
        if (dc && width >= w && height >= h) return true;
        w = Imax(w, width); h = Imax(h, height);
        Reset();
        dc = CreateCompatibleDC(screen);
        if (!dc) return false;
        bitmap = MakeDibSimple(screen, w, h, &bits);
        if (!bitmap || !bits) { Reset(); return false; }
        original = SelectObject(dc, bitmap);
        if (!original || original == HGDI_ERROR) {
            original = nullptr; Reset(); return false;
        }
        width = w; height = h;
        SetStretchBltMode(dc, HALFTONE);
        SetBrushOrgEx(dc, 0, 0, nullptr);
        return true;
    }
};
static thread_local ScreenSampleBuffer g_screenSample;

static bool SameColors(const ColorTable& a, const ColorTable& b) {
    return a.valid == b.valid && (!a.valid || memcmp(a.c, b.c, sizeof(a.c)) == 0);
}

// Pure sampling stage; no window or GDI calls. Integer capture pixels whose
// footprints overlap the app are rejected, including downsampling at boundaries.
static bool BuildOutsideCinemaTable(ColorTable& out,const BYTE* pixels,int stride,int sw,int sh,
    const RECT& r,int pad,float sx,float sy,int vx,int vy,int vr,int vb){
    float w=(float)(r.right-r.left),h=(float)(r.bottom-r.top),perim=2*(w+h),segment=perim/kSeg;
    ColorTable next;bool any=false;
    for(int segmentId=0;segmentId<kSeg;segmentId++){
        float along=(segmentId+.5f)*segment,cx,cy,hw,hh;int side;
        if(along<w){side=0;cx=along;cy=-pad*.5f;hw=segment*.5f;hh=pad*.5f;}
        else if(along<w+h){side=1;cx=w+pad*.5f;cy=along-w;hw=pad*.5f;hh=segment*.5f;}
        else if(along<2*w+h){side=2;cx=w-(along-w-h);cy=h+pad*.5f;hw=segment*.5f;hh=pad*.5f;}
        else{side=3;cx=-pad*.5f;cy=h-(along-2*w-h);hw=pad*.5f;hh=segment*.5f;}
        double sums[3]={};int count=0;
        for(int iy=0;iy<5;iy++)for(int ix=0;ix<5;ix++){
            float x=cx-hw+(ix+.5f)*2*hw/5,y=cy-hh+(iy+.5f)*2*hh/5;
            int px=(int)floorf((x+pad)*sx),py=(int)floorf((y+pad)*sy);
            if(px<0||px>=sw||py<0||py>=sh)continue;
            float x0=px/sx-pad,x1=(px+1)/sx-pad,y0=py/sy-pad,y1=(py+1)/sy-pad;
            if((side==0&&y1>0)||(side==1&&x0<w)||(side==2&&y0<h)||(side==3&&x1>0))continue;
            if(r.left+x0<vx||r.left+x1>vr||r.top+y0<vy||r.top+y1>vb)continue;
            const BYTE* pixel=pixels+((size_t)py*stride+px)*4;
            sums[0]+=pixel[2];sums[1]+=pixel[1];sums[2]+=pixel[0];++count;
        }
        if(count){for(int channel=0;channel<3;channel++)next.c[segmentId*3+channel]=(float)(sums[channel]/count/255.);any=true;}
        // Unavailable strips remain black; a fully unavailable capture returns false.
        // The caller then keeps its last valid table or configured palette fallback.
    }
    if(!any)return false;next.valid=true;out=next;return true;
}


// The padded capture contains the app in its centre. Sample only the four
// surrounding strips, preserving the original app-perimeter phase/order.
// Out-of-desktop pixels are skipped rather than folded back into the app.
static bool OutsideCinemaTable(const RECT& r,ColorTable& out,float depthFrac,bool* changed){
    int w=(int)(r.right-r.left),h=(int)(r.bottom-r.top);
    if(w<16||h<16)return false;
    int pad=Imax(8,Imin(96,(int)ceilf(std::min(w,h)*fabsf(depthFrac))));
    int cw=w+2*pad,ch=h+2*pad,k=Imax(1,Imax(cw,ch)/240);
    int sw=(cw+k-1)/k,sh=(ch+k-1)/k;
    HDC scr=GetDC(nullptr);if(!scr)return false;
    bool ok=false;
    if(g_screenSample.Ensure(scr,sw,sh)){
        // GDI clips off-desktop source pixels. Clear old captures before reuse.
        memset(g_screenSample.bits,0,(size_t)g_screenSample.width*g_screenSample.height*4);
        if(StretchBlt(g_screenSample.dc,0,0,sw,sh,scr,r.left-pad,r.top-pad,cw,ch,SRCCOPY)){
            GdiFlush();ColorTable tmp;
            float sx=(float)sw/cw,sy=(float)sh/ch;
            int vx=GetSystemMetrics(SM_XVIRTUALSCREEN),vy=GetSystemMetrics(SM_YVIRTUALSCREEN);
            int vr=vx+GetSystemMetrics(SM_CXVIRTUALSCREEN),vb=vy+GetSystemMetrics(SM_CYVIRTUALSCREEN);
            ok=BuildOutsideCinemaTable(tmp,(const BYTE*)g_screenSample.bits,g_screenSample.width,sw,sh,
                r,pad,sx,sy,vx,vy,vr,vb);
            if(ok){if(changed)*changed=!SameColors(out,tmp);out=tmp;}
        }
    }
    ReleaseDC(nullptr,scr);return ok;
}

// Return capture success separately from change detection: unchanged pixels
// must not trigger a fallback, invalidate a frame, or boost adaptive FPS.
static bool ScreenTable(const RECT& r, ColorTable& out, float depthFrac, bool* changed = nullptr) {
    RenderTiming::Scope timing(RenderTiming::Capture);
    if (changed) *changed = false;
    if(depthFrac<0)return OutsideCinemaTable(r,out,-depthFrac,changed);
    int w = (int)(r.right - r.left), h = (int)(r.bottom - r.top);
    if (w < 16 || h < 16) return false;
    int k = Imax(1, Imax(w, h) / 240);
    int sw = (w + k - 1) / k, sh = (h + k - 1) / k;
    HDC scr = GetDC(nullptr);
    if (!scr) return false;
    bool ok = false;
    if (g_screenSample.Ensure(scr, sw, sh) &&
        StretchBlt(g_screenSample.dc, 0, 0, sw, sh, scr, r.left, r.top, w, h, SRCCOPY)) {
        GdiFlush();
        ColorTable tmp;
        BuildTable(tmp, (const BYTE*)g_screenSample.bits, g_screenSample.width, 0, 0, sw, sh, depthFrac);
        if (changed) *changed = !SameColors(out, tmp);
        out = tmp;
        ok = true;
    }
    ReleaseDC(nullptr, scr);
    return ok;
}
static CinemaCapture::Worker<ColorTable> g_cinemaCapture;
static void CleanupCinemaCapture(){g_screenSample.Reset();}
static void InitializeCinemaCapture(){PerformanceOptions::WorkerPriority();SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);}
static void ConfigureCinemaCapture(){g_cinemaCapture.Configure(ScreenTable,CleanupCinemaCapture,InitializeCinemaCapture);}
static bool ReadCinemaSample(UINT_PTR key,const RECT& rect,float depth,ColorTable& out){
    ColorTable latest;if(!g_cinemaCapture.Read(key,rect,depth,latest))return false;
    bool changed=!SameColors(out,latest);out=latest;return changed;
}

// ====================================================================== settings
struct IconCfg {
    GpuStages::Category gpuCategory=GpuStages::DesktopIcons;
    std::wstring effect=L"none";
    Program prog;
    float colors[3]={0,0.47f,0.84f}, color2[3]={1,0,1};
    float speed=1, power=0.7f; int count=2, fps=15;
    bool gray = false, colorize = false, transp = false, hideNames = false, hideArrow = false;
    bool shadow = false, glow = false;
    float hue = 0, sat = 1, contrast = 1, opacity = 1;
    int shadowRgb[3] = {0, 0, 0}, shadowOpacity = 55, shadowBlur = 5, shadowX = 2, shadowY = 3;
    int glowRgb[3] = {0, 191, 255}, glowIntensity = 70, glowBlur = 6, glowOutline = 1;
    int settleMs = 2000;
    bool HasEffects() const { return gray || colorize || transp || shadow || glow || effect!=L"none"; }
    bool NeedsWork() const { return HasEffects() || hideArrow; }
};
struct SelCfg {
    bool desktop = true, explorer = true;
    COLORREF fill = RGB(0, 120, 215), border = RGB(0, 120, 215);
    int fillA = 64, borderA = 255, bw = 1;
    bool blur = false;
    int blurStrength = 10;
    bool styleItems = true;
};
struct BorderCfg {
    bool enable = true;
    bool cinemaOutside = false;
    bool inheritWindhawkExclusions = true;
    int bw = 3, radius = 8;
    std::wstring effect = L"static", custom;
    int c1[3] = {0, 191, 255}, c2[3] = {255, 0, 255};
    float op = 1.f, speed = 1.f, power = 0.7f;
    int count = 2;
    std::wstring inactiveMode = L"solid";
    int inactiveColor[3] = {85, 85, 85};
    bool hideMax = true;
    int fps = 30, sampleRate = 10, ambientSeconds = 5;
    std::wstring sampleMode = L"r10";
    std::wstring fpsMode = L"f30";
    std::vector<std::wstring> rules, excludes;
};
struct MotionCfg {
    bool on = true;
    int lengthMs = 250;
    float strength = 0.5f;
    int blur = 12, copies = 6;
};
struct Settings {
    IconCfg icon, explorerIcon;
    bool explorerIconsEnabled=false;
    SelCfg sel;
    BorderCfg border;
    MotionCfg mb;
};

static Settings g_s;
static CRITICAL_SECTION g_cs;

static IconCfg GetIcon() { EnterCriticalSection(&g_cs); IconCfg s = g_s.icon; LeaveCriticalSection(&g_cs); return s; }
namespace ExplorerIconFx {bool HandlesIcons();}
static IconCfg GetListIcon(bool explorer) { EnterCriticalSection(&g_cs); IconCfg s=explorer?(ExplorerIconFx::HandlesIcons()?IconCfg{}:(g_s.explorerIconsEnabled?g_s.explorerIcon:IconCfg{})):g_s.icon; LeaveCriticalSection(&g_cs);return s; }
static SelCfg GetSel() { EnterCriticalSection(&g_cs); SelCfg s = g_s.sel; LeaveCriticalSection(&g_cs); return s; }
static BorderCfg GetBorderCfg() { EnterCriticalSection(&g_cs); BorderCfg s = g_s.border; LeaveCriticalSection(&g_cs); return s; }
static MotionCfg GetMotion() { EnterCriticalSection(&g_cs); MotionCfg s = g_s.mb; LeaveCriticalSection(&g_cs); return s; }

// processed desktop icons (see the icon section)
struct Cached {
    BYTE* pixels = nullptr;
    HBITMAP bmp = nullptr;
    int w = 0, h = 0;
};
typedef std::tuple<UINT_PTR, int, int, int, int, UINT_PTR, int, int, int, int, int> CacheKey;  // image identity + spatial domain/list/item position
static std::map<CacheKey, Cached> g_cache, g_explorerIconCache;
static double g_desktopCachePhase=-1,g_explorerCachePhase=-1;
static size_t g_explorerCacheBytes=0;
static DWORD g_cacheStamp = 0;
static std::atomic<unsigned> g_iconSourceEpoch{1};

static void ClearCacheLocked(bool animationFrame = false) {
    if (!animationFrame) ++g_iconSourceEpoch;
    for (auto& kv : g_cache)
        if (kv.second.bmp) DeleteObject(kv.second.bmp);
    g_cache.clear();
    for(auto& kv:g_explorerIconCache)if(kv.second.bmp)DeleteObject(kv.second.bmp);
    g_explorerIconCache.clear();g_explorerCacheBytes=0;
    g_desktopCachePhase=g_explorerCachePhase=-1;
    g_cacheStamp = GetTickCount();
}
static void ClearCache(bool animationFrame = false) {
    EnterCriticalSection(&g_cs);
    ClearCacheLocked(animationFrame);
    LeaveCriticalSection(&g_cs);
}

static void AdvanceListIconFrame(bool explorer,double phase){
    EnterCriticalSection(&g_cs);
    auto& stamp=explorer?g_explorerCachePhase:g_desktopCachePhase;
    if(stamp!=phase){
        auto& cache=explorer?g_explorerIconCache:g_cache;
        for(auto& kv:cache)if(kv.second.bmp)DeleteObject(kv.second.bmp);
        cache.clear();if(explorer)g_explorerCacheBytes=0;stamp=phase;
    }
    LeaveCriticalSection(&g_cs);
}

static std::wstring GetStr(const wchar_t* name) {
    PCWSTR v = Wh_GetStringSetting(name);
    std::wstring s = v ? v : L"";
    Wh_FreeStringSetting(v);
    return s;
}

static std::vector<std::wstring> GetStrArray(const wchar_t* base) {
    std::vector<std::wstring> v;
    for (int i = 0; i < 200; i++) {
        std::wstring name = std::wstring(base) + L"[" + std::to_wstring(i) + L"]";
        PCWSTR s = Wh_GetStringSetting(name.c_str());
        bool has = s && *s;
        std::wstring str = s ? s : L"";
        Wh_FreeStringSetting(s);
        if (!has) break;
        v.push_back(str);
    }
    return v;
}

static COLORREF ParseColorRef(const wchar_t* name, const wchar_t* def) {
    PCWSTR c = Wh_GetStringSetting(name);
    int r, g, b;
    ParseRgb(c, def, &r, &g, &b);
    Wh_FreeStringSetting(c);
    return RGB(r, g, b);
}

static float HueOfRgb(float r, float g, float b) {
    float mx = std::max(r, std::max(g, b)), mn = std::min(r, std::min(g, b));
    float d = mx - mn;
    if (d <= 0) return 0;
    float h;
    if (mx == r) h = fmodf((g - b) / d, 6.f);
    else if (mx == g) h = (b - r) / d + 2;
    else h = (r - g) / d + 4;
    h /= 6;
    if (h < 0) h += 1;
    return h;
}

static int IconDelayMs(const std::wstring& k) {
    if (k == L"s05") return 500;
    if (k == L"s1") return 1000;
    if (k == L"s3") return 3000;
    if (k == L"s5") return 5000;
    return 2000;
}

static int ClampI(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

static int ParseFPSValue(const std::wstring& mode) {
    if (mode == L"f10") return 10; if (mode == L"f15") return 15;
    if (mode == L"f30") return 30; if (mode == L"f60") return 60;
    if (mode == L"f75") return 75; if (mode == L"f100") return 100;
    if (mode == L"f120") return 120; if (mode == L"f144") return 144;
    if (mode == L"f200") return 200; if (mode == L"f240") return 240;
    if (mode == L"f360") return 360; if (mode == L"f480") return 480;
    if (mode == L"f500") return 500; if (mode == L"f720") return 720;
    return 30;
}

static int MonitorRefreshHz(HWND hwnd) {
    HMONITOR mon = hwnd ? MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST) : MonitorFromPoint(POINT{0,0}, MONITOR_DEFAULTTOPRIMARY);
    struct RefreshEntry { HMONITOR monitor; DWORD checked; int hz; bool valid; };
    static thread_local RefreshEntry cache[8] = {};
    static thread_local unsigned next = 0;
    DWORD now = GetTickCount();
    RefreshEntry* entry = nullptr;
    for (auto& candidate : cache) if (candidate.valid && candidate.monitor == mon) {
        if (now - candidate.checked < 1000) return candidate.hz;
        entry = &candidate; break;
    }
    if (!entry) entry = &cache[next++ % 8];
    int hz = 60;
    MONITORINFOEXW mi = {}; mi.cbSize = sizeof(mi);
    if (mon && GetMonitorInfoW(mon, &mi)) {
        DEVMODEW dm = {}; dm.dmSize = sizeof(dm);
        if (EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm) && dm.dmDisplayFrequency >= 24 && dm.dmDisplayFrequency <= 1000)
            hz = (int)dm.dmDisplayFrequency;
    }
    *entry = {mon, now, hz, true};
    return hz;
}

static void CacheIconGpuPlan(IconCfg& ic){
    if(ic.effect==L"none"||!GpuFormula::enabled.load()||!GpuStages::targets[ic.gpuCategory].load())return;
    try{ic.prog.gpuPlan=std::make_shared<GpuFormula::Plan>(GpuFormula::Emit(ic.prog));}
    catch(...){ic.prog.gpuPlan.reset();} // CPU parser remains usable.
}

static IconCfg ReadIndependentIconConfig(PCWSTR prefix){IconCfg ic;
    ic.gpuCategory=wcscmp(prefix,L"StartMenuIcons")==0?GpuStages::StartMenuIcons:GpuStages::ExplorerIcons;
    auto key=[&](PCWSTR tail){return std::wstring(prefix)+tail;};
    ic.gray = Wh_GetIntSetting(key(L".Color.IconGrayscale").c_str()) != 0;
    ic.colorize = Wh_GetIntSetting(key(L".Color.IconColorize").c_str()) != 0;
    ic.transp = Wh_GetIntSetting(key(L".Color.IconTransparency").c_str()) != 0;
    {
        PCWSTR c = Wh_GetStringSetting(key(L".Color.IconColorizeColor").c_str());
        int r, g, b;
        ParseRgb(c, L"0078D7", &r, &g, &b);
        Wh_FreeStringSetting(c);
        ic.hue = HueOfRgb(r / 255.f, g / 255.f, b / 255.f);
    }
    ic.sat = (float)ClampI(Wh_GetIntSetting(key(L".Color.IconColorizeSaturation").c_str()), 0, 100) / 100.f;
    ic.contrast = (float)ClampI(Wh_GetIntSetting(key(L".Color.IconColorizeContrast").c_str()), 0, 200) / 100.f;
    ic.opacity = 1.f - (float)ClampI(Wh_GetIntSetting(key(L".Color.IconTransparencyAmount").c_str()), 0, 100) / 100.f;
    ic.shadow = Wh_GetIntSetting(key(L".Shadow.IconShadow").c_str()) != 0;
    ic.glow = Wh_GetIntSetting(key(L".Glow.IconGlow").c_str()) != 0;
    {
        PCWSTR c = Wh_GetStringSetting(key(L".Shadow.IconShadowColor").c_str());
        ParseRgb(c, L"000000", &ic.shadowRgb[0], &ic.shadowRgb[1], &ic.shadowRgb[2]); Wh_FreeStringSetting(c);
        c = Wh_GetStringSetting(key(L".Glow.IconGlowColor").c_str());
        ParseRgb(c, L"00BFFF", &ic.glowRgb[0], &ic.glowRgb[1], &ic.glowRgb[2]); Wh_FreeStringSetting(c);
    }
    ic.shadowOpacity = ClampI(Wh_GetIntSetting(key(L".Shadow.IconShadowOpacity").c_str()), 0, 100);
    ic.shadowBlur = ClampI(Wh_GetIntSetting(key(L".Shadow.IconShadowBlur").c_str()), 0, 16);
    ic.shadowX = ClampI(Wh_GetIntSetting(key(L".Shadow.IconShadowOffsetX").c_str()), -16, 16);
    ic.shadowY = ClampI(Wh_GetIntSetting(key(L".Shadow.IconShadowOffsetY").c_str()), -16, 16);
    ic.glowIntensity = ClampI(Wh_GetIntSetting(key(L".Glow.IconGlowIntensity").c_str()), 0, 100);
    ic.glowBlur = ClampI(Wh_GetIntSetting(key(L".Glow.IconGlowBlur").c_str()), 0, 16);
    ic.glowOutline = ClampI(Wh_GetIntSetting(key(L".Glow.IconGlowOutline").c_str()), 0, 8);
    ic.effect=Lower(GetStr(key(L".Animation.IconEffect").c_str()));
    if(ic.effect.empty()) ic.effect=L"none";
    ic.speed=ClampI(Wh_GetIntSetting(key(L".Animation.IconEffectSpeed").c_str()),1,100)/20.f;
    ic.power=ClampI(Wh_GetIntSetting(key(L".Animation.IconEffectIntensity").c_str()),0,100)/100.f;
    ic.count=ClampI(Wh_GetIntSetting(key(L".Animation.IconEffectCount").c_str()),1,16);
    ic.fps=PerformanceOptions::Cap(ClampI(Wh_GetIntSetting(key(L".Animation.IconEffectFPS").c_str()),1,30));
    { int r,g,b; auto c=GetStr(key(L".Color.IconColorizeColor").c_str()); ParseRgb(c.c_str(),L"0078D7",&r,&g,&b);
      ic.colors[0]=r/255.f;ic.colors[1]=g/255.f;ic.colors[2]=b/255.f;
      c=GetStr(key(L".Animation.IconEffectColor2").c_str()); ParseRgb(c.c_str(),L"FF00FF",&r,&g,&b);
      ic.color2[0]=r/255.f;ic.color2[1]=g/255.f;ic.color2[2]=b/255.f; }
    if(ic.effect!=L"none") CompileEffectCode(ic.effect,GetStr(key(L".Animation.IconCustomEffect").c_str()),&ic.prog);
    CacheIconGpuPlan(ic);


    return ic;
}


static void LoadSettings() {
    Settings s;

    // icons
    IconCfg& ic = s.icon;
    ic.hideNames = Wh_GetIntSetting(L"Icons.Appearance.IconHideNames") != 0;
    ic.hideArrow = Wh_GetIntSetting(L"Icons.Appearance.IconHideArrow") != 0;
    ic.gray = Wh_GetIntSetting(L"Icons.Color.IconGrayscale") != 0;
    ic.colorize = Wh_GetIntSetting(L"Icons.Color.IconColorize") != 0;
    ic.transp = Wh_GetIntSetting(L"Icons.Color.IconTransparency") != 0;
    {
        PCWSTR c = Wh_GetStringSetting(L"Icons.Color.IconColorizeColor");
        int r, g, b;
        ParseRgb(c, L"0078D7", &r, &g, &b);
        Wh_FreeStringSetting(c);
        ic.hue = HueOfRgb(r / 255.f, g / 255.f, b / 255.f);
    }
    ic.sat = (float)ClampI(Wh_GetIntSetting(L"Icons.Color.IconColorizeSaturation"), 0, 100) / 100.f;
    ic.contrast = (float)ClampI(Wh_GetIntSetting(L"Icons.Color.IconColorizeContrast"), 0, 200) / 100.f;
    ic.opacity = 1.f - (float)ClampI(Wh_GetIntSetting(L"Icons.Color.IconTransparencyAmount"), 0, 100) / 100.f;
    ic.shadow = Wh_GetIntSetting(L"Icons.Shadow.IconShadow") != 0;
    ic.glow = Wh_GetIntSetting(L"Icons.Glow.IconGlow") != 0;
    {
        PCWSTR c = Wh_GetStringSetting(L"Icons.Shadow.IconShadowColor");
        ParseRgb(c, L"000000", &ic.shadowRgb[0], &ic.shadowRgb[1], &ic.shadowRgb[2]); Wh_FreeStringSetting(c);
        c = Wh_GetStringSetting(L"Icons.Glow.IconGlowColor");
        ParseRgb(c, L"00BFFF", &ic.glowRgb[0], &ic.glowRgb[1], &ic.glowRgb[2]); Wh_FreeStringSetting(c);
    }
    ic.shadowOpacity = ClampI(Wh_GetIntSetting(L"Icons.Shadow.IconShadowOpacity"), 0, 100);
    ic.shadowBlur = ClampI(Wh_GetIntSetting(L"Icons.Shadow.IconShadowBlur"), 0, 16);
    ic.shadowX = ClampI(Wh_GetIntSetting(L"Icons.Shadow.IconShadowOffsetX"), -16, 16);
    ic.shadowY = ClampI(Wh_GetIntSetting(L"Icons.Shadow.IconShadowOffsetY"), -16, 16);
    ic.glowIntensity = ClampI(Wh_GetIntSetting(L"Icons.Glow.IconGlowIntensity"), 0, 100);
    ic.glowBlur = ClampI(Wh_GetIntSetting(L"Icons.Glow.IconGlowBlur"), 0, 16);
    ic.glowOutline = ClampI(Wh_GetIntSetting(L"Icons.Glow.IconGlowOutline"), 0, 8);
    ic.settleMs = Imax(300, IconDelayMs(GetStr(L"Icons.Appearance.IconRefreshDelay")));
    ic.effect=Lower(GetStr(L"Icons.Animation.IconEffect"));
    if(ic.effect.empty()) ic.effect=L"none";
    ic.speed=ClampI(Wh_GetIntSetting(L"Icons.Animation.IconEffectSpeed"),1,100)/20.f;
    ic.power=ClampI(Wh_GetIntSetting(L"Icons.Animation.IconEffectIntensity"),0,100)/100.f;
    ic.count=ClampI(Wh_GetIntSetting(L"Icons.Animation.IconEffectCount"),1,16);
    ic.fps=PerformanceOptions::Cap(ClampI(Wh_GetIntSetting(L"Icons.Animation.IconEffectFPS"),1,30));
    { int r,g,b; auto c=GetStr(L"Icons.Color.IconColorizeColor"); ParseRgb(c.c_str(),L"0078D7",&r,&g,&b);
      ic.colors[0]=r/255.f;ic.colors[1]=g/255.f;ic.colors[2]=b/255.f;
      c=GetStr(L"Icons.Animation.IconEffectColor2"); ParseRgb(c.c_str(),L"FF00FF",&r,&g,&b);
      ic.color2[0]=r/255.f;ic.color2[1]=g/255.f;ic.color2[2]=b/255.f; }
    if(ic.effect!=L"none") CompileEffectCode(ic.effect,GetStr(L"Icons.Animation.IconCustomEffect"),&ic.prog);
    CacheIconGpuPlan(ic);

    s.explorerIconsEnabled=Wh_GetIntSetting(L"ExplorerIcons.Enabled")!=0;
    s.explorerIcon=ReadIndependentIconConfig(L"ExplorerIcons");

    // selection box
    SelCfg& sc = s.sel;
    sc.desktop = Wh_GetIntSetting(L"Selection.SelDesktop") != 0;
    sc.explorer = Wh_GetIntSetting(L"Selection.SelExplorer") != 0;
    sc.fill = ParseColorRef(L"Selection.SelFillColor", L"0078D7");
    sc.border = ParseColorRef(L"Selection.SelBorderColor", L"0078D7");
    sc.fillA = ClampI(Wh_GetIntSetting(L"Selection.SelFillOpacity"), 0, 100) * 255 / 100;
    sc.borderA = ClampI(Wh_GetIntSetting(L"Selection.SelBorderOpacity"), 0, 100) * 255 / 100;
    sc.bw = ClampI(Wh_GetIntSetting(L"Selection.SelBorderWidth"), 0, 8);
    sc.blur = Wh_GetIntSetting(L"Selection.SelBlur") != 0;
    sc.blurStrength = ClampI(Wh_GetIntSetting(L"Selection.SelBlurStrength"), 1, 30);
    sc.styleItems = Wh_GetIntSetting(L"Selection.SelStyleItems") != 0;

    // app borders
    BorderCfg& bc = s.border;
    bc.enable = Wh_GetIntSetting(L"Borders.Geometry.BorderEnable") != 0;
    bc.bw = ClampI(Wh_GetIntSetting(L"Borders.Geometry.BorderWidth"), 0, 40);
    bc.radius = ClampI(Wh_GetIntSetting(L"Borders.Geometry.BorderRadius"), 0, 60);
    bc.effect = Lower(GetStr(L"Borders.Effects.BorderEffect"));
    bc.cinemaOutside = Wh_GetIntSetting(L"Borders.Effects.CinemaOutside") != 0;
    bc.custom = GetStr(L"Borders.Effects.BorderCustom");
    {
        PCWSTR c = Wh_GetStringSetting(L"Borders.Effects.BorderColor1");
        ParseRgb(c, L"00BFFF", &bc.c1[0], &bc.c1[1], &bc.c1[2]);
        Wh_FreeStringSetting(c);
        c = Wh_GetStringSetting(L"Borders.Effects.BorderColor2");
        ParseRgb(c, L"FF00FF", &bc.c2[0], &bc.c2[1], &bc.c2[2]);
        Wh_FreeStringSetting(c);
        c = Wh_GetStringSetting(L"Borders.Inactive.InactiveColor");
        ParseRgb(c, L"555555", &bc.inactiveColor[0], &bc.inactiveColor[1], &bc.inactiveColor[2]);
        Wh_FreeStringSetting(c);
    }
    bc.op = (float)ClampI(Wh_GetIntSetting(L"Borders.Effects.BorderOpacity"), 1, 100) / 100.f;
    bc.speed = (float)ClampI(Wh_GetIntSetting(L"Borders.Effects.EffectSpeed"), 1, 100) / 20.f;
    bc.power = (float)ClampI(Wh_GetIntSetting(L"Borders.Effects.EffectIntensity"), 0, 100) / 100.f;
    bc.count = ClampI(Wh_GetIntSetting(L"Borders.Effects.EffectCount"), 1, 16);
    bc.inactiveMode = Lower(GetStr(L"Borders.Inactive.InactiveMode"));
    bc.hideMax = Wh_GetIntSetting(L"Borders.Geometry.HideMaximized") != 0;
    {
        std::wstring f = GetStr(L"Borders.Timing.AnimationFPS");
        bc.fpsMode = f.empty() ? L"f30" : f;
        bc.fps = ParseFPSValue(bc.fpsMode);
        std::wstring sr = GetStr(L"Borders.Timing.SampleRate");
        bc.sampleMode = sr;
        bc.sampleRate = CinemaSampling::FixedRate(sr);
    { auto a = GetStr(L"Borders.Timing.AmbientInterval"); int n = a.size()>1 ? _wtoi(a.c_str()+1) : 5; bc.ambientSeconds = (n==2||n==5||n==10||n==20||n==30||n==60||n==120||n==240) ? n : 5; }
    }
    bc.rules = GetStrArray(L"Borders.Rules.AppRules");
    bc.excludes = GetStrArray(L"Borders.Rules.AppExclude");
    bc.inheritWindhawkExclusions = Wh_GetIntSetting(L"Borders.Rules.InheritWindhawkExclusions") != 0;

    // motion blur
    MotionCfg& mb = s.mb;
    mb.on = Wh_GetIntSetting(L"Borders.Motion.MotionBlur") != 0;
    Wh_Log(L"Motion blur settings: Borders.Motion.MotionBlur=%d", mb.on ? 1 : 0);
    mb.lengthMs = ClampI(Wh_GetIntSetting(L"Borders.Motion.TrailLength"), 50, 1500);
    mb.strength = (float)ClampI(Wh_GetIntSetting(L"Borders.Motion.TrailStrength"), 1, 100) / 100.f;
    mb.blur = ClampI(Wh_GetIntSetting(L"Borders.Motion.TrailBlur"), 1, 60);
    mb.copies = ClampI(Wh_GetIntSetting(L"Borders.Motion.TrailCopies"), 2, 12);

    EnterCriticalSection(&g_cs);
    g_s = s;
    ClearCacheLocked();
    LeaveCriticalSection(&g_cs);
}

// ====================================================================== graphics and blur helpers
static HBITMAP MakeDib(HDC ref, int w, int h, void** bits) { return MakeDibSimple(ref, w, h, bits); }

struct TmpDib {
    HDC dc = nullptr;
    HBITMAP bmp = nullptr;
    HGDIOBJ old = nullptr;
    void* bits = nullptr;
    int w = 0, h = 0;
};
static void FreeDib(TmpDib& t) {
    if (t.dc) { SelectObject(t.dc, t.old); DeleteDC(t.dc); }
    if (t.bmp) DeleteObject(t.bmp);
    t = TmpDib();
}
static bool EnsureDib(TmpDib& t, HDC ref, int w, int h) {
    if (t.dc && t.w >= w && t.h >= h) return true;
    FreeDib(t);
    int nw = (w + 127) & ~127, nh = (h + 127) & ~127;
    void* bits = nullptr;
    HBITMAP b = MakeDib(ref, nw, nh, &bits);
    HDC dc = CreateCompatibleDC(ref);
    if (!b || !bits || !dc) {
        if (b) DeleteObject(b);
        if (dc) DeleteDC(dc);
        return false;
    }
    t.old = SelectObject(dc, b);
    t.dc = dc;
    t.bmp = b;
    t.bits = bits;
    t.w = nw;
    t.h = nh;
    return true;
}

static double Now() {
    static LARGE_INTEGER freq = {};
    static LARGE_INTEGER start = {};
    if (!freq.QuadPart) { QueryPerformanceFrequency(&freq); QueryPerformanceCounter(&start); }
    LARGE_INTEGER n;
    QueryPerformanceCounter(&n);
    return (double)(n.QuadPart - start.QuadPart) / (double)freq.QuadPart;
}

static void FlushDwm() {
    typedef HRESULT (WINAPI* Fn)();
    static Fn f = nullptr;
    static bool tried = false;
    if (!tried) {
        tried = true;
        HMODULE m = LoadLibraryW(L"dwmapi.dll");
        if (m) f = (Fn)GetProcAddress(m, "DwmFlush");
    }
    if (f) f();
}

// 3-channel box blurs (BGR, alpha forced to 255) for the selection box
static void BoxBlurH(const BYTE* in, BYTE* out, int w, int h, int r) {
    for (int y = 0; y < h; y++) {
        const BYTE* row = in + (size_t)y * w * 4;
        BYTE* orow = out + (size_t)y * w * 4;
        int sb = 0, sg = 0, sr = 0, cnt = 0;
        int hi = Imin(r, w - 1);
        for (int x = 0; x <= hi; x++) { sb += row[x * 4]; sg += row[x * 4 + 1]; sr += row[x * 4 + 2]; cnt++; }
        for (int x = 0; x < w; x++) {
            orow[x * 4] = (BYTE)(sb / cnt); orow[x * 4 + 1] = (BYTE)(sg / cnt); orow[x * 4 + 2] = (BYTE)(sr / cnt); orow[x * 4 + 3] = 255;
            int add = x + r + 1, rem = x - r;
            if (add < w) { sb += row[add * 4]; sg += row[add * 4 + 1]; sr += row[add * 4 + 2]; cnt++; }
            if (rem >= 0) { sb -= row[rem * 4]; sg -= row[rem * 4 + 1]; sr -= row[rem * 4 + 2]; cnt--; }
        }
    }
}
static void BoxBlurV(const BYTE* in, BYTE* out, int w, int h, int r) {
    for (int x = 0; x < w; x++) {
        int sb = 0, sg = 0, sr = 0, cnt = 0;
        int hi = Imin(r, h - 1);
        for (int y = 0; y <= hi; y++) { const BYTE* p = in + ((size_t)y * w + x) * 4; sb += p[0]; sg += p[1]; sr += p[2]; cnt++; }
        for (int y = 0; y < h; y++) {
            BYTE* o = out + ((size_t)y * w + x) * 4;
            o[0] = (BYTE)(sb / cnt); o[1] = (BYTE)(sg / cnt); o[2] = (BYTE)(sr / cnt); o[3] = 255;
            int add = y + r + 1, rem = y - r;
            if (add < h) { const BYTE* p = in + ((size_t)add * w + x) * 4; sb += p[0]; sg += p[1]; sr += p[2]; cnt++; }
            if (rem >= 0) { const BYTE* p = in + ((size_t)rem * w + x) * 4; sb -= p[0]; sg -= p[1]; sr -= p[2]; cnt--; }
        }
    }
}
static void BlurSmall(BYTE* buf, int w, int h, int r) {
    std::vector<BYTE> tmp((size_t)w * h * 4);
    BoxBlurH(buf, tmp.data(), w, h, r);
    BoxBlurV(tmp.data(), buf, w, h, r);
    BoxBlurH(buf, tmp.data(), w, h, r);
    BoxBlurV(tmp.data(), buf, w, h, r);
}
static void Downscale(const BYTE* S, int stridepx, int w, int h, int k, BYTE* A, int sw, int sh) {
    int step = Imax(1, k / 4);
    for (int sy = 0; sy < sh; sy++) {
        for (int sx = 0; sx < sw; sx++) {
            int xa = sx * k, xb = Imin(w, xa + k), ya = sy * k, yb = Imin(h, ya + k);
            int cb = 0, cg = 0, cr = 0, n = 0;
            for (int y = ya; y < yb; y += step)
                for (int x = xa; x < xb; x += step) {
                    const BYTE* p = S + ((size_t)y * stridepx + x) * 4;
                    cb += p[0]; cg += p[1]; cr += p[2]; n++;
                }
            if (n == 0) n = 1;
            BYTE* d = A + ((size_t)sy * sw + sx) * 4;
            d[0] = (BYTE)(cb / n); d[1] = (BYTE)(cg / n); d[2] = (BYTE)(cr / n); d[3] = 255;
        }
    }
}

// 4-channel (premultiplied RGBA) blur used for the motion blur sprite
static void Box4H(const BYTE* in, BYTE* out, int w, int h, int r) {
    for (int y = 0; y < h; y++) {
        const BYTE* row = in + (size_t)y * w * 4;
        BYTE* orow = out + (size_t)y * w * 4;
        int s[4] = {0, 0, 0, 0}, cnt = 0;
        int hi = Imin(r, w - 1);
        for (int x = 0; x <= hi; x++) { for (int c = 0; c < 4; c++) s[c] += row[x * 4 + c]; cnt++; }
        for (int x = 0; x < w; x++) {
            for (int c = 0; c < 4; c++) orow[x * 4 + c] = (BYTE)(s[c] / cnt);
            int add = x + r + 1, rem = x - r;
            if (add < w) { for (int c = 0; c < 4; c++) s[c] += row[add * 4 + c]; cnt++; }
            if (rem >= 0) { for (int c = 0; c < 4; c++) s[c] -= row[rem * 4 + c]; cnt--; }
        }
    }
}
static void Box4V(const BYTE* in, BYTE* out, int w, int h, int r) {
    for (int x = 0; x < w; x++) {
        int s[4] = {0, 0, 0, 0}, cnt = 0;
        int hi = Imin(r, h - 1);
        for (int y = 0; y <= hi; y++) { const BYTE* p = in + ((size_t)y * w + x) * 4; for (int c = 0; c < 4; c++) s[c] += p[c]; cnt++; }
        for (int y = 0; y < h; y++) {
            BYTE* o = out + ((size_t)y * w + x) * 4;
            for (int c = 0; c < 4; c++) o[c] = (BYTE)(s[c] / cnt);
            int add = y + r + 1, rem = y - r;
            if (add < h) { const BYTE* p = in + ((size_t)add * w + x) * 4; for (int c = 0; c < 4; c++) s[c] += p[c]; cnt++; }
            if (rem >= 0) { const BYTE* p = in + ((size_t)rem * w + x) * 4; for (int c = 0; c < 4; c++) s[c] -= p[c]; cnt--; }
        }
    }
}

// Blurs a premultiplied RGBA image in place (shrink, blur, bilinear enlarge).
static void BlurRGBA(std::vector<BYTE>& img, int w, int h, int radius) {
    if (radius < 1 || w < 4 || h < 4) return;
    int k = Imax(1, Imin(8, radius / 3));
    int sw = (w + k - 1) / k, sh = (h + k - 1) / k;
    std::vector<BYTE> A((size_t)sw * sh * 4), B((size_t)sw * sh * 4);
    for (int sy = 0; sy < sh; sy++) {
        for (int sx = 0; sx < sw; sx++) {
            int xa = sx * k, xb = Imin(w, xa + k), ya = sy * k, yb = Imin(h, ya + k);
            int s[4] = {0, 0, 0, 0}, n = 0;
            for (int y = ya; y < yb; y++)
                for (int x = xa; x < xb; x++) {
                    const BYTE* p = &img[((size_t)y * w + x) * 4];
                    for (int c = 0; c < 4; c++) s[c] += p[c];
                    n++;
                }
            if (n == 0) n = 1;
            BYTE* d = &A[((size_t)sy * sw + sx) * 4];
            for (int c = 0; c < 4; c++) d[c] = (BYTE)(s[c] / n);
        }
    }
    int r = Imax(1, radius / k);
    Box4H(A.data(), B.data(), sw, sh, r);
    Box4V(B.data(), A.data(), sw, sh, r);
    Box4H(A.data(), B.data(), sw, sh, r);
    Box4V(B.data(), A.data(), sw, sh, r);

    std::vector<int> x0(w), x1(w), wx(w);
    for (int x = 0; x < w; x++) {
        float fx = ((float)x + 0.5f) / (float)k - 0.5f;
        if (fx < 0) fx = 0;
        int ix = (int)fx;
        if (ix >= sw - 1) { x0[x] = x1[x] = sw - 1; wx[x] = 0; }
        else { x0[x] = ix; x1[x] = ix + 1; wx[x] = (int)((fx - (float)ix) * 256.f); }
    }
    for (int y = 0; y < h; y++) {
        float fy = ((float)y + 0.5f) / (float)k - 0.5f;
        if (fy < 0) fy = 0;
        int iy = (int)fy, ya, yb, wy;
        if (iy >= sh - 1) { ya = yb = sh - 1; wy = 0; }
        else { ya = iy; yb = iy + 1; wy = (int)((fy - (float)iy) * 256.f); }
        const BYTE* r0 = &A[(size_t)ya * sw * 4];
        const BYTE* r1 = &A[(size_t)yb * sw * 4];
        BYTE* o = &img[(size_t)y * w * 4];
        for (int x = 0; x < w; x++)
            for (int c = 0; c < 4; c++) {
                int top = r0[x0[x] * 4 + c] * (256 - wx[x]) + r0[x1[x] * 4 + c] * wx[x];
                int bot = r1[x0[x] * 4 + c] * (256 - wx[x]) + r1[x1[x] * 4 + c] * wx[x];
                o[x * 4 + c] = (BYTE)((top * (256 - wy) + bot * wy) >> 16);
            }
    }
}

// ====================================================================== desktop: icon colors
static float Hue2Rgb(float p, float q, float t) {
    if (t < 0) t += 1;
    if (t > 1) t -= 1;
    if (t < 1.f / 6) return p + (q - p) * 6 * t;
    if (t < 0.5f) return q;
    if (t < 2.f / 3) return p + (q - p) * (2.f / 3 - t) * 6;
    return p;
}
static void HslToRgb(float h, float s, float l, float* r, float* g, float* b) {
    if (s <= 0) { *r = *g = *b = l; return; }
    float q = l < 0.5f ? l * (1 + s) : l + s - l * s;
    float p = 2 * l - q;
    *r = Hue2Rgb(p, q, h + 1.f / 3);
    *g = Hue2Rgb(p, q, h);
    *b = Hue2Rgb(p, q, h - 1.f / 3);
}

// Input: straight-alpha BGRA. Output: premultiplied BGRA. Order: black-white, colorize, transparency.
struct IconSpace {
    float x=0,y=0,scaleX=1,scaleY=1,w=1,h=1;
    double time=0;
};
static double IconFrameTime(int fps) { double f=(double)Imax(1,fps); return floor(Now()*f)/f; }
static bool GpuIconFormula(const IconCfg& s,int n,int width,int height,const IconSpace* space,float effectTime,std::vector<float>& output){
    if(s.effect==L"none"||n<=0||width<=0||height<=0||!GpuStages::Allowed(s.gpuCategory,(size_t)n))return false;
    float values[kMaxVars]={};
    const float W=space?space->w:(float)width,H=space?space->h:(float)height;
    values[V_T]=effectTime;values[V_W]=W;values[V_H]=H;
    values[V_BW]=(float)width;values[V_N]=(float)s.count;values[V_PI]=3.14159265f;values[V_TAU]=6.2831853f;
    values[V_SPEED]=s.speed;values[V_POWER]=s.power;values[V_OP]=s.opacity;
    for(int k=0;k<3;k++){values[V_C1R+k]=s.colors[k];values[V_C2R+k]=s.color2[k];}
    return GpuStages::Compute(s.gpuCategory,(size_t)n,values,[&]()->const GpuFormula::Plan*{
        // Icon settings are copied to renderers; plan access is serialized by the pool.
        if(!s.prog.gpuPlan)s.prog.gpuPlan=std::make_shared<GpuFormula::Plan>(GpuFormula::Emit(s.prog));
        return static_cast<const GpuFormula::Plan*>(s.prog.gpuPlan.get());
    },[&](std::vector<GpuFormula::Geometry>& work){
        for(int i=0;i<n;i++){
            float x=(float)(i%width),y=(float)(i/width);
            if(space){x=space->x+x*space->scaleX;y=space->y+y*space->scaleY;}
            const float u=x/std::max(1.f,W-1),v=y/std::max(1.f,H-1);
            work[i]={x,y,u,v,u,0,atan2f(v-.5f,u-.5f),1};
        }
    },[&](const float* result){output.assign(result,result+(size_t)n*4);});
}
static void ProcessBuffer(BYTE* p, int n, const IconCfg& s, int width, int height, const IconSpace* space=nullptr) {
    RenderTiming::Scope timing(RenderTiming::Icons);
    BYTE lutR[256], lutG[256], lutB[256];
    if (s.colorize) {
        for (int i = 0; i < 256; i++) {
            float l = (float)i / 255.f;
            l = Clamp01((l - 0.5f) * s.contrast + 0.5f);
            float r, g, b;
            HslToRgb(s.hue, s.sat, l, &r, &g, &b);
            lutR[i] = (BYTE)(Clamp01(r) * 255 + 0.5f);
            lutG[i] = (BYTE)(Clamp01(g) * 255 + 0.5f);
            lutB[i] = (BYTE)(Clamp01(b) * 255 + 0.5f);
        }
    }
    const float effectTime=(float)(space?space->time:IconFrameTime(s.fps));
    std::vector<float> gpuColors;
    const bool gpu=GpuIconFormula(s,n,width,height,space,effectTime,gpuColors);
    int op = (int)(s.opacity * 256 + 0.5f);
    for (int i = 0; i < n; i++) {
        BYTE* q = p + i * 4;
        int a = q[3];
        if (a == 0) { q[0] = q[1] = q[2] = 0; continue; }
        int b = q[0], g = q[1], r = q[2];
        if (s.gray) { int l = (r * 77 + g * 150 + b * 29) >> 8; r = g = b = l; }
        if (s.colorize) { int l = (r * 77 + g * 150 + b * 29) >> 8; r = lutR[l]; g = lutG[l]; b = lutB[l]; }
        if(s.effect!=L"none") {
            float V[kMaxVars]={};
            if(!gpu){
                float x=(float)(i%width), y=(float)(i/width);
                float W=(float)width,H=(float)height;
                if(space){x=space->x+x*space->scaleX;y=space->y+y*space->scaleY;W=space->w;H=space->h;}
                V[V_T]=effectTime; V[V_X]=x;V[V_Y]=y; V[V_W]=W;V[V_H]=H;
                V[V_U]=x/std::max(1.f,W-1);V[V_V]=y/std::max(1.f,H-1);
                V[V_ANG]=atan2f(V[V_V]-0.5f,V[V_U]-0.5f);
                V[V_P]=V[V_U];V[V_D]=0;V[V_BW]=(float)width;V[V_N]=(float)s.count;
                V[V_PI]=3.14159265f;V[V_TAU]=6.2831853f;V[V_SPEED]=s.speed;V[V_POWER]=s.power;V[V_OP]=s.opacity;
                for(int k=0;k<3;++k) {V[V_C1R+k]=s.colors[k];V[V_C2R+k]=s.color2[k];}
                V[V_R]=s.colors[0];V[V_G]=s.colors[1];V[V_B]=s.colors[2];V[V_A]=1;
            }
            if(gpu){for(int k=0;k<4;k++)V[V_R+k]=gpuColors[(size_t)i*4+k];}
            else for(const auto& st:s.prog.stmts) {float v=Eval(s.prog,st.root,V); V[st.var]=std::isfinite(v)?v:0;}
            float shade=(r*0.299f+g*0.587f+b*0.114f)/255.f;
            r=(int)(Clamp01(V[V_R])*shade*255);g=(int)(Clamp01(V[V_G])*shade*255);b=(int)(Clamp01(V[V_B])*shade*255);
            a=(int)(a*Clamp01(V[V_A]));
        }
        if (s.transp) a = (a * op) >> 8;
        q[0] = (BYTE)((b * a + 127) / 255);
        q[1] = (BYTE)((g * a + 127) / 255);
        q[2] = (BYTE)((r * a + 127) / 255);
        q[3] = (BYTE)a;
    }
}

static void BlurAlphaPlane(std::vector<BYTE>& a, int w, int h, int radius) {
    if (radius <= 0 || w <= 0 || h <= 0) return;
    std::vector<BYTE> tmp(a.size());
    for (int y = 0; y < h; ++y) {
        int sum = 0;
        for (int x = -radius; x <= radius; ++x) if (x >= 0 && x < w) sum += a[(size_t)y * w + x];
        for (int x = 0; x < w; ++x) {
            int n = Imin(w - 1, x + radius) - Imax(0, x - radius) + 1;
            tmp[(size_t)y * w + x] = (BYTE)(sum / Imax(1, n));
            int rm = x - radius, add = x + radius + 1;
            if (rm >= 0) sum -= a[(size_t)y * w + rm];
            if (add < w) sum += a[(size_t)y * w + add];
        }
    }
    for (int x = 0; x < w; ++x) {
        int sum = 0;
        for (int y = -radius; y <= radius; ++y) if (y >= 0 && y < h) sum += tmp[(size_t)y * w + x];
        for (int y = 0; y < h; ++y) {
            int n = Imin(h - 1, y + radius) - Imax(0, y - radius) + 1;
            a[(size_t)y * w + x] = (BYTE)(sum / Imax(1, n));
            int rm = y - radius, add = y + radius + 1;
            if (rm >= 0) sum -= tmp[(size_t)rm * w + x];
            if (add < h) sum += tmp[(size_t)add * w + x];
        }
    }
}

static void DilateAlphaPlane(std::vector<BYTE>& a, int w, int h, int radius) {
    if (radius <= 0 || w <= 0 || h <= 0) return;
    std::vector<BYTE> tmp(a.size());
    for (int y = 0; y < h; ++y) {
        BYTE mx = 0;
        for (int x = 0; x < w; ++x) {
            mx = 0;
            int lo = Imax(0, x - radius), hi = Imin(w - 1, x + radius);
            for (int xx = lo; xx <= hi; ++xx) mx = (BYTE)Imax(mx, a[(size_t)y * w + xx]);
            tmp[(size_t)y * w + x] = mx;
        }
    }
    for (int y = 0; y < h; ++y) for (int x = 0; x < w; ++x) {
        BYTE mx = 0;
        for (int yy = Imax(0, y - radius); yy <= Imin(h - 1, y + radius); ++yy)
            mx = (BYTE)Imax(mx, tmp[(size_t)yy * w + x]);
        a[(size_t)y * w + x] = mx;
    }
}

static bool BuildProcessed(HICON icon, const IconCfg& s, Cached* out, const IconSpace* space=nullptr) {
    ICONINFO ii;
    if (!GetIconInfo(icon, &ii)) return false;
    bool ok = false;
    if (ii.hbmColor) {
        BITMAP bm = {};
        GetObjectW(ii.hbmColor, sizeof(bm), &bm);
        int w = bm.bmWidth, h = bm.bmHeight;
        if (w > 0 && h > 0 && w <= 1024 && h <= 1024) {
            std::vector<BYTE> buf((size_t)w * h * 4);
            BITMAPINFO bi = {};
            bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bi.bmiHeader.biWidth = w; bi.bmiHeader.biHeight = -h;
            bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
            HDC dc = GetDC(nullptr);
            if (GetDIBits(dc, ii.hbmColor, 0, h, buf.data(), &bi, DIB_RGB_COLORS)) {
                bool hasAlpha = false;
                for (size_t i = 0; i < (size_t)w * h; i++) if (buf[i * 4 + 3]) { hasAlpha = true; break; }
                if (!hasAlpha && ii.hbmMask) {
                    std::vector<BYTE> mb((size_t)w * h * 4);
                    if (GetDIBits(dc, ii.hbmMask, 0, h, mb.data(), &bi, DIB_RGB_COLORS))
                        for (size_t i = 0; i < (size_t)w * h; i++) buf[i * 4 + 3] = mb[i * 4] ? 0 : 255;
                }
                std::vector<BYTE> mask((size_t)w * h);
                for (size_t i = 0; i < mask.size(); ++i) mask[i] = buf[i * 4 + 3];
                ProcessBuffer(buf.data(), w * h, s, w, h, space); // grayscale/colorizer/transparency first

                int glowPad = s.glow ? s.glowBlur * 2 + s.glowOutline + 1 : 0;
                int shadowPad = s.shadow ? s.shadowBlur * 2 + Imax(abs(s.shadowX), abs(s.shadowY)) + 1 : 0;
                int pad = Imax(glowPad, shadowPad);
                int ow = w + pad * 2, oh = h + pad * 2;
                if (ow <= 2048 && oh <= 2048) {
                    std::vector<BYTE> glowMask((size_t)ow * oh, 0), shadowMask((size_t)ow * oh, 0);
                    for (int y = 0; y < h; ++y) for (int x = 0; x < w; ++x) {
                        BYTE a = mask[(size_t)y * w + x];
                        glowMask[(size_t)(y + pad) * ow + x + pad] = a;
                        shadowMask[(size_t)(y + pad) * ow + x + pad] = a;
                    }
                    if (s.glow && s.glowOutline > 0) DilateAlphaPlane(glowMask, ow, oh, s.glowOutline);
                    if (s.glow) BlurAlphaPlane(glowMask, ow, oh, s.glowBlur);
                    if (s.shadow) BlurAlphaPlane(shadowMask, ow, oh, s.shadowBlur);

                    std::vector<BYTE> outbuf((size_t)ow * oh * 4, 0);
                    for (int y = 0; y < oh; ++y) for (int x = 0; x < ow; ++x) {
                        size_t oi = ((size_t)y * ow + x) * 4;
                        int ba = 0, bb = 0, bg = 0, br = 0;
                        if (s.shadow && s.shadowOpacity > 0) {
                            int sx = x - s.shadowX, sy = y - s.shadowY;
                            if (sx >= 0 && sy >= 0 && sx < ow && sy < oh) {
                                int a = (int)shadowMask[(size_t)sy * ow + sx] * s.shadowOpacity / 100;
                                ba = a; bb = s.shadowRgb[2] * a / 255; bg = s.shadowRgb[1] * a / 255; br = s.shadowRgb[0] * a / 255;
                            }
                        }
                        if (s.glow && s.glowIntensity > 0) {
                            int a = (int)glowMask[(size_t)y * ow + x] * s.glowIntensity / 100;
                            int inv = 255 - a;
                            bb = s.glowRgb[2] * a / 255 + bb * inv / 255;
                            bg = s.glowRgb[1] * a / 255 + bg * inv / 255;
                            br = s.glowRgb[0] * a / 255 + br * inv / 255;
                            ba = a + ba * inv / 255;
                        }
                        int ix = x - pad, iy = y - pad;
                        if (ix >= 0 && iy >= 0 && ix < w && iy < h) {
                            const BYTE* fg = &buf[((size_t)iy * w + ix) * 4];
                            int inv = 255 - fg[3];
                            bb = fg[0] + bb * inv / 255; bg = fg[1] + bg * inv / 255; br = fg[2] + br * inv / 255;
                            ba = fg[3] + ba * inv / 255;
                        }
                        outbuf[oi] = (BYTE)Imin(255, bb); outbuf[oi+1] = (BYTE)Imin(255, bg);
                        outbuf[oi+2] = (BYTE)Imin(255, br); outbuf[oi+3] = (BYTE)Imin(255, ba);
                    }
                    void* bits = nullptr;
                    HBITMAP dib = MakeDib(dc, ow, oh, &bits);
                    if (dib && bits) {
                        memcpy(bits, outbuf.data(), outbuf.size()); out->pixels = (BYTE*)bits; out->bmp = dib; out->w = ow; out->h = oh; ok = true;
                    } else if (dib) DeleteObject(dib);
                }
            }
            ReleaseDC(nullptr, dc);
        }
    }
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask) DeleteObject(ii.hbmMask);
    return ok;
}

// overlay index of the shortcut arrow in the system overlay list
static int LinkOverlayIndex() {
    static int idx = 0;
    if (idx > 0) return idx;
    typedef int (WINAPI* Fn)(LPCWSTR, int);
    HMODULE h = GetModuleHandleW(L"shell32.dll");
    Fn f = h ? (Fn)GetProcAddress(h, "SHGetIconOverlayIndexW") : nullptr;
    int r = f ? f(nullptr, 0x0FFFFFFE /* IDO_SHGIOI_LINK */) : 0;
    idx = r > 0 ? r : 2;
    return idx;
}

// ====================================================================== desktop: per-list state
enum Kind { K_NONE, K_DESKTOP, K_EXPLORER };

struct ListCtx {
    HWND h = nullptr, parent = nullptr;
    Kind kind = K_NONE;
    bool retired=false, startupApplied=false, startupPaintSeen=false;
    unsigned startupEpoch=0;
    int startupState=-1;
    ULONGLONG startupSince=0,lastStartupNotice=0;
    unsigned recoveryPass=0,hookRebinds=0;
    ULONGLONG lastHookRebind=0;
    bool rebinding=false;
    HIMAGELIST readyHiml=nullptr;
    int readyCx=0,readyCy=0;

    // icon effects
    IconCfg ic;
    double iconFrameTime=0;
    unsigned smoothingEpoch=0;
    size_t smoothingBytes=0;
    std::map<CacheKey,EffectSmoothing::Frame> iconSmoothing;
    TmpDib iconSmoothDib;
    bool iconActive = false, settling = false, sizeKnown = false;
    ULONGLONG settleUntil=0;
    HIMAGELIST lastHiml = nullptr;
    int lastCx = 0, lastCy = 0;
    LRESULT lastView = -1;
    HDC iconDC = nullptr;
    HDC bgDC = nullptr;
    HBITMAP bgBmp = nullptr;
    HGDIOBJ bgOld = nullptr;
    int bgW = 0, bgH = 0;
    RECT rc = {};
    bool hasBg = false;

    // selection box
    SelCfg sc;
    bool selOn = false;
    LRESULT pview = 0;
    SelCfg ds;
    bool dragging = false, moved = false, toggle = false, dirty = false;
    int tick = 0;
    double lastApply = 0;
    POINT start = {}, cur = {}, origin0 = {};
    RECT rect = {};
    std::vector<RECT> items;
    std::vector<char> base, state;
    HDC memDC = nullptr;
    DWORD fillPx = 0, borderPx = 0;
    HBITMAP fill1 = nullptr, border1 = nullptr, surf = nullptr;
    int surfW = 0, surfH = 0;
    DWORD surfPx = 0;
    TmpDib srcT, outT;
    std::vector<BYTE> scratch;
    std::vector<BYTE> snap;
    int snapK = 1, snapW = 0, snapH = 0;
};

static std::map<HWND, ListCtx*> g_ctxByParent;   // one context per SHELLDLL_DefView
static std::vector<HWND> g_lists;
static std::vector<ListCtx*> g_retiredListContexts;
static std::atomic<bool> g_desktopStopping=false;
// Hold our code while Windows owns subclass/hook callbacks. Release only after
// verified cleanup; failed cleanup leaves an inert module alive until process exit.
static std::atomic<HMODULE> g_desktopModuleLease=nullptr;
static std::atomic<bool> g_desktopRetainState=false;
static bool EnsureDesktopModuleLease(){
    if(g_desktopModuleLease.load())return true;
    HMODULE module=nullptr;
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
            reinterpret_cast<LPCWSTR>(&EnsureDesktopModuleLease),&module))return false;
    HMODULE expected=nullptr;
    if(!g_desktopModuleLease.compare_exchange_strong(expected,module))FreeLibrary(module);
    return true;
}
static std::atomic<unsigned> g_desktopEpoch=1;
static UINT g_desktopDispatchMsg=0,g_desktopHealthMsg=0;
static constexpr UINT_PTR kParentSubclassId=0xD0D10001,kListSubclassId=0xD0D10002;
static constexpr UINT_PTR kDesktopReadyTimer=0xD1E7;
static void DesktopReadinessTick(HWND parent,ListCtx* c);
static const UINT_PTR kIconAnimationTimer = 0xD1E6;
static const UINT_PTR kSettleTimer = 0xD1E5;
static const UINT_PTR kScrollTimer = 0x5E1B;
static wchar_t g_emptyText[2] = L"";

static void FreeBgDc(ListCtx* c) {
    if (c->bgDC) { SelectObject(c->bgDC, c->bgOld); DeleteDC(c->bgDC); c->bgDC = nullptr; }
    if (c->bgBmp) { DeleteObject(c->bgBmp); c->bgBmp = nullptr; }
    c->bgW = c->bgH = 0;
    c->hasBg = false;
}
static void FreeSurface(ListCtx* c) {
    if (c->surf) { DeleteObject(c->surf); c->surf = nullptr; }
    c->surfW = c->surfH = 0;
}
static void FreeSnapshot(ListCtx* c) {
    std::vector<BYTE>().swap(c->snap);
    c->snapW = c->snapH = 0;
}
static void FreeGfx(ListCtx* c) {
    FreeSurface(c);
    FreeDib(c->srcT);
    FreeDib(c->outT);
    std::vector<BYTE>().swap(c->scratch);
    if (c->fill1) { DeleteObject(c->fill1); c->fill1 = nullptr; }
    if (c->border1) { DeleteObject(c->border1); c->border1 = nullptr; }
    if (c->memDC) { DeleteDC(c->memDC); c->memDC = nullptr; }
}
static void FreeAll(ListCtx* c) {
    GdiFlush();
    FreeDib(c->iconSmoothDib);
    c->iconSmoothing.clear(); c->smoothingBytes=0;
    if (c->iconDC) { DeleteDC(c->iconDC); c->iconDC = nullptr; }
    FreeBgDc(c);
    FreeSnapshot(c);
    FreeGfx(c);
}

static Kind Classify(HWND h){
    for(HWND p=h;p;p=GetParent(p)){
        wchar_t cls[64]={};GetClassNameW(p,cls,64);
        if(!wcscmp(cls,L"CabinetWClass"))return K_EXPLORER;
        if(!wcscmp(cls,L"Progman")||!wcscmp(cls,L"WorkerW"))return K_DESKTOP;
    }
    return K_NONE;
}
static bool KindEnabled(Kind k, const SelCfg& s) {
    return (k == K_DESKTOP && s.desktop) || (k == K_EXPLORER && s.explorer);
}

// ====================================================================== desktop: selection box drawing
static DWORD PremulC(COLORREF c, int a) {
    unsigned r = GetRValue(c), g = GetGValue(c), b = GetBValue(c);
    return ((unsigned)a << 24) | ((r * a / 255) << 16) | ((g * a / 255) << 8) | (b * a / 255);
}
static HBITMAP Make1x1(DWORD px) {
    void* bits = nullptr;
    HBITMAP b = MakeDib(nullptr, 1, 1, &bits);
    if (b && bits) *(DWORD*)bits = px;
    return b;
}
static HDC GetMemDC(ListCtx* c) {
    if (!c->memDC) c->memDC = CreateCompatibleDC(nullptr);
    return c->memDC;
}
static void UpdateGfx(ListCtx* c, const SelCfg& s) {
    DWORD f = PremulC(s.fill, s.fillA), b = PremulC(s.border, s.borderA);
    if (!c->fill1 || f != c->fillPx) {
        if (c->fill1) DeleteObject(c->fill1);
        c->fill1 = Make1x1(f);
        c->fillPx = f;
    }
    if (!c->border1 || b != c->borderPx) {
        if (c->border1) DeleteObject(c->border1);
        c->border1 = Make1x1(b);
        c->borderPx = b;
    }
}
static void Blend1(ListCtx* c, HDC hdc, HBITMAP bmp, RECT rc) {
    int w = (int)(rc.right - rc.left), h = (int)(rc.bottom - rc.top);
    if (w <= 0 || h <= 0 || !bmp) return;
    HDC mem = GetMemDC(c);
    HGDIOBJ old = SelectObject(mem, bmp);
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    AlphaBlend(hdc, rc.left, rc.top, w, h, mem, 0, 0, 1, 1, bf);
    SelectObject(mem, old);
}
static void FillSurface(ListCtx* c, HDC hdc, RECT rc) {
    int w = (int)(rc.right - rc.left), h = (int)(rc.bottom - rc.top);
    if (w <= 0 || h <= 0) return;
    if (!c->surf || c->surfW < w || c->surfH < h || c->surfPx != c->fillPx) {
        FreeSurface(c);
        int nw = (Imax(w, c->surfW) + 127) & ~127;
        int nh = (Imax(h, c->surfH) + 127) & ~127;
        void* bits = nullptr;
        HBITMAP b = MakeDib(nullptr, nw, nh, &bits);
        if (!b || !bits) {
            if (b) DeleteObject(b);
            Blend1(c, hdc, c->fill1, rc);
            return;
        }
        std::fill((DWORD*)bits, (DWORD*)bits + (size_t)nw * nh, c->fillPx);
        c->surf = b;
        c->surfW = nw;
        c->surfH = nh;
        c->surfPx = c->fillPx;
    }
    HDC mem = GetMemDC(c);
    HGDIOBJ old = SelectObject(mem, c->surf);
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    AlphaBlend(hdc, rc.left, rc.top, w, h, mem, 0, 0, w, h, bf);
    SelectObject(mem, old);
}
static void DrawBorderRect(ListCtx* c, HDC hdc, RECT rc, int bw) {
    RECT t = {rc.left, rc.top, rc.right, rc.top + bw};
    RECT b = {rc.left, rc.bottom - bw, rc.right, rc.bottom};
    RECT l = {rc.left, rc.top + bw, rc.left + bw, rc.bottom - bw};
    RECT r = {rc.right - bw, rc.top + bw, rc.right, rc.bottom - bw};
    Blend1(c, hdc, c->border1, t);
    Blend1(c, hdc, c->border1, b);
    Blend1(c, hdc, c->border1, l);
    Blend1(c, hdc, c->border1, r);
}
static void DrawItemBox(ListCtx* c, HDC hdc, RECT rc, const SelCfg& s) {
    int w = (int)(rc.right - rc.left), h = (int)(rc.bottom - rc.top);
    if (w <= 0 || h <= 0) return;
    UpdateGfx(c, s);
    int bw = (s.borderA > 0) ? Imin(s.bw, Imin(w, h) / 2) : 0;
    if (bw > 0) DrawBorderRect(c, hdc, rc, bw);
    if (s.fillA > 0) {
        RECT in = {rc.left + bw, rc.top + bw, rc.right - bw, rc.bottom - bw};
        Blend1(c, hdc, c->fill1, in);
    }
}
static void DrawMarqueeBox(ListCtx* c, HDC hdc, RECT R, RECT cl, const SelCfg& s, bool withFill) {
    int w = (int)(R.right - R.left), h = (int)(R.bottom - R.top);
    if (w <= 0 || h <= 0) return;
    UpdateGfx(c, s);
    int bw = (s.borderA > 0) ? Imin(s.bw, Imin(w, h) / 2) : 0;
    if (bw > 0) DrawBorderRect(c, hdc, R, bw);
    if (withFill && s.fillA > 0) {
        RECT in = {R.left + bw, R.top + bw, R.right - bw, R.bottom - bw};
        RECT vis;
        if (IntersectRect(&vis, &in, &cl)) FillSurface(c, hdc, vis);
    }
}

// Upscales a blurred small image, mixes the box color into it (inside `inner`) and copies it 1:1 to the window.
static bool RenderBlur(ListCtx* c, HDC dst, RECT vis, const BYTE* small, int sw, int sh, int k,
                       int ox, int oy, RECT inner, const SelCfg& s) {
    int w = (int)(vis.right - vis.left), h = (int)(vis.bottom - vis.top);
    if (w <= 0 || h <= 0 || sw <= 0 || sh <= 0) return false;
    if (!EnsureDib(c->outT, dst, w, h)) return false;
    std::vector<int> x0(w), x1(w), wx(w);
    for (int x = 0; x < w; x++) {
        float fx = ((float)(vis.left + x - ox) + 0.5f) / (float)k - 0.5f;
        if (fx < 0) fx = 0;
        int ix = (int)fx;
        if (ix >= sw - 1) { x0[x] = x1[x] = sw - 1; wx[x] = 0; }
        else { x0[x] = ix; x1[x] = ix + 1; wx[x] = (int)((fx - (float)ix) * 256.f); }
    }
    int fa = s.fillA, ia = 255 - s.fillA;
    int fcol[3] = {(int)GetBValue(s.fill), (int)GetGValue(s.fill), (int)GetRValue(s.fill)};
    int stride = c->outT.w;
    for (int y = 0; y < h; y++) {
        float fy = ((float)(vis.top + y - oy) + 0.5f) / (float)k - 0.5f;
        if (fy < 0) fy = 0;
        int iy = (int)fy, ya, yb, wy;
        if (iy >= sh - 1) { ya = yb = sh - 1; wy = 0; }
        else { ya = iy; yb = iy + 1; wy = (int)((fy - (float)iy) * 256.f); }
        const BYTE* r0 = small + (size_t)ya * sw * 4;
        const BYTE* r1 = small + (size_t)yb * sw * 4;
        BYTE* o = (BYTE*)c->outT.bits + (size_t)y * stride * 4;
        int ay = (int)vis.top + y;
        bool rowIn = fa > 0 && ay >= inner.top && ay < inner.bottom;
        for (int x = 0; x < w; x++) {
            bool useFill = rowIn && ((int)vis.left + x) >= inner.left && ((int)vis.left + x) < inner.right;
            for (int ch = 0; ch < 3; ch++) {
                int top = r0[x0[x] * 4 + ch] * (256 - wx[x]) + r0[x1[x] * 4 + ch] * wx[x];
                int bot = r1[x0[x] * 4 + ch] * (256 - wx[x]) + r1[x1[x] * 4 + ch] * wx[x];
                int v = (top * (256 - wy) + bot * wy) >> 16;
                if (useFill) v = (v * ia + fcol[ch] * fa + 127) / 255;
                o[x * 4 + ch] = (BYTE)v;
            }
            o[x * 4 + 3] = 255;
        }
    }
    BitBlt(dst, vis.left, vis.top, w, h, c->outT.dc, 0, 0, SRCCOPY);
    return true;
}

// Desktop: the paint DC has no wallpaper, so grab a clean screen copy once and blur it once.
static void TakeSnapshot(ListCtx* c, int strength) {
    FreeSnapshot(c);
    RECT cl;
    GetClientRect(c->h, &cl);
    int w = (int)cl.right, h = (int)cl.bottom;
    if (w <= 0 || h <= 0 || (long long)w * h > 16000000LL) return;
    UpdateWindow(c->h);
    FlushDwm();
    POINT o = {0, 0};
    ClientToScreen(c->h, &o);
    HDC scr = GetDC(nullptr);
    void* fbits = nullptr;
    HBITMAP full = MakeDib(scr, w, h, &fbits);
    HDC fdc = CreateCompatibleDC(scr);
    if (full && fbits && fdc) {
        HGDIOBJ fo = SelectObject(fdc, full);
        BitBlt(fdc, 0, 0, w, h, scr, o.x, o.y, SRCCOPY);
        GdiFlush();
        int k = ((long long)w * h > 4000000LL) ? 4 : 2;
        int sw = (w + k - 1) / k, sh = (h + k - 1) / k;
        c->snap.assign((size_t)sw * sh * 4, 0);
        Downscale((const BYTE*)fbits, w, w, h, k, c->snap.data(), sw, sh);
        BlurSmall(c->snap.data(), sw, sh, Imax(1, strength / k));
        c->snapW = sw;
        c->snapH = sh;
        c->snapK = k;
        SelectObject(fdc, fo);
    }
    if (fdc) DeleteDC(fdc);
    if (full) DeleteObject(full);
    ReleaseDC(nullptr, scr);
}

static bool BlurRegion(ListCtx* c, HDC hdc, RECT vis, const SelCfg& s) {
    RECT R = c->rect;
    int rw = (int)(R.right - R.left), rh = (int)(R.bottom - R.top);
    int bw = (s.borderA > 0) ? Imin(s.bw, Imin(rw, rh) / 2) : 0;
    RECT inner = {R.left + bw, R.top + bw, R.right - bw, R.bottom - bw};
    if (!c->snap.empty())
        return RenderBlur(c, hdc, vis, c->snap.data(), c->snapW, c->snapH, c->snapK, 0, 0, inner, s);
    int w = (int)(vis.right - vis.left), h = (int)(vis.bottom - vis.top);
    if (w < 4 || h < 4) return false;
    int k = Imax(1, Imin(16, (Imax(w, h) + 159) / 160));
    int sw = Imax(1, (w + k - 1) / k), sh = Imax(1, (h + k - 1) / k);
    if (!EnsureDib(c->srcT, hdc, w, h)) return false;
    BitBlt(c->srcT.dc, 0, 0, w, h, hdc, vis.left, vis.top, SRCCOPY);
    GdiFlush();
    c->scratch.assign((size_t)sw * sh * 4, 0);
    Downscale((const BYTE*)c->srcT.bits, c->srcT.w, w, h, k, c->scratch.data(), sw, sh);
    BlurSmall(c->scratch.data(), sw, sh, Imax(1, s.blurStrength / k));
    return RenderBlur(c, hdc, vis, c->scratch.data(), sw, sh, k, (int)vis.left, (int)vis.top, inner, s);
}

static void DrawMarquee(ListCtx* c, HDC hdc, const SelCfg& s) {
    RECT cl;
    GetClientRect(c->h, &cl);
    RECT vis;
    if (!IntersectRect(&vis, &c->rect, &cl)) return;
    bool blurred = false;
    if (s.blur) blurred = BlurRegion(c, hdc, vis, s);
    DrawMarqueeBox(c, hdc, c->rect, cl, s, !blurred);
}

// ====================================================================== desktop: drag selection logic
static POINT GetOrigin(HWND h) {
    POINT p = {0, 0};
    if (!SendMessageW(h, LVM_GETITEMPOSITION, 0, (LPARAM)&p)) p.x = p.y = 0;
    return p;
}

static void BeginDrag(ListCtx* c, POINT pt, bool ctrl, bool shift) {
    HWND h = c->h;
    c->ds = GetSel();
    c->dragging = true;
    c->moved = false;
    c->dirty = false;
    c->tick = 0;
    c->lastApply = Now() * 1000.0;
    c->start = pt;
    c->cur = pt;
    c->origin0 = GetOrigin(h);
    c->rect = RECT{0, 0, 0, 0};
    c->toggle = ctrl;

    int n = (int)SendMessageW(h, LVM_GETITEMCOUNT, 0, 0);
    if (n < 0) n = 0;
    c->items.assign((size_t)n, RECT{0, 0, 0, 0});
    for (int i = 0; i < n; i++) {
        RECT ir = {};
        ir.left = LVIR_BOUNDS;
        if (SendMessageW(h, LVM_GETITEMRECT, i, (LPARAM)&ir)) {
            OffsetRect(&ir, -(int)c->origin0.x, -(int)c->origin0.y);
            c->items[i] = ir;
        }
    }
    c->base.assign((size_t)n, 0);
    if (ctrl || shift) {
        for (int i = 0; i < n; i++)
            c->base[i] = (SendMessageW(h, LVM_GETITEMSTATE, i, LVIS_SELECTED) & LVIS_SELECTED) ? 1 : 0;
    } else {
        LVITEMW lvi = {};
        lvi.stateMask = LVIS_SELECTED;
        lvi.state = 0;
        SendMessageW(h, LVM_SETITEMSTATE, (WPARAM)-1, (LPARAM)&lvi);
    }
    c->state = c->base;
    if (GetFocus() != h) SetFocus(h);
    SetCapture(h);
    SetTimer(h, kScrollTimer, 16, nullptr);
}

static void ApplySelection(ListCtx* c) {
    if (!c->moved) return;
    HWND h = c->h;
    POINT o = GetOrigin(h);
    RECT Rc = {c->rect.left - o.x, c->rect.top - o.y, c->rect.right - o.x, c->rect.bottom - o.y};
    size_t n = c->items.size();
    for (size_t i = 0; i < n; i++) {
        const RECT& ir = c->items[i];
        bool inter = ir.left < Rc.right && ir.right > Rc.left && ir.top < Rc.bottom && ir.bottom > Rc.top;
        bool b = c->base[i] != 0;
        bool want = c->toggle ? (b != inter) : (b || inter);
        if (want != (c->state[i] != 0)) {
            LVITEMW lvi = {};
            lvi.stateMask = LVIS_SELECTED;
            lvi.state = want ? LVIS_SELECTED : 0;
            SendMessageW(h, LVM_SETITEMSTATE, (WPARAM)i, (LPARAM)&lvi);
            c->state[i] = want ? 1 : 0;
        }
    }
}

static void EndDrag(ListCtx* c) {
    if (!c->dragging) return;
    if (c->moved && c->dirty) ApplySelection(c);
    c->dragging = false;
    if (c->h) {
        KillTimer(c->h, kScrollTimer);
        if (GetCapture() == c->h) ReleaseCapture();
    }
    FreeSnapshot(c);
    FreeSurface(c);
    FreeDib(c->srcT);
    FreeDib(c->outT);
    RECT inv = c->rect;
    c->rect = RECT{0, 0, 0, 0};
    c->moved = false;
    c->dirty = false;
    std::vector<RECT>().swap(c->items);
    std::vector<char>().swap(c->base);
    std::vector<char>().swap(c->state);
    InflateRect(&inv, 12, 12);
    if (c->h) InvalidateRect(c->h, &inv, FALSE);
}

static void UpdateRect(ListCtx* c) {
    HWND h = c->h;
    if (!c->moved) {
        if (abs((int)(c->cur.x - c->start.x)) < 3 && abs((int)(c->cur.y - c->start.y)) < 3) return;
        c->moved = true;
        if (c->ds.blur && c->kind == K_DESKTOP) TakeSnapshot(c, c->ds.blurStrength);
    }
    POINT o = GetOrigin(h);
    POINT a = {c->start.x + (o.x - c->origin0.x), c->start.y + (o.y - c->origin0.y)};
    RECT R = {Imin((int)a.x, (int)c->cur.x), Imin((int)a.y, (int)c->cur.y),
              Imax((int)a.x, (int)c->cur.x) + 1, Imax((int)a.y, (int)c->cur.y) + 1};
    RECT inv;
    UnionRect(&inv, &c->rect, &R);
    c->rect = R;
    c->dirty = true;
    InflateRect(&inv, c->ds.bw + 2, c->ds.bw + 2);
    InvalidateRect(h, &inv, FALSE);
}

static void AutoScroll(ListCtx* c) {
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(c->h, &pt);
    RECT cl;
    GetClientRect(c->h, &cl);
    int dx = 0, dy = 0;
    if (pt.y < cl.top) dy = -Imin(80, 24 + (int)(cl.top - pt.y) / 3);
    else if (pt.y >= cl.bottom) dy = Imin(80, 24 + (int)(pt.y - cl.bottom) / 3);
    if (pt.x < cl.left) dx = -Imin(80, 24 + (int)(cl.left - pt.x) / 3);
    else if (pt.x >= cl.right) dx = Imin(80, 24 + (int)(pt.x - cl.right) / 3);
    if (!dx && !dy) return;
    SendMessageW(c->h, LVM_SCROLL, dx, dy);
    c->cur = pt;
    UpdateRect(c);
}

// ====================================================================== desktop: icon painting
static HIMAGELIST GetListImageList(HWND hList, int* cx, int* cy, LRESULT* viewOut) {
    LRESULT view = SendMessageW(hList, LVM_GETVIEW, 0, 0);
    if (viewOut) *viewOut = view;
    int which = (view == LV_VIEW_ICON || view == LV_VIEW_TILE) ? LVSIL_NORMAL : LVSIL_SMALL;
    HIMAGELIST himl = (HIMAGELIST)SendMessageW(hList, LVM_GETIMAGELIST, which, 0);
    *cx = *cy = 0;
    if (himl) ImageList_GetIconSize(himl, cx, cy);
    return himl;
}

static bool CaptureBg(ListCtx* c, HWND hList, HDC hdc, int idx) {
    RECT rc = {};
    rc.left = LVIR_ICON;
    if (!SendMessageW(hList, LVM_GETITEMRECT, idx, (LPARAM)&rc)) return false;
    int w = (int)(rc.right - rc.left), h = (int)(rc.bottom - rc.top);
    if (w <= 0 || h <= 0 || w > 1024 || h > 1024) return false;
    if (!c->bgDC || c->bgW != w || c->bgH != h) {
        FreeBgDc(c);
        void* bits = nullptr;
        HBITMAP bmp = MakeDib(hdc, w, h, &bits);
        HDC mem = CreateCompatibleDC(hdc);
        if (!bmp || !bits || !mem) {
            if (bmp) DeleteObject(bmp);
            if (mem) DeleteDC(mem);
            return false;
        }
        c->bgOld = SelectObject(mem, bmp);
        c->bgDC = mem;
        c->bgBmp = bmp;
        c->bgW = w;
        c->bgH = h;
    }
    BitBlt(c->bgDC, 0, 0, w, h, hdc, rc.left, rc.top, SRCCOPY);
    c->rc = rc;
    c->hasBg = true;
    return true;
}

static void ApplyIcon(ListCtx* c, HWND hList, HDC hdc, int idx) {
    int cx, cy;
    HIMAGELIST himl = GetListImageList(hList, &cx, &cy, nullptr);
    if (!himl) return;
    LVITEMW it = {};
    it.mask = LVIF_IMAGE | LVIF_STATE;
    it.iItem = idx;
    it.stateMask = LVIS_OVERLAYMASK | LVIS_CUT;
    if (!SendMessageW(hList, LVM_GETITEMW, 0, (LPARAM)&it)) return;
    if (it.iImage < 0 || (it.state & LVIS_CUT)) return;
    int overlay = (int)((it.state & LVIS_OVERLAYMASK) >> 8);
    int rw = (int)(c->rc.right - c->rc.left), rh = (int)(c->rc.bottom - c->rc.top);

    EnterCriticalSection(&g_cs);
    const IconCfg& s = c->kind==K_EXPLORER?g_s.explorerIcon:g_s.icon;
    auto& cache=c->kind==K_EXPLORER?g_explorerIconCache:g_cache;   // locked for the entire lookup, build and draw
    if(c->kind==K_EXPLORER&&!g_s.explorerIconsEnabled){LeaveCriticalSection(&g_cs);return;}
    bool arrowHidden = false;
    if (s.hideArrow && overlay > 0 && overlay == LinkOverlayIndex()) { overlay = 0; arrowHidden = true; }
    if (!s.HasEffects() && !arrowHidden) { LeaveCriticalSection(&g_cs); return; }

    POINT location={c->rc.left+(rw-cx)/2,c->rc.top+(rh-cy)/2};
    ClientToScreen(hList,&location);
    int virtualX=GetSystemMetrics(SM_XVIRTUALSCREEN),virtualY=GetSystemMetrics(SM_YVIRTUALSCREEN);
    int virtualW=Imax(1,GetSystemMetrics(SM_CXVIRTUALSCREEN)),virtualH=Imax(1,GetSystemMetrics(SM_CYVIRTUALSCREEN));
    IconSpace space;space.x=(float)(location.x-virtualX);space.y=(float)(location.y-virtualY);
    space.w=(float)virtualW;space.h=(float)virtualH;space.time=c->iconFrameTime;
    if(c->kind==K_EXPLORER){
        RECT client{};GetClientRect(hList,&client);
        space.x=(float)(c->rc.left+(rw-cx)/2);space.y=(float)(c->rc.top+(rh-cy)/2);
        space.w=(float)Imax(1,(int)client.right);space.h=(float)Imax(1,(int)client.bottom);
        virtualW=(int)space.w;virtualH=(int)space.h;
        location={static_cast<LONG>(space.x),static_cast<LONG>(space.y)};
    }
    bool spatial=s.effect!=L"none";
    CacheKey key((UINT_PTR)himl,it.iImage,overlay,cx,cy,spatial?(UINT_PTR)hList:0,spatial?idx:0,
                 spatial?location.x:0,spatial?location.y:0,spatial?virtualW:0,spatial?virtualH:0);
    auto f = cache.find(key);
    if (f == cache.end()) {
        HICON icon = ImageList_GetIcon(himl, it.iImage, ILD_NORMAL | INDEXTOOVERLAYMASK(overlay));
        if (!icon) { LeaveCriticalSection(&g_cs); return; }
        Cached nc;
        bool ok = BuildProcessed(icon, s, &nc, &space);
        DestroyIcon(icon);
        if (!ok) { LeaveCriticalSection(&g_cs); return; }
        size_t newBytes=(size_t)nc.w*nc.h*4;
        bool explorer=c->kind==K_EXPLORER;
        if (cache.size() >= (size_t)(PerformanceOptions::enabled.load()?PerformanceOptions::iconEntries.load():1024)||
            (explorer&&g_explorerCacheBytes+newBytes>16u*1024u*1024u)) {
            for(auto& kv:cache)if(kv.second.bmp)DeleteObject(kv.second.bmp);cache.clear();
            if(explorer)g_explorerCacheBytes=0;
        }
        if(explorer)g_explorerCacheBytes+=newBytes;
        f = cache.emplace(key, nc).first;
    }
    const Cached& cc = f->second;
    HBITMAP drawBitmap = cc.bmp;
    bool useSmoothDib=false;
    if (EffectSmoothing::Use(s.prog.usesTime && s.effect!=L"none",s.fps) && cc.pixels) {
        unsigned epoch=g_iconSourceEpoch.load();
        if(c->smoothingEpoch!=epoch){c->iconSmoothing.clear();c->smoothingBytes=0;c->smoothingEpoch=epoch;}
        size_t needed=(size_t)cc.w*cc.h*8;
        if(needed<=32u*1024u*1024u && c->iconSmoothing.find(key)==c->iconSmoothing.end() &&
           (c->iconSmoothing.size()>=512 || c->smoothingBytes+needed>32u*1024u*1024u)) {
            c->iconSmoothing.clear();c->smoothingBytes=0;
        }
        if(needed<=32u*1024u*1024u){
            auto& frame=c->iconSmoothing[key];
            if(!frame.ready || frame.stamp!=c->iconFrameTime){
                size_t oldBytes=frame.Bytes();
                frame.Push(cc.pixels,(size_t)cc.w*cc.h*4,c->iconFrameTime,1.0/std::max(1,s.fps));
                c->smoothingBytes=c->smoothingBytes-oldBytes+frame.Bytes();
            }
            if(frame.ready && EnsureDib(c->iconSmoothDib,hdc,cc.w,cc.h)) {
                GdiFlush(); // Finish the previous icon's blit before reusing this source DIB.
                frame.Blend((BYTE*)c->iconSmoothDib.bits,(size_t)c->iconSmoothDib.w*4,(size_t)cc.w*4,cc.h,Now());
                drawBitmap=c->iconSmoothDib.bmp;useSmoothDib=true;
            }
        }
    }
    if (cc.w <= 2048 && cc.h <= 2048) {
        int ox = (rw - cc.w) / 2, oy = (rh - cc.h) / 2;
        if (!useSmoothDib && !c->iconDC) c->iconDC = CreateCompatibleDC(hdc);
        HDC mdc = useSmoothDib ? c->iconSmoothDib.dc : c->iconDC;
        if (!mdc) { LeaveCriticalSection(&g_cs); return; }
        HGDIOBJ old = nullptr;
        if(!useSmoothDib){
            old = SelectObject(mdc, drawBitmap);
            if (!old || old == HGDI_ERROR) { LeaveCriticalSection(&g_cs); return; }
        }
        BitBlt(hdc, c->rc.left, c->rc.top, rw, rh, c->bgDC, 0, 0, SRCCOPY);   // wipe the original icon
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        BOOL applied=AlphaBlend(hdc,c->rc.left+ox,c->rc.top+oy,cc.w,cc.h,mdc,0,0,cc.w,cc.h,bf);
        if(applied && c->kind==K_DESKTOP && !c->startupApplied){
            c->startupApplied=true;
            Wh_Log(L"Desktop startup: first icon processed (list=%p, item=%d)",hList,idx);
        }
        if(old)SelectObject(mdc, old);
    }
    LeaveCriticalSection(&g_cs);
}

static void RefreshListLabels(HWND list) {
    if (!list) return;
    DWORD_PTR n = 0;
    if (!SendMessageTimeoutW(list, LVM_GETITEMCOUNT, 0, 0, SMTO_ABORTIFHUNG, 300, &n)) return;
    for (int i = 0; i < (int)n; i++) {
        DWORD_PTR res;
        SendMessageTimeoutW(list, LVM_UPDATE, i, 0, SMTO_ABORTIFHUNG, 200, &res);
    }
}

// ====================================================================== desktop: window procedures
static LRESULT HandleCustomDraw(HWND hWnd, WPARAM wp, LPARAM lp, ListCtx* c) {
    LRESULT r = DefSubclassProc(hWnd, WM_NOTIFY, wp, lp);
    NMLVCUSTOMDRAW* cd = (NMLVCUSTOMDRAW*)lp;
    HWND hList = cd->nmcd.hdr.hwndFrom;
    if(c->kind==K_DESKTOP && !c->startupPaintSeen){
        c->startupPaintSeen=true;Wh_Log(L"Desktop startup: custom draw received (list=%p)",hList);
    }

    switch (cd->nmcd.dwDrawStage) {
        case CDDS_PREPAINT: {
            FreeBgDc(c);
            c->iconActive = false;
            c->kind=Classify(hList);
            c->ic = GetListIcon(c->kind==K_EXPLORER);
            if((!EffectSmoothing::enabled.load()||!c->ic.prog.usesTime)&&!c->iconSmoothing.empty()){
                GdiFlush();FreeDib(c->iconSmoothDib);c->iconSmoothing.clear();c->smoothingBytes=0;
            }
            double phase=IconFrameTime(c->ic.fps);
            if(phase!=c->iconFrameTime){c->iconFrameTime=phase;AdvanceListIconFrame(c->kind==K_EXPLORER,phase);}
            if((c->kind==K_DESKTOP||c->kind==K_EXPLORER) && c->ic.effect!=L"none" && c->ic.prog.usesTime)
                SetTimer(hList,kIconAnimationTimer,1000/(EffectSmoothing::enabled.load()?60:c->ic.fps),nullptr);
            else KillTimer(hList,kIconAnimationTimer);
            c->sc = GetSel();
            c->kind = Classify(hList);
            c->selOn = KindEnabled(c->kind, c->sc);
            if ((c->kind == K_DESKTOP || c->kind == K_EXPLORER) && c->ic.NeedsWork()) {
                int cx, cy;
                LRESULT view;
                HIMAGELIST himl = GetListImageList(hList, &cx, &cy, &view);
                if (!himl || cx<=0 || cy<=0) break;
                if (!c->sizeKnown) {
                    c->sizeKnown = true;
                    c->lastHiml = himl; c->lastCx = cx; c->lastCy = cy; c->lastView = view;
                } else if (himl != c->lastHiml || cx != c->lastCx || cy != c->lastCy || view != c->lastView) {
                    c->lastHiml = himl; c->lastCx = cx; c->lastCy = cy; c->lastView = view;
                    ClearCache();
                    c->settling = true;
                    c->settleUntil=GetTickCount64()+(ULONGLONG)std::max(1,c->ic.settleMs);
                    SetTimer(hWnd, kSettleTimer, (UINT)c->ic.settleMs, nullptr);
                }
                EnterCriticalSection(&g_cs);
                if (GetTickCount() - g_cacheStamp > 30000) ClearCacheLocked();   // never keep stale icons for long
                LeaveCriticalSection(&g_cs);
                c->iconActive = !c->settling;
            }
            if (c->selOn && c->sc.styleItems) c->pview = SendMessageW(hList, LVM_GETVIEW, 0, 0);
            if (!(r & CDRF_SKIPDEFAULT)) {
                if ((c->selOn && c->sc.styleItems) || c->iconActive) r |= CDRF_NOTIFYITEMDRAW;
                if (c->selOn && c->dragging && c->moved) r |= CDRF_NOTIFYPOSTPAINT;
            }
            break;
        }
        case CDDS_ITEMPREPAINT: {
            FreeBgDc(c);
            if (r & CDRF_SKIPDEFAULT) break;
            // 1. selected items get the selection box look (drawn before the icon and name)
            if (c->selOn && c->sc.styleItems && (cd->nmcd.uItemState & CDIS_SELECTED)) {
                RECT ir = {};
                ir.left = (c->pview == LV_VIEW_DETAILS) ? LVIR_SELECTBOUNDS : LVIR_BOUNDS;
                if (SendMessageW(hList, LVM_GETITEMRECT, (WPARAM)cd->nmcd.dwItemSpec, (LPARAM)&ir))
                    DrawItemBox(c, cd->nmcd.hdc, ir, c->sc);
                cd->nmcd.uItemState &= ~(UINT)(CDIS_SELECTED | CDIS_HOT | CDIS_FOCUS | CDIS_MARKED);
                r |= CDRF_NEWFONT;
            }
            // 2. icon effects (hovered icons keep their normal look)
            if (c->iconActive && !(cd->nmcd.uItemState & CDIS_HOT)) {
                if (CaptureBg(c, hList, cd->nmcd.hdc, (int)cd->nmcd.dwItemSpec)) r |= CDRF_NOTIFYPOSTPAINT;
            }
            break;
        }
        case CDDS_ITEMPOSTPAINT: {
            if (c->hasBg) {
                ApplyIcon(c, hList, cd->nmcd.hdc, (int)cd->nmcd.dwItemSpec);
                c->hasBg = false;
            }
            break;
        }
        case CDDS_POSTPAINT: {
            if (c->selOn && c->dragging && c->moved) DrawMarquee(c, cd->nmcd.hdc, c->sc);
            break;
        }
    }
    return r;
}

static void OnListGone(ListCtx* c, HWND list) {
    EndDrag(c);
    c->h = nullptr;
    EnterCriticalSection(&g_cs);
    g_lists.erase(std::remove(g_lists.begin(), g_lists.end(), list), g_lists.end());
    LeaveCriticalSection(&g_cs);
}

static LRESULT CALLBACK ListProc(HWND h, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR id, DWORD_PTR ref) {
    if(g_desktopStopping){
        KillTimer(h,kIconAnimationTimer);
        KillTimer(h,kScrollTimer);
        RemoveWindowSubclass(h,ListProc,id);
        return DefSubclassProc(h,msg,wp,lp);
    }
    ListCtx* c = (ListCtx*)ref;
    if(c->retired)return DefSubclassProc(h,msg,wp,lp);
    switch (msg) {
        case WM_NCDESTROY:
            RemoveWindowSubclass(h,ListProc,id);
            KillTimer(h,kIconAnimationTimer);
            OnListGone(c, h);
            return DefSubclassProc(h, msg, wp, lp);
        case WM_LBUTTONDOWN: {
            SelCfg s = GetSel();
            c->kind = Classify(h);
            if (!KindEnabled(c->kind, s)) break;
            POINT pt = {GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            LVHITTESTINFO hi = {};
            hi.pt = pt;
            SendMessageW(h, LVM_HITTEST, 0, (LPARAM)&hi);
            // only start on truly empty space; clicks on items stay native
            if (hi.iItem != -1 || !(hi.flags & LVHT_NOWHERE) || (hi.flags & LVHT_ONITEM)) break;
            bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
            BeginDrag(c, pt, ctrl, shift);
            return 0;
        }
        case WM_MOUSEMOVE:
            if (c->dragging) {
                c->cur.x = GET_X_LPARAM(lp);
                c->cur.y = GET_Y_LPARAM(lp);
                UpdateRect(c);
                if (c->moved && c->dirty) {
                    double now = Now() * 1000.0;
                    if (now - c->lastApply >= 16.0) {
                        c->dirty = false;
                        c->lastApply = now;
                        ApplySelection(c);
                    }
                }
                return 0;
            }
            break;
        case WM_LBUTTONUP:
            if (c->dragging) { EndDrag(c); return 0; }
            break;
        case WM_CAPTURECHANGED:
        case WM_CANCELMODE:
            if (c->dragging) EndDrag(c);
            break;
        case WM_TIMER:
            if(wp==kIconAnimationTimer) {
                IconCfg ic=GetListIcon(c->kind==K_EXPLORER);
                if(ic.effect!=L"none" && ic.prog.usesTime && (c->kind!=K_EXPLORER||(IsWindowVisible(h)&&!IsIconic(GetAncestor(h,GA_ROOT))))) {
                    double phase=IconFrameTime(ic.fps);
                    if(phase!=c->iconFrameTime){AdvanceListIconFrame(c->kind==K_EXPLORER,phase);c->iconFrameTime=phase;}
                    RECT client{};GetClientRect(h,&client);
                    RECT changed{};bool any=false;
                    int count=(int)SendMessageW(h,LVM_GETITEMCOUNT,0,0);
                    if(count>256)InvalidateRect(h,nullptr,FALSE);
                    else for(int index=0;index<count;index++){
                        RECT item{};item.left=LVIR_BOUNDS;
                        RECT visible{};
                        if(SendMessageW(h,LVM_GETITEMRECT,index,(LPARAM)&item)&&IntersectRect(&visible,&client,&item)){
                            // Bounds include labels, selection and the original icon wipe.
                            if(any)UnionRect(&changed,&changed,&visible);else {changed=visible;any=true;}
                        }
                    }
                    if(any)InvalidateRect(h,&changed,FALSE);
                }
                return 0;
            }
            if (wp == kScrollTimer) {
                if (c->dragging) {
                    if (c->moved && c->dirty) {
                        c->dirty = false;
                        c->lastApply = Now() * 1000.0;
                        ApplySelection(c);
                    }
                    if ((++c->tick % 3) == 0) AutoScroll(c);
                }
                return 0;
            }
            break;
    }
    return DefSubclassProc(h, msg, wp, lp);
}

static LRESULT CALLBACK ParentProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR id, DWORD_PTR ref) {
    if(g_desktopStopping){
        KillTimer(hWnd,kDesktopReadyTimer);
        KillTimer(hWnd,kSettleTimer);
        RemoveWindowSubclass(hWnd,ParentProc,id);
        return DefSubclassProc(hWnd,msg,wp,lp);
    }
    ListCtx* c = (ListCtx*)ref;

    if(c->retired)return DefSubclassProc(hWnd,msg,wp,lp);
    if (msg == WM_NCDESTROY) {
        RemoveWindowSubclass(hWnd,ParentProc,id);
        KillTimer(hWnd,kDesktopReadyTimer);KillTimer(hWnd,kSettleTimer);
        if(IsWindow(c->h)){
            KillTimer(c->h,kIconAnimationTimer);
            RemoveWindowSubclass(c->h,ListProc,kListSubclassId);
        }
        EnterCriticalSection(&g_cs);
        auto it=g_ctxByParent.find(hWnd);
        if(it!=g_ctxByParent.end()&&it->second==c)g_ctxByParent.erase(it);
        g_lists.erase(std::remove(g_lists.begin(),g_lists.end(),c->h),g_lists.end());
        LeaveCriticalSection(&g_cs);
        EndDrag(c);FreeAll(c);delete c;
        return DefSubclassProc(hWnd,msg,wp,lp);
    }
    if((msg==WM_TIMER&&wp==kDesktopReadyTimer)||
       (msg==g_desktopHealthMsg&&(HWND)lp==c->h)){
        if(!g_desktopStopping)DesktopReadinessTick(hWnd,c);
        return 0;
    }

    if (msg == WM_TIMER && wp == kSettleTimer) {
        KillTimer(hWnd, kSettleTimer);
        c->settling = false;
        c->settleUntil=0;
        ClearCache();
        RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
        return 0;
    }

    if (msg == WM_NOTIFY && lp) {
        NMHDR* nh = (NMHDR*)lp;
        if (c->h && nh->hwndFrom == c->h) {
            // hide icon names: return empty text for every label on the desktop
            if (nh->code == (UINT)LVN_GETDISPINFOW) {
                LRESULT r = DefSubclassProc(hWnd, msg, wp, lp);
                if (c->kind == K_DESKTOP && GetIcon().hideNames) {
                    NMLVDISPINFOW* di = (NMLVDISPINFOW*)lp;
                    if (di->item.mask & LVIF_TEXT) di->item.pszText = g_emptyText;
                }
                return r;
            }
            // desktop reloaded (refresh): wait until it settles, then re-apply
            if (nh->code == (UINT)LVN_DELETEALLITEMS) {
                IconCfg ic = GetListIcon(c->kind==K_EXPLORER);
                if ((c->kind == K_DESKTOP || c->kind == K_EXPLORER) && ic.NeedsWork()) {
                    ClearCache();
                    c->settling = true;
                    c->settleUntil=GetTickCount64()+(ULONGLONG)std::max(1,ic.settleMs);
                    SetTimer(hWnd, kSettleTimer, (UINT)ic.settleMs, nullptr);
                }
            }
            if (nh->code == (UINT)NM_CUSTOMDRAW) return HandleCustomDraw(hWnd, wp, lp, c);
        }
    }
    return DefSubclassProc(hWnd, msg, wp, lp);
}

// ====================================================================== desktop: attaching
static HWND ListParentIfDefView(HWND list) {
    HWND parent = GetParent(list);
    if (!parent) return nullptr;
    wchar_t cls[32] = {};
    GetClassNameW(parent, cls, 32);
    return wcscmp(cls, L"SHELLDLL_DefView") ? nullptr : parent;
}

// All state/subclass operations below run on the window's owning UI thread.
// Retired contexts stay allocated until unload so reentrant callbacks cannot
// retain a freed ref-data pointer while Explorer reparents/replaces a list.
static bool RetireContextOnUI(HWND parent,ListCtx* c){
    if(!c||c->retired)return true;
    HWND list=c->h;
    if(IsWindow(list)){
        KillTimer(list,kIconAnimationTimer);
        RemoveWindowSubclass(list,ListProc,kListSubclassId);
    }
    if(IsWindow(parent)){
        KillTimer(parent,kDesktopReadyTimer);KillTimer(parent,kSettleTimer);
        RemoveWindowSubclass(parent,ParentProc,kParentSubclassId);
    }
    DWORD_PTR ref=0;
    bool listAttached=IsWindow(list)&&GetWindowSubclass(list,ListProc,kListSubclassId,&ref);
    ref=0;
    bool parentAttached=IsWindow(parent)&&GetWindowSubclass(parent,ParentProc,kParentSubclassId,&ref);
    if(listAttached||parentAttached){
        Wh_Log(L"Desktop detach incomplete; keeping callback context alive");
        return false;
    }
    GdiFlush(); // Complete owner-thread icon blits before retiring their DIB state.
    EndDrag(c);c->retired=true;c->h=nullptr;
    EnterCriticalSection(&g_cs);
    auto it=g_ctxByParent.find(parent);
    if(it!=g_ctxByParent.end()&&it->second==c)g_ctxByParent.erase(it);
    g_lists.erase(std::remove(g_lists.begin(),g_lists.end(),list),g_lists.end());
    g_retiredListContexts.push_back(c);
    LeaveCriticalSection(&g_cs);
    return true;
}
static void StartupState(ListCtx* c,int state,PCWSTR text){
    if(c->startupState==state)return;
    c->startupState=state;c->lastStartupNotice=GetTickCount64();
    Wh_Log(L"Desktop startup: %s (list=%p)",text,c->h);
}
// Re-enabling rebuilt these callbacks, whereas a redraw-only retry did not.
// Reinstall only our callback/ID pairs, without unloading other mod features.
static bool RebindDesktopCallbacks(HWND parent,ListCtx* c){
    c->rebinding=true;
    RemoveWindowSubclass(c->h,ListProc,kListSubclassId);
    RemoveWindowSubclass(parent,ParentProc,kParentSubclassId);
    bool parentOK=SetWindowSubclass(parent,ParentProc,kParentSubclassId,(DWORD_PTR)c)!=FALSE;
    bool listOK=SetWindowSubclass(c->h,ListProc,kListSubclassId,(DWORD_PTR)c)!=FALSE;
    c->rebinding=false;
    if(!parentOK||!listOK){
        Wh_Log(L"Desktop startup: callback rebind failed (parent=%d, list=%d)",parentOK,listOK);
        RetireContextOnUI(parent,c);return false;
    }
    c->startupApplied=false;c->startupPaintSeen=false;c->sizeKnown=false;
    c->lastHookRebind=GetTickCount64();++c->hookRebinds;
    ClearCache();
    if(GetIcon().hideNames)RefreshListLabels(c->h);
    Wh_Log(L"Desktop startup: callbacks freshly rebound on UI thread (pass=%u, list=%p)",c->hookRebinds,c->h);
    return true;
}
static void DesktopReadinessTick(HWND parent,ListCtx* c){
    if(c->retired||c->rebinding||g_desktopStopping)return;
    HWND list=c->h;
    if(!IsWindow(list)||GetParent(list)!=parent){
        RetireContextOnUI(parent,c);return;
    }
    // The shell can replace the parent callback before its settle timer fires.
    // Keep the same configured deadline, but allow the worker tick to finish it.
    if(c->settling&&c->settleUntil&&GetTickCount64()>=c->settleUntil){
        KillTimer(parent,kSettleTimer);
        c->settling=false;c->settleUntil=0;
        ClearCache();RedrawWindow(list,nullptr,nullptr,RDW_INVALIDATE|RDW_ERASE);
        Wh_Log(L"Desktop startup: refresh settle deadline recovered");
    }
    c->kind=Classify(list);
    if(c->kind!=K_DESKTOP){KillTimer(parent,kDesktopReadyTimer);return;}
    DWORD_PTR ref=0;
    if(!GetWindowSubclass(parent,ParentProc,kParentSubclassId,&ref)||ref!=(DWORD_PTR)c)
        if(!SetWindowSubclass(parent,ParentProc,kParentSubclassId,(DWORD_PTR)c))return;
    ref=0;
    if(!GetWindowSubclass(list,ListProc,kListSubclassId,&ref)||ref!=(DWORD_PTR)c){
        if(!SetWindowSubclass(list,ListProc,kListSubclassId,(DWORD_PTR)c))return;
        c->startupApplied=false;c->startupPaintSeen=false;
        Wh_Log(L"Desktop startup: repaired list subclass (list=%p)",list);
    }
    unsigned epoch=g_desktopEpoch.load();
    if(c->startupEpoch!=epoch){
        c->startupEpoch=epoch;c->startupApplied=false;c->startupPaintSeen=false;
        c->startupState=-1;c->startupSince=GetTickCount64();c->recoveryPass=0;
        if(GetIcon().hideNames)RefreshListLabels(list);
    }
    IconCfg cfg=GetIcon();
    if(!cfg.NeedsWork()){
        StartupState(c,4,L"ready; image effects disabled");
        SetTimer(parent,kDesktopReadyTimer,2000,nullptr);return;
    }
    int cx=0,cy=0;LRESULT view=0;
    HIMAGELIST images=GetListImageList(list,&cx,&cy,&view);
    int items=(int)SendMessageW(list,LVM_GETITEMCOUNT,0,0);
    bool ready=images&&cx>0&&cy>0&&items>0&&ImageList_GetImageCount(images)>0;
    if(!ready){
        c->startupApplied=false;
        StartupState(c,1,L"waiting for icons/image list");
        SetTimer(parent,kDesktopReadyTimer,500,nullptr);return;
    }
    if(c->readyHiml!=images||c->readyCx!=cx||c->readyCy!=cy){
        c->readyHiml=images;c->readyCx=cx;c->readyCy=cy;
        c->startupApplied=false;c->startupPaintSeen=false;
        c->startupSince=GetTickCount64();c->recoveryPass=0;
        ClearCache();
    }
    // Window creation can finish before the shell finishes installing its own
    // callbacks. Repeat the desktop-only detach/attach once the data has settled.
    // A worker dispatches this tick too, so recovery does not depend on our
    // parent timer still being in the message chain.
    ULONGLONG now=GetTickCount64();
    bool initialRebind=c->hookRebinds==0&&now-c->startupSince>=2000;
    bool stalledRebind=c->hookRebinds>0&&c->hookRebinds<3&&
                       !c->startupApplied&&now-c->lastHookRebind>=5000;
    if((initialRebind||stalledRebind)&&!c->settling&&!c->dragging&&IsWindowVisible(list)){
        if(!RebindDesktopCallbacks(parent,c))return;
    }
    // Explorer can populate the same image list after our first successful draw.
    // Rebuild cached pixels in bounded startup passes, on the owning UI thread.
    static constexpr ULONGLONG recoveryDelays[]={1000,3000,7000,15000};
    if(c->recoveryPass<ARRAYSIZE(recoveryDelays)&&
       GetTickCount64()-c->startupSince>=recoveryDelays[c->recoveryPass]){
        ++c->recoveryPass;ClearCache();c->startupApplied=false;c->startupPaintSeen=false;
        Wh_Log(L"Desktop startup: delayed reapply %u",c->recoveryPass);
    }
    // Arrow-only customization may have no shortcut overlays to replace.
    // A confirmed custom-draw pass is sufficient in that case.
    if(!cfg.HasEffects()&&c->startupPaintSeen)c->startupApplied=true;
    if(c->settling){
        StartupState(c,5,L"waiting for refresh/resize to settle");
        SetTimer(parent,kDesktopReadyTimer,500,nullptr);return;
    }
    if(!c->startupApplied){
        StartupState(c,2,L"icon data ready; waiting for first processed frame");
        RedrawWindow(list,nullptr,nullptr,RDW_INVALIDATE|RDW_ERASE);
        if(GetTickCount64()-c->lastStartupNotice>=10000){
            c->lastStartupNotice=GetTickCount64();
            Wh_Log(L"Desktop startup: still retrying; customDraw=%d, items=%d, rebinds=%u, settling=%d",
                   c->startupPaintSeen,items,c->hookRebinds,c->settling);
        }
        SetTimer(parent,kDesktopReadyTimer,500,nullptr);
    }else{
        StartupState(c,3,L"rendering confirmed");
        SetTimer(parent,kDesktopReadyTimer,c->recoveryPass<4?500:2000,nullptr);
    }
}
static void AttachListOnUI(HWND list){
    if(g_desktopStopping||!IsWindow(list))return;
    if(!EnsureDesktopModuleLease()){
        Wh_Log(L"Desktop attach skipped: unable to retain callback module");
        return;
    }
    HWND parent=ListParentIfDefView(list);if(!parent)return;
    Kind kind=Classify(list);if(kind==K_NONE)return;
    // A tracked handle can have a new DefView parent after shell initialization.
    HWND oldParent=nullptr;ListCtx* previous=nullptr;
    EnterCriticalSection(&g_cs);
    for(auto const& kv:g_ctxByParent)
        if(kv.second->h==list&&kv.first!=parent){oldParent=kv.first;previous=kv.second;break;}
    LeaveCriticalSection(&g_cs);
    if(previous&&!RetireContextOnUI(oldParent,previous))return;
    ListCtx* c=nullptr;
    EnterCriticalSection(&g_cs);
    auto it=g_ctxByParent.find(parent);
    if(it!=g_ctxByParent.end())c=it->second;
    LeaveCriticalSection(&g_cs);
    if(c&&c->h&&c->h!=list){
        if(!RetireContextOnUI(parent,c))return;
        c=nullptr;
    }
    bool fresh=!c;
    if(fresh){c=new ListCtx();c->h=list;c->parent=parent;c->kind=kind;}
    DWORD_PTR ref=0;
    bool parentOK=GetWindowSubclass(parent,ParentProc,kParentSubclassId,&ref)&&ref==(DWORD_PTR)c;
    if(!parentOK&&!SetWindowSubclass(parent,ParentProc,kParentSubclassId,(DWORD_PTR)c)){
        if(fresh)delete c;Wh_Log(L"Desktop attach: parent subclass failed");return;
    }
    ref=0;
    bool listOK=GetWindowSubclass(list,ListProc,kListSubclassId,&ref)&&ref==(DWORD_PTR)c;
    if(!listOK&&!SetWindowSubclass(list,ListProc,kListSubclassId,(DWORD_PTR)c)){
        if(fresh){
            RemoveWindowSubclass(parent,ParentProc,kParentSubclassId);
            ref=0;
            if(GetWindowSubclass(parent,ParentProc,kParentSubclassId,&ref)){
                EnterCriticalSection(&g_cs);
                g_ctxByParent[parent]=c;
                LeaveCriticalSection(&g_cs);
            }else delete c;
        }
        Wh_Log(L"Desktop attach: list subclass failed");return;
    }
    c->h=list;c->parent=parent;c->kind=kind;
    EnterCriticalSection(&g_cs);
    g_ctxByParent[parent]=c;
    if(std::find(g_lists.begin(),g_lists.end(),list)==g_lists.end())g_lists.push_back(list);
    LeaveCriticalSection(&g_cs);
    if(fresh||!parentOK||!listOK){
        c->startupApplied=false;c->startupPaintSeen=false;
        Wh_Log(L"Attached/verified %s list on UI thread %lu",kind==K_DESKTOP?L"desktop":L"Explorer",GetCurrentThreadId());
        InvalidateRect(list,nullptr,TRUE);
    }
    if(kind==K_DESKTOP)DesktopReadinessTick(parent,c);
}
static void DetachParentOnUI(HWND parent){
    ListCtx* c=nullptr;
    EnterCriticalSection(&g_cs);
    auto it=g_ctxByParent.find(parent);if(it!=g_ctxByParent.end())c=it->second;
    LeaveCriticalSection(&g_cs);
    if(c)RetireContextOnUI(parent,c);
}
static LRESULT CALLBACK DesktopDispatchHook(int code,WPARAM wp,LPARAM lp){
    if(g_desktopRetainState)return CallNextHookEx(nullptr,code,wp,lp);
    if(code==HC_ACTION){auto m=reinterpret_cast<const CWPSTRUCT*>(lp);
        if(m->message==g_desktopDispatchMsg){
            if(m->wParam==1&&!g_desktopStopping)AttachListOnUI(m->hwnd);
            else if(m->wParam==2)DetachParentOnUI(m->hwnd);
            else if(m->wParam==3)DetachParentOnUI((HWND)m->lParam);
        }
    }
    return CallNextHookEx(nullptr,code,wp,lp);
}
static void DispatchDesktopUI(HWND target,WPARAM action,LPARAM payload=0){
    DWORD pid=0,tid=GetWindowThreadProcessId(target,&pid);
    if(!tid||pid!=GetCurrentProcessId())return;
    if(tid==GetCurrentThreadId()){
        if(action==1)AttachListOnUI(target);else DetachParentOnUI(action==3?(HWND)payload:target);return;
    }
    if(!EnsureDesktopModuleLease()){
        Wh_Log(L"Desktop dispatch skipped: unable to retain callback module");
        return;
    }
    HHOOK hook=SetWindowsHookExW(WH_CALLWNDPROC,DesktopDispatchHook,nullptr,tid);
    if(!hook){Wh_Log(L"Desktop attach: UI dispatch hook failed (%lu)",GetLastError());return;}
    // Synchronous, pointer-free dispatch: the hook is removed only after its
    // callback completes, so neither queued arguments nor callbacks outlive unload.
    SendMessageW(target,g_desktopDispatchMsg,action,payload);
    if(!UnhookWindowsHookEx(hook)){
        g_desktopRetainState=true;
        Wh_Log(L"Desktop dispatch hook removal failed; retaining callback module");
    }
}
static void AttachList(HWND list){
    if(!g_desktopStopping)DispatchDesktopUI(list,1);
}

static BOOL CALLBACK FindShellListChild(HWND h,LPARAM lp){
    auto found=reinterpret_cast<HWND*>(lp);if(*found)return FALSE;
    DWORD pid=0;GetWindowThreadProcessId(h,&pid);if(pid!=GetCurrentProcessId())return TRUE;
    wchar_t cls[64]={};GetClassNameW(h,cls,64);
    if(!wcscmp(cls,L"SysListView32")&&ListParentIfDefView(h)&&Classify(h)==K_DESKTOP){*found=h;return FALSE;}
    return TRUE;
}
static BOOL CALLBACK FindShellListRoot(HWND h,LPARAM lp){
    DWORD pid=0;GetWindowThreadProcessId(h,&pid);if(pid!=GetCurrentProcessId())return TRUE;
    wchar_t cls[64]={};GetClassNameW(h,cls,64);
    if(!wcscmp(cls,L"Progman")||!wcscmp(cls,L"WorkerW"))EnumChildWindows(h,FindShellListChild,lp);
    return !*reinterpret_cast<HWND*>(lp);
}
static HWND FindDesktopList(){HWND found=nullptr;EnumWindows(FindShellListRoot,(LPARAM)&found);return found;}

static BOOL CALLBACK EnumListProc(HWND h, LPARAM) {
    wchar_t cls[32] = {};
    GetClassNameW(h, cls, 32);
    if (!wcscmp(cls, L"SysListView32")) AttachList(h);
    return TRUE;
}

static BOOL CALLBACK EnumExplorerProc(HWND h, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    wchar_t cls[32] = {};
    GetClassNameW(h, cls, 32);
    if (!wcscmp(cls, L"CabinetWClass")) EnumChildWindows(h, EnumListProc, 0);
    return TRUE;
}

// the desktop and every File Explorer window that is already open when the mod loads
static void AttachExisting() {
    HWND list = FindDesktopList();
    if (list) {
        Wh_Log(L"Desktop list view found at load time");
    } else {
        Wh_Log(L"Desktop list view not created yet - waiting for explorer to create it");
    }
    // Desktop attachment is deferred to DesktopWatch: seeing the window during
    // shell creation is not evidence that its callback setup is complete.
    EnumWindows(EnumExplorerProc, 0);
}

static bool DetachAll(){
    std::vector<std::pair<HWND,HWND>> parents;
    EnterCriticalSection(&g_cs);for(auto const& kv:g_ctxByParent)parents.push_back({kv.first,kv.second->h});LeaveCriticalSection(&g_cs);
    for(auto const& target:parents){
        if(IsWindow(target.first))DispatchDesktopUI(target.first,2);
        else if(IsWindow(target.second))DispatchDesktopUI(target.second,3,(LPARAM)target.first);
    }
    EnterCriticalSection(&g_cs);
    // A still-live owner window means dispatch/removal failed. Do not release
    // ref-data or DLL code that Windows may still call. Dead windows own no callbacks.
    for(auto const& kv:g_ctxByParent){
        if(IsWindow(kv.first)||IsWindow(kv.second->h)){
            g_desktopRetainState=true;
            LeaveCriticalSection(&g_cs);
            Wh_Log(L"Desktop cleanup incomplete; restart Explorer before re-enabling this version");
            return false;
        }
    }
    if(g_desktopRetainState){LeaveCriticalSection(&g_cs);return false;}
    for(auto const& kv:g_ctxByParent){FreeAll(kv.second);delete kv.second;}
    for(auto c:g_retiredListContexts){FreeAll(c);delete c;}
    g_retiredListContexts.clear();g_ctxByParent.clear();g_lists.clear();
    LeaveCriticalSection(&g_cs);
    return true;
}

static void InvalidateAllLists() {
    std::vector<HWND> copy;
    EnterCriticalSection(&g_cs);
    copy = g_lists;
    LeaveCriticalSection(&g_cs);
    for (HWND h : copy)
        if (IsWindow(h)) InvalidateRect(h, nullptr, TRUE);
}

static void RefreshDesktopLabelsAll() {
    std::vector<HWND> copy;
    EnterCriticalSection(&g_cs);
    copy = g_lists;
    LeaveCriticalSection(&g_cs);
    for (HWND h : copy)
        if (IsWindow(h) && Classify(h) == K_DESKTOP) RefreshListLabels(h);
}

// ====================================================================== app borders: styles and per-app rules
struct Look {
    std::wstring effect = L"static", custom;
    float c1[3] = {0.f, 0.75f, 1.f}, c2[3] = {1.f, 0.f, 1.f};
    float op = 1.f, speed = 1.f, power = 0.7f;
    int count = 2;
    Program prog;
    bool animated = false;
};
struct Style {
    int bw = 3, radius = 8;
    bool hide = false;
    int inactKind = 0;                 // 0 = solid color, 1 = same look (frozen), 2 = no border
    float inactColor[3] = {0.33f, 0.33f, 0.33f};
    Look act, inact;
};

static BorderCfg g_bc;
static ExperimentalExtras::Config g_extra;
static RECT g_adaptiveLastRect = {};
static HWND g_adaptiveLastWindow = nullptr;
static DWORD g_adaptiveActiveUntil = 0;
static void ObserveAdaptiveActivity() {
    HWND fg = GetForegroundWindow(); RECT r = {};
    if (fg && GetWindowRect(fg, &r)) {
        if (fg != g_adaptiveLastWindow || !EqualRect(&r, &g_adaptiveLastRect)) g_adaptiveActiveUntil = GetTickCount() + 350;
        g_adaptiveLastWindow = fg; g_adaptiveLastRect = r;
    } else g_adaptiveLastWindow = nullptr;
}
static ExperimentalBorders::Config g_experimental;
static int UncappedAnimationFPS() {
    
    if (g_bc.fpsMode == L"auto") return MonitorRefreshHz(GetForegroundWindow());
    if (g_bc.fpsMode == L"adaptive")
        return (int)(GetTickCount() - g_adaptiveActiveUntil) < 0 ? MonitorRefreshHz(GetForegroundWindow()) : 60;
    if (g_bc.fpsMode == L"adaptiveLite")
        return (int)(GetTickCount() - g_adaptiveActiveUntil) < 0 ? MonitorRefreshHz(GetForegroundWindow()) : 30;
    return g_bc.fps;
}

static int EffectiveAnimationFPS(){return PerformanceOptions::Cap(UncappedAnimationFPS());}
static MotionCfg g_mb;
static Style g_defStyle;
static std::map<std::wstring, Style> g_rules;
static std::vector<std::wstring> g_excl;
static bool g_anyAmb = false, g_anyCin = false;

static void RgbToF(const int c[3], float out[3]) { for (int i = 0; i < 3; i++) out[i] = (float)c[i] / 255.f; }

static bool HexToF(const std::wstring& v, float out[3]) {
    std::wstring t = Trim(v);
    if (t.empty()) return false;
    int r, g, b;
    ParseRgb(t.c_str(), L"000000", &r, &g, &b);
    out[0] = (float)r / 255.f; out[1] = (float)g / 255.f; out[2] = (float)b / 255.f;
    return true;
}

static void CompileLook(Look& L) {
    CompileEffectCode(L.effect, L.custom, &L.prog);
    L.animated = L.prog.usesTime;
    if (L.prog.usesAmb) g_anyAmb = true;
    if (L.prog.usesCin) g_anyCin = true;
}

static void FinalizeStyle(Style& st) {
    CompileLook(st.act);
    if (st.inactKind == 0) {
        st.inact = Look();
        st.inact.effect = L"static";
        for (int i = 0; i < 3; i++) st.inact.c1[i] = st.inactColor[i];
        st.inact.op = st.act.op;
        CompileLook(st.inact);
    } else if (st.inactKind == 1) {
        st.inact = st.act;
    }
}

static void BuildDefaultStyle(Style& st, const BorderCfg& bc) {
    st = Style();
    st.bw = bc.bw;
    st.radius = bc.radius;
    st.act.effect = bc.effect;
    st.act.custom = bc.custom;
    RgbToF(bc.c1, st.act.c1);
    RgbToF(bc.c2, st.act.c2);
    st.act.op = bc.op;
    st.act.speed = bc.speed;
    st.act.power = bc.power;
    st.act.count = bc.count;
    st.inactKind = bc.inactiveMode == L"off" ? 2 : (bc.inactiveMode == L"same" ? 1 : 0);
    RgbToF(bc.inactiveColor, st.inactColor);
}

static bool ParseBoolStr(const std::wstring& v) {
    std::wstring l = Lower(Trim(v));
    return l == L"yes" || l == L"true" || l == L"1" || l == L"on";
}

// "app.exe: key=value, key=value" (a formula goes inside { })
static bool ParseRule(const std::wstring& line, std::wstring& exe, std::vector<std::pair<std::wstring, std::wstring>>& kv) {
    size_t colon = line.find(L':');
    if (colon == std::wstring::npos) return false;
    exe = Lower(Trim(line.substr(0, colon)));
    if (exe.empty()) return false;
    std::wstring rest = line.substr(colon + 1);
    std::vector<std::wstring> toks;
    std::wstring cur;
    int depth = 0;
    for (wchar_t ch : rest) {
        if (ch == L'{') depth++;
        if (ch == L'}' && depth > 0) depth--;
        if (ch == L',' && depth == 0) { toks.push_back(cur); cur.clear(); }
        else cur.push_back(ch);
    }
    toks.push_back(cur);
    for (const std::wstring& tk : toks) {
        size_t eq = tk.find(L'=');
        if (eq == std::wstring::npos) continue;
        std::wstring k = Lower(Trim(tk.substr(0, eq)));
        std::wstring v = Trim(tk.substr(eq + 1));
        if (v.size() >= 2 && v.front() == L'{' && v.back() == L'}') v = v.substr(1, v.size() - 2);
        if (!k.empty()) kv.push_back({k, v});
    }
    return true;
}

static void ApplyRuleKeys(Style& st, const std::vector<std::pair<std::wstring, std::wstring>>& kv) {
    bool customGiven = false, effectGiven = false;
    for (const auto& p : kv) {
        const std::wstring& k = p.first;
        const std::wstring& v = p.second;
        if (k == L"effect") { st.act.effect = Lower(Trim(v)); effectGiven = true; }
        else if (k == L"color" || k == L"color1") HexToF(v, st.act.c1);
        else if (k == L"color2") HexToF(v, st.act.c2);
        else if (k == L"width") st.bw = ClampI(_wtoi(v.c_str()), 0, 40);
        else if (k == L"radius") st.radius = ClampI(_wtoi(v.c_str()), 0, 60);
        else if (k == L"opacity") st.act.op = (float)ClampI(_wtoi(v.c_str()), 1, 100) / 100.f;
        else if (k == L"speed") st.act.speed = (float)ClampI(_wtoi(v.c_str()), 1, 100) / 20.f;
        else if (k == L"intensity") st.act.power = (float)ClampI(_wtoi(v.c_str()), 0, 100) / 100.f;
        else if (k == L"count") st.act.count = ClampI(_wtoi(v.c_str()), 1, 16);
        else if (k == L"custom") { st.act.custom = v; customGiven = true; }
        else if (k == L"hide") st.hide = ParseBoolStr(v);
        else if (k == L"inactive") {
            std::wstring l = Lower(Trim(v));
            if (l == L"off") st.inactKind = 2;
            else if (l == L"same") st.inactKind = 1;
            else if (HexToF(v, st.inactColor)) st.inactKind = 0;
        }
    }
    if (customGiven && !effectGiven) st.act.effect = L"custom";
}

static void LoadEngineExclusions();
static void LoadStyles() {
    g_extra=ExperimentalExtras::Load();
    g_experimental=ExperimentalBorders::Load(false);
    g_bc = GetBorderCfg();
    LoadEngineExclusions();
    g_mb = GetMotion();
    g_anyAmb = g_anyCin = false;
    g_rules.clear();
    g_excl.clear();
    BuildDefaultStyle(g_defStyle, g_bc);
    FinalizeStyle(g_defStyle);
    for (const std::wstring& line : g_bc.rules) {
        std::wstring exe;
        std::vector<std::pair<std::wstring, std::wstring>> kv;
        if (!ParseRule(line, exe, kv)) { Wh_Log(L"Ignored rule (no 'app.exe:' prefix): %s", line.c_str()); continue; }
        Style st;
        BuildDefaultStyle(st, g_bc);
        ApplyRuleKeys(st, kv);
        FinalizeStyle(st);
        g_rules[exe] = st;
    }
    for (const std::wstring& e : g_bc.excludes) g_excl.push_back(Lower(Trim(e)));
    Wh_Log(L"App borders loaded: enabled=%d selected=%s animated=%d experimental=%d motion=%d rules=%u",
        g_bc.enable,g_bc.effect.c_str(),g_defStyle.act.animated,g_experimental.enabled,g_mb.on,(unsigned)g_rules.size());
}

static const Style* StyleFor(const std::wstring& exe) {
    auto it = g_rules.find(exe);
    return it != g_rules.end() ? &it->second : &g_defStyle;
}

// ====================================================================== app borders: monitors and colors
struct MonInfo { HMONITOR h; RECT rc; };
static std::vector<MonInfo> g_mons;
static std::vector<ColorTable> g_amb, g_wall;
static double g_nextMon = 0, g_nextAmb = 0;

static BOOL CALLBACK EnumMonProc(HMONITOR m, HDC, LPRECT, LPARAM lp) {
    MONITORINFO mi = {sizeof(mi)};
    if (GetMonitorInfoW(m, &mi)) {
        MonInfo mo;
        mo.h = m;
        mo.rc = mi.rcMonitor;
        ((std::vector<MonInfo>*)lp)->push_back(mo);
    }
    return TRUE;
}
static void RefreshMons() {
    std::vector<MonInfo> v;
    EnumDisplayMonitors(nullptr, nullptr, EnumMonProc, (LPARAM)&v);
    g_mons = v;
    if (g_amb.size() != g_mons.size()) { g_amb.assign(g_mons.size(), ColorTable()); g_wall.assign(g_mons.size(), ColorTable()); g_nextAmb=0; }
}
static int MonIndex(HWND h) {
    HMONITOR m = MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST);
    for (size_t i = 0; i < g_mons.size(); i++)
        if (g_mons[i].h == m) return (int)i;
    return 0;
}

// ====================================================================== app borders: windows
static const wchar_t* kBorderClass = L"DodiBorderOverlay";
static const wchar_t* kTrailClass = L"DodiTrailOverlay";
static std::set<HWND> g_own;

static LRESULT CALLBACK OverlayWndProc(HWND h, UINT msg, WPARAM w, LPARAM l) {
    switch (msg) {
        case WM_NCHITTEST: return HTTRANSPARENT;
        case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    }
    return DefWindowProcW(h, msg, w, l);
}

struct Border {
    HWND target = nullptr, hwnd = nullptr;
    HWND tiles[3]{};
    BorderPresentation::Rect tileRects[4]{};
    bool tiled=false,tilingFailed=false;
    std::wstring exe;
    const Style* st = nullptr;
    RECT frame = {};
    int ext = 0, W = 0, H = 0, ow = 0, oh = 0;
    HDC dc = nullptr;
    HBITMAP bmp = nullptr;
    HGDIOBJ old = nullptr;
    DWORD* bits = nullptr;
    std::vector<BandPx> band, inactiveBand;
    ExperimentalBorders::Motion interaction;
    bool active = false, shown = false, dirty = true, dirtyPix = true;
    int mon = 0;
    ColorTable cin, ambient;
    CinemaSampling::Clock cinema;
    double lastRender = 0, lastPresent = 0, lastSampleLog = -100;
    EffectSmoothing::Frame smooth;
    ExperimentalExtras::Transition focus;
};
static int CinemaRate(const Border& b, double now);
static std::map<HWND, Border*> g_borders;
static std::map<HWND, bool> g_elig;
static std::map<HWND, RECT> g_lastFrame;
static bool g_dirty = false;
static HWND g_fgRoot = nullptr;

static bool Cloaked(HWND h) {
    DWORD v = 0;
    return SUCCEEDED(DwmGetWindowAttribute(h, 14 /* DWMWA_CLOAKED */, &v, sizeof(v))) && v != 0;
}
static bool GetFrame(HWND h, RECT* r) {
    if (SUCCEEDED(DwmGetWindowAttribute(h, 9 /* DWMWA_EXTENDED_FRAME_BOUNDS */, r, sizeof(RECT)))) return true;
    return GetWindowRect(h, r) != 0;
}
static std::wstring ExeOf(HWND h) {
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (!pid) return L"";
    HANDLE ph = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!ph) return L"";
    wchar_t buf[MAX_PATH];
    DWORD n = MAX_PATH;
    std::wstring r;
    if (QueryFullProcessImageNameW(ph, 0, buf, &n)) {
        const wchar_t* b = wcsrchr(buf, L'\\');
        r = Lower(b ? b + 1 : buf);
    }
    CloseHandle(ph);
    return r;
}


// Overlay targeting must independently honor engine and mod exclusions: the
// overlay lives inside Explorer, so Windhawk injection filtering alone cannot do it.
static std::wstring ReadRegString(HKEY root, const std::wstring& key, PCWSTR name) {
    DWORD type=0, cb=0;
    if(RegGetValueW(root,key.c_str(),name,RRF_RT_REG_SZ|RRF_RT_REG_EXPAND_SZ|RRF_NOEXPAND|RRF_SUBKEY_WOW6464KEY,
        &type,nullptr,&cb)!=ERROR_SUCCESS || cb<sizeof(wchar_t) || cb>1024*1024) return L"";
    std::vector<wchar_t> v(cb/sizeof(wchar_t)+1,0);
    if(RegGetValueW(root,key.c_str(),name,RRF_RT_REG_SZ|RRF_RT_REG_EXPAND_SZ|RRF_NOEXPAND|RRF_SUBKEY_WOW6464KEY,
        &type,v.data(),&cb)!=ERROR_SUCCESS) return L"";
    DWORD needed=ExpandEnvironmentStringsW(v.data(),nullptr,0);
    if(needed && needed<32768) { std::vector<wchar_t> expanded(needed);
        if(ExpandEnvironmentStringsW(v.data(),expanded.data(),needed)) return expanded.data(); }
    return v.data();
}

// Built-in list snapshot from the official Windhawk source, 2026-10-15:
// https://github.com/ramensoftware/windhawk/blob/main/src/windhawk/engine/process_lists.h
namespace WindhawkDefaultExclusions {

// Originally based on:
// https://www.elastic.co/guide/en/security/current/unusual-parent-child-relationship.html
// https://github.com/elastic/security-docs/blob/9e98d789cb7b8d8fe98a3c3dec5012c4e1f22e99/docs/detections/prebuilt-rules/rule-details/unusual-parent-child-relationship.asciidoc
inline constexpr WCHAR kCriticalProcesses[] =
    LR"(%systemroot%\system32\audiodg.exe|)"
    LR"(%systemroot%\system32\autochk.exe|)"
    LR"(%systemroot%\syswow64\autochk.exe|)"
    LR"(%systemroot%\system32\chkdsk.exe|)"
    LR"(%systemroot%\syswow64\chkdsk.exe|)"
    // LR"(%systemroot%\system32\conhost.exe|)"
    LR"(%systemroot%\system32\consent.exe|)"
    LR"(%systemroot%\system32\csrss.exe|)"
    // LR"(%systemroot%\system32\dllhost.exe|)"
    // LR"(%systemroot%\syswow64\dllhost.exe|)"
    LR"(%systemroot%\system32\doskey.exe|)"
    LR"(%systemroot%\syswow64\doskey.exe|)"
    LR"(%systemroot%\system32\dwm.exe|)"
    LR"(%systemroot%\system32\fontdrvhost.exe|)"
    LR"(%systemroot%\system32\logonui.exe|)"
    LR"(%systemroot%\system32\lsaiso.exe|)"
    LR"(%systemroot%\system32\lsass.exe|)"
    // LR"(%systemroot%\system32\runtimebroker.exe|)"
    LR"(%systemroot%\system32\searchindexer.exe|)"
    LR"(%systemroot%\syswow64\searchindexer.exe|)"
    LR"(%systemroot%\system32\searchprotocolhost.exe|)"
    LR"(%systemroot%\syswow64\searchprotocolhost.exe|)"
    LR"(%systemroot%\system32\services.exe|)"
    LR"(%systemroot%\system32\setupcl.exe|)"
    LR"(%systemroot%\system32\smss.exe|)"
    LR"(%systemroot%\system32\spoolsv.exe|)"
    // LR"(%systemroot%\system32\svchost.exe|)"
    // LR"(%systemroot%\syswow64\svchost.exe|)"
    // LR"(%systemroot%\system32\taskhostw.exe|)"
    // LR"(%systemroot%\system32\userinit.exe|)"
    // LR"(%systemroot%\syswow64\userinit.exe|)"
    // LR"(%systemroot%\system32\werfault.exe|)"
    // LR"(%systemroot%\syswow64\werfault.exe|)"
    LR"(%systemroot%\system32\werfaultsecure.exe|)"
    LR"(%systemroot%\syswow64\werfaultsecure.exe|)"
    LR"(%systemroot%\system32\wermgr.exe|)"
    LR"(%systemroot%\syswow64\wermgr.exe|)"
    LR"(%systemroot%\system32\wininit.exe|)"
    // LR"(%systemroot%\system32\winlogon.exe|)"
    LR"(%systemroot%\system32\winrshost.exe|)"
    LR"(%systemroot%\syswow64\winrshost.exe|)"
    LR"(%systemroot%\system32\wbem\wmiprvse.exe|)"
    LR"(%systemroot%\syswow64\wbem\wmiprvse.exe|)"
    LR"(%systemroot%\system32\wsmprovhost.exe|)"
    LR"(%systemroot%\syswow64\wsmprovhost.exe)";

inline constexpr WCHAR kCriticalProcessesForMods[] =
    LR"(%systemroot%\system32\svchost.exe|)"
    LR"(%systemroot%\syswow64\svchost.exe|)"
    LR"(%systemroot%\system32\werfault.exe|)"
    LR"(%systemroot%\syswow64\werfault.exe|)"
    LR"(%systemroot%\system32\winlogon.exe)";

inline constexpr WCHAR kIncompatiblePrograms[] =
    LR"(%ProgramFiles%\Oracle\VirtualBox\*|)"
    LR"(%ProgramFiles(X86)%\Oracle\VirtualBox\*)";

#define ALL_PROGRAM_FILES(folder) \
    LR"(?:\Program Files\)" folder LR"(\*|?:\Program Files (x86)\)" folder LR"(\*|)"

inline constexpr WCHAR kGames[] =
    ALL_PROGRAM_FILES(L"2K Games")
    ALL_PROGRAM_FILES(L"Activision")
    ALL_PROGRAM_FILES(L"Battle.net")
    ALL_PROGRAM_FILES(L"Bethesda Softworks")
    ALL_PROGRAM_FILES(L"Bethesda.net Launcher")
    ALL_PROGRAM_FILES(L"Blizzard Entertainment")
    ALL_PROGRAM_FILES(L"Diablo IV")
    ALL_PROGRAM_FILES(L"EA Games")
    ALL_PROGRAM_FILES(L"EA")
    ALL_PROGRAM_FILES(L"EasyAntiCheat_EOS")
    ALL_PROGRAM_FILES(L"Electronic Arts")
    ALL_PROGRAM_FILES(L"Epic Games")
    ALL_PROGRAM_FILES(L"GOG Galaxy")
    ALL_PROGRAM_FILES(L"Google\\Play Games")
    ALL_PROGRAM_FILES(L"Google\\Play Games Services")
    ALL_PROGRAM_FILES(L"Grinding Gear Games")
    ALL_PROGRAM_FILES(L"Microsoft Games")
    ALL_PROGRAM_FILES(L"Origin Games")
    ALL_PROGRAM_FILES(L"Paradox Interactive")
    ALL_PROGRAM_FILES(L"Riot Games")
    ALL_PROGRAM_FILES(L"Riot Vanguard")
    ALL_PROGRAM_FILES(L"Rockstar Games")
    ALL_PROGRAM_FILES(L"Square Enix")
    ALL_PROGRAM_FILES(L"Steam")
    ALL_PROGRAM_FILES(L"Ubisoft")
    ALL_PROGRAM_FILES(L"Valve")
    ALL_PROGRAM_FILES(L"Wargaming.net")
    ALL_PROGRAM_FILES(L"WeGame")
    ALL_PROGRAM_FILES(L"World of Warcraft")
    LR"(?:\Epic Games\*|)"
    LR"(?:\Games\*|)"
    LR"(?:\Riot Games\*|)"
    LR"(?:\WeGameApps\*|)"
    LR"(?:\WindowsApps\Microsoft.MinecraftUWP_*\*|)"
    LR"(?:\WindowsApps\Microsoft.SunriseBaseGame_*\*|)"
    LR"(?:\XboxGames\*|)"
    LR"(*\steamapps\common\*)";

#undef ALL_PROGRAM_FILES

} // namespace WindhawkDefaultExclusions

static std::vector<std::wstring> g_engineExclusions;
static std::wstring ExpandExclusionText(const std::wstring& text){
    DWORD n=ExpandEnvironmentStringsW(text.c_str(),nullptr,0);
    if(!n||n>1024*1024)return text;
    std::vector<wchar_t> expanded(n);
    return ExpandEnvironmentStringsW(text.c_str(),expanded.data(),n)?std::wstring(expanded.data()):text;
}
static void AddInheritedExclusions(const std::wstring& text){
    std::wstring expanded=ExpandExclusionText(text);
    size_t at=0;
    while(at<expanded.size()){
        size_t end=expanded.find(L'|',at);
        std::wstring item=Lower(Trim(expanded.substr(at,end==std::wstring::npos?end:end-at)));
        if(!item.empty())g_engineExclusions.push_back(std::move(item));
        if(end==std::wstring::npos)break;
        at=end+1;
    }
}
static std::wstring ExclusionParentPath(const std::wstring& path){
    size_t pos=path.find_last_of(L"\\/");
    return pos==std::wstring::npos?L"":path.substr(0,pos);
}
static std::wstring ReadExclusionIni(const std::wstring& file,PCWSTR section,PCWSTR name){
    // Process lists can exceed the small buffers used by typical INI examples.
    for(size_t capacity=512;capacity<=1024*1024;capacity*=2){
        std::vector<wchar_t> value(capacity);
        DWORD n=GetPrivateProfileStringW(section,name,L"",value.data(),(DWORD)value.size(),file.c_str());
        if(n<value.size()-1)return std::wstring(value.data(),n);
    }
    return L"";
}
struct ExclusionStorage {
    bool portable=false;
    HKEY root=HKEY_LOCAL_MACHINE;
    std::wstring registry=L"SOFTWARE\\Windhawk\\Engine",appData;
    std::wstring Read(const std::wstring& section,PCWSTR name,bool mod=false) const {
        if(portable){
            std::wstring file=appData+(mod?L"\\Mods\\" WH_MOD_ID L".ini":L"\\settings.ini");
            return ExpandExclusionText(ReadExclusionIni(file,section.c_str(),name));
        }
        std::wstring key=registry+(mod?L"\\Mods\\" WH_MOD_ID:L"");
        if(!section.empty()&&!(mod&&section==L"Mod"))key+=L"\\"+section;
        return ReadRegString(root,key,name);
    }
    int ReadInt(const std::wstring& section,PCWSTR name,bool mod=false,int fallback=0) const {
        if(portable){
            std::wstring value=Read(section,name,mod);
            return value.empty()?fallback:(int)wcstol(value.c_str(),nullptr,10);
        }
        std::wstring key=registry+(mod?L"\\Mods\\" WH_MOD_ID:L"");
        if(!section.empty()&&!(mod&&section==L"Mod"))key+=L"\\"+section;
        DWORD value=0,cb=sizeof(value);
        return RegGetValueW(root,key.c_str(),name,RRF_RT_REG_DWORD|RRF_SUBKEY_WOW6464KEY,
                            nullptr,&value,&cb)==ERROR_SUCCESS?(int)value:fallback;
    }
};
static ExclusionStorage LocateExclusionStorage(){
    ExclusionStorage result;
    // Same engine.ini location/storage fields used by Windhawk's StorageManager.
    HMODULE engine=GetModuleHandleW(L"windhawk.dll");
    wchar_t path[32768];
    DWORD n=engine?GetModuleFileNameW(engine,path,ARRAYSIZE(path)):0;
    std::wstring folder=n&&n<ARRAYSIZE(path)?ExclusionParentPath(ExclusionParentPath(path)):L"";
    std::wstring ini=folder+L"\\engine.ini";
    std::wstring portable=folder.empty()?L"":ReadExclusionIni(ini,L"Storage",L"Portable");
    std::wstring registry=folder.empty()?L"":ReadExclusionIni(ini,L"Storage",L"RegistryKey");
    if(!portable.empty()&&wcstol(portable.c_str(),nullptr,10)!=0){
        std::wstring appData=ExpandExclusionText(ReadExclusionIni(ini,L"Storage",L"AppDataPath"));
        if(!appData.empty()){
            if(PathIsRelativeW(appData.c_str()))appData=folder+L"\\"+appData;
            result.portable=true;result.appData=std::move(appData);return result;
        }
    }else if(!registry.empty()){
        size_t slash=registry.find(L'\\');
        std::wstring hive=Lower(registry.substr(0,slash));
        if(slash!=std::wstring::npos){
            if(hive==L"hklm"||hive==L"hkey_local_machine")result.root=HKEY_LOCAL_MACHINE;
            else if(hive==L"hkcu"||hive==L"hkey_current_user")result.root=HKEY_CURRENT_USER;
            else if(hive==L"hku"||hive==L"hkey_users")result.root=HKEY_USERS;
            else return result;
            result.registry=registry.substr(slash+1);return result;
        }
    }
    static bool logged=false;
    if(!logged){logged=true;Wh_Log(L"Border exclusions: engine storage not resolved; using default HKLM storage and bundled built-in lists");}
    return result;
}
static void LoadEngineExclusions() {
    g_engineExclusions.clear();
    if(!g_bc.inheritWindhawkExclusions)return;
    ExclusionStorage storage=LocateExclusionStorage();
    // Windhawk's built-ins are compiled into the engine, not stored in Exclude.
    // Honor the corresponding global injection switches (default: excluded).
    if(!storage.ReadInt(L"Settings",L"InjectIntoCriticalProcesses"))
        AddInheritedExclusions(WindhawkDefaultExclusions::kCriticalProcesses);
    if(!storage.ReadInt(L"Settings",L"InjectIntoIncompatiblePrograms"))
        AddInheritedExclusions(WindhawkDefaultExclusions::kIncompatiblePrograms);
    if(!storage.ReadInt(L"Settings",L"InjectIntoGames"))
        AddInheritedExclusions(WindhawkDefaultExclusions::kGames);
    if(!storage.ReadInt(L"Mod",L"PatternsMatchCriticalSystemProcesses",true))
        AddInheritedExclusions(WindhawkDefaultExclusions::kCriticalProcessesForMods);
    AddInheritedExclusions(storage.Read(L"Settings",L"Exclude"));
    bool customOnly=storage.ReadInt(L"Mod",L"IncludeExcludeCustomOnly",true)!=0;
    if(!customOnly)AddInheritedExclusions(storage.Read(L"Mod",L"Exclude",true));
    AddInheritedExclusions(storage.Read(L"Mod",L"ExcludeCustom",true));
    std::sort(g_engineExclusions.begin(),g_engineExclusions.end());
    g_engineExclusions.erase(std::unique(g_engineExclusions.begin(),g_engineExclusions.end()),g_engineExclusions.end());
}

static bool MatchesExclusion(HWND h, const std::wstring& cls) {
    DWORD pid=0;GetWindowThreadProcessId(h,&pid);
    HANDLE ph=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
    wchar_t path[32768]={};DWORD n=32768;
    bool got=ph && QueryFullProcessImageNameW(ph,0,path,&n);
    if(ph) CloseHandle(ph);
    // Unable to inspect a protected process: leave its windows alone.
    if(!got) return true;
    std::wstring full=Lower(path); const wchar_t* b=wcsrchr(full.c_str(),L'\\');
    std::wstring exe=b?b+1:full;
    auto match=[&](const std::wstring& p) {
        return !p.empty() && (PathMatchSpecW(full.c_str(),p.c_str()) || PathMatchSpecW(exe.c_str(),p.c_str()));
    };
    for(const auto& p:g_engineExclusions) if(match(p)) return true;
    for(const auto& p:g_excl) if(p==cls || match(p)) return true;
    return false;
}
static bool Eligible(HWND h) {
    if (!h || !IsWindow(h) || g_own.count(h)) return false;
    if (GetAncestor(h, GA_ROOT) != h) return false;
    DWORD ex = (DWORD)GetWindowLongW(h, GWL_EXSTYLE), st = (DWORD)GetWindowLongW(h, GWL_STYLE);
    if (!(st & WS_VISIBLE) || (st & WS_CHILD)) return false;
    if (ex & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) return false;
    if ((st & WS_CAPTION) != WS_CAPTION && !(st & WS_THICKFRAME) && !(ex & WS_EX_APPWINDOW)) return false;
    wchar_t cls[96] = {};
    GetClassNameW(h, cls, 96);
    std::wstring c = Lower(cls);
    static const wchar_t* bad[] = {L"shell_traywnd", L"shell_secondarytraywnd", L"progman", L"workerw", L"tasklistthumbnailwnd",
        L"notifyiconoverflowwindow", L"windows.ui.core.corewindow", L"foregroundstaging", L"xamlexplorerhostislandwindow",
        L"toplevelwindowforoverflowxamlisland", L"tooltips_class32", L"#32768", L"wh_simpleroundedcornersoverlay"};
    for (const wchar_t* b : bad)
        if (c == b) return false;
    if (Cloaked(h) || MatchesExclusion(h,c)) return false;
    RECT fr;
    if (!GetFrame(h, &fr) || fr.right - fr.left < 60 || fr.bottom - fr.top < 40) return false;
    if (!g_excl.empty()) {
        std::wstring exe = ExeOf(h);
        for (const std::wstring& e : g_excl)
            if (e == exe || e == c) return false;
    }
    return true;
}
static bool EligibleCached(HWND h) {
    auto it = g_elig.find(h);
    if (it != g_elig.end()) return it->second;
    bool e = Eligible(h);
    g_elig[h] = e;
    return e;
}

// distance to a rounded rectangle: negative inside, positive outside
static float RRectDist(float px, float py, float x0, float y0, float x1, float y1, float r) {
    float cx = (x0 + x1) * 0.5f, cy = (y0 + y1) * 0.5f, hx = (x1 - x0) * 0.5f, hy = (y1 - y0) * 0.5f;
    float qx = fabsf(px - cx) - (hx - r), qy = fabsf(py - cy) - (hy - r);
    float ox = std::max(qx, 0.f), oy = std::max(qy, 0.f);
    return sqrtf(ox * ox + oy * oy) + std::min(std::max(qx, qy), 0.f) - r;
}

// The band is centered on the window edge: half outside (visible) and half inside (behind the window).
static void BuildBand(Border& b) {
    b.band.clear();b.inactiveBand.clear();
    float bw = (float)b.st->bw;
    if (bw <= 0.f) return;
    bool experimental=g_experimental.enabled;
    float half = experimental?std::max((float)g_experimental.reach,bw*.5f):bw * 0.5f, ext = (float)b.ext, W = (float)b.W, H = (float)b.H;
    float r = (float)Imin(b.st->radius, Imin(b.W, b.H) / 2);
    int m = b.ext + (int)r + (int)half + 3;
    auto proc = [&](int lx, int ly) {
        float px = (float)lx + 0.5f, py = (float)ly + 0.5f;
        float dist = RRectDist(px, py, ext, ext, ext + W, ext + H, r);
        float cov = Clamp01(half - fabsf(dist) + 0.5f);
        if (cov <= 0.f) return;
        float cx = px - ext, cy = py - ext;
        auto anchor=ExperimentalBorders::Project(cx,cy,W,H,r);
        BandPx bp;
        bp.idx = ly * b.ow + lx;
        bp.x = (float)lx; bp.y = (float)ly;
        bp.u = cx / W; bp.v = cy / H;
        bp.p = anchor.p;
        bp.d = experimental?dist:Clamp01((half - dist) / bw);
        float a = atan2f(cy - H * 0.5f, cx - W * 0.5f) / 6.2831853f;
        bp.ang = a - floorf(a);
        bp.cov = cov;
        if(experimental){
            bp.p=anchor.p;bp.u=anchor.x;bp.v=anchor.y;
        }
        b.band.push_back(bp);
        if(g_experimental.enabled){
            float coverage=Clamp01(bw*.5f-fabsf(dist)+.5f);
            if(coverage>0){bp.cov=coverage;bp.d=Clamp01((bw*.5f-dist)/bw);bp.p=anchor.p;bp.u=cx/W;bp.v=cy/H;b.inactiveBand.push_back(bp);}
        }
    };
    for (int ly = 0; ly < b.oh; ly++) {
        if(RenderShutdown::requested.load())return;
        if (ly < m || ly >= b.oh - m) {
            for (int lx = 0; lx < b.ow; lx++) proc(lx, ly);
        } else {
            for (int lx = 0; lx < Imin(m, b.ow); lx++) proc(lx, ly);
            for (int lx = Imax(m, b.ow - m); lx < b.ow; lx++) proc(lx, ly);
        }
    }
}

static void ReleaseBorderTiles(Border& b){
    for(auto& h:b.tiles)if(h){g_own.erase(h);DestroyWindow(h);h=nullptr;}
    b.tiled=false;
}
template<class F> static void EachBorderWindow(Border& b,F operation){
    operation(b.hwnd,0);if(b.tiled)for(int i=0;i<3;i++)operation(b.tiles[i],i+1);
}
static bool PrepareBorderTiles(Border& b){
    if(b.tilingFailed||!PerformanceOptions::enabled.load()||!PerformanceOptions::tiledBorders.load())return false;
    float half=g_experimental.enabled?std::max((float)g_experimental.reach,b.st->bw*.5f):b.st->bw*.5f;
    int r=Imin(b.st->radius,Imin(b.W,b.H)/2);
    int margin=b.ext+r+(int)half+3;
    BorderPresentation::Tiles(b.ow,b.oh,margin,b.tileRects);
    for(auto& h:b.tiles)if(!h){
        h=CreateWindowExW(WS_EX_LAYERED|WS_EX_TRANSPARENT|WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE,
            kBorderClass,L"",WS_POPUP,0,0,1,1,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
        if(!h){ReleaseBorderTiles(b);b.tilingFailed=true;return false;}
        g_own.insert(h);CaptureVisibility::Apply(h);
    }
    b.tiled=true;return true;
}
static void StackBorder(Border& b){
    EachBorderWindow(b,[&](HWND h,int i){
        const auto& r=b.tileRects[i];
        if(b.tiled&&(r.w<=0||r.h<=0)){ShowWindow(h,SW_HIDE);return;}
        SetWindowPos(h,b.target,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_SHOWWINDOW);
    });b.shown=true;
}
static void FreeBorderBitmap(Border& b) {
    b.focus.Reset();
    b.smooth.Reset();
    if (b.dc) { SelectObject(b.dc, b.old); DeleteDC(b.dc); b.dc = nullptr; }
    if (b.bmp) { DeleteObject(b.bmp); b.bmp = nullptr; }
    b.bits = nullptr;
}
static bool AllocBorder(Border& b) {
    FreeBorderBitmap(b);
    b.ow = b.W + 2 * b.ext;
    b.oh = b.H + 2 * b.ext;
    HDC scr = GetDC(nullptr);
    void* bits = nullptr;
    HBITMAP bmp = MakeDib(scr, b.ow, b.oh, &bits);
    HDC dc = CreateCompatibleDC(scr);
    ReleaseDC(nullptr, scr);
    if (!bmp || !bits || !dc) {
        if (bmp) DeleteObject(bmp);
        if (dc) DeleteDC(dc);
        return false;
    }
    b.old = SelectObject(dc, bmp);
    b.dc = dc;
    b.bmp = bmp;
    b.bits = (DWORD*)bits;
    memset(b.bits,0,(size_t)b.ow*b.oh*4); // Initialize transparent centre once.
    return true;
}

static void HideBorder(Border& b) {
    b.focus.Reset();
    if(b.shown)EachBorderWindow(b,[](HWND h,int){if(h)ShowWindow(h,SW_HIDE);});
    b.shown = false;
}

static bool BehindNear(HWND t, HWND b) {
    HWND w = GetWindow(t, GW_HWNDNEXT);
    for (int i = 0; i < 5 && w; i++) {
        if (w == b) return true;
        w = GetWindow(w, GW_HWNDNEXT);
    }
    return false;
}

static void PlaceBorder(Border& b) {
    EachBorderWindow(b,[&](HWND h,int i){
        const auto& r=b.tileRects[i];
        if(b.tiled&&(r.w<=0||r.h<=0)){ShowWindow(h,SW_HIDE);return;}
        int x=(int)b.frame.left-b.ext+(b.tiled?r.x:0),y=(int)b.frame.top-b.ext+(b.tiled?r.y:0);
        SetWindowPos(h,b.target,x,y,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_SHOWWINDOW);
    });b.shown=true;
}

static void PresentBorderPixels(Border& b) {
    RenderTiming::Scope timing(RenderTiming::AppUpload);
    // Shading can take longer than a movement interval. Do not upload to the
    // rectangle captured before that work; keep same-size frames at the latest position.
    if(PerformanceOptions::follow.load()){
        RECT latest{};
        if(GetFrame(b.target,&latest)){
            if(latest.right-latest.left==b.W&&latest.bottom-latest.top==b.H)b.frame=latest;
            else b.dirty=true; // resizing still belongs to UpdateBorder/BuildBand
        }
    }
    HDC screen = GetDC(nullptr);
    POINT dst = {b.frame.left - b.ext, b.frame.top - b.ext};
    SIZE sz = {b.ow, b.oh};
    POINT src = {0, 0};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    bool presented=false;
    if(PrepareBorderTiles(b)){
        presented=true;
        EachBorderWindow(b,[&](HWND h,int i){
            const auto& r=b.tileRects[i];
            if(r.w<=0||r.h<=0){ShowWindow(h,SW_HIDE);return;}
            POINT at={dst.x+r.x,dst.y+r.y},from={r.x,r.y};SIZE size={r.w,r.h};
            if(!UpdateLayeredWindow(h,screen,&at,&size,b.dc,&from,0,&bf,ULW_ALPHA))presented=false;
        });
        if(!presented){ReleaseBorderTiles(b);b.tilingFailed=true;}
    }
    if(!presented)UpdateLayeredWindow(b.hwnd,screen,&dst,&sz,b.dc,&src,0,&bf,ULW_ALPHA);
    ReleaseDC(nullptr, screen);
}

static void RenderBorder(Border& b, double t) {
    const Style* st = b.st;
    const Look* L = b.active ? &st->act : &st->inact;
    if (!b.active && st->inactKind == 2) { HideBorder(b); return; }
    double tt = (b.active || g_bc.fpsMode == L"adaptive") ? t : 0.0;   // Adaptive animates unfocused borders at a lower rate

    for(const auto& px:b.band)b.bits[px.idx]=0; // Only cached edge pixels change.
    EvalParams ep;
    ep.timecycle = L->effect == L"timecycle";
    ep.weather = L->effect == L"weather";
    ep.W = (float)b.W; ep.H = (float)b.H; ep.mon = (float)b.mon; ep.bw = (float)st->bw; ep.rad = (float)st->radius;
    ep.op = L->op; ep.speed = L->speed; ep.power = L->power; ep.count = L->count;
    for (int i = 0; i < 3; i++) { ep.c1[i] = L->c1[i]; ep.c2[i] = L->c2[i]; }
    // App Ambient must never substitute monitor-edge colors for app-edge colors.
    const ColorTable* amb = L->effect == L"wallpaper"
        ? ((b.mon >= 0 && b.mon < (int)g_wall.size()) ? &g_wall[b.mon] : nullptr)
        : (b.ambient.valid ? &b.ambient : nullptr);
    if(g_experimental.enabled&&(b.active||st->inactKind==1)){
        b.interaction.Update(t,b.frame.left,b.frame.top,b.W,b.H);
        ExperimentalBorders::Draw(b.bits,b.band,g_experimental,b.interaction,tt,
            (float)b.W,(float)b.H,L->op,false,(float)b.ext,(float)b.ext,(float)st->radius);
    }else RenderBand(b.bits, g_experimental.enabled?b.inactiveBand:b.band, L->prog, ep, tt, amb, &b.cin);
    if(RenderShutdown::requested.load())return;
    if(g_extra.glass||g_extra.palette||g_extra.focus){
        float phase=0,mouseProximity=0;
        if(g_extra.glass&&g_extra.glassApp){POINT mouse{};if(GetCursorPos(&mouse)){
            float mx=(float)(mouse.x-b.frame.left),my=(float)(mouse.y-b.frame.top);
            auto at=ExperimentalBorders::Project(mx,my,(float)b.W,(float)b.H,(float)st->radius);
            phase=at.p;float dx=at.x-mx,dy=at.y-my;mouseProximity=expf(-(dx*dx+dy*dy)/(120.f*120.f));}}
        ExperimentalExtras::BorderPass(b.bits,b.band,g_extra,t,false,phase,mouseProximity,
            g_extra.focus&&!b.active?g_extra.inactive:1.f);
        b.focus.Apply(b.bits,b.band,t);
    }

    int rate=EffectiveAnimationFPS();
    if(L->prog.usesCin)rate=L->animated?std::max(rate,CinemaRate(b,t)):CinemaRate(b,t);
    bool smooth=!b.focus.Pending(t)&&!g_experimental.enabled&&EffectSmoothing::Use((L->animated&&(b.active||g_bc.fpsMode==L"adaptive"))||L->prog.usesCin,rate);
    if(smooth&&b.smooth.PushBand(b.bits,b.band,t,1.0/std::max(1,rate)))b.smooth.BlendBand(b.bits,b.band,t);
    else b.smooth.Reset();
    PresentBorderPixels(b);b.lastPresent=t;
    b.lastRender = t;
    if (!b.shown || !BehindNear(b.target, b.hwnd)) {
        StackBorder(b);
    }
    b.dirtyPix = false;
}

static void UpdateBorder(Border& b) {
    const Style* st = b.st;
    HWND t = b.target;
    if(st->inactKind==2&&t!=g_fgRoot){b.active=false;HideBorder(b);return;}
    RECT fr = {};
    bool vis = Eligible(t) && g_bc.enable && st->bw > 0 && !st->hide && IsWindowVisible(t) && !IsIconic(t) && !Cloaked(t) && GetFrame(t, &fr);
    if (vis && g_bc.hideMax) {
        if (IsZoomed(t)) vis = false;
        else {
            MONITORINFO mi = {sizeof(mi)};
            HMONITOR m = MonitorFromWindow(t, MONITOR_DEFAULTTONEAREST);
            if (GetMonitorInfoW(m, &mi) && fr.left <= mi.rcMonitor.left && fr.top <= mi.rcMonitor.top &&
                fr.right >= mi.rcMonitor.right && fr.bottom >= mi.rcMonitor.bottom)
                vis = false;
        }
    }
    if (!vis) { HideBorder(b); return; }

    int W = (int)(fr.right - fr.left), H = (int)(fr.bottom - fr.top);
    bool act=(t==g_fgRoot);
    if(act!=b.active){
        if(g_extra.focus)b.focus.Begin(b.bits,b.band,Now(),g_extra.focusMS);
        b.active=act;b.dirtyPix=true;if(g_experimental.enabled)b.interaction.activeUntil=Now()+.8;
    }
    bool experimental=g_experimental.enabled; // Reuse both geometry bands across focus changes.
    int ext = experimental?std::max(g_experimental.reach+2,(st->bw+1)/2+2):(st->bw + 1) / 2 + 2;
    bool sizeChanged = (W != b.W || H != b.H || ext != b.ext || !b.bits);
    bool moved = (fr.left != b.frame.left || fr.top != b.frame.top);
    b.frame = fr;
    if(g_experimental.enabled&&(sizeChanged||moved))b.interaction.activeUntil=Now()+.8;
    // Translation does not invalidate local geometry or demand an extra shader pass.
    // Mouse interaction and animation keep their normal scheduled render cadence.
    if(moved&&b.shown&&!sizeChanged)PlaceBorder(b);
    if (sizeChanged) {
        b.W = W; b.H = H; b.ext = ext;
        if (!AllocBorder(b)) { HideBorder(b); return; }
        BuildBand(b);
        b.dirtyPix = true;
    }
    if (!b.active && st->inactKind == 2) { HideBorder(b); return; }   // unfocused windows get no border
    b.mon = MonIndex(t);
    if (b.dirtyPix) RenderBorder(b, Now());
    else if (moved || !b.shown) PlaceBorder(b);
}

static void RemoveBorder(HWND t) {
    g_cinemaCapture.Forget((UINT_PTR)t);
    auto it = g_borders.find(t);
    if (it == g_borders.end()) return;
    Border* b = it->second;
    ReleaseBorderTiles(*b);
    if (b->hwnd) { g_own.erase(b->hwnd); DestroyWindow(b->hwnd); }
    FreeBorderBitmap(*b);
    delete b;
    g_borders.erase(it);
}

static void AddBorder(HWND t) {
    if (!g_bc.enable || g_borders.count(t) || !EligibleCached(t)) return;
    Border* b = new Border();
    b->target = t;
    b->exe = ExeOf(t);
    b->st = StyleFor(b->exe);
    if(b->st->hide||(b->st->inactKind==2&&t!=g_fgRoot)){delete b;return;}
    b->hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                              kBorderClass, L"", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!b->hwnd) { delete b; return; }
    CaptureVisibility::Apply(b->hwnd);
    g_own.insert(b->hwnd);
    g_borders[t] = b;
    UpdateBorder(*b);
}

static void DestroyAllBorders() {
    while (!g_borders.empty()) RemoveBorder(g_borders.begin()->first);
}

// ====================================================================== motion blur trail
struct TSample { int x, y; double t; };
struct Trail {
    HWND target = nullptr, wnd = nullptr;
    bool active = false;
    int W = 0, H = 0, pad = 0, gw = 0, gh = 0;
    HDC gdc = nullptr;
    HBITMAP gbmp = nullptr;
    HGDIOBJ gold = nullptr;
    HDC bdc = nullptr;
    HBITMAP bbmp = nullptr;
    HGDIOBJ bold = nullptr;
    DWORD* bbits = nullptr;
    int bcw = 0, bch = 0;
    std::vector<TSample> hist;
    double lastMove = 0;
};
static Trail g_tr;
struct MotionCounters {unsigned events=0,starts=0,uploads=0,failures=0;double last=0;};
static MotionCounters g_motionCounters;

static void TrailFreeSprite() {
    if (g_tr.gdc) { SelectObject(g_tr.gdc, g_tr.gold); DeleteDC(g_tr.gdc); g_tr.gdc = nullptr; }
    if (g_tr.gbmp) { DeleteObject(g_tr.gbmp); g_tr.gbmp = nullptr; }
}
static void TrailFreeBuffer() {
    if (g_tr.bdc) { SelectObject(g_tr.bdc, g_tr.bold); DeleteDC(g_tr.bdc); g_tr.bdc = nullptr; }
    if (g_tr.bbmp) { DeleteObject(g_tr.bbmp); g_tr.bbmp = nullptr; }
    g_tr.bbits = nullptr;
    g_tr.bcw = g_tr.bch = 0;
}
static void TrailStop() {
    if (g_tr.wnd) ShowWindow(g_tr.wnd, SW_HIDE);
    TrailFreeSprite();
    TrailFreeBuffer();
    g_tr.active = false;
    g_tr.target = nullptr;
    g_tr.hist.clear();
}

static void MotionDiagnostic(HWND target,int reason,PCWSTR message,DWORD error=0){
    if((reason>=1&&reason<=4)||reason==7||reason==9)++g_motionCounters.failures;
    if(!PerformanceOptions::timing.load())return;
    static HWND lastTarget=nullptr;static int lastReason=-1;static DWORD lastError=0;static double last=-100;
    double now=Now();if(target==lastTarget&&reason==lastReason&&error==lastError&&now-last<10)return;
    lastTarget=target;lastReason=reason;lastError=error;last=now;
    Wh_Log(L"Motion blur: target=0x%llX %s (error=%lu)",(unsigned long long)(UINT_PTR)target,message,error);
}
// Captures the window once and builds a soft, blurry copy of it (with a margin so the blur can spread).
static bool TrailBuild(const RECT& fr) {
    TrailFreeSprite();
    int W = (int)(fr.right - fr.left), H = (int)(fr.bottom - fr.top);
    if (W < 40 || H < 30 || (long long)W * H > 6000000LL){MotionDiagnostic(g_tr.target,1,L"capture rejected by window-size safety limit");return false;}
    int pad = Imax(6, g_mb.blur * 2);
    int gw = W + 2 * pad, gh = H + 2 * pad;
    std::vector<BYTE> raw((size_t)gw * gh * 4, 0);

    HDC scr = GetDC(nullptr);
    void* cb = nullptr;
    HBITMAP cbmp = MakeDib(scr, W, H, &cb);
    HDC cdc = CreateCompatibleDC(scr);
    bool ok = false;DWORD captureError=0;
    if (cbmp && cb && cdc) {
        HGDIOBJ o = SelectObject(cdc, cbmp);
        ok = BitBlt(cdc, 0, 0, W, H, scr, fr.left, fr.top, SRCCOPY) != 0;
        if(!ok)captureError=GetLastError();
        GdiFlush();
        if (ok) {
            float rad = (float)Imin(g_defStyle.radius, Imin(W, H) / 2);
            const BYTE* src = (const BYTE*)cb;
            for (int y = 0; y < H; y++)
                for (int x = 0; x < W; x++) {
                    float a = Clamp01(0.5f - RRectDist((float)x + 0.5f, (float)y + 0.5f, 0, 0, (float)W, (float)H, rad));
                    if (a <= 0.f) continue;
                    const BYTE* s = src + ((size_t)y * W + x) * 4;
                    BYTE* d = &raw[((size_t)(y + pad) * gw + (x + pad)) * 4];
                    d[0] = (BYTE)(s[0] * a); d[1] = (BYTE)(s[1] * a); d[2] = (BYTE)(s[2] * a); d[3] = (BYTE)(a * 255.f);
                }
        }
        SelectObject(cdc, o);
    }
    if (cdc) DeleteDC(cdc);
    if (cbmp) DeleteObject(cbmp);
    if (!ok) { MotionDiagnostic(g_tr.target,2,L"window capture/allocation failed",captureError);ReleaseDC(nullptr, scr); return false; }

    BlurRGBA(raw, gw, gh, g_mb.blur);

    void* sb = nullptr;
    HBITMAP sbmp = MakeDib(scr, gw, gh, &sb);
    HDC sdc = CreateCompatibleDC(scr);
    ReleaseDC(nullptr, scr);
    if (!sbmp || !sb || !sdc) {
        MotionDiagnostic(g_tr.target,3,L"blurred sprite allocation failed",GetLastError());
        if (sbmp) DeleteObject(sbmp);
        if (sdc) DeleteDC(sdc);
        return false;
    }
    memcpy(sb, raw.data(), raw.size());
    g_tr.gold = SelectObject(sdc, sbmp);
    g_tr.gdc = sdc;
    g_tr.gbmp = sbmp;
    g_tr.W = W; g_tr.H = H; g_tr.pad = pad; g_tr.gw = gw; g_tr.gh = gh;
    return true;
}

static void TrailStart(HWND t, const RECT& fr) {
    ++g_motionCounters.starts;
    if (g_tr.active) TrailStop();
    g_tr.target=t;
    if (!g_tr.wnd) {
        g_tr.wnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                                   kTrailClass, L"", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
        if (!g_tr.wnd){MotionDiagnostic(t,4,L"trail window creation failed",GetLastError());g_tr.target=nullptr;return;}
        CaptureVisibility::Apply(g_tr.wnd);
        g_own.insert(g_tr.wnd);
    }
    if (!TrailBuild(fr)){g_tr.target=nullptr;return;}
    g_tr.target = t;
    g_tr.active = true;
    g_tr.hist.clear();
    TSample s = {(int)fr.left, (int)fr.top, Now()};
    g_tr.hist.push_back(s);
    g_tr.lastMove = s.t;
    MotionDiagnostic(t,0,L"capture succeeded; trail history started");
}

// A window moved (called from the window events).
static void TrailConsider(HWND t, const RECT& prev, bool hadPrev, const RECT& fr) {
    if (!g_mb.on) { if (g_tr.active) TrailStop(); return; }
    if (!EligibleCached(t)){MotionDiagnostic(t,5,L"target excluded or ineligible");return;}
    if (IsZoomed(t) || IsIconic(t)) { MotionDiagnostic(t,10,L"maximized/minimized target intentionally skipped");if (g_tr.active && g_tr.target == t) TrailStop(); return; }
    int W = (int)(fr.right - fr.left), H = (int)(fr.bottom - fr.top);
    if (g_tr.active && g_tr.target == t) {
        if (W != g_tr.W || H != g_tr.H) { MotionDiagnostic(t,6,L"resize ends motion trail");TrailStop(); return; }   // resizing, not moving
        g_tr.lastMove = Now();
        return;
    }
    if (!hadPrev) return;
    if ((prev.right - prev.left) != (fr.right - fr.left) || (prev.bottom - prev.top) != (fr.bottom - fr.top)) return;
    int dx = (int)(fr.left - prev.left), dy = (int)(fr.top - prev.top);
    if (abs(dx) + abs(dy) < 2) return;
    TrailStart(t, fr);
}

static void TrailTick(double now) {
    if (!g_tr.active) return;
    RECT fr;
    if (!IsWindow(g_tr.target) || IsIconic(g_tr.target) || !GetFrame(g_tr.target, &fr)) { TrailStop(); return; }
    if (g_tr.hist.empty() || fr.left != g_tr.hist.back().x || fr.top != g_tr.hist.back().y) {
        TSample s = {(int)fr.left, (int)fr.top, now};
        g_tr.hist.push_back(s);
        g_tr.lastMove = now;
    }
    double len = (double)g_mb.lengthMs / 1000.0;
    while (g_tr.hist.size() > 2 && g_tr.hist[1].t < now - len * 1.3) g_tr.hist.erase(g_tr.hist.begin());
    float fade = 1.f;
    double idle = now - g_tr.lastMove;
    if (idle > 0.10) fade = 1.f - (float)((idle - 0.10) / 0.25);
    if (fade <= 0.f) { TrailStop(); return; }

    int G = g_mb.copies;
    int gx[12], gy[12], ga[12];
    long left = 2147483647L, top = 2147483647L, right = -2147483647L, bottom = -2147483647L;
    for (int g = 1; g <= G; g++) {
        double tt = now - len * (double)g / (double)G;
        const TSample* s = &g_tr.hist[0];
        for (size_t i = 0; i < g_tr.hist.size(); i++) {
            if (g_tr.hist[i].t <= tt) s = &g_tr.hist[i];
            else break;
        }
        float k = 1.f - (float)(g - 1) / (float)G;
        gx[g - 1] = s->x; gy[g - 1] = s->y;
        ga[g - 1] = (int)(255.f * g_mb.strength * fade * powf(k, 1.5f));
        left = std::min(left, (long)(s->x - g_tr.pad));
        top = std::min(top, (long)(s->y - g_tr.pad));
        right = std::max(right, (long)(s->x - g_tr.pad + g_tr.gw));
        bottom = std::max(bottom, (long)(s->y - g_tr.pad + g_tr.gh));
    }
    long vx = GetSystemMetrics(SM_XVIRTUALSCREEN), vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    long vr = vx + GetSystemMetrics(SM_CXVIRTUALSCREEN), vb = vy + GetSystemMetrics(SM_CYVIRTUALSCREEN);
    left = std::max(left, vx); top = std::max(top, vy); right = std::min(right, vr); bottom = std::min(bottom, vb);
    int bw = (int)(right - left), bh = (int)(bottom - top);
    if (bw <= 0 || bh <= 0) return;

    if (!g_tr.bdc || bw > g_tr.bcw || bh > g_tr.bch) {
        int nw = Imax(bw, g_tr.bcw) + 64, nh = Imax(bh, g_tr.bch) + 64;
        TrailFreeBuffer();
        HDC scr = GetDC(nullptr);
        void* bits = nullptr;
        HBITMAP bmp = MakeDib(scr, nw, nh, &bits);
        HDC dc = CreateCompatibleDC(scr);
        ReleaseDC(nullptr, scr);
        if (!bmp || !bits || !dc) {
            if (bmp) DeleteObject(bmp);
            if (dc) DeleteDC(dc);
            return;
        }
        g_tr.bold = SelectObject(dc, bmp);
        g_tr.bdc = dc;
        g_tr.bbmp = bmp;
        g_tr.bbits = (DWORD*)bits;
        g_tr.bcw = nw;
        g_tr.bch = nh;
    }
    for (int y = 0; y < bh; y++) memset(g_tr.bbits + (size_t)y * g_tr.bcw, 0, (size_t)bw * 4);
    for (int g = G; g >= 1; g--) {   // oldest (faintest) first
        if (ga[g - 1] <= 0) continue;
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, (BYTE)Imin(255, ga[g - 1]), AC_SRC_ALPHA};
        if(!AlphaBlend(g_tr.bdc, gx[g - 1] - g_tr.pad - (int)left, gy[g - 1] - g_tr.pad - (int)top, g_tr.gw, g_tr.gh,
                   g_tr.gdc, 0, 0, g_tr.gw, g_tr.gh, bf))MotionDiagnostic(g_tr.target,9,L"ghost alpha blending failed",GetLastError());
    }
    HDC screen = GetDC(nullptr);
    POINT dst = {left, top};
    SIZE sz = {bw, bh};
    POINT src = {0, 0};
    BLENDFUNCTION bf1 = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    ++g_motionCounters.uploads;
    if(!UpdateLayeredWindow(g_tr.wnd, screen, &dst, &sz, g_tr.bdc, &src, 0, &bf1, ULW_ALPHA))
        MotionDiagnostic(g_tr.target,7,L"trail presentation failed",GetLastError());
    ReleaseDC(nullptr, screen);
    SetWindowPos(g_tr.wnd, g_tr.target, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

// ====================================================================== overlay thread: window events and ticking
static HANDLE g_ovThread = nullptr, g_ovStop = nullptr, g_ovReload = nullptr;

static HWND g_dragTarget=nullptr;
static UINT_PTR g_followTimer=0;
static double g_dragLastEvent=0;
static bool g_followBusy=false;
static void StopFollowTimer(){
    if(g_followTimer)KillTimer(nullptr,g_followTimer);
    g_followTimer=0;g_dragTarget=nullptr;
}
static void FollowCachedBorder(HWND hwnd){
    if(!PerformanceOptions::follow.load()||g_followBusy||RenderShutdown::requested.load())return;
    auto it=g_borders.find(hwnd);if(it==g_borders.end())return;
    Border& b=*it->second;
    if(!b.shown)return;
    g_followBusy=true;
    if(!IsWindowVisible(hwnd)||IsIconic(hwnd)||Cloaked(hwnd)||(g_bc.hideMax&&IsZoomed(hwnd))){
        HideBorder(b);b.dirty=true;g_dirty=true;
    }else{
        RECT fr{};
        if(GetFrame(hwnd,&fr)){
            bool moved=fr.left!=b.frame.left||fr.top!=b.frame.top;
            bool resized=fr.right-fr.left!=b.W||fr.bottom-fr.top!=b.H;
            // Resize geometry is rebuilt by the regular render loop, never in a hook.
            if(moved&&!resized){b.frame=fr;PlaceBorder(b);
                if(g_experimental.enabled)b.interaction.activeUntil=Now()+.8;}
            if(resized){b.dirty=true;g_dirty=true;}
            else if(!BehindNear(b.target,b.hwnd))StackBorder(b);
        }
    }
    g_followBusy=false;
}
static void CALLBACK FollowTimer(HWND,UINT,UINT_PTR,DWORD){
    if(!PerformanceOptions::follow.load()||!IsWindow(g_dragTarget)||Now()-g_dragLastEvent>10){StopFollowTimer();return;}
    FollowCachedBorder(g_dragTarget);
}
static void OnLocation(HWND hwnd) {
    if(g_mb.on)++g_motionCounters.events;
    if (GetAncestor(hwnd, GA_ROOT) != hwnd) return;   // only top-level windows
    RECT fr;
    if (!GetFrame(hwnd, &fr)) return;
    auto lf = g_lastFrame.find(hwnd);
    bool hadPrev = lf != g_lastFrame.end();
    RECT prev = hadPrev ? lf->second : RECT{0, 0, 0, 0};
    g_lastFrame[hwnd] = fr;

    auto it = g_borders.find(hwnd);
    if (it != g_borders.end()) { it->second->dirty = true; g_dirty = true; }
    else if (g_bc.enable && EligibleCached(hwnd)) AddBorder(hwnd);
    if (PerformanceOptions::follow.load()){
        if(hwnd==g_dragTarget)g_dragLastEvent=Now();
        FollowCachedBorder(hwnd);
    }
    if (EligibleCached(hwnd)) TrailConsider(hwnd, prev, hadPrev, fr);
}

static void RefreshActive() {
    HWND fg = GetForegroundWindow();
    g_fgRoot = fg ? GetAncestor(fg, GA_ROOT) : nullptr;
    for (auto& kv : g_borders) { kv.second->dirty = true; }
    g_dirty = true;
}

static void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD ev, HWND hwnd, LONG idObj, LONG idChild, DWORD, DWORD) {
    if (!hwnd || idObj != OBJID_WINDOW || idChild != CHILDID_SELF || g_own.count(hwnd)) return;
    switch (ev) {
        case EVENT_SYSTEM_MOVESIZESTART:
            if(PerformanceOptions::follow.load()&&g_borders.count(hwnd)){
                StopFollowTimer();g_dragTarget=hwnd;g_dragLastEvent=Now();
                g_followTimer=SetTimer(nullptr,0,16,FollowTimer);FollowCachedBorder(hwnd);
            }
            break;
        case EVENT_SYSTEM_MOVESIZEEND:
            if(PerformanceOptions::follow.load()){FollowCachedBorder(hwnd);OnLocation(hwnd);}
            StopFollowTimer();break;
        case EVENT_OBJECT_LOCATIONCHANGE:
            OnLocation(hwnd);
            break;
        case EVENT_SYSTEM_FOREGROUND:
            if(PerformanceOptions::follow.load())FollowCachedBorder(hwnd);
            g_fgRoot = GetAncestor(hwnd, GA_ROOT);
            for (auto& kv : g_borders) kv.second->dirty = true;
            g_dirty = true;
            if (g_bc.enable && EligibleCached(g_fgRoot)) AddBorder(g_fgRoot);
            break;
        case EVENT_OBJECT_SHOW:
        case EVENT_OBJECT_UNCLOAKED:
        case EVENT_SYSTEM_MINIMIZEEND:
            g_elig.erase(hwnd);
            if (GetAncestor(hwnd, GA_ROOT) == hwnd) {
                auto it = g_borders.find(hwnd);
                if (it != g_borders.end()) it->second->dirty = true;
                else AddBorder(hwnd);
                g_dirty = true;
            }
            break;
        case EVENT_OBJECT_HIDE:
        case EVENT_OBJECT_CLOAKED:
        case EVENT_SYSTEM_MINIMIZESTART: {
            auto it = g_borders.find(hwnd);
            if (it != g_borders.end()) { if(PerformanceOptions::follow.load())HideBorder(*it->second);it->second->dirty = true; g_dirty = true; }
            if(hwnd==g_dragTarget)StopFollowTimer();
            break;
        }
        case EVENT_OBJECT_DESTROY:
            if(hwnd==g_dragTarget)StopFollowTimer();
            RemoveBorder(hwnd);
            g_elig.erase(hwnd);
            g_lastFrame.erase(hwnd);
            if (g_tr.active && g_tr.target == hwnd) TrailStop();
            break;
    }
}

static BOOL CALLBACK EnumInitProc(HWND h, LPARAM) {
    if(RenderShutdown::requested.load())return FALSE;
    if (IsWindowVisible(h) && !IsIconic(h)) {
        RECT fr;
        if (GetFrame(h, &fr)) g_lastFrame[h] = fr;
        AddBorder(h);
    }
    return TRUE;
}

static void RefreshAmbient(double now) {
    if (now < g_nextAmb) return;
    g_nextAmb = now + g_bc.ambientSeconds;
    WallpaperSrc wp;
    bool needWall=false;
    auto check=[&](const Style& st) { for (const Look* l : {&st.act,&st.inact})
        if(l->prog.usesAmb&&l->effect==L"wallpaper")needWall=true;
    };
    check(g_defStyle); for (const auto& kv:g_rules) check(kv.second);
    bool wallOK=needWall && GrabWallpaper(wp);
    for(size_t i=0;i<g_mons.size();++i) {
        if(wallOK) MakeAmbientTable(wp,g_mons[i].rc,g_wall[i]);
    }
    for (auto& kv : g_borders) {
        Border* b = kv.second;
        if (b->shown && (b->st->act.prog.usesAmb || b->st->inact.prog.usesAmb)) {
            if ((b->st->act.prog.usesAmb && b->st->act.effect!=L"wallpaper") ||
                (b->st->inact.prog.usesAmb && b->st->inact.effect!=L"wallpaper")) ScreenTable(b->frame,b->ambient,-0.12f);
            b->dirtyPix = true;
        }
    }
}

static int CinemaRate(const Border& b, double now) {
    bool windowActivity = (int)(GetTickCount() - g_adaptiveActiveUntil) < 0;
    bool active = b.cinema.Active(now, windowActivity);
    int refresh = g_bc.sampleMode == L"auto" || g_bc.sampleMode == L"adaptive" ||
        g_bc.sampleMode == L"adaptiveLite" ? MonitorRefreshHz(GetForegroundWindow()) : 60;
    return CinemaSampling::Rate(g_bc.sampleMode, g_bc.sampleRate, refresh, active);
}
static double CinemaDelay(double now) {
    double delay = 3600.0;
    if (!g_bc.enable || g_experimental.enabled) return delay;
    for (const auto& kv : g_borders) {
        const Border& b = *kv.second;
        if (b.shown && b.st && (b.active?b.st->act:b.st->inact).prog.usesCin)
            delay = std::min(delay, b.cinema.Deadline(now, CinemaRate(b, now)) - now);
    }
    return delay;
}

static double SmoothingDelay(double now) {
    double delay=3600;
    if(EffectSmoothing::enabled.load())for(const auto& kv:g_borders){
        const Border& b=*kv.second;
        if(b.shown&&b.smooth.pending)delay=std::min(delay,b.lastPresent+1.0/60.0-now);
    }
    return delay;
}

// Returns true when something is animating (the thread then wakes up more often).
static bool TickAll(double now, double* lastZ) {
    if (now >= g_nextMon) {
        g_nextMon = now + (PerformanceOptions::enabled.load()?PerformanceOptions::maintenance.load():2); RefreshMons(); LoadEngineExclusions(); g_elig.clear();
        for(auto& kv:g_borders) kv.second->dirty=true;
        EnumWindows(EnumInitProc,0);
        if(g_tr.active && !Eligible(g_tr.target)) TrailStop();
    }
    if (!g_experimental.enabled && g_bc.enable && g_anyAmb) RefreshAmbient(now);

    bool anim = false;
    bool zCheck = (now - *lastZ) >= 0.12;
    if (zCheck) *lastZ = now;
    for (auto& kv : g_borders) {
        if(RenderShutdown::requested.load())return false;
        Border& b = *kv.second;
        if (b.dirty) { b.dirty = false; UpdateBorder(b); }
        if (!b.shown) continue;
        const Look* L = b.active ? &b.st->act : &b.st->inact;
        if(PerformanceOptions::timing.load()&&(L->prog.usesCin||L->prog.usesAmb)&&now-b.lastSampleLog>=10){
            Wh_Log(L"App sample region: target=0x%llX effect=%s experimental=%d outside=%d rect=(%ld,%ld,%ld,%ld) cinema-valid=%d ambient-valid=%d",
                (unsigned long long)(UINT_PTR)b.target,L->effect.c_str(),g_experimental.enabled,g_bc.cinemaOutside,
                b.frame.left,b.frame.top,b.frame.right,b.frame.bottom,b.cin.valid,b.ambient.valid);
            b.lastSampleLog=now;
        }
        if(!g_experimental.enabled && L->prog.usesAmb && L->effect!=L"wallpaper" && !b.ambient.valid) {
            if(ScreenTable(b.frame,b.ambient,-0.12f)) b.dirtyPix=true;
        }
        int cinemaRate = L->prog.usesCin ? CinemaRate(b, now) : 10;
        if(!g_experimental.enabled&&L->prog.usesCin){
            const float cinemaDepth=g_bc.cinemaOutside?-.12f:.12f;
            bool colorsChanged=false;
            if(b.cinema.Due(now,cinemaRate)){
                if(!g_cinemaCapture.Request((UINT_PTR)b.target,b.frame,cinemaDepth))ScreenTable(b.frame,b.cin,cinemaDepth,&colorsChanged);
                b.cinema.Sampled(now,cinemaRate,false);
            }
            colorsChanged=ReadCinemaSample((UINT_PTR)b.target,b.frame,cinemaDepth,b.cin)||colorsChanged;
            if(colorsChanged){b.dirtyPix=true;b.cinema.activeUntil=now+.5;
                if(g_bc.fpsMode==L"adaptive"||g_bc.fpsMode==L"adaptiveLite")g_adaptiveActiveUntil=GetTickCount()+350;}
            anim=true;
        }
        bool experimental=g_experimental.enabled&&(b.active||b.st->inactKind==1);
        if(experimental)b.interaction.Update(now,b.frame.left,b.frame.top,b.W,b.H);
        bool animate=ExperimentalBorders::ShouldAnimate(g_experimental,b.interaction,now,
            b.active,b.st->inactKind,g_bc.fpsMode,L->animated||(g_extra.glass&&g_extra.glassApp));
        if(!b.focus.from.empty()){b.dirtyPix=true;anim=true;}
        if(g_extra.glass&&g_extra.glassApp&&(b.active||b.st->inactKind==1))animate=true;
        if (animate) {
            if (b.active) {
                int activeFps = g_bc.fps;
                if (g_bc.fpsMode == L"auto") activeFps = MonitorRefreshHz(b.target);
                else if (g_bc.fpsMode == L"adaptive" || g_bc.fpsMode == L"adaptiveLite")
                    activeFps = (int)(GetTickCount() - g_adaptiveActiveUntil) < 0 ? MonitorRefreshHz(b.target) : 30;
                activeFps=PerformanceOptions::Cap(activeFps);
                if(g_experimental.enabled)activeFps=std::min(activeFps,g_experimental.fps);
                if (now - b.lastRender >= 1.0 / (double)Imax(1, activeFps)) b.dirtyPix = true;
                anim = true;
            } else if (g_bc.fpsMode == L"adaptive" && b.st->inactKind == 1) {
                if (now - b.lastRender >= 1.0 / std::max(1,PerformanceOptions::Cap(g_experimental.enabled?std::min(g_experimental.fps,60):60))) b.dirtyPix = true;
                anim = true;
            }
        }
        if (b.dirtyPix) RenderBorder(b, now);
        else if (EffectSmoothing::enabled.load()&&b.smooth.pending&&now-b.lastPresent>=1.0/60.0) {
            b.smooth.BlendBand(b.bits,b.band,now);PresentBorderPixels(b);b.lastPresent=now;
        }
        else if (zCheck && !BehindNear(b.target, b.hwnd))StackBorder(b);
    }
    TrailTick(now);
    if(g_mb.on&&PerformanceOptions::timing.load()&&now-g_motionCounters.last>=10){
        Wh_Log(L"Motion blur counters: location events=%u start attempts=%u upload attempts=%u failures=%u",g_motionCounters.events,g_motionCounters.starts,g_motionCounters.uploads,g_motionCounters.failures);
        g_motionCounters={0,0,0,0,now};
    }
    if (g_tr.active) anim = true;
    return anim;
}

static DWORD WINAPI OverlayThread(LPVOID) {
    PerformanceOptions::WorkerPriority();
    // Request 1 ms timer granularity so high-FPS modes are not needlessly limited by coarse waits.
    timeBeginPeriod(1);
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    WNDCLASSW wc = {};
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kBorderClass;
    RegisterClassW(&wc);
    wc.lpszClassName = kTrailClass;
    RegisterClassW(&wc);

    LoadStyles();
    RefreshMons();
    RefreshActive();
    HWINEVENTHOOK h1 = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_MINIMIZEEND, nullptr, WinEventProc, 0, 0,
                                       WINEVENT_OUTOFCONTEXT);
    HWINEVENTHOOK h2 = SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_UNCLOAKED, nullptr, WinEventProc, 0, 0,
                                       WINEVENT_OUTOFCONTEXT);
    if(!h1||!h2)Wh_Log(L"App window-event hook unavailable: system=%d object=%d error=%lu",h1!=nullptr,h2!=nullptr,GetLastError());
    EnumWindows(EnumInitProc, 0);

    ConfigureCinemaCapture();
    HANDLE evts[3]={g_ovStop,g_ovReload,nullptr};
    double lastTick = 0, lastZ = 0;
    bool anim = false, messagesPending = false;
    for (;;) {
        ObserveAdaptiveActivity();
        int effectiveFps = EffectiveAnimationFPS();
        if(g_experimental.enabled)effectiveFps=std::min(effectiveFps,g_experimental.fps);
        double tickPeriod=g_tr.active?1.0/120.0:(anim?1.0/Imax(1,effectiveFps):.12);
        if(g_experimental.enabled&&g_bc.enable)tickPeriod=std::min(tickPeriod,.033);
        DWORD waitMs=CinemaSampling::WaitMs(std::max(0.,lastTick+tickPeriod-Now()));
        if(g_experimental.enabled&&g_bc.enable)waitMs=std::min<DWORD>(waitMs,33);
        waitMs = std::min(waitMs, CinemaSampling::WaitMs(CinemaDelay(Now())));
        waitMs = std::min(waitMs, CinemaSampling::WaitMs(SmoothingDelay(Now())));
        // PeekMessage marks input as seen. Explicitly avoid sleeping over the
        // remainder of a budgeted batch, while still checking stop/reload events.
        if(messagesPending)waitMs=0;
        evts[2]=g_cinemaCapture.ReadyEvent();
        DWORD r = MsgWaitForMultipleObjects(evts[2]?3:2, evts, FALSE, waitMs, QS_ALLINPUT);
        if (r == WAIT_FAILED) { Wh_Log(L"Overlay wait failed; stopping worker"); break; }
        if (r == WAIT_OBJECT_0) break;
        if (r == WAIT_OBJECT_0 + 1) {   // settings changed
            StopFollowTimer();g_cinemaCapture.Stop();GpuFormula::Reset();
            TrailStop();
            DestroyAllBorders();
            g_elig.clear();
            g_amb.assign(g_amb.size(), ColorTable());
            g_nextAmb = 0;
            PerformanceOptions::WorkerPriority();
            LoadStyles();
            CaptureVisibility::Apply(g_tr.wnd); // The trail window survives settings reloads.
            if (!g_mb.on) TrailStop();
            RefreshActive();
            EnumWindows(EnumInitProc, 0);
        }
        if(evts[2]&&r==WAIT_OBJECT_0+2)g_dirty=true;
        MSG msg;
        // A continuous stream of location events must not starve rendering,
        // timers, reload handling or stop handling. Leave the remainder queued.
        double messageStart=Now();unsigned messageCount=0;
        while(messageCount<64&&Now()-messageStart<.004&&PeekMessageW(&msg,nullptr,0,0,PM_REMOVE)){
            TranslateMessage(&msg);DispatchMessageW(&msg);++messageCount;
        }
        messagesPending=PeekMessageW(&msg,nullptr,0,0,PM_NOREMOVE)!=FALSE;
        double now = Now();
        effectiveFps = EffectiveAnimationFPS();
        if(g_experimental.enabled)effectiveFps=std::min(effectiveFps,g_experimental.fps);
        double frame = g_tr.active ? 1.0/120.0 : (anim ? 1.0 / (double)Imax(1, effectiveFps) : 0.12);
        if(g_experimental.enabled&&g_bc.enable)frame=std::min(frame,.033);
        if (g_dirty || now - lastTick >= frame || CinemaDelay(now) <= 0 || SmoothingDelay(now) <= 0) {
            g_dirty = false;
            lastTick = now;
            anim = TickAll(now, &lastZ);
        }
    }

    StopFollowTimer();g_cinemaCapture.Stop();GpuFormula::Reset();
    if (h1) UnhookWinEvent(h1);
    if (h2) UnhookWinEvent(h2);
    TrailStop();
    if (g_tr.wnd) { g_own.erase(g_tr.wnd); DestroyWindow(g_tr.wnd); g_tr.wnd = nullptr; }
    DestroyAllBorders();
    UnregisterClassW(kBorderClass, GetModuleHandleW(nullptr));
    UnregisterClassW(kTrailClass, GetModuleHandleW(nullptr));
    GdipShutdownAll();
    g_screenSample.Reset();
    timeEndPeriod(1);
    return 0;
}

// ====================================================================== hook and lifecycle
using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t CreateWindowExW_Original;

// File Explorer lists attach on creation. Desktop lists attach through the
// watcher after discovery has stabilized, outside shell window construction.
static HWND WINAPI CreateWindowExW_Hook(DWORD ex, LPCWSTR cls, LPCWSTR name, DWORD style,
                                        int x, int y, int w, int h, HWND parent, HMENU menu,
                                        HINSTANCE inst, LPVOID param) {
    HWND hwnd = CreateWindowExW_Original(ex, cls, name, style, x, y, w, h, parent, menu, inst, param);
    if (hwnd) {
        wchar_t cn[32] = {};
        if (GetClassNameW(hwnd, cn, 32) && !wcscmp(cn, L"SysListView32") &&
            Classify(hwnd)!=K_DESKTOP) AttachList(hwnd);
    }
    return hwnd;
}

static HANDLE g_attachStop=nullptr, g_attachThread=nullptr;
namespace ExplorerIconFx {void Scan();}
static DWORD WINAPI DesktopWatch(LPVOID) {
    ULONGLONG lastMissing=0;
    HWND candidate=nullptr;
    ULONGLONG candidateSince=0;
    while(WaitForSingleObject(g_attachStop,500)==WAIT_TIMEOUT) {
        ExplorerIconFx::Scan();
        HWND list=FindDesktopList();
        ULONGLONG now=GetTickCount64();
        if(list!=candidate){candidate=list;candidateSince=now;}
        if(list&&now-candidateSince>=2000) { DWORD pid=0; GetWindowThreadProcessId(list,&pid);
            if(pid==GetCurrentProcessId()) AttachList(list);
        }else if(!list&&GetTickCount64()-lastMissing>=10000){
            lastMissing=GetTickCount64();
            Wh_Log(L"Desktop startup: discovery waiting for a desktop list in process %lu",GetCurrentProcessId());
        }
    }
    return 0;
}
// Taskbar integration follows the GPL-3.0 taskbar-labels/taskbar-icon-size
// symbol and FrameworkElement bridge technique by m417z. Combined source GPL-3.0.
namespace TaskbarFx {
using namespace winrt::Windows::UI::Xaml;
using winrt::Windows::Storage::Streams::IRandomAccessStream;
static std::shared_ptr<const IconCfg> g_config;
static std::atomic<bool> g_stopping=false;
static std::atomic<bool> g_enabled=true;
static bool g_initialized=false;
static std::atomic<bool> g_hooked=false;
static std::atomic<unsigned> g_generation=1;
static void LoadSettings(){IconCfg ic;ic.gpuCategory=GpuStages::TaskbarIcons;
    g_enabled=Wh_GetIntSetting(L"TaskbarIcons.Enabled")!=0;
    ic.gray = Wh_GetIntSetting(L"TaskbarIcons.Color.IconGrayscale") != 0;
    ic.colorize = Wh_GetIntSetting(L"TaskbarIcons.Color.IconColorize") != 0;
    ic.transp = Wh_GetIntSetting(L"TaskbarIcons.Color.IconTransparency") != 0;
    {
        PCWSTR c = Wh_GetStringSetting(L"TaskbarIcons.Color.IconColorizeColor");
        int r, g, b;
        ParseRgb(c, L"0078D7", &r, &g, &b);
        Wh_FreeStringSetting(c);
        ic.hue = HueOfRgb(r / 255.f, g / 255.f, b / 255.f);
    }
    ic.sat = (float)ClampI(Wh_GetIntSetting(L"TaskbarIcons.Color.IconColorizeSaturation"), 0, 100) / 100.f;
    ic.contrast = (float)ClampI(Wh_GetIntSetting(L"TaskbarIcons.Color.IconColorizeContrast"), 0, 200) / 100.f;
    ic.opacity = 1.f - (float)ClampI(Wh_GetIntSetting(L"TaskbarIcons.Color.IconTransparencyAmount"), 0, 100) / 100.f;
    ic.shadow = Wh_GetIntSetting(L"TaskbarIcons.Shadow.IconShadow") != 0;
    ic.glow = Wh_GetIntSetting(L"TaskbarIcons.Glow.IconGlow") != 0;
    {
        PCWSTR c = Wh_GetStringSetting(L"TaskbarIcons.Shadow.IconShadowColor");
        ParseRgb(c, L"000000", &ic.shadowRgb[0], &ic.shadowRgb[1], &ic.shadowRgb[2]); Wh_FreeStringSetting(c);
        c = Wh_GetStringSetting(L"TaskbarIcons.Glow.IconGlowColor");
        ParseRgb(c, L"00BFFF", &ic.glowRgb[0], &ic.glowRgb[1], &ic.glowRgb[2]); Wh_FreeStringSetting(c);
    }
    ic.shadowOpacity = ClampI(Wh_GetIntSetting(L"TaskbarIcons.Shadow.IconShadowOpacity"), 0, 100);
    ic.shadowBlur = ClampI(Wh_GetIntSetting(L"TaskbarIcons.Shadow.IconShadowBlur"), 0, 16);
    ic.shadowX = ClampI(Wh_GetIntSetting(L"TaskbarIcons.Shadow.IconShadowOffsetX"), -16, 16);
    ic.shadowY = ClampI(Wh_GetIntSetting(L"TaskbarIcons.Shadow.IconShadowOffsetY"), -16, 16);
    ic.glowIntensity = ClampI(Wh_GetIntSetting(L"TaskbarIcons.Glow.IconGlowIntensity"), 0, 100);
    ic.glowBlur = ClampI(Wh_GetIntSetting(L"TaskbarIcons.Glow.IconGlowBlur"), 0, 16);
    ic.glowOutline = ClampI(Wh_GetIntSetting(L"TaskbarIcons.Glow.IconGlowOutline"), 0, 8);
    ic.effect=Lower(GetStr(L"TaskbarIcons.Animation.IconEffect"));
    if(ic.effect.empty()) ic.effect=L"none";
    ic.speed=ClampI(Wh_GetIntSetting(L"TaskbarIcons.Animation.IconEffectSpeed"),1,100)/20.f;
    ic.power=ClampI(Wh_GetIntSetting(L"TaskbarIcons.Animation.IconEffectIntensity"),0,100)/100.f;
    ic.count=ClampI(Wh_GetIntSetting(L"TaskbarIcons.Animation.IconEffectCount"),1,16);
    ic.fps=PerformanceOptions::Cap(ClampI(Wh_GetIntSetting(L"TaskbarIcons.Animation.IconEffectFPS"),1,30));
    { int r,g,b; auto c=GetStr(L"TaskbarIcons.Color.IconColorizeColor"); ParseRgb(c.c_str(),L"0078D7",&r,&g,&b);
      ic.colors[0]=r/255.f;ic.colors[1]=g/255.f;ic.colors[2]=b/255.f;
      c=GetStr(L"TaskbarIcons.Animation.IconEffectColor2"); ParseRgb(c.c_str(),L"FF00FF",&r,&g,&b);
      ic.color2[0]=r/255.f;ic.color2[1]=g/255.f;ic.color2[2]=b/255.f; }
    if(ic.effect!=L"none") CompileEffectCode(ic.effect,GetStr(L"TaskbarIcons.Animation.IconCustomEffect"),&ic.prog);
    CacheIconGpuPlan(ic);


    std::atomic_store(&g_config,std::make_shared<const IconCfg>(std::move(ic)));
    ++g_generation;
}

// Decode a clone so the shell's stream position and lifetime are untouched.
struct Pixels { int w=0,h=0; std::vector<BYTE> bgra; };
static bool HasVisiblePixels(const Pixels& pixels){
    for(size_t i=3;i<pixels.bgra.size();i+=4)if(pixels.bgra[i])return true;
    return false;
}
static bool DecodeIcon(const IRandomAccessStream& input,Pixels& out){
    if(!input) return false;
    auto clone=input.CloneStream(); clone.Seek(0);
    winrt::com_ptr<::IStream> stream;
    winrt::check_hresult(CreateStreamOverRandomAccessStream(
        reinterpret_cast<::IUnknown*>(winrt::get_abi(clone)),IID_PPV_ARGS(stream.put())));
    winrt::com_ptr<IWICImagingFactory> factory;
    winrt::check_hresult(CoCreateInstance(CLSID_WICImagingFactory,nullptr,CLSCTX_INPROC_SERVER,
                                          IID_PPV_ARGS(factory.put())));
    winrt::com_ptr<IWICBitmapDecoder> decoder;
    winrt::check_hresult(factory->CreateDecoderFromStream(stream.get(),nullptr,WICDecodeMetadataCacheOnLoad,decoder.put()));
    winrt::com_ptr<IWICBitmapFrameDecode> frame; winrt::check_hresult(decoder->GetFrame(0,frame.put()));
    UINT w=0,h=0;winrt::check_hresult(frame->GetSize(&w,&h));
    if(!w||!h||w>512||h>512) return false;
    winrt::com_ptr<IWICFormatConverter> conv;winrt::check_hresult(factory->CreateFormatConverter(conv.put()));
    winrt::check_hresult(conv->Initialize(frame.get(),GUID_WICPixelFormat32bppBGRA,WICBitmapDitherTypeNone,
                                         nullptr,0,WICBitmapPaletteTypeCustom));
    out.w=(int)w;out.h=(int)h;out.bgra.resize((size_t)w*h*4);
    winrt::check_hresult(conv->CopyPixels(nullptr,w*4,(UINT)out.bgra.size(),out.bgra.data()));
    return HasVisiblePixels(out);
}

// Bitmap composition only: no global icon lists or HWND icon handles are changed.
struct FrameWorkspace {
    Pixels output;
    std::vector<BYTE> foreground, background;
    const IconCfg* config = nullptr; // identity only; never dereferenced
    unsigned generation = 0;
    bool valid = false;
};
static const Pixels& MakeFrame(const Pixels& raw,const IconCfg& cfg,FrameWorkspace& work,
                               unsigned generation,const IconSpace* space=nullptr){
    Pixels& out = work.output;
    if(raw.bgra.empty()){out.w=out.h=0;out.bgra.clear();return out;}
    const bool decorated=cfg.glow||cfg.shadow;
    auto& fg=decorated?work.foreground:out.bgra;
    fg.resize(raw.bgra.size());
    std::copy(raw.bgra.begin(),raw.bgra.end(),fg.begin());
    int glowPad=cfg.glow?cfg.glowBlur*2+cfg.glowOutline+1:0;
    int shadowPad=cfg.shadow?cfg.shadowBlur*2+std::max(abs(cfg.shadowX),abs(cfg.shadowY))+1:0;
    int pad=std::max(glowPad,shadowPad);
    out.w=raw.w+pad*2;out.h=raw.h+pad*2;
    const int W=out.w,H=out.h;
    IconSpace mapped;
    if(space){mapped=*space;mapped.scaleX*=raw.w/(float)W;mapped.scaleY*=raw.h/(float)H;
        mapped.x+=pad*mapped.scaleX;mapped.y+=pad*mapped.scaleY;}
    ProcessBuffer(fg.data(),raw.w*raw.h,cfg,raw.w,raw.h,space?&mapped:nullptr);
    if(!cfg.glow && !cfg.shadow){
        // With no decoration, compositing over transparent black is just a copy.
        // Already processed in the output buffer; no second full-icon copy.
        if(!work.background.empty())std::vector<BYTE>().swap(work.background);
        work.generation=generation;work.config=&cfg;work.valid=true;
        return out;
    }
    auto over=[](BYTE* d,int r,int g,int b,int a){
        int inv=255-a;
        d[0]=(BYTE)std::min(255,b+d[0]*inv/255);
        d[1]=(BYTE)std::min(255,g+d[1]*inv/255);
        d[2]=(BYTE)std::min(255,r+d[2]*inv/255);
        d[3]=(BYTE)std::min(255,a+d[3]*inv/255);
    };
    if(!work.valid || work.generation!=generation || work.config!=&cfg){
        work.valid=false;
        work.background.assign((size_t)W*H*4,0);
        if(cfg.glow||cfg.shadow){
            std::vector<BYTE> gm((size_t)W*H,0),sm(gm.size(),0);
            for(int y=0;y<raw.h;++y)for(int x=0;x<raw.w;++x){
                size_t dest=(size_t)(y+pad)*W+x+pad;
                // Shadow/glow follow source opacity but do not inherit animation's hue.
                BYTE a=raw.bgra[((size_t)y*raw.w+x)*4+3];
                if(cfg.transp)a=(BYTE)(a*cfg.opacity);
                gm[dest]=sm[dest]=a;
            }
            if(cfg.glow){DilateAlphaPlane(gm,W,H,cfg.glowOutline);BlurAlphaPlane(gm,W,H,cfg.glowBlur);}
            if(cfg.shadow)BlurAlphaPlane(sm,W,H,cfg.shadowBlur);
            for(int y=0;y<H;++y)for(int x=0;x<W;++x){
                BYTE* d=&work.background[((size_t)y*W+x)*4];
                if(cfg.shadow){int sx=x-cfg.shadowX,sy=y-cfg.shadowY;
                    if(sx>=0&&sy>=0&&sx<W&&sy<H){int a=sm[(size_t)sy*W+sx]*cfg.shadowOpacity/100;
                        over(d,cfg.shadowRgb[0]*a/255,cfg.shadowRgb[1]*a/255,cfg.shadowRgb[2]*a/255,a);}}
                if(cfg.glow){int a=gm[(size_t)y*W+x]*cfg.glowIntensity/100;
                    over(d,cfg.glowRgb[0]*a/255,cfg.glowRgb[1]*a/255,cfg.glowRgb[2]*a/255,a);}
            }
        }
        work.generation=generation;
        work.config=&cfg;
        work.valid=true;
    }
    out.bgra.resize(work.background.size());
    std::copy(work.background.begin(),work.background.end(),out.bgra.begin());
    for(int y=0;y<raw.h;++y)for(int x=0;x<raw.w;++x){
        BYTE* d=&out.bgra[((size_t)(y+pad)*W+x+pad)*4];
        const BYTE* p=&fg[((size_t)y*raw.w+x)*4];over(d,p[2],p[1],p[0],p[3]);
    }
    return out;
}

static Controls::Image FindAppImage(DependencyObject const& root,int depth=0){
    if(!root||depth>12)return nullptr;
    if(auto image=root.try_as<Controls::Image>()){
        if(image.Name()==L"Icon")return image;
    }
    int n=Media::VisualTreeHelper::GetChildrenCount(root);
    for(int i=0;i<n;++i){auto found=FindAppImage(Media::VisualTreeHelper::GetChild(root,i),depth+1);if(found)return found;}
    return nullptr;
}
static FrameworkElement ButtonElement(void* impl){
    FrameworkElement element=nullptr;
    // Public symbol implementation layout used by upstream taskbar-labels.
    auto unknown=reinterpret_cast<::IUnknown*>(static_cast<void**>(impl)+3);
    winrt::check_hresult(unknown->QueryInterface(winrt::guid_of<FrameworkElement>(),winrt::put_abi(element)));
    return element;
}
struct Entry {
    winrt::weak_ref<FrameworkElement> button;
    winrt::weak_ref<Controls::Image> image;
    Media::ImageSource original=nullptr;
    Media::Imaging::WriteableBitmap drawn=nullptr;
    Pixels raw;
    FrameWorkspace frame;
    EffectSmoothing::Frame smooth;
    IRandomAccessStream sourceStream=nullptr;
    double startupAt=0,lastProbe=-1;
    unsigned recoveryPass=0;
    unsigned generation=0;
    float lastX=-1,lastY=-1,lastW=-1,lastH=-1,lastScaleX=-1,lastScaleY=-1;
    double retryAfter=0;
    unsigned lastError=0;
    bool loggedApplied=false;
};
struct Context { HWND window=nullptr; std::vector<Entry> entries; };
static std::mutex g_contextMutex;
static std::map<DWORD,HWND> g_contexts;
static HINSTANCE g_instance=nullptr;
static constexpr wchar_t kWindowClass[]=L"Windhawk_DodiTaskbarIconEffects_UI_01";
static constexpr UINT kReload=WM_APP+0x661,kStop=WM_APP+0x662;
static constexpr UINT_PTR kTimer=0x71C0;

static void Restore(Entry& e){
    if(auto image=e.image.get()){
        // Preserve an unrelated mod's newer source if it took ownership.
        if(e.drawn && image.Source()==e.drawn)image.Source(e.original);
    }
    e.drawn=nullptr;e.smooth.Reset();
}
static bool DrawEntry(Entry& e,const IconCfg& cfg,bool animated,const IconSpace& fallback){
    auto button=e.button.get();if(!button)return false;
    const double now=Now();
    static constexpr double recoveryDelays[]={1,3,7,15};
    bool reapply=e.recoveryPass<ARRAYSIZE(recoveryDelays)&&
                 now-e.startupAt>=recoveryDelays[e.recoveryPass];
    if(reapply){++e.recoveryPass;e.generation=0;}
    auto image=e.image.get();
    // A live weak reference can still point at an old, detached template.
    if(!image||now-e.lastProbe>=0.5||reapply){
        auto latest=FindAppImage(button);e.lastProbe=now;
        if(!latest){Restore(e);e.image={};return true;}
        if(latest!=image){
            Restore(e);image=latest;e.image=winrt::make_weak(image);
            e.original=image.Source();e.generation=0;
        }
    }
    auto current=image.Source();
    if(current!=e.drawn){
        // Capture null too: Windows may clear a source during template updates.
        e.original=current;e.generation=0;
    }
    if(!g_enabled||!cfg.HasEffects()){Restore(e);e.frame={};e.generation=g_generation.load();return true;}
    if(!image.IsLoaded()||image.ActualWidth()<=0||image.ActualHeight()<=0||!current)return true;
    // A transparent placeholder must never replace a native icon. Retry the
    // original stream, without reading back any of our processed bitmaps.
    if(e.raw.bgra.empty()||reapply){
        Pixels decoded;
        if(!DecodeIcon(e.sourceStream,decoded)){
            Restore(e);e.raw={};e.retryAfter=now+0.5;return true;
        }
        e.raw=std::move(decoded);e.frame.valid=false;e.smooth.Reset();e.generation=0;
    }
    unsigned gen=g_generation.load();
    // Shared coordinates are optional decoration, never a prerequisite for
    // grayscale/tint/opacity. XamlRoot.Content need not be a FrameworkElement.
    IconSpace space=fallback;
    if(cfg.effect!=L"none"){
        try{
            auto root=image.XamlRoot();
            if(root){
                auto size=root.Size();
                if(size.Width>0&&size.Height>0){
                    auto transform=image.TransformToVisual(nullptr);
                    auto p0=transform.TransformPoint({0,0});
                    auto p1=transform.TransformPoint({(float)image.ActualWidth(),(float)image.ActualHeight()});
                    space.x=p0.X;space.y=p0.Y;space.w=size.Width;space.h=size.Height;
                    space.scaleX=(p1.X-p0.X)/std::max(1,e.raw.w);
                    space.scaleY=(p1.Y-p0.Y)/std::max(1,e.raw.h);
                }
            }
        }catch(winrt::hresult_error const&){
            // Layout may not have a root yet. Keep rendering and retry next tick.
        }
    }
    if(e.generation==gen&&e.frame.config==&cfg&&!animated&&image.Source()==e.drawn&&
        e.lastX==space.x&&e.lastY==space.y&&e.lastW==space.w&&e.lastH==space.h&&
        e.lastScaleX==space.scaleX&&e.lastScaleY==space.scaleY)return true;
    bool layoutChanged=e.lastX!=space.x||e.lastY!=space.y||e.lastW!=space.w||e.lastH!=space.h||
        e.lastScaleX!=space.scaleX||e.lastScaleY!=space.scaleY;
    bool smooth=EffectSmoothing::Use(animated,cfg.fps);
    if(layoutChanged||e.generation!=gen||e.frame.config!=&cfg||!smooth)e.smooth.Reset();
    e.lastX=space.x;e.lastY=space.y;e.lastW=space.w;e.lastH=space.h;
    e.lastScaleX=space.scaleX;e.lastScaleY=space.scaleY;
    if(!smooth||!e.smooth.ready||e.smooth.stamp!=space.time){
        const Pixels& generated=MakeFrame(e.raw,cfg,e.frame,gen,&space);
        if(smooth&&!generated.bgra.empty())e.smooth.Push(generated.bgra.data(),generated.bgra.size(),space.time,1.0/std::max(1,cfg.fps));
    }
    const Pixels& output=e.frame.output;
    if(output.bgra.empty())return true;
    if(!e.drawn||e.drawn.PixelWidth()!=output.w||e.drawn.PixelHeight()!=output.h)
        e.drawn=Media::Imaging::WriteableBitmap(output.w,output.h);
    auto buffer=e.drawn.PixelBuffer();
    if(buffer.Capacity()<output.bgra.size())return true;
    if(smooth&&e.smooth.ready)e.smooth.Blend(buffer.data(),(size_t)output.w*4,(size_t)output.w*4,output.h,now);
    else memcpy(buffer.data(),output.bgra.data(),output.bgra.size());
    e.drawn.Invalidate();
    if(image.Source()!=e.drawn)image.Source(e.drawn);
    e.generation=gen;e.retryAfter=0;e.lastError=0;
    if(!e.loggedApplied){e.loggedApplied=true;Wh_Log(L"Taskbar icons: first processed image applied");}
    return true;
}
static void Tick(Context& c){
    auto cfg=std::atomic_load(&g_config);if(!cfg)return;
    bool animated=g_enabled&&cfg->effect!=L"none"&&cfg->prog.usesTime;
    const double frameTime=IconFrameTime(cfg->fps);
    size_t index=0,total=std::max<size_t>(1,c.entries.size());
    for(auto it=c.entries.begin();it!=c.entries.end();){
        if(Now()<it->retryAfter){++it;++index;continue;}
        IconSpace fallback;fallback.x=(float)index*std::max(1,it->raw.w);
        fallback.w=(float)total*std::max(1,it->raw.w);fallback.h=(float)std::max(1,it->raw.h);
        fallback.time=frameTime;
        try{if(!DrawEntry(*it,*cfg,animated,fallback)){it=c.entries.erase(it);continue;}}
        catch(winrt::hresult_error const& ex){
            unsigned code=(unsigned)ex.code().value;
            if(it->lastError!=code){Wh_Log(L"Taskbar icons: update deferred, retrying (0x%08X)",code);it->lastError=code;}
            // Keep the decoded source and weak references through transient XAML failures.
            it->retryAfter=Now()+0.5;
        }catch(...){
            if(it->lastError!=0xFFFFFFFFu){Wh_Log(L"Taskbar icons: update deferred, retrying");it->lastError=0xFFFFFFFFu;}
            it->retryAfter=Now()+0.5;
        }
        ++it;++index;
    }
    SetTimer(c.window,kTimer,animated?(UINT)(1000/(EffectSmoothing::enabled.load()?60:std::clamp(cfg->fps,1,30))):500,nullptr);
}
static LRESULT CALLBACK WindowProc(HWND h,UINT msg,WPARAM w,LPARAM l){
    auto ctx=reinterpret_cast<Context*>(GetWindowLongPtrW(h,GWLP_USERDATA));
    if(!ctx)return DefWindowProcW(h,msg,w,l);
    if((msg==WM_TIMER&&w==kTimer)||msg==kReload){
        if(!g_stopping){try{Tick(*ctx);}catch(...){Wh_Log(L"UI tick failed");}}return 0;
    }
    if(msg==kStop){
        KillTimer(h,kTimer);
        for(auto& e:ctx->entries)try{Restore(e);}catch(...){}
        ctx->entries.clear();DestroyWindow(h);return 0;
    }
    if(msg==WM_NCDESTROY){
        KillTimer(h,kTimer);
        {std::lock_guard<std::mutex> lock(g_contextMutex);g_contexts.erase(GetCurrentThreadId());}
        SetWindowLongPtrW(h,GWLP_USERDATA,0);delete ctx;
    }
    return DefWindowProcW(h,msg,w,l);
}
static Context* GetContext(){
    std::lock_guard<std::mutex> lock(g_contextMutex);
    if(g_stopping)return nullptr;
    DWORD thread=GetCurrentThreadId();auto it=g_contexts.find(thread);
    if(it!=g_contexts.end())return reinterpret_cast<Context*>(GetWindowLongPtrW(it->second,GWLP_USERDATA));
    auto ctx=std::make_unique<Context>();
    HWND h=CreateWindowExW(0,kWindowClass,L"",0,0,0,0,0,HWND_MESSAGE,nullptr,g_instance,nullptr);
    if(!h)return nullptr;
    Context* result=ctx.release();result->window=h;SetWindowLongPtrW(h,GWLP_USERDATA,(LONG_PTR)result);g_contexts[thread]=h;
    SetTimer(h,kTimer,100,nullptr);return result;
}
static std::vector<HWND> ContextWindows(){
    std::lock_guard<std::mutex> lock(g_contextMutex);std::vector<HWND> out;
    for(auto const& kv:g_contexts)out.push_back(kv.second);return out;
}

using IconSetter=void(WINAPI*)(void*,LONG_PTR);
static IconSetter g_iconOriginal=nullptr;
static void WINAPI IconHook(void* impl,LONG_PTR streamArg){
    // A projected IRandomAccessStream value is passed indirectly under the x64
    // MSVC C++ ABI; the const-reference setter variant also passes this address.
    // Retain the input for UI-thread retries; decode only after native setup.
    IRandomAccessStream source=nullptr;
    if(!g_stopping&&streamArg){try{
        source=*reinterpret_cast<IRandomAccessStream const*>(streamArg);
    }catch(...){Wh_Log(L"Taskbar icons: source capture deferred");}}
    g_iconOriginal(impl,streamArg);
    if(g_stopping)return;
    try{
        auto button=ButtonElement(impl);auto ctx=GetContext();if(!ctx)return;
        if(!source){
            for(auto it=ctx->entries.begin();it!=ctx->entries.end();++it)if(it->button.get()==button){
                Restore(*it);ctx->entries.erase(it);break;
            }
            return;
        }
        Entry* existing=nullptr;
        for(auto& e:ctx->entries)if(e.button.get()==button){existing=&e;break;}
        if(!existing){if(ctx->entries.size()>=256)return;ctx->entries.emplace_back();existing=&ctx->entries.back();}
        else {Restore(*existing);}
        existing->button=winrt::make_weak(button);existing->raw={};existing->frame.valid=false;existing->smooth.Reset();
        existing->sourceStream=std::move(source);existing->startupAt=Now();
        existing->recoveryPass=0;existing->lastProbe=-1;
        existing->image={};existing->original=nullptr;existing->drawn=nullptr;existing->generation=0;
        existing->retryAfter=0;existing->lastError=0;existing->loggedApplied=false;
        // Defer until the native setter has finished its visual/template updates.
        PostMessageW(ctx->window,kReload,0,0);
    }catch(winrt::hresult_error const& ex){Wh_Log(L"Taskbar icon attach failed: 0x%08X",(unsigned)ex.code().value);}
    catch(...){Wh_Log(L"Taskbar icon attach failed");}
}
static bool HookTaskbar(HMODULE module){
    WindhawkUtils::SYMBOL_HOOK symbols[]={
        {{LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::Icon(struct winrt::Windows::Storage::Streams::IRandomAccessStream))",
          LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::Icon(struct winrt::Windows::Storage::Streams::IRandomAccessStream const &))"},
          &g_iconOriginal,IconHook}
    };
    bool ok=WindhawkUtils::HookSymbols(module,symbols,ARRAYSIZE(symbols));
    if(!ok)Wh_Log(L"Unsupported taskbar: app Icon setter symbol not found");
    return ok;
}
static HMODULE TaskbarModule(){auto m=GetModuleHandleW(L"Taskbar.View.dll");return m?m:GetModuleHandleW(L"ExplorerExtensions.dll");}
using Loader=decltype(&LoadLibraryExW);
static Loader g_loadOriginal=nullptr;
static HMODULE WINAPI LoadHook(LPCWSTR path,HANDLE file,DWORD flags){
    HMODULE m=g_loadOriginal(path,file,flags);
    if(m&&!g_stopping&&!g_hooked&&TaskbarModule()==m){
        bool expected=false;
        if(g_hooked.compare_exchange_strong(expected,true)){
            if(HookTaskbar(m)){Wh_ApplyHookOperations();Wh_Log(L"Taskbar icon hook ready");}
        }
    }
    return m;
}
BOOL Initialize(){
    LoadSettings();
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      reinterpret_cast<LPCWSTR>(&WindowProc),&g_instance);
    WNDCLASSW wc={};wc.lpfnWndProc=WindowProc;wc.hInstance=g_instance;wc.lpszClassName=kWindowClass;
    if(!RegisterClassW(&wc))return FALSE;
    HMODULE taskbar=TaskbarModule();
    if(taskbar){g_hooked=true;if(!HookTaskbar(taskbar)){g_stopping=true;UnregisterClassW(kWindowClass,g_instance);return FALSE;}}
    auto kernel=GetModuleHandleW(L"kernelbase.dll");
    auto loader=reinterpret_cast<Loader>(GetProcAddress(kernel,"LoadLibraryExW"));
    if(!loader||!WindhawkUtils::SetFunctionHook(loader,LoadHook,&g_loadOriginal)){
        g_stopping=true;UnregisterClassW(kWindowClass,g_instance);return FALSE;
    }
    g_initialized=true;return TRUE;
}
void AfterInit(){
    if(!g_initialized)return;
    if(auto m=TaskbarModule()){
        bool expected=false;
        if(g_hooked.compare_exchange_strong(expected,true)&&HookTaskbar(m))Wh_ApplyHookOperations();
    }
    Wh_Log(L"Taskbar Icon Effects ready; restart Explorer once to attach icons already loaded before installation");
}
void Reload(){
    if(!g_initialized)return;
    LoadSettings();
    for(HWND h:ContextWindows())PostMessageW(h,kReload,0,0);
}
void Stop(){
    if(!g_initialized)return;
    g_stopping=true;
    // Send synchronously so no timer/window callback can survive DLL unloading.
    // All XAML sources are restored on the same thread that created them.
    for(HWND h:ContextWindows())if(IsWindow(h))SendMessageW(h,kStop,0,0);
}
void Uninitialize(){
    if(!g_initialized)return;
    UnregisterClassW(kWindowClass,g_instance);
    std::atomic_store(&g_config,std::shared_ptr<const IconCfg>{});
    g_initialized=false;
}

} // namespace TaskbarFx

// Shell-owned image lists are never edited. Process a private copy at draw time,
// scoped to Explorer file views and the optional navigation tree.
namespace ExplorerIconFx {
struct Config {IconCfg icon;bool enabled=false,navigation=false;unsigned generation=0;};
static std::shared_ptr<const Config> config;
static std::atomic<bool> stopping=false,available=false;
bool HandlesIcons(){return available.load();}
static unsigned generation=0;
static std::mutex mutex;
static std::set<HWND> windows;
static thread_local HWND paintWindow=nullptr;
static thread_local bool drawing=false;
static constexpr UINT_PTR timer=0xD0D121,subclass=0;
static constexpr UINT refresh=WM_APP+0x6A1,detach=WM_APP+0x6A2;
static constexpr size_t budgetLimit=24u*1024u*1024u;
static TmpDib capture,surface;
using DrawFn=HRESULT(STDMETHODCALLTYPE*)(IImageList*,IMAGELISTDRAWPARAMS*);
struct Hook {void* target=nullptr;DrawFn original=nullptr;};
static Hook hooks[5];
using IndirectFn=BOOL(WINAPI*)(IMAGELISTDRAWPARAMS*);
static IndirectFn indirect=nullptr;
// Native list identity, item/overlay/state, draw geometry, owner and shared canvas.
using Key=std::tuple<UINT_PTR,int,int,int,UINT,DWORD,DWORD,COLORREF,COLORREF,int,int,UINT_PTR,int,int,int,int,COLORREF>;
struct Entry {TaskbarFx::Pixels raw;TaskbarFx::FrameWorkspace work;EffectSmoothing::Frame smooth;double captured=-10,phase=-1;};
static std::map<Key,Entry> entries;
static size_t Bytes(const Entry& e){return e.raw.bgra.capacity()+e.work.output.bgra.capacity()+e.work.foreground.capacity()+e.work.background.capacity()+e.smooth.Bytes();}
static size_t Bytes(){size_t n=0;for(auto& kv:entries)n+=Bytes(kv.second);return n;}
static int Area(HWND h){
    if(!h||Classify(h)!=K_EXPLORER)return 0;
    wchar_t name[80]{};GetClassNameW(h,name,ARRAYSIZE(name));
    if(!wcscmp(name,L"SysTreeView32"))return 2;
    if(wcscmp(name,L"DirectUIHWND")&&wcscmp(name,L"SysListView32"))return 0;
    for(HWND p=GetParent(h);p;p=GetParent(p)){
        GetClassNameW(p,name,ARRAYSIZE(name));
        if(!wcscmp(name,L"SHELLDLL_DefView"))return 1;
    }
    return 0;
}
static bool Enabled(HWND h,const Config& cfg){int area=Area(h);return cfg.enabled&&cfg.icon.HasEffects()&&(area==1||(area==2&&cfg.navigation));}
static void Clear(){entries.clear();GdiFlush();FreeDib(capture);FreeDib(surface);}
static void Load(){
    auto cfg=std::make_shared<Config>();cfg->enabled=Wh_GetIntSetting(L"ExplorerIcons.Enabled")!=0;
    cfg->navigation=Wh_GetIntSetting(L"ExplorerIcons.IncludeNavigationPane")!=0;
    cfg->icon=ReadIndependentIconConfig(L"ExplorerIcons");cfg->generation=++generation;
    std::atomic_store(&config,std::shared_ptr<const Config>(cfg));
    std::lock_guard<std::mutex> lock(mutex);Clear();
}
template<class NativeDraw> static bool Draw(UINT_PTR identity,IMAGELISTDRAWPARAMS* params,int naturalW,int naturalH,NativeDraw native){
    if(drawing||stopping||!params||params->cbSize<sizeof(IMAGELISTDRAWPARAMS)||!params->hdcDst)return false;
    auto cfg=std::atomic_load(&config);if(!cfg)return false;
    HWND dcWindow=WindowFromDC(params->hdcDst);HWND owner=dcWindow?dcWindow:paintWindow;
    if(!Enabled(owner,*cfg)||GetMapMode(params->hdcDst)!=MM_TEXT)return false;
    int w=params->cx>0?params->cx:naturalW,h=params->cy>0?params->cy:naturalH;
    if(w<=0||h<=0||w>256||h>256||params->i<0||(params->fStyle&ILD_ROP))return false;
    std::unique_lock<std::mutex> lock(mutex,std::try_to_lock);if(!lock.owns_lock()||stopping)return false;
    struct Guard{Guard(){drawing=true;}~Guard(){drawing=false;}} guard;
    try{
        RECT rect{};if(!GetClientRect(owner,&rect))return false;
        POINT point={params->x,params->y};if(dcWindow)LPtoDP(params->hdcDst,&point,1);
        // Memory DCs retain view-client coordinates; a real window DC is likewise local.
        int canvasW=std::max(1,(int)(rect.right-rect.left)),canvasH=std::max(1,(int)(rect.bottom-rect.top));
        bool spatial=cfg->icon.effect!=L"none";
        Key key(identity,params->i,w,h,params->fStyle,params->fState,params->Frame,params->rgbFg,params->rgbBk,
            params->xBitmap,params->yBitmap,(UINT_PTR)owner,spatial?point.x:0,spatial?point.y:0,canvasW,canvasH,params->crEffect);
        auto found=entries.find(key);
        if(found==entries.end()){
            if(entries.size()>=256||Bytes()>budgetLimit/2)entries.clear();
            found=entries.emplace(key,Entry{}).first;
        }
        auto& entry=found->second;double now=Now();bool sourceChanged=false;
        if(entry.raw.bgra.empty()||now-entry.captured>=.5){
            if(!EnsureDib(capture,params->hdcDst,w,h))return false;
            GdiFlush();memset(capture.bits,0,(size_t)capture.w*capture.h*4);
            auto copy=*params;copy.hdcDst=capture.dc;copy.x=copy.y=0;
            copy.rgbBk=CLR_NONE;copy.fStyle|=ILD_TRANSPARENT;
            if(!native(copy))return false;
            GdiFlush();TaskbarFx::Pixels raw;raw.w=w;raw.h=h;raw.bgra.resize((size_t)w*h*4);
            for(int y=0;y<h;y++)memcpy(raw.bgra.data()+(size_t)y*w*4,(BYTE*)capture.bits+(size_t)y*capture.w*4,(size_t)w*4);
            // Shell alpha is premultiplied; reject blank/unsupported captures.
            if(!TaskbarFx::HasVisiblePixels(raw)){static std::atomic<bool> warned=false;if(!warned.exchange(true))Wh_Log(L"Explorer icons: image-list capture has no usable alpha; keeping native rendering");return false;}
            for(size_t i=0;i<raw.bgra.size();i+=4){unsigned a=raw.bgra[i+3];for(int k=0;k<3;k++)raw.bgra[i+k]=a?(BYTE)std::min(255u,(raw.bgra[i+k]*255u+a/2)/a):0;}
            sourceChanged=entry.raw.bgra!=raw.bgra;entry.raw=std::move(raw);entry.captured=now;
        }
        bool animated=cfg->icon.effect!=L"none"&&cfg->icon.prog.usesTime;
        bool smoothing=EffectSmoothing::Use(animated,cfg->icon.fps);
        double phase=IconFrameTime(cfg->icon.fps);
        if(sourceChanged){entry.work.valid=false;entry.smooth.Reset();}
        if(sourceChanged||entry.work.output.bgra.empty()||(animated&&phase!=entry.phase)){
            IconSpace space;space.x=(float)point.x;space.y=(float)point.y;space.w=(float)canvasW;space.h=(float)canvasH;space.time=phase;
            auto& frame=TaskbarFx::MakeFrame(entry.raw,cfg->icon,entry.work,cfg->generation,&space);
            if(smoothing)entry.smooth.Push(frame.bgra.data(),frame.bgra.size(),phase,1.0/std::max(1,cfg->icon.fps));
            entry.phase=phase;
        }
        auto& frame=entry.work.output;if(frame.bgra.empty())return false;
        if(Bytes()>budgetLimit){entries.erase(found);return false;}
        if(!EnsureDib(surface,params->hdcDst,frame.w,frame.h))return false;
        GdiFlush();
        if(smoothing&&entry.smooth.ready)entry.smooth.Blend((BYTE*)surface.bits,(size_t)surface.w*4,(size_t)frame.w*4,frame.h,now);
        else for(int y=0;y<frame.h;y++)memcpy((BYTE*)surface.bits+(size_t)y*surface.w*4,frame.bgra.data()+(size_t)y*frame.w*4,(size_t)frame.w*4);
        int padX=(frame.w-w)/2,padY=(frame.h-h)/2;
        BLENDFUNCTION blend{AC_SRC_OVER,0,255,AC_SRC_ALPHA};
        bool ok=AlphaBlend(params->hdcDst,params->x-padX,params->y-padY,frame.w,frame.h,surface.dc,0,0,frame.w,frame.h,blend)!=FALSE;
        static std::atomic<bool> loggedFiles=false,loggedNavigation=false;
        auto& logged=Area(owner)==2?loggedNavigation:loggedFiles;
        if(ok&&!logged.exchange(true))Wh_Log(L"Explorer icons: first processed %s icon drawn",Area(owner)==2?L"navigation pane":L"file view");
        return ok;
    }catch(...){return false;}
}
template<int N> static HRESULT STDMETHODCALLTYPE DrawHook(IImageList* list,IMAGELISTDRAWPARAMS* params){
    auto original=hooks[N].original;auto cfg=std::atomic_load(&config);
    if(drawing||stopping||!params||!cfg||!cfg->enabled||!cfg->icon.HasEffects())return original(list,params);
    int w=0,h=0;list->GetIconSize(&w,&h);
    if(Draw((UINT_PTR)list,params,w,h,[&](auto& copy){return SUCCEEDED(original(list,&copy));}))return S_OK;
    struct NativeGuard{NativeGuard(){drawing=true;}~NativeGuard(){drawing=false;}} guard;
    return original(list,params);
}
static BOOL WINAPI IndirectHook(IMAGELISTDRAWPARAMS* params){
    auto cfg=std::atomic_load(&config);
    if(drawing||stopping||!params||!cfg||!cfg->enabled||!cfg->icon.HasEffects())return indirect(params);
    int w=0,h=0;ImageList_GetIconSize(params->himl,&w,&h);
    if(Draw((UINT_PTR)params->himl,params,w,h,[&](auto& copy){return indirect(&copy)!=FALSE;}))return TRUE;
    struct NativeGuard{NativeGuard(){drawing=true;}~NativeGuard(){drawing=false;}} guard;
    return indirect(params);
}
static LRESULT CALLBACK ViewProc(HWND h,UINT message,WPARAM w,LPARAM l,DWORD_PTR){
    if(message==detach||stopping){
        KillTimer(h,timer);WindhawkUtils::RemoveWindowSubclassFromAnyThread(h,ViewProc);
        {std::lock_guard<std::mutex> lock(mutex);windows.erase(h);}
        if(message==detach){InvalidateRect(h,nullptr,FALSE);return 0;}
        return DefSubclassProc(h,message,w,l);
    }
    auto cfg=std::atomic_load(&config);
    if(message==refresh||(message==WM_TIMER&&w==timer)){
        bool active=cfg&&Enabled(h,*cfg)&&IsWindowVisible(h)&&!IsIconic(GetAncestor(h,GA_ROOT));
        bool animated=active&&cfg->icon.effect!=L"none"&&cfg->icon.prog.usesTime;
        if(animated||message==refresh)InvalidateRect(h,nullptr,FALSE);
        UINT delay=animated?1000/(EffectSmoothing::Use(true,cfg->icon.fps)?60:std::max(1,cfg->icon.fps)):1000;
        SetTimer(h,timer,delay,nullptr);return 0;
    }
    if(message==WM_PAINT||message==WM_PRINTCLIENT){
        struct Paint{HWND old;Paint(HWND h):old(paintWindow){paintWindow=h;}~Paint(){paintWindow=old;}} scope(h);
        return DefSubclassProc(h,message,w,l);
    }
    if(message==WM_NCDESTROY){KillTimer(h,timer);WindhawkUtils::RemoveWindowSubclassFromAnyThread(h,ViewProc);std::lock_guard<std::mutex> lock(mutex);windows.erase(h);
        for(auto it=entries.begin();it!=entries.end();)if(std::get<11>(it->first)==(UINT_PTR)h)it=entries.erase(it);else ++it;}
    return DefSubclassProc(h,message,w,l);
}
static BOOL CALLBACK FindChild(HWND h,LPARAM){
    if(!Area(h)||stopping)return TRUE;
    {std::lock_guard<std::mutex> lock(mutex);if(windows.count(h)||windows.size()>=128)return TRUE;}
    if(!EnsureDesktopModuleLease())return TRUE;
    if(WindhawkUtils::SetWindowSubclassFromAnyThread(h,ViewProc,subclass)){
        if(stopping){WindhawkUtils::RemoveWindowSubclassFromAnyThread(h,ViewProc);return TRUE;}
        {std::lock_guard<std::mutex> lock(mutex);windows.insert(h);}
        PostMessageW(h,refresh,0,0);
        wchar_t name[80]{};GetClassNameW(h,name,ARRAYSIZE(name));
        Wh_Log(L"Explorer icons: watching %s (class=%s, window=%p)",Area(h)==2?L"navigation pane":L"file view",name,h);
    }
    return TRUE;
}
static BOOL CALLBACK FindRoot(HWND h,LPARAM){DWORD pid=0;GetWindowThreadProcessId(h,&pid);wchar_t name[64]{};GetClassNameW(h,name,ARRAYSIZE(name));if(pid==GetCurrentProcessId()&&!wcscmp(name,L"CabinetWClass"))EnumChildWindows(h,FindChild,0);return TRUE;}
void Scan(){auto cfg=std::atomic_load(&config);if(!stopping&&cfg&&cfg->enabled)EnumWindows(FindRoot,0);}
void Reload(){Load();Scan();std::vector<HWND> copy;{std::lock_guard<std::mutex> lock(mutex);copy.assign(windows.begin(),windows.end());}for(HWND h:copy)PostMessageW(h,refresh,0,0);}
void Initialize(){
    HRESULT apartment=CoInitializeEx(nullptr,COINIT_MULTITHREADED);
    stopping=false;Load();
    using GetLists=HRESULT(WINAPI*)(int,REFIID,void**);
    auto getLists=reinterpret_cast<GetLists>(GetProcAddress(GetModuleHandleW(L"shell32.dll"),"SHGetImageList"));
    static DrawFn replacements[]={DrawHook<0>,DrawHook<1>,DrawHook<2>,DrawHook<3>,DrawHook<4>};
    int count=0;
    if(getLists)for(int size:{SHIL_SMALL,SHIL_LARGE,SHIL_EXTRALARGE,SHIL_SYSSMALL,SHIL_JUMBO}){
        winrt::com_ptr<IImageList> list;if(FAILED(getLists(size,__uuidof(IImageList),list.put_void())))continue;
        void* target=(*reinterpret_cast<void***>(list.get()))[8];bool duplicate=false;
        for(int i=0;i<count;i++)if(hooks[i].target==target)duplicate=true;
        if(!duplicate&&WindhawkUtils::SetFunctionHook(reinterpret_cast<DrawFn>(target),replacements[count],&hooks[count].original)){hooks[count].target=target;++count;}
    }
    auto target=reinterpret_cast<IndirectFn>(GetProcAddress(GetModuleHandleW(L"comctl32.dll"),"ImageList_DrawIndirect"));
    bool exported=target&&WindhawkUtils::SetFunctionHook(target,IndirectHook,&indirect);
    available=count>0||exported;
    Wh_Log(L"Explorer icon draw hooks: COM implementations=%d, image-list export=%d",count,exported);
    if(SUCCEEDED(apartment))CoUninitialize();
}
void RequestStop(){stopping=true;}
void Stop(){
    stopping=true;std::vector<HWND> copy;{std::unique_lock<std::mutex> lock(mutex,std::try_to_lock);if(!lock.owns_lock()){g_desktopRetainState=true;return;}copy.assign(windows.begin(),windows.end());}
    for(HWND h:copy)if(IsWindow(h)){DWORD_PTR result=0;if(!SendMessageTimeoutW(h,detach,0,0,SMTO_ABORTIFHUNG|SMTO_BLOCK,1000,&result))g_desktopRetainState=true;}
    std::unique_lock<std::mutex> lock(mutex,std::try_to_lock);if(!lock.owns_lock()){g_desktopRetainState=true;return;}if(!windows.empty())g_desktopRetainState=true;Clear();
}
} // namespace ExplorerIconFx

// Start menu bitmap icons: public XAML tree/render APIs, no private shell ABI.
// UI dispatch pattern informed by m417z's GPL-3.0 Start Menu Styler. Only the
// local CoreWindow dispatch technique is used; no telemetry/layout modification.
namespace StartMenuFx {
using namespace winrt::Windows::UI::Xaml;
using winrt::Windows::Foundation::AsyncStatus;
using winrt::Windows::Foundation::IAsyncAction;
using winrt::Windows::Foundation::IAsyncOperation;
using winrt::Windows::Storage::Streams::IBuffer;
using TaskbarFx::Pixels;
using TaskbarFx::FrameWorkspace;
static constexpr size_t kMaxEntries=64,kBudget=24u*1024u*1024u;
static constexpr UINT kTick=WM_APP+0x681,kReset=WM_APP+0x682,kStop=WM_APP+0x683;
static constexpr UINT_PTR kTimer=0x71C1;
static constexpr wchar_t kClass[]=L"Windhawk_DodiStartIconEffects_UI_01";
struct Config {IconCfg icon;bool enabled=false;unsigned generation=0;};
static std::shared_ptr<const Config> g_config;
static std::atomic<bool> g_stopping=false,g_retain=false;
static HINSTANCE g_instance=nullptr;
static HMODULE g_lease=nullptr;
static HANDLE g_stop=nullptr,g_worker=nullptr;
static UINT g_dispatch=0;
static std::mutex g_mutex;
static std::map<DWORD,HWND> g_windows;
static unsigned g_generation=0;
static bool g_initialized=false;
struct Entry {
    winrt::weak_ref<Controls::Image> image;
    Media::ImageSource original=nullptr;
    Media::Imaging::WriteableBitmap drawn=nullptr;
    Media::Imaging::RenderTargetBitmap capture=nullptr;
    IAsyncAction render=nullptr;
    IAsyncOperation<IBuffer> pixels=nullptr;
    Pixels raw;
    FrameWorkspace work;
    EffectSmoothing::Frame smooth;
    unsigned generation=0;
    double retryAt=0,lastSeen=0,lastNotice=-10;
    std::vector<BYTE> presentation;
    float x=0,y=0,w=0,h=0,sx=0,sy=0,rootW=0,rootH=0;
};
struct Context {
    HWND window=nullptr,owner=nullptr;
    std::vector<Entry> entries;
    std::vector<winrt::weak_ref<DependencyObject>> scan,roots;
    std::vector<winrt::weak_ref<FrameworkElement>> candidates;
    double lastScan=-10;
    unsigned generation=0;
    bool busy=false,logged=false,rootLogged=false,stopPending=false,notificationQueued=false;
};
static void Load(){
    auto c=std::make_shared<Config>();c->enabled=Wh_GetIntSetting(L"StartMenuIcons.Enabled")!=0;
    c->icon=ReadIndependentIconConfig(L"StartMenuIcons");c->generation=++g_generation;
    std::atomic_store(&g_config,std::shared_ptr<const Config>(std::move(c)));
}
static void Cancel(Entry& e){
    try{if(e.render&&e.render.Status()==AsyncStatus::Started)e.render.Cancel();}catch(...){}
    try{if(e.pixels&&e.pixels.Status()==AsyncStatus::Started)e.pixels.Cancel();}catch(...){}
    e.render=nullptr;e.pixels=nullptr;e.capture=nullptr;
}
static void Restore(Entry& e){
    Cancel(e);
    if(auto image=e.image.get())if(e.drawn&&image.Source()==e.drawn)image.Source(e.original);
    e.drawn=nullptr;e.raw={};e.work={};e.smooth.Reset();e.presentation.clear();e.generation=0;
}
static size_t Bytes(const Entry& e){
    return e.presentation.capacity()+e.raw.bgra.capacity()+e.work.output.bgra.capacity()+e.work.foreground.capacity()+e.work.background.capacity()+
        e.smooth.Bytes()+(e.drawn?(size_t)e.drawn.PixelWidth()*e.drawn.PixelHeight()*4:0)+
        (e.capture?96u*96u*4u:0);
}
static size_t Bytes(const Context& c){size_t n=0;for(const auto& e:c.entries)n+=Bytes(e);return n;}
static bool IconImage(const Controls::Image& image){
    auto name=image.Name();
    if(name==L"UserAvatar"||name==L"UserPicture"||name==L"ProfilePicture")return false;
    // Explicit shell image names avoid user avatars, recommended thumbnails and
    // other large imagery. Unknown future templates retain native rendering.
    bool icon=name==L"Logo"||name==L"AllAppsItemLogo"||name==L"AllAppsTileLogo"||name==L"AppIcon";
    // Folder preview mini-icons can be unnamed; constrain them by their shell container.
    auto parent=Media::VisualTreeHelper::GetParent(image);
    for(int depth=0;parent&&depth<8;depth++,parent=Media::VisualTreeHelper::GetParent(parent)){
        if(auto element=parent.try_as<FrameworkElement>()){
            auto container=element.Name();
            if(container==L"LogosContainer"||container==L"LogosItemsControl"||container==L"LogoContainer")icon=true;
            if(container==L"UserAvatar"||container==L"UserTile"||container==L"RecommendedList")return false;
        }
    }
    return icon;
}
static void Scan(Context& c,double now){
    // Revisit elements announced before their XamlRoot/Name/Loaded properties settled.
    for(auto it=c.candidates.begin();it!=c.candidates.end();){
        auto element=it->get();if(!element){it=c.candidates.erase(it);continue;}
        if(auto root=element.XamlRoot();root&&root.Content()){
            auto content=root.Content().as<DependencyObject>();bool found=false;
            for(auto const& weak:c.roots)if(weak.get()==content){found=true;break;}
            if(!found&&c.roots.size()<8)c.roots.push_back(winrt::make_weak(content));
            it=c.candidates.erase(it);
        }else ++it;
    }
    if(c.scan.empty()){
        if(now-c.lastScan<1)return;
        c.lastScan=now;
        for(auto it=c.roots.begin();it!=c.roots.end();){if(auto root=it->get()){c.scan.push_back(*it);++it;}else it=c.roots.erase(it);}
        if(auto window=Window::Current();window&&window.Content())c.scan.push_back(winrt::make_weak(window.Content().as<DependencyObject>()));
        if(c.scan.empty()){
            if(!c.rootLogged){c.rootLogged=true;Wh_Log(L"Start menu icons: waiting for a visual-tree root; native sources preserved");}return;
        }
    }
    unsigned visited=0;
    while(!c.scan.empty()&&visited++<256){
        auto object=c.scan.back().get();c.scan.pop_back();if(!object)continue;
        if(auto image=object.try_as<Controls::Image>();image&&IconImage(image)&&image.IsLoaded()){
            bool found=false;
            for(auto& e:c.entries)if(e.image.get()==image){e.lastSeen=now;found=true;break;}
            if(!found&&c.entries.size()<kMaxEntries){
                Entry e;e.image=winrt::make_weak(image);e.original=image.Source();e.lastSeen=now;
                c.entries.push_back(std::move(e));
            }
        }
        int n=Media::VisualTreeHelper::GetChildrenCount(object);
        // A bounded weak-reference traversal: no permanent retention of templates.
        for(int i=n-1;i>=0&&c.scan.size()<2048;--i)
            c.scan.push_back(winrt::make_weak(Media::VisualTreeHelper::GetChild(object,i)));
    }
}
static bool ReadyPixels(Entry& e,Controls::Image const& image,double now){
    if(!e.raw.bgra.empty())return true;
    if(now<e.retryAt)return false;
    // Read an already-loaded writable source directly when available. This avoids
    // a visual render/readback round-trip and also works if visual capture is unavailable.
    if(!e.render&&!e.pixels)if(auto bitmap=e.original.try_as<Media::Imaging::WriteableBitmap>()){
        int w=bitmap.PixelWidth(),h=bitmap.PixelHeight();auto buffer=bitmap.PixelBuffer();
        if(w>0&&h>0&&w<=96&&h<=96&&buffer.Capacity()>=(size_t)w*h*4){
            e.raw.w=w;e.raw.h=h;e.raw.bgra.assign(buffer.data(),buffer.data()+(size_t)w*h*4);
            for(size_t i=0;i<e.raw.bgra.size();i+=4){unsigned a=e.raw.bgra[i+3];for(int k=0;k<3;k++)e.raw.bgra[i+k]=a?(BYTE)std::min(255u,(e.raw.bgra[i+k]*255u+a/2)/a):0;}
            if(TaskbarFx::HasVisiblePixels(e.raw))return true;
            e.raw={};e.retryAt=now+2;return false;
        }
    }
    if(e.render){
        auto status=e.render.Status();if(status==AsyncStatus::Started)return false;
        if(status!=AsyncStatus::Completed){if(now-e.lastNotice>=10){e.lastNotice=now;Wh_Log(L"Start menu icons: render capture did not complete (status=%u); native icon retained",(unsigned)status);}Cancel(e);e.retryAt=now+2;return false;}
        e.render.GetResults();e.render=nullptr;e.pixels=e.capture.GetPixelsAsync();return false;
    }
    if(e.pixels){
        auto status=e.pixels.Status();if(status==AsyncStatus::Started)return false;
        if(status!=AsyncStatus::Completed){if(now-e.lastNotice>=10){e.lastNotice=now;Wh_Log(L"Start menu icons: pixel readback did not complete (status=%u); native icon retained",(unsigned)status);}Cancel(e);e.retryAt=now+2;return false;}
        auto buffer=e.pixels.GetResults();
        int w=e.capture.PixelWidth(),h=e.capture.PixelHeight();
        if(w>0&&h>0&&w<=96&&h<=96&&buffer.Length()>=(size_t)w*h*4){
            e.raw.w=w;e.raw.h=h;e.raw.bgra.assign(buffer.data(),buffer.data()+(size_t)w*h*4);
            // RenderTargetBitmap returns premultiplied BGRA; MakeFrame expects straight alpha.
            for(size_t i=0;i<e.raw.bgra.size();i+=4){unsigned a=e.raw.bgra[i+3];
                for(int k=0;k<3;++k)e.raw.bgra[i+k]=a?(BYTE)std::min(255u,(e.raw.bgra[i+k]*255u+a/2)/a):0;}
            if(!TaskbarFx::HasVisiblePixels(e.raw)){
                e.raw={};if(now-e.lastNotice>=10){e.lastNotice=now;Wh_Log(L"Start menu icons: capture returned no visible pixels; native icon retained");}
            }
        }
        Cancel(e);if(e.raw.bgra.empty())e.retryAt=now+2;return !e.raw.bgra.empty();
    }
    double actualW=image.ActualWidth(),actualH=image.ActualHeight();
    if(!std::isfinite(actualW)||!std::isfinite(actualH)||actualW<=0||actualH<=0||actualW>256||actualH>256)return false;
    int w=(int)std::ceil(actualW),h=(int)std::ceil(actualH);
    if(w<=0||h<=0||w>256||h>256)return false;
    float factor=std::min(1.f,96.f/(float)std::max(w,h));
    e.capture=Media::Imaging::RenderTargetBitmap();
    // Capture once per native source/layout change, never once per animation frame.
    // No completion delegates/coroutines can survive DLL unload: the UI timer polls.
    e.render=e.capture.RenderAsync(image,std::max(1,(int)(w*factor)),std::max(1,(int)(h*factor)));
    return false;
}
static void Draw(Context& c,Entry& e,const Config& cfg,double now,size_t& budget,unsigned& captures){
    auto image=e.image.get();if(!image)return;
    auto current=image.Source();
    if(current!=e.drawn&&current!=e.original){Restore(e);e.original=current;}
    if(!image.IsLoaded()||image.Visibility()!=Visibility::Visible||!current)return;
    auto root=image.XamlRoot();if(!root)return;
    auto size=root.Size();if(!std::isfinite(size.Width)||!std::isfinite(size.Height)||size.Width<=0||size.Height<=0)return;
    auto transform=image.TransformToVisual(nullptr);
    auto a=transform.TransformPoint({0,0});
    auto b=transform.TransformPoint({(float)image.ActualWidth(),(float)image.ActualHeight()});
    if(!std::isfinite(a.X)||!std::isfinite(a.Y)||!std::isfinite(b.X)||!std::isfinite(b.Y)||
        b.X<=0||b.Y<=0||a.X>=size.Width||a.Y>=size.Height)return;
    float iw=(float)image.ActualWidth(),ih=(float)image.ActualHeight();
    if(!std::isfinite(iw)||!std::isfinite(ih)||iw<=0||ih<=0||iw>256||ih>256)return;
    bool layout=e.w!=iw||e.h!=ih;
    if(layout){Restore(e);e.original=image.Source();e.w=iw;e.h=ih;}
    if(e.raw.bgra.empty()&&!e.render&&!e.pixels){
        if(!captures||budget+96u*96u*4u>kBudget)return;
        --captures;
    }
    size_t old=Bytes(e);
    if(!ReadyPixels(e,image,now)){budget=budget-old+Bytes(e);return;}
    int pad=std::max(cfg.icon.glow?cfg.icon.glowBlur*2+cfg.icon.glowOutline+1:0,
        cfg.icon.shadow?cfg.icon.shadowBlur*2+std::max(abs(cfg.icon.shadowX),abs(cfg.icon.shadowY))+1:0);
    size_t maximum=(size_t)(e.raw.w+2*pad)*(e.raw.h+2*pad)*4*7+e.raw.bgra.size()*3;
    if(budget-old+maximum>kBudget){Restore(e);e.retryAt=now+2;budget=budget-old+Bytes(e);return;}
    bool animated=cfg.icon.effect!=L"none"&&cfg.icon.prog.usesTime;
    bool smooth=EffectSmoothing::Use(animated,cfg.icon.fps);
    IconSpace space;space.x=a.X;space.y=a.Y;space.w=size.Width;space.h=size.Height;
    space.scaleX=(b.X-a.X)/e.raw.w;space.scaleY=(b.Y-a.Y)/e.raw.h;
    space.time=IconFrameTime(cfg.icon.fps);
    bool changed=e.generation!=cfg.generation||e.x!=space.x||e.y!=space.y||e.sx!=space.scaleX||e.sy!=space.scaleY||e.rootW!=space.w||e.rootH!=space.h;
    if(!changed&&!animated&&e.drawn&&image.Source()==e.drawn){budget=budget-old+Bytes(e);return;}
    if(changed||!smooth)e.smooth.Reset();
    if(changed||!animated||!smooth||!e.smooth.ready||e.smooth.stamp!=space.time){
        auto& result=TaskbarFx::MakeFrame(e.raw,cfg.icon,e.work,cfg.generation,&space);
        if(smooth)e.smooth.Push(result.bgra.data(),result.bgra.size(),space.time,1.0/std::max(1,cfg.icon.fps));
    }
    auto& output=e.work.output;if(output.bgra.empty())return;
    // Preserve the native icon dimensions/layout. Decoration outside the icon's
    // own image rectangle is clipped rather than shrinking the app icon.
    if(!e.drawn||e.drawn.PixelWidth()!=e.raw.w||e.drawn.PixelHeight()!=e.raw.h)
        e.drawn=Media::Imaging::WriteableBitmap(e.raw.w,e.raw.h);
    auto buffer=e.drawn.PixelBuffer();if(buffer.Capacity()<(size_t)e.raw.w*e.raw.h*4)return;
    const BYTE* source=output.bgra.data();
    if(smooth&&e.smooth.ready){e.presentation.resize(output.bgra.size());e.smooth.Blend(e.presentation.data(),(size_t)output.w*4,(size_t)output.w*4,output.h,now);source=e.presentation.data();}
    int padX=(output.w-e.raw.w)/2,padY=(output.h-e.raw.h)/2;
    for(int y=0;y<e.raw.h;y++)memcpy(buffer.data()+(size_t)y*e.raw.w*4,source+((size_t)(y+padY)*output.w+padX)*4,(size_t)e.raw.w*4);
    e.drawn.Invalidate();if(image.Source()!=e.drawn)image.Source(e.drawn);
    e.generation=cfg.generation;e.x=space.x;e.y=space.y;e.sx=space.scaleX;e.sy=space.scaleY;e.rootW=space.w;e.rootH=space.h;
    budget=budget-old+Bytes(e);
    if(!c.logged){c.logged=true;Wh_Log(L"Start menu icons: first processed bitmap applied");}
}
static void Reset(Context& c){
    for(auto& e:c.entries)try{Restore(e);}catch(...){}
    c.entries.clear();c.scan.clear();c.lastScan=-10;c.logged=false;c.rootLogged=false;
}
static void Tick(Context& c){
    auto cfg=std::atomic_load(&g_config);if(!cfg||g_stopping)return;
    if(c.generation!=cfg->generation){Reset(c);c.generation=cfg->generation;}
    bool enabled=cfg->enabled&&cfg->icon.HasEffects();
    if(!enabled){if(!c.entries.empty())Reset(c);SetTimer(c.window,kTimer,500,nullptr);return;}
    if(!IsWindowVisible(c.owner)){SetTimer(c.window,kTimer,500,nullptr);return;}
    double now=Now();Scan(c,now);
    size_t budget=Bytes(c);unsigned captures=2;bool pending=false;
    for(auto it=c.entries.begin();it!=c.entries.end();){
        auto image=it->image.get();
        if(!image||!image.IsLoaded()){
            size_t old=Bytes(*it);try{Restore(*it);}catch(...){}budget-=old;
            it=c.entries.erase(it);continue;
        }
        try{Draw(c,*it,*cfg,now,budget,captures);}catch(...){
            HRESULT error=winrt::to_hresult();
            if(now-it->lastNotice>=10){it->lastNotice=now;Wh_Log(L"Start menu icons: bitmap capture/apply failed (0x%08X); native source preserved",(unsigned)error);}
            try{Restore(*it);}catch(...){}budget=Bytes(c);it->retryAt=now+2;
        }
        budget=Bytes(c);pending=pending||it->render||it->pixels;++it;
    }
    bool animated=cfg->icon.effect!=L"none"&&cfg->icon.prog.usesTime;
    UINT delay=animated?1000/(EffectSmoothing::Use(true,cfg->icon.fps)?60:std::max(1,cfg->icon.fps)):500;
    if(pending||!c.scan.empty())delay=std::min<UINT>(delay,50);
    SetTimer(c.window,kTimer,delay,nullptr);
}
static LRESULT CALLBACK WindowProc(HWND h,UINT msg,WPARAM w,LPARAM l){
    auto c=reinterpret_cast<Context*>(GetWindowLongPtrW(h,GWLP_USERDATA));
    if(!c)return DefWindowProcW(h,msg,w,l);
    if(msg==kStop){
        if(c->busy){c->stopPending=true;return 0;}
        KillTimer(h,kTimer);Reset(*c);DestroyWindow(h);return 0;
    }
    if(msg==kReset||(msg==WM_TIMER&&w==kTimer)){
        if(msg==kReset)c->notificationQueued=false;
        if(!IsWindow(c->owner)){KillTimer(h,kTimer);Reset(*c);DestroyWindow(h);return 0;}
        if(!g_stopping&&!c->busy){
            c->busy=true;try{Tick(*c);}catch(...){}c->busy=false;
            if(c->stopPending){KillTimer(h,kTimer);Reset(*c);DestroyWindow(h);}
        }return 0;
    }
    if(msg==WM_NCDESTROY){
        KillTimer(h,kTimer);Reset(*c);
        {std::lock_guard<std::mutex> lock(g_mutex);g_windows.erase(GetCurrentThreadId());}
        SetWindowLongPtrW(h,GWLP_USERDATA,0);delete c;
    }
    return DefWindowProcW(h,msg,w,l);
}
static void Attach(HWND core){
    if(g_stopping)return;
    std::lock_guard<std::mutex> lock(g_mutex);
    if(g_windows.count(GetCurrentThreadId())||g_windows.size()>=2)return;
    auto c=std::make_unique<Context>();c->owner=core;
    HWND h=CreateWindowExW(0,kClass,L"",0,0,0,0,0,HWND_MESSAGE,nullptr,g_instance,nullptr);
    if(!h)return;
    c->window=h;SetWindowLongPtrW(h,GWLP_USERDATA,(LONG_PTR)c.release());
    g_windows[GetCurrentThreadId()]=h;SetTimer(h,kTimer,50,nullptr);
}
// Public XAML diagnostics supplies visual-tree roots and realized icon elements.
// The TAP/visual-tree notification pattern is also used by m417z's GPL-3.0
// Start Menu Styler; this implementation only discovers icon hosts.
static constexpr CLSID diagnosticsClsid={0x9ef7a941,0xf247,0x4918,{0x87,0xd4,0xa2,0x51,0x62,0x11,0xd0,0x21}};
static std::mutex diagnosticsMutex;
static winrt::com_ptr<IXamlDiagnostics> diagnostics;
static BOOL CALLBACK ThreadCore(HWND h,LPARAM value){wchar_t name[80]{};GetClassNameW(h,name,ARRAYSIZE(name));if(!wcscmp(name,L"Windows.UI.Core.CoreWindow")){*reinterpret_cast<HWND*>(value)=h;return FALSE;}return TRUE;}
static void Observe(DependencyObject const& object){
    if(g_stopping||!object)return;
    HWND message=nullptr;
    {std::lock_guard<std::mutex> lock(g_mutex);auto found=g_windows.find(GetCurrentThreadId());if(found!=g_windows.end())message=found->second;}
    if(!message){HWND core=nullptr;EnumThreadWindows(GetCurrentThreadId(),ThreadCore,(LPARAM)&core);if(!core)return;Attach(core);
        std::lock_guard<std::mutex> lock(g_mutex);auto found=g_windows.find(GetCurrentThreadId());if(found!=g_windows.end())message=found->second;}
    if(!message)return;
    auto context=reinterpret_cast<Context*>(GetWindowLongPtrW(message,GWLP_USERDATA));if(!context)return;
    if(auto element=object.try_as<FrameworkElement>()){
        if(auto root=element.XamlRoot();root&&root.Content()){
            auto content=root.Content().as<DependencyObject>();bool known=false;
            for(auto const& weak:context->roots)if(weak.get()==content){known=true;break;}
            if(!known&&context->roots.size()<8){context->roots.push_back(winrt::make_weak(content));context->lastScan=-10;
                Wh_Log(L"Start menu icons: visual-tree root discovered on UI thread %lu",GetCurrentThreadId());}
        }
        // Some ADD notifications precede Name/Loaded/XamlRoot assignment. Revisit
        // weak candidates on the UI timer; never retain a virtualized template.
        if(context->candidates.size()<256)context->candidates.push_back(winrt::make_weak(element));
    }
    if(!context->notificationQueued){context->notificationQueued=true;PostMessageW(message,kReset,0,0);}
}
struct Watcher:winrt::implements<Watcher,IVisualTreeServiceCallback2>{
    winrt::com_ptr<IXamlDiagnostics> source;
    explicit Watcher(winrt::com_ptr<IXamlDiagnostics> value):source(std::move(value)){}
    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(ParentChildRelation,VisualElement element,VisualMutationType mutation) override{
        if(g_stopping||mutation!=Add)return S_OK;
        try{winrt::Windows::Foundation::IInspectable object=nullptr;
            if(SUCCEEDED(source->GetIInspectableFromHandle(element.Handle,reinterpret_cast<::IInspectable**>(winrt::put_abi(object)))))
                if(auto dependency=object.try_as<DependencyObject>())Observe(dependency);
        }catch(...){}return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle,VisualElementState,LPCWSTR) noexcept override{return S_OK;}
};
struct Tap:winrt::implements<Tap,IObjectWithSite>{
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* site) override{
        try{
            winrt::com_ptr<IUnknown> unknown;unknown.copy_from(site);
            auto value=unknown?unknown.as<IXamlDiagnostics>():nullptr;
            {std::lock_guard<std::mutex> lock(diagnosticsMutex);diagnostics=std::move(value);}
            // Balance the diagnostic injector's DLL load, retaining our explicit lease.
            if(site&&g_instance)FreeLibrary(g_instance);
            return S_OK;
        }catch(...){return winrt::to_hresult();}
    }
    HRESULT STDMETHODCALLTYPE GetSite(REFIID iid,void** result) noexcept override{
        try{
            std::lock_guard<std::mutex> lock(diagnosticsMutex);
            if(!result)return E_POINTER;
            *result=nullptr;
            if(!diagnostics)return E_FAIL;
            return diagnostics.as(iid,result);
        }
        catch(...){return winrt::to_hresult();}
    }
};
struct Factory:winrt::implements<Factory,IClassFactory>{
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* outer,REFIID iid,void** result) override{
        if(!result)return E_POINTER;*result=nullptr;if(outer)return CLASS_E_NOAGGREGATION;
        try{return winrt::make<Tap>().as(iid,result);}catch(...){return winrt::to_hresult();}
    }
    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override{return S_OK;}
};
HRESULT DiagnosticsFactory(REFCLSID clsid,REFIID iid,void** result){
    if(!result)return E_POINTER;*result=nullptr;if(clsid!=diagnosticsClsid)return CLASS_E_CLASSNOTAVAILABLE;
    try{return winrt::make<Factory>().as(iid,result);}catch(...){return winrt::to_hresult();}
}
static HRESULT ConnectDiagnostics(){
    auto xaml=GetModuleHandleW(L"Windows.UI.Xaml.dll");if(!xaml)return HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
    auto initialize=reinterpret_cast<decltype(&InitializeXamlDiagnosticsEx)>(GetProcAddress(xaml,"InitializeXamlDiagnosticsEx"));if(!initialize)return E_NOINTERFACE;
    wchar_t path[32768]{};DWORD length=GetModuleFileNameW(g_instance,path,ARRAYSIZE(path));if(!length||length>=ARRAYSIZE(path))return E_FAIL;
    HRESULT result=HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    for(int i=1;i<=256&&!g_stopping;i++){
        wchar_t connection[64]{};swprintf_s(connection,ARRAYSIZE(connection),L"VisualDiagConnection%d",i);
        result=initialize(connection,GetCurrentProcessId(),L"",path,diagnosticsClsid,nullptr);
        if(result!=HRESULT_FROM_WIN32(ERROR_NOT_FOUND))break;
    }
    return result;
}

static LRESULT CALLBACK DispatchHook(int code,WPARAM w,LPARAM l){
    if(code==HC_ACTION){auto m=reinterpret_cast<const CWPSTRUCT*>(l);
        if(m->message==g_dispatch&&!g_stopping)try{Attach(m->hwnd);}catch(...){}
    }
    return CallNextHookEx(nullptr,code,w,l);
}
static BOOL CALLBACK FindCore(HWND h,LPARAM){
    DWORD pid=0,tid=GetWindowThreadProcessId(h,&pid);
    if(pid!=GetCurrentProcessId())return TRUE;
    wchar_t name[80]{};GetClassNameW(h,name,ARRAYSIZE(name));
    if(wcscmp(name,L"Windows.UI.Core.CoreWindow"))return TRUE;
    {std::lock_guard<std::mutex> lock(g_mutex);if(g_windows.count(tid))return TRUE;}
    HHOOK hook=SetWindowsHookExW(WH_CALLWNDPROC,DispatchHook,nullptr,tid);if(!hook)return TRUE;
    DWORD_PTR result=0;
    if(!SendMessageTimeoutW(h,g_dispatch,0,0,SMTO_ABORTIFHUNG|SMTO_BLOCK,500,&result))g_retain=true;
    if(!UnhookWindowsHookEx(hook))g_retain=true;
    return TRUE;
}
static DWORD WINAPI Watch(LPVOID){
    HRESULT apartment=CoInitializeEx(nullptr,COINIT_MULTITHREADED);
    winrt::com_ptr<Watcher> watcher;winrt::com_ptr<IVisualTreeService3> service;
    bool advised=false;double retry=0;
    while(WaitForSingleObject(g_stop,500)==WAIT_TIMEOUT){
        auto cfg=std::atomic_load(&g_config);
        if(!cfg||!cfg->enabled||!cfg->icon.HasEffects()||g_stopping){
            if(advised&&service){HRESULT hr=service->UnadviseVisualTreeChange(watcher.get());if(FAILED(hr))g_retain=true;
                else{advised=false;watcher=nullptr;service=nullptr;}}
            continue;
        }
        EnumWindows(FindCore,0);
        if(!advised&&Now()>=retry){
            retry=Now()+5;
            try{
                winrt::com_ptr<IXamlDiagnostics> source;
                {std::lock_guard<std::mutex> lock(diagnosticsMutex);source=diagnostics;}
                if(!source){HRESULT hr=ConnectDiagnostics();if(FAILED(hr))Wh_Log(L"Start menu icons: visual-tree connection failed (0x%08X); native icons preserved",(unsigned)hr);
                    std::lock_guard<std::mutex> lock(diagnosticsMutex);source=diagnostics;}
                if(source){service=source.as<IVisualTreeService3>();watcher=winrt::make_self<Watcher>(source);
                    HRESULT hr=service->AdviseVisualTreeChange(watcher.get());advised=SUCCEEDED(hr);
                    Wh_Log(L"Start menu icons: visual-tree notifications %s (0x%08X)",advised?L"ready":L"unavailable",(unsigned)hr);
                    if(!advised){watcher=nullptr;service=nullptr;}}
            }catch(...){Wh_Log(L"Start menu icons: visual-tree discovery unavailable; using CoreWindow scan");}
        }
    }
    if(advised&&service){if(FAILED(service->UnadviseVisualTreeChange(watcher.get())))g_retain=true;}
    watcher=nullptr;service=nullptr;
    if(SUCCEEDED(apartment))CoUninitialize();
    return 0;
}
BOOL Initialize(){
    Load();g_stopping=false;
    g_dispatch=RegisterWindowMessageW(L"Windhawk_DodiStartIconsDispatch_v1170");if(!g_dispatch)return FALSE;
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,reinterpret_cast<LPCWSTR>(&WindowProc),&g_lease))return FALSE;
    g_instance=g_lease;
    WNDCLASSW wc{};wc.lpfnWndProc=WindowProc;wc.hInstance=g_instance;wc.lpszClassName=kClass;
    if(!RegisterClassW(&wc)){FreeLibrary(g_lease);g_lease=nullptr;return FALSE;}
    g_stop=CreateEventW(nullptr,TRUE,FALSE,nullptr);
    if(g_stop)g_worker=CreateThread(nullptr,0,Watch,nullptr,0,nullptr);
    if(!g_worker){if(g_stop)CloseHandle(g_stop);g_stop=nullptr;UnregisterClassW(kClass,g_instance);FreeLibrary(g_lease);g_lease=nullptr;return FALSE;}
    g_initialized=true;return TRUE;
}
static std::vector<HWND> Windows(){std::lock_guard<std::mutex> lock(g_mutex);std::vector<HWND> v;for(auto& p:g_windows)v.push_back(p.second);return v;}
void Reload(){if(!g_initialized)return;Load();for(HWND h:Windows())PostMessageW(h,kReset,0,0);}
void Stop(){
    if(!g_initialized)return;g_stopping=true;
    SetEvent(g_stop);
    if(WaitForSingleObject(g_worker,2000)!=WAIT_OBJECT_0){g_retain=true;Wh_Log(L"Start menu icons: visual-tree worker still stopping; retaining inert callbacks");}
    else{CloseHandle(g_worker);g_worker=nullptr;CloseHandle(g_stop);g_stop=nullptr;}
    for(HWND h:Windows()){
        DWORD_PTR result=0;if(IsWindow(h)&&!SendMessageTimeoutW(h,kStop,0,0,SMTO_ABORTIFHUNG|SMTO_BLOCK,1000,&result))g_retain=true;
    }
    if(!Windows().empty())g_retain=true;
    if(g_retain)Wh_Log(L"Start menu icon cleanup incomplete; inert callback module retained until host exits");
}
void Uninitialize(){
    if(!g_initialized)return;g_initialized=false;
    if(!g_retain){
        {std::lock_guard<std::mutex> lock(diagnosticsMutex);diagnostics=nullptr;}
        if(winrt::get_module_lock()){g_retain=true;Wh_Log(L"Start menu icons: outstanding COM objects; retaining callback module");}
    }
    if(!g_retain){UnregisterClassW(kClass,g_instance);std::atomic_store(&g_config,std::shared_ptr<const Config>{});
        if(g_lease){FreeLibrary(g_lease);g_lease=nullptr;}}
}
} // namespace StartMenuFx

BOOL Initialize() {
    if(g_desktopRetainState){
        Wh_Log(L"Previous desktop cleanup was incomplete; restart Explorer before re-enabling this version");
        return FALSE;
    }
    InitializeCriticalSection(&g_cs);
    g_desktopDispatchMsg=RegisterWindowMessageW(L"Windhawk_DodiDesktopDispatch_v131");
    g_desktopHealthMsg=RegisterWindowMessageW(L"Windhawk_DodiDesktopHealth_v131");
    if(!g_desktopDispatchMsg||!g_desktopHealthMsg){DeleteCriticalSection(&g_cs);return FALSE;}
    g_desktopStopping=false;
    LoadSettings();
    {auto cfg=GetIcon();Wh_Log(L"Dodi v1.3.3: process=%lu, desktop work=%d, image effects=%d",
                             GetCurrentProcessId(),cfg.NeedsWork(),cfg.HasEffects());}
    ExplorerIconFx::Initialize();
    if(!TaskbarFx::Initialize())Wh_Log(L"Taskbar icon integration could not initialize; other features remain available");
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original);
    // the overlay thread is started without waiting for it, so loading can never block
    g_ovStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_ovReload = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (g_ovStop && g_ovReload)
        g_ovThread = CreateThread(nullptr, 0, OverlayThread, nullptr, 0, nullptr);
    if (!g_ovThread) {
        Wh_Log(L"App border worker could not start; desktop and taskbar features remain available");
        if (g_ovStop) { CloseHandle(g_ovStop); g_ovStop = nullptr; }
        if (g_ovReload) { CloseHandle(g_ovReload); g_ovReload = nullptr; }
    }
    return TRUE;
}

// The hook is active here, so no list can slip through between the scan and the hook.
void AfterInitialize() {
    ExplorerIconFx::Scan();
    TaskbarFx::AfterInit();
    AttachExisting();
    g_attachStop=CreateEventW(nullptr,TRUE,FALSE,nullptr);
    if(g_attachStop) g_attachThread=CreateThread(nullptr,0,DesktopWatch,nullptr,0,nullptr);
    if(!g_attachThread){
        if(g_attachStop){CloseHandle(g_attachStop);g_attachStop=nullptr;}
        Wh_Log(L"Desktop recovery watcher could not start");
    }
    if (GetIcon().hideNames) RefreshDesktopLabelsAll();
}

void SettingsChanged() {
    TaskbarFx::Reload();
    bool hadNamesHidden = GetIcon().hideNames;
    LoadSettings();
    ExplorerIconFx::Reload();
    if (GetIcon().hideNames != hadNamesHidden) RefreshDesktopLabelsAll();
    ++g_desktopEpoch;
    InvalidateAllLists();
    if (g_ovReload) SetEvent(g_ovReload);
}

void RequestStop(){if(g_ovStop)SetEvent(g_ovStop);if(g_attachStop)SetEvent(g_attachStop);}
void BeforeUninitialize() {
    RequestStop();
    g_desktopStopping=true;
    ExplorerIconFx::RequestStop();
    TaskbarFx::Stop();
    if(g_attachThread) { SetEvent(g_attachStop); WaitForSingleObject(g_attachThread,INFINITE);
        CloseHandle(g_attachThread); g_attachThread=nullptr; }
    if(g_attachStop) { CloseHandle(g_attachStop); g_attachStop=nullptr; }
    ExplorerIconFx::Stop();
}
void Uninitialize() {
    TaskbarFx::Uninitialize();
    if (g_ovThread) {
        SetEvent(g_ovStop);
        WaitForSingleObject(g_ovThread, INFINITE);
        CloseHandle(g_ovThread);
        g_ovThread = nullptr;
    }
    if (g_ovStop) { CloseHandle(g_ovStop); g_ovStop = nullptr; }
    if (g_ovReload) { CloseHandle(g_ovReload); g_ovReload = nullptr; }

    bool hadNamesHidden = GetIcon().hideNames;
    std::vector<HWND> lists;
    EnterCriticalSection(&g_cs);
    lists = g_lists;
    LeaveCriticalSection(&g_cs);

    if(!DetachAll())return; // Retain live callback state and code on cleanup failure.
    if (hadNamesHidden) {   // names come back as soon as our hook is gone; make the list ask for them again
        HWND dl = FindDesktopList();
        if (dl) RefreshListLabels(dl);
    }
    for (HWND h : lists)
        if (IsWindow(h)) InvalidateRect(h, nullptr, TRUE);
    ClearCache();
    DeleteCriticalSection(&g_cs);
    if(HMODULE module=g_desktopModuleLease.exchange(nullptr))FreeLibrary(module);
}
} // namespace DodiEngine

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"
extern "C" __declspec(dllexport) HRESULT __stdcall DllGetClassObject(REFCLSID clsid,REFIID iid,void** result){return DodiEngine::StartMenuFx::DiagnosticsFactory(clsid,iid,result);}
extern "C" __declspec(dllexport) HRESULT __stdcall DllCanUnloadNow(){return winrt::get_module_lock()?S_FALSE:S_OK;}
#pragma clang diagnostic pop


namespace IntegratedFormula {
using namespace DodiEngine;
// Bounded register program for experimental fields only. No generated machine
// code, approximate math, pixel downsampling or changes to the shared parser.
constexpr int kBorderRegisters=1024;
enum {B_FINITE=100,B_COPY,B_BOOL,B_ZERO,B_JUMP};
struct BorderInstruction {int op=0,fn=0,a=-1,b=-1,c=-1,out=-1,jump=0;};
struct BorderPlan {
    bool valid=false,needsFallback=true,usesProximity=false;
    int registers=kMaxVars;
    std::vector<BorderInstruction> frame,pixel;
    std::vector<std::pair<int,float>> constants;
    std::vector<std::pair<int,int>> outputs;
};
static float BorderMath(int fn,float x,float y,float z){
    switch (fn) {
        case F_SIN: return sinf(x);
        case F_COS: return cosf(x);
        case F_TAN: return tanf(x);
        case F_ATAN: return atanf(x);
        case F_ATAN2: return atan2f(x, y);
        case F_ABS: return fabsf(x);
        case F_SQRT: return x > 0 ? sqrtf(x) : 0.f;
        case F_FLOOR: return floorf(x);
        case F_CEIL: return ceilf(x);
        case F_FRACT: return Fract(x);
        case F_ROUND: return roundf(x);
        case F_SIGN: return x > 0 ? 1.f : (x < 0 ? -1.f : 0.f);
        case F_EXP: return expf(x < -60.f ? -60.f : (x > 60.f ? 60.f : x));
        case F_LOG: return x > 0 ? logf(x) : 0.f;
        case F_POW: { float r = powf(x, y); return std::isfinite(r) ? r : 0.f; }
        case F_MIN: return x < y ? x : y;
        case F_MAX: return x > y ? x : y;
        case F_CLAMP: return x < y ? y : (x > z ? z : x);
        case F_MIX: return x + (y - x) * z;
        case F_STEP: return y < x ? 0.f : 1.f;   // step(edge, value)
        case F_SMOOTHSTEP: {
            if (y == x) return z < x ? 0.f : 1.f;
            float k = Clamp01((z - x) / (y - x));
            return k * k * (3.f - 2.f * k);
        }
        case F_MOD: return y == 0.f ? 0.f : x - y * floorf(x / y);
        case F_SAW: return Fract(x);
        case F_TRI: return 1.f - fabsf(2.f * Fract(x) - 1.f);
        case F_SQ: return Fract(x) < 0.5f ? 1.f : 0.f;
        case F_HR: return Clamp01(fabsf(6.f * Fract(x) - 3.f) - 1.f);
        case F_HG: return Clamp01(2.f - fabsf(6.f * Fract(x) - 2.f));
        case F_HB: return Clamp01(2.f - fabsf(6.f * Fract(x) - 4.f));
        case F_NOISE: return Noise1(x);
        case F_HASH: return Hash01((int)floorf(x));
        case F_HASH2: return Hash2(x, y);
        case F_NOISE2: return Noise2(x, y);
        case F_BAYER: return Bayer(x, y);
    }
    return 0.f;

}
static float BorderOperation(const BorderInstruction& n,const float* r){
    float a=n.a>=0?r[n.a]:0,b=n.b>=0?r[n.b]:0,c=n.c>=0?r[n.c]:0;
    switch(n.op){
        case B_FINITE:return std::isfinite(a)?a:0;
        case B_COPY:return a;
        case B_BOOL:return a!=0?1.f:0.f;
        case N_NEG:return -a;
        case N_NOT:return a==0?1.f:0.f;
        case N_ADD:return a+b;case N_SUB:return a-b;case N_MUL:return a*b;
        case N_DIV:return b==0?0:a/b;
        case N_MOD:return b==0?0:a-b*floorf(a/b);
        case N_POW:{float value=powf(a,b);return std::isfinite(value)?value:0;}
        case N_LT:return a<b?1.f:0.f;case N_GT:return a>b?1.f:0.f;
        case N_LE:return a<=b?1.f:0.f;case N_GE:return a>=b?1.f:0.f;
        case N_EQ:return a==b?1.f:0.f;case N_NE:return a!=b?1.f:0.f;
        case N_CALL:return BorderMath(n.fn,a,b,c);
    }
    return 0;
}
static void BorderExecute(const std::vector<BorderInstruction>& ops,float* r){
    for(size_t pc=0;pc<ops.size();){
        const auto& n=ops[pc];
        if(n.op==B_ZERO){pc=r[n.a]==0?(size_t)n.jump:pc+1;continue;}
        if(n.op==B_JUMP){pc=(size_t)n.jump;continue;}
        r[n.out]=BorderOperation(n,r);pc++;
    }
}
struct BorderCompiler {
    const Program& source;BorderPlan plan;
    int variables[kMaxVars];bool varying[kBorderRegisters]{};
    int branchDepth=0;bool field=false,readFallback=false,assigned[4]{},failed=false;
    std::map<std::tuple<int,int,int,int,int>,int> common;
    std::map<unsigned,int> literal;
    explicit BorderCompiler(const Program& p,bool isField):source(p),field(isField){
        for(int i=0;i<kMaxVars;i++){variables[i]=i;varying[i]=!field;}
        if(field){
            const int spatial[]={V_X,V_Y,V_U,V_V,V_P,V_D,V_ANG,V_AB,V_R,V_G,V_B,V_A,V_FIRST_TEMP+8};
            for(int i:spatial)
                varying[i]=true;
        }
    }
    int Register(){
        if(plan.registers>=kBorderRegisters){failed=true;return 0;}
        return plan.registers++;
    }
    int Constant(float value){
        unsigned bits;static_assert(sizeof(bits)==sizeof(value));memcpy(&bits,&value,sizeof(bits));
        auto found=literal.find(bits);if(found!=literal.end())return found->second;
        int out=Register();plan.constants.emplace_back(out,value);literal[bits]=out;return out;
    }
    int Emit(int op,int fn,int a,int b=-1,int c=-1){
        auto key=std::make_tuple(op,fn,a,b,c);
        auto found=common.find(key);if(found!=common.end())return found->second;
        int out=Register();
        varying[out]=(a>=0&&varying[a])||(b>=0&&varying[b])||(c>=0&&varying[c]);
        BorderInstruction n{op,fn,a,b,c,out,0};
        if(field&&!branchDepth&&!varying[out])plan.frame.push_back(n);
        else plan.pixel.push_back(n);
        common[key]=out;return out;
    }
    int Branch(int condition,int yes,int no,bool logical=false){
        // CSE never crosses a conditional block: no speculative evaluations
        // and no register can be reused from a branch that did not execute.
        int test=Expression(condition);common.clear();
        size_t jump=plan.pixel.size();plan.pixel.push_back({B_ZERO,0,test,-1,-1,-1,0});
        int out=Register();varying[out]=true;branchDepth++;
        int a=Expression(yes);if(logical)a=Emit(B_BOOL,0,a);
        plan.pixel.push_back({B_COPY,0,a,-1,-1,out,0});common.clear();
        size_t done=plan.pixel.size();plan.pixel.push_back({B_JUMP,0,-1,-1,-1,-1,0});
        plan.pixel[jump].jump=(int)plan.pixel.size();
        int b=Expression(no);if(logical)b=Emit(B_BOOL,0,b);
        plan.pixel.push_back({B_COPY,0,b,-1,-1,out,0});common.clear();
        branchDepth--;plan.pixel[done].jump=(int)plan.pixel.size();return out;
    }
    int Expression(int index){
        const Node& n=source.nodes[index];
        if(n.op==N_NUM)return Constant(n.num);
        if(n.op==N_VAR){
            if(n.var==V_AB||n.var==V_FIRST_TEMP+8)plan.usesProximity=true;
            int reg=variables[n.var];
            if(n.var>=V_R&&n.var<=V_A&&reg==n.var)readFallback=true;
            return reg;
        }
        if(n.op==N_CALL&&n.fn==F_IF)return Branch(n.a,n.b,n.c);
        if(n.op==N_AND||n.op==N_OR){
            int test=Expression(n.a);common.clear();
            size_t jump=plan.pixel.size();plan.pixel.push_back({B_ZERO,0,test,-1,-1,-1,0});
            int out=Register();varying[out]=true;branchDepth++;
            int yes=n.op==N_OR?Constant(1):Emit(B_BOOL,0,Expression(n.b));
            plan.pixel.push_back({B_COPY,0,yes,-1,-1,out,0});common.clear();
            size_t done=plan.pixel.size();plan.pixel.push_back({B_JUMP,0,-1,-1,-1,-1,0});
            plan.pixel[jump].jump=(int)plan.pixel.size();
            int no=n.op==N_AND?Constant(0):Emit(B_BOOL,0,Expression(n.b));
            plan.pixel.push_back({B_COPY,0,no,-1,-1,out,0});common.clear();
            branchDepth--;plan.pixel[done].jump=(int)plan.pixel.size();return out;
        }
        int a=n.a>=0?Expression(n.a):-1;
        int b=n.b>=0?Expression(n.b):-1;
        int c=n.c>=0?Expression(n.c):-1;
        return Emit(n.op,n.fn,a,b,c);
    }
    BorderPlan Build(){
        bool written[kMaxVars]{};
        for(const auto& st:source.stmts){
            int value=Emit(B_FINITE,0,Expression(st.root));
            variables[st.var]=value;written[st.var]=true;
            if(st.var>=V_R&&st.var<=V_A)assigned[st.var-V_R]=true;
        }
        for(int i=0;i<kMaxVars;i++)if(written[i])plan.outputs.emplace_back(i,variables[i]);
        plan.needsFallback=readFallback||!assigned[0]||!assigned[1]||!assigned[2]||!assigned[3];
        plan.valid=!failed&&plan.pixel.size()+plan.frame.size()<=768;
        return std::move(plan);
    }
};
static BorderPlan CompileBorderPlan(const Program& source,bool field)try{
    return BorderCompiler(source,field).Build();
}catch(const std::bad_alloc&){return {};}
struct PreparedBorder {
    const void* owner=nullptr;
    unsigned token=0;
    bool uniform=false;
    ExperimentalBorders::Color color{};
    float registers[kBorderRegisters];
};
static thread_local PreparedBorder preparedBorder;
static void BorderInit(const BorderPlan& plan,const float* v,float* r){
    memcpy(r,v,kMaxVars*sizeof(float));
    for(const auto& value:plan.constants)r[value.first]=value.second;
}
static void BorderRun(const BorderPlan& plan,float* v,const PreparedBorder* prepared=nullptr){
    float r[kBorderRegisters];BorderInit(plan,v,r);
    if(prepared)for(const auto& op:plan.frame)r[op.out]=prepared->registers[op.out];
    else BorderExecute(plan.frame,r);
    BorderExecute(plan.pixel,r);
    for(const auto& out:plan.outputs)v[out.first]=r[out.second];
}

struct ClassicFrame {const BorderPlan* plan=nullptr;PreparedBorder prepared;};
template<class Source> static const void* PrepareClassic(const Source& source,const float* values){
    if(!PerformanceOptions::enabled.load()||!PerformanceOptions::fastClassic.load())return nullptr;
    // Defensive fallback for externally assembled programs that assign inputs.
    // The public classic parser already rejects these assignments.
    for(const auto& st:source.stmts)if(st.var>V_ANG&&st.var<V_FIRST_TEMP&&!(st.var>=V_R&&st.var<=V_A))return nullptr;
    if(!source.classicPlan)try{
        Program copy;copy.nodes.reserve(source.nodes.size());copy.stmts.reserve(source.stmts.size());
        for(const auto& n:source.nodes){Node out;out.op=n.op;out.num=n.num;out.var=n.var;out.fn=n.fn;out.a=n.a;out.b=n.b;out.c=n.c;copy.nodes.push_back(out);}
        for(const auto& st:source.stmts)copy.stmts.push_back({st.var,st.root});
        BorderCompiler compiler(copy,true);
        for(int var=V_AR;var<=V_KB;var++)compiler.varying[var]=true;
        source.classicPlan=std::make_shared<BorderPlan>(compiler.Build());
    }catch(const std::bad_alloc&){return nullptr;}
    auto* plan=static_cast<const BorderPlan*>(source.classicPlan.get());if(!plan||!plan->valid)return nullptr;
    static thread_local ClassicFrame frame;frame.plan=plan;
    BorderInit(*plan,values,frame.prepared.registers);BorderExecute(plan->frame,frame.prepared.registers);
    return &frame;
}
static void RunClassic(const void* value,float* values){const auto& frame=*static_cast<const ClassicFrame*>(value);BorderRun(*frame.plan,values,&frame.prepared);}
struct Compiled {Program program;bool valid=false;BorderPlan border;};
static std::shared_ptr<Compiled> Build(const std::wstring& source,PCWSTR label) {
    auto result=std::make_shared<Compiled>();std::wstring error;
    result->valid=CompileProgram(source,&result->program,&error);
    // Smaller evaluation budget for the new high-density cursor/geometry paths.
    if(result->valid&&result->program.nodes.size()>256){result->valid=false;error=L"Maximum 256 nodes in cursor/experimental formulas";}
    if(!result->valid)Wh_Log(L"%s custom formula rejected: %s",label,error.c_str());
    return result;
}
static void Run(const Compiled& code,const ExperimentalBorders::Inputs& in,
                ExperimentalBorders::Color& color,const float* c1,const float* c2) {
    if(!code.valid)return;
    float v[kMaxVars]{};
    v[V_X]=in.x;v[V_Y]=in.y;v[V_U]=in.u;v[V_V]=in.v;v[V_P]=in.p;v[V_D]=in.d;
    v[V_T]=in.t;v[V_W]=in.w;v[V_H]=in.h;v[V_PI]=3.14159265f;v[V_TAU]=6.2831853f;
    // New effect families repurpose ambient inputs as pointer/instance inputs.
    v[V_AR]=in.mx;v[V_AG]=in.my;v[V_AB]=in.proximity;
    v[V_SPEED]=v[V_POWER]=v[V_N]=v[V_OP]=1;
    v[V_C1R]=c1[0];v[V_C1G]=c1[1];v[V_C1B]=c1[2];
    v[V_C2R]=c2[0];v[V_C2G]=c2[1];v[V_C2B]=c2[2];
    v[V_R]=color.r;v[V_G]=color.g;v[V_B]=color.b;v[V_A]=color.a;
    for(const auto& st:code.program.stmts){float value=Eval(code.program,st.root,v);v[st.var]=std::isfinite(value)?value:0;}
    color={Clamp01(v[V_R]),Clamp01(v[V_G]),Clamp01(v[V_B]),Clamp01(v[V_A])};
}
// Scoped bindings: the shared desktop/taskbar/cursor parser remains unchanged.
enum class BorderKind {Field,Layout,Particle};
static const wchar_t* borderCommon[]={L"reach",L"radius",L"perimeter",L"clock",
    L"enter",L"inside",L"mx",L"my",L"prox"};
static std::shared_ptr<Compiled> BuildBorder(const std::wstring& source,PCWSTR label,BorderKind kind){
    auto result=std::make_shared<Compiled>();Parser parser;std::wstring error;
    parser.s=source.c_str();parser.P=&result->program;
    for(auto name:borderCommon)parser.temps.emplace_back(name);
    if(kind==BorderKind::Layout){parser.temps.emplace_back(L"count");parser.temps.emplace_back(L"body");}
    if(kind==BorderKind::Particle)
        for(auto name:{L"id",L"seed",L"home",L"count",L"pos",L"depth",L"size",L"stretch",L"rotation",L"offsetx",L"offsety",L"nx",L"ny"})
            parser.temps.emplace_back(name);
    if(source.size()>16384)error=L"Maximum 16384 characters";
    else if(!parser.ParseProgram())error=parser.err;
    else if(result->program.nodes.size()>256)error=L"Maximum 256 nodes in experimental formulas";
    else {
        result->valid=true;result->program.tempCount=(int)parser.temps.size();
        if(kind==BorderKind::Particle)for(const auto& statement:result->program.stmts)
            if((statement.var>=V_FIRST_TEMP+9&&statement.var<V_FIRST_TEMP+13)||
               (statement.var>=V_FIRST_TEMP+20&&statement.var<V_FIRST_TEMP+22)){
                result->valid=false;error=L"id, seed, home, count, nx and ny are read-only particle inputs";break;
            }
    }
    if(!result->valid)Wh_Log(L"%s custom formula rejected: %s",label,error.c_str());
    if(result->valid)result->border=CompileBorderPlan(result->program,kind==BorderKind::Field);
    return result;
}
static void BorderValues(const ExperimentalBorders::Inputs& in,const float* c1,const float* c2,
                         const ExperimentalBorders::Color& color,float* v){
    v[V_X]=in.x;v[V_Y]=in.y;v[V_U]=in.u;v[V_V]=in.v;v[V_P]=in.p;v[V_D]=in.d;
    v[V_T]=in.t;v[V_W]=in.w;v[V_H]=in.h;v[V_PI]=3.14159265f;v[V_TAU]=6.2831853f;
    v[V_ANG]=in.angle;
    v[V_AR]=in.mx;v[V_AG]=in.my;v[V_AB]=in.proximity;
    v[V_SPEED]=v[V_POWER]=v[V_N]=v[V_OP]=1;
    v[V_C1R]=c1[0];v[V_C1G]=c1[1];v[V_C1B]=c1[2];
    v[V_C2R]=c2[0];v[V_C2G]=c2[1];v[V_C2B]=c2[2];
    v[V_R]=color.r;v[V_G]=color.g;v[V_B]=color.b;v[V_A]=color.a;
    const float common[]={in.reach,in.radius,in.perimeter,in.clock,in.entered,in.inside,in.mx,in.my,in.proximity};
    for(int k=0;k<9;k++)v[V_FIRST_TEMP+k]=common[k];
}
static void EvaluateBorder(const Compiled& code,float* v,const PreparedBorder* prepared=nullptr){
    if(PerformanceOptions::enabled.load()&&PerformanceOptions::fastFormulas.load()&&code.border.valid){
        BorderRun(code.border,v,prepared);return;
    }
    for(const auto& st:code.program.stmts){
        float value=Eval(code.program,st.root,v);
        v[st.var]=std::isfinite(value)?value:0;
    }
}

}
namespace ExperimentalBorders {
void Compile(Config& c){
    c.serial=configSerial.fetch_add(1);
    c.program=IntegratedFormula::BuildBorder(c.code,L"Experimental field",IntegratedFormula::BorderKind::Field);
    if(!c.layoutCode.empty())
        c.layoutProgram=IntegratedFormula::BuildBorder(c.layoutCode,L"Experimental layout",IntegratedFormula::BorderKind::Layout);
    if(!c.particleCode.empty())
        c.particleProgram=IntegratedFormula::BuildBorder(c.particleCode,L"Experimental particles",IntegratedFormula::BorderKind::Particle);
}
unsigned PrepareCustomFrame(const Config& c,const Inputs& in){
    auto* code=static_cast<const IntegratedFormula::Compiled*>(c.program.get());
    auto& prepared=IntegratedFormula::preparedBorder;
    prepared.owner=nullptr;prepared.uniform=false;
    if(!PerformanceOptions::enabled.load()||!PerformanceOptions::fastFormulas.load()||
       !code||!code->valid||!code->border.valid||code->border.frame.empty())return 0;
    float v[DodiEngine::kMaxVars]{};
    IntegratedFormula::BorderValues(in,c.c1,c.c2,{0,0,0,0},v);
    IntegratedFormula::BorderInit(code->border,v,prepared.registers);
    IntegratedFormula::BorderExecute(code->border.frame,prepared.registers);
    prepared.owner=code;if(++prepared.token==0)++prepared.token;
    if(code->border.pixel.empty()&&!code->border.needsFallback){
        for(const auto& out:code->border.outputs)v[out.first]=prepared.registers[out.second];
        prepared.color={Unit(v[DodiEngine::V_R]),Unit(v[DodiEngine::V_G]),Unit(v[DodiEngine::V_B]),Unit(v[DodiEngine::V_A])};
        prepared.uniform=true;
    }
    return prepared.token;
}
bool CustomFrameCurrent(const Config& c,unsigned token){
    const auto& frame=IntegratedFormula::preparedBorder;
    return token&&token==frame.token&&frame.owner==c.program.get();
}
bool CustomNeedsProximity(const Config& c){
    auto* code=static_cast<const IntegratedFormula::Compiled*>(c.program.get());
    return !PerformanceOptions::enabled.load()||!PerformanceOptions::fastFormulas.load()||
        !code||!code->valid||!code->border.valid||code->border.usesProximity;
}
bool CustomReplacesFallback(const Config& c){
    auto* code=static_cast<const IntegratedFormula::Compiled*>(c.program.get());
    return PerformanceOptions::enabled.load()&&PerformanceOptions::fastFormulas.load()&&
        code&&code->valid&&code->border.valid&&!code->border.needsFallback;
}
void Custom(const Config& c,const Inputs& in,Color& color){
    // Config/program lifetime is owned by this render worker; no per-pixel
    // shared_ptr reference-count changes are needed.
    auto* code=static_cast<const IntegratedFormula::Compiled*>(c.program.get());
    if(!code||!code->valid)return;
    const auto& frame=IntegratedFormula::preparedBorder;
    const auto* prepared=in.frameToken&&in.frameToken==frame.token&&frame.owner==code?&frame:nullptr;
    if(prepared&&prepared->uniform&&PerformanceOptions::enabled.load()&&PerformanceOptions::fastFormulas.load()){
        color=prepared->color;return;
    }
    float v[DodiEngine::kMaxVars]{};
    IntegratedFormula::BorderValues(in,c.c1,c.c2,color,v);
    IntegratedFormula::EvaluateBorder(*code,v,prepared);
    color={Unit(v[DodiEngine::V_R]),Unit(v[DodiEngine::V_G]),Unit(v[DodiEngine::V_B]),Unit(v[DodiEngine::V_A])};
}
bool PrepareGpuField(const Config& c,bool screen,size_t pixels){
    if(!GpuFormula::enabled.load()||!GpuFormula::fields.load())return false;
    auto* code=static_cast<const IntegratedFormula::Compiled*>(c.program.get());
    if(!code||!code->valid)return false;
    // Uniform fields already run once per frame through the existing optimizer.
    if(PerformanceOptions::enabled.load()&&PerformanceOptions::fastFormulas.load()&&code->border.valid&&code->border.pixel.empty())return false;
    return GpuFormula::PrepareField(code->program,screen,pixels)!=nullptr;
}
void RunGpuField(const Config& c,const Inputs& frame,std::vector<PendingFieldPixel>& work,bool screen){
    const float* result=nullptr;
    auto* code=static_cast<const IntegratedFormula::Compiled*>(c.program.get());
    if(!RenderShutdown::requested.load()&&code&&code->valid&&GpuFormula::enabled.load()&&GpuFormula::fields.load()&&
       work.size()>=(size_t)GpuFormula::minimumPixels.load()&&
       GpuFormula::g_failedEpoch!=GpuFormula::epoch.load()&&(screen?GpuFormula::screens.load():GpuFormula::apps.load())){
        try{
            auto plan=static_cast<const GpuFormula::Plan*>(code->program.gpuPlan.get());
            if(plan&&plan->valid&&GpuFormula::g_engine){
                float values[DodiEngine::kMaxVars]{};
                IntegratedFormula::BorderValues(frame,c.c1,c.c2,{0,0,0,0},values);
                result=GpuFormula::g_engine->Evaluate(*plan,work,values,nullptr,nullptr,true);
            }
        }catch(...){GpuFormula::g_failedEpoch=GpuFormula::epoch.load();/* Keep work alive for CPU fallback. */}
    }
    bool gpu=result!=nullptr;
    static thread_local unsigned loggedEpoch=0;
    if(gpu&&loggedEpoch!=GpuFormula::epoch.load()){loggedEpoch=GpuFormula::epoch.load();Wh_Log(L"Direct3D 11: first experimental custom field evaluated on GPU (%zu pixels)",work.size());}
    for(size_t i=0;i<work.size();i++){
        if((i&511)==0&&RenderShutdown::requested.load())return;
        auto& point=work[i].sample;
        if(gpu){point.color={Unit(result[i*4]),Unit(result[i*4+1]),Unit(result[i*4+2]),Unit(result[i*4+3])};}
        else{
            Inputs pixel=frame;pixel.x=point.x;pixel.y=point.y;pixel.u=point.u;pixel.v=point.v;
            pixel.p=point.p;pixel.d=point.d;pixel.angle=point.angle;pixel.proximity=point.proximity;
            Custom(c,pixel,point.color);
        }
    }
}
Layout CustomLayout(const Config& c,const Inputs& in){
    Layout out;
    auto code=std::static_pointer_cast<IntegratedFormula::Compiled>(c.layoutProgram);
    if(!code||!code->valid)return out;
    float v[DodiEngine::kMaxVars]{};
    IntegratedFormula::BorderValues(in,c.c1,c.c2,{0,0,0,1},v);
    v[DodiEngine::V_FIRST_TEMP+9]=0;v[DodiEngine::V_FIRST_TEMP+10]=1;
    IntegratedFormula::EvaluateBorder(*code,v);
    out.count=(int)std::clamp(v[DodiEngine::V_FIRST_TEMP+9],0.f,128.f);
    out.body=Unit(v[DodiEngine::V_FIRST_TEMP+10]);return out;
}
bool CustomParticle(const Config& c,const Inputs& in,int id,int count,ParticleSpec& out){
    using namespace DodiEngine;
    if(count<1||id<0||id>=count)return false;
    auto code=std::static_pointer_cast<IntegratedFormula::Compiled>(c.particleProgram);
    if(code&&!code->valid)return false;
    float home=(float)id/count;
    out.pos=home-in.clock*75/std::max(1.f,in.perimeter);
    out.color={c.c2[0],c.c2[1],c.c2[2],1};
    float v[kMaxVars]{};
    IntegratedFormula::BorderValues(in,c.c1,c.c2,out.color,v);
    const int first=V_FIRST_TEMP+9;
    v[first]=float(id);v[first+1]=Hash01(id+12345);v[first+2]=home;v[first+3]=float(count);
    v[first+4]=out.pos;v[first+5]=out.depth;v[first+6]=out.size;v[first+7]=out.stretch;
    v[first+8]=out.rotation;v[first+9]=out.offsetx;v[first+10]=out.offsety;
    v[first+11]=in.nx;v[first+12]=in.ny;
    if(code)IntegratedFormula::EvaluateBorder(*code,v);
    out.pos=v[first+4]-floorf(v[first+4]);
    out.depth=std::clamp(v[first+5],-.9f,.9f);
    out.size=std::clamp(v[first+6],.5f,24.f);
    out.stretch=std::clamp(v[first+7],.25f,4.f);
    out.rotation=v[first+8]-floorf(v[first+8]);
    out.offsetx=std::clamp(v[first+9],-96.f,96.f);out.offsety=std::clamp(v[first+10],-96.f,96.f);
    out.color={Unit(v[V_R]),Unit(v[V_G]),Unit(v[V_B]),Unit(v[V_A])};return true;
}
}


namespace DodiEngine {
static const void* PrepareClassicBorder(const Program& p,const float* v){return IntegratedFormula::PrepareClassic(p,v);}
static void RunClassicBorder(const void* p,float* v){IntegratedFormula::RunClassic(p,v);}
}
namespace CursorEffectsEngine {
static int CursorInt(PCWSTR key){return Wh_GetIntSetting((std::wstring(L"CursorEffects.")+key).c_str());}
static PCWSTR CursorString(PCWSTR key){return Wh_GetStringSetting((std::wstring(L"CursorEffects.")+key).c_str());}
namespace DodiFrameSmoothing {
struct Bounds { int left=0,top=0,right=0,bottom=0; };

class FrameSmoother {
    struct Frame {
        std::vector<uint32_t> bgra;
        int width=0,height=0,left=0,top=0;
    };
    Frame previous_, current_;
    double receivedAt_=0.0, interval_=1.0/30.0;
    int lastPresentedMix_=-1;
    bool ready_=false, changed_=false;
    bool previousHasContent_=false, currentHasContent_=false;

    static uint32_t At(const Frame& f,int x,int y) {
        x-=f.left; y-=f.top;
        if((unsigned)x>=(unsigned)f.width || (unsigned)y>=(unsigned)f.height)return 0;
        return f.bgra[(size_t)y*f.width+x];
    }
    static uint32_t Mix(uint32_t a,uint32_t b,int t) {
        if(t==0)return a;
        if(t==255)return b;
        const unsigned u=(unsigned)(255-t),v=(unsigned)t;
        uint32_t result=0;
        for(unsigned shift=0;shift<32;shift+=8) {
            unsigned c=(((a>>shift)&255u)*u+((b>>shift)&255u)*v+127u)/255u;
            result|=c<<shift;
        }
        return result;
    }
    static void Over(uint32_t& destination,uint32_t source) {
        unsigned sa=source>>24;
        if(!sa)return;
        if(sa==255){destination=source;return;}
        const unsigned inv=255-sa;
        uint32_t result=0;
        for(unsigned shift=0;shift<32;shift+=8) {
            unsigned s=(source>>shift)&255u;
            unsigned d=(destination>>shift)&255u;
            unsigned c=std::min(255u,s+(d*inv+127u)/255u);
            result|=c<<shift;
        }
        destination=result;
    }
    int MixAt(double now,bool enableSmoothing,bool bypass) const {
        if(!enableSmoothing||bypass||!changed_)return 255;
        double t=(now-receivedAt_)/interval_;
        t=std::max(0.0,std::min(1.0,t));
        return (int)std::lround(t*255.0);
    }
public:
    void Reset(){previous_=Frame{};current_=Frame{};receivedAt_=0;
                 lastPresentedMix_=-1;ready_=changed_=false;
                 previousHasContent_=currentHasContent_=false;}
    bool Ready() const{return ready_;}
    bool CurrentGeometry(int& left,int& top,int& width,int& height) const {
        if(!ready_)return false;
        left=current_.left;top=current_.top;width=current_.width;height=current_.height;
        return true;
    }
    // Re-anchor a cached cursor-local layer to the *live* pointer hotspot.
    // Move BOTH interpolation frames together. This is position only: do not
    // regenerate effect pixels, restart fades, or advance animation clocks.
    // Trails and particles deliberately do not call this method because their
    // positions are world-space history rather than cursor-local geometry.
    bool AnchorAt(int left,int top) {
        if(!ready_ || (current_.left==left && current_.top==top))return false;
        current_.left=left;current_.top=top;
        previous_.left=left;previous_.top=top;
        lastPresentedMix_=-1; // screen geometry has changed despite same pixels
        return true;
    }
    bool ContentVisible(double now,bool smoothing,bool bypass) const {
        if(!ready_)return false;
        return currentHasContent_ ||
               (previousHasContent_&&MixAt(now,smoothing,bypass)<255);
    }
    // Returns false for an identical frame; unchanged frames don't restart fades.
    // A change of buffer dimensions resets the interpolation history.
    bool PushFrame(const uint32_t* input,int inputStride,int width,int height,
                   int left,int top,double now,double sourceInterval,bool keepHistory=true){
        if(!input||width<1||height<1||inputStride<width||width>4096||height>4096)return false;
        const size_t count=(size_t)width*height;
        const bool sizeChanged=!ready_||current_.width!=width||current_.height!=height||
            (keepHistory&&previous_.bgra.size()!=count);
        if(!sizeChanged&&current_.left==left&&current_.top==top){
            bool equal=true;
            for(int y=0;y<height;y++)if(std::memcmp(
                current_.bgra.data()+(size_t)y*width,
                input+(size_t)y*inputStride,(size_t)width*4)!=0){equal=false;break;}
            if(equal){
                if(!keepHistory){previous_=Frame{};previousHasContent_=false;changed_=false;}
                return false;
            }
        }
        if(!keepHistory){
            current_.bgra.resize(count);current_.width=width;current_.height=height;
            current_.left=left;current_.top=top;
            for(int y=0;y<height;y++)std::memcpy(current_.bgra.data()+(size_t)y*width,input+(size_t)y*inputStride,(size_t)width*4);
            previous_=Frame{};previousHasContent_=false;currentHasContent_=false;
            for(uint32_t pixel:current_.bgra)if(pixel>>24){currentHasContent_=true;break;}
            receivedAt_=now;interval_=std::max(.001,sourceInterval);
            ready_=true;changed_=false;lastPresentedMix_=-1;return true;
        }
        const bool oldContent=currentHasContent_;
        if(sizeChanged){
            previous_.bgra.resize(count); current_.bgra.resize(count);
        } else {
            previous_.bgra.swap(current_.bgra); // reuse both allocations
            previous_.left=current_.left;previous_.top=current_.top;
            previous_.width=width;previous_.height=height;
        }
        current_.width=width;current_.height=height;
        current_.left=left;current_.top=top;
        for(int y=0;y<height;y++)std::memcpy(
            current_.bgra.data()+(size_t)y*width,
            input+(size_t)y*inputStride,(size_t)width*4);
        currentHasContent_=false;
        for(uint32_t pixel:current_.bgra)if((pixel>>24)!=0){currentHasContent_=true;break;}
        previousHasContent_=sizeChanged?currentHasContent_:oldContent;
        if(sizeChanged){
            previous_.bgra=current_.bgra;
            previous_.width=width;previous_.height=height;
            previous_.left=left;previous_.top=top;
        }
        receivedAt_=now;
        interval_=std::max(0.001,sourceInterval);
        ready_=true;changed_=!sizeChanged;
        lastPresentedMix_=-1;
        return true;
    }
    bool NeedsPresentation(double now,bool smoothing,bool bypass) const {
        return ready_&&MixAt(now,smoothing,bypass)!=lastPresentedMix_;
    }
    bool IsTransitioning(double now,bool smoothing,bool bypass) const {
        return ready_&&MixAt(now,smoothing,bypass)<255;
    }
    Bounds VisibleBounds(double now,bool smoothing,bool bypass) const {
        if(!ready_)return {};
        Bounds r{current_.left,current_.top,current_.left+current_.width,
                 current_.top+current_.height};
        if(MixAt(now,smoothing,bypass)<255){
            r.left=std::min(r.left,previous_.left);
            r.top=std::min(r.top,previous_.top);
            r.right=std::max(r.right,previous_.left+previous_.width);
            r.bottom=std::max(r.bottom,previous_.top+previous_.height);
        }
        return r;
    }
    void Present(uint32_t* output,int outputStride,int width,int height,
                 int outputLeft,int outputTop,double now,bool smoothing,bool bypass,
                 GpuStages::Category category=GpuStages::CategoryCount){
        if(!ready_||!output||outputStride<width||width<1||height<1)return;
        const int mix=MixAt(now,smoothing,bypass);
        const Bounds r=VisibleBounds(now,smoothing,bypass);
        const int x0=std::max(r.left,outputLeft),x1=std::min(r.right,outputLeft+width);
        const int y0=std::max(r.top,outputTop),y1=std::min(r.bottom,outputTop+height);
        if(x1>x0&&y1>y0){
            const size_t count=(size_t)(x1-x0)*(y1-y0);
            float values[64]={};values[0]=(float)mix;
            if(GpuStages::Compute(category,count,values,[](){return &GpuStages::CompositionPlan();},
                [&](std::vector<GpuFormula::Geometry>& work){
                    size_t index=0;for(int y=y0;y<y1;y++)for(int x=x0;x<x1;x++){
                        work[index++]=GpuStages::CompositionPixels(mix==255?0:At(previous_,x,y),At(current_,x,y),
                            output[(size_t)(y-outputTop)*outputStride+(x-outputLeft)]);
                    }
                },[&](const float* result){
                    size_t index=0;for(int y=y0;y<y1;y++)for(int x=x0;x<x1;x++){
                        output[(size_t)(y-outputTop)*outputStride+(x-outputLeft)]=GpuStages::Pack(result+index*4);++index;
                    }
                }))return;
        }
        for(int y=y0;y<y1;y++)for(int x=x0;x<x1;x++){
            const uint32_t nowPixel=At(current_,x,y);
            const uint32_t pixel=mix==255?nowPixel:Mix(At(previous_,x,y),nowPixel,mix);
            Over(output[(size_t)(y-outputTop)*outputStride+(x-outputLeft)],pixel);
        }
    }
    void MarkPresented(double now,bool smoothing,bool bypass){
        if(ready_)lastPresentedMix_=MixAt(now,smoothing,bypass);
    }
};
} // namespace DodiFrameSmoothing

namespace DodiFluid {
struct Vec2 { float x=0, y=0; };
struct Color { float r=0, g=0, b=0, a=0; };
struct Cell { float vx=0,vy=0,r=0,g=0,b=0,a=0; };
struct Rect { int left=0,top=0,right=0,bottom=0; bool empty=true; };
class FluidGrid {
    int n_=0;
    float cellSize_=1, originX_=0, originY_=0;
    std::vector<Cell> grid_, tmp_;
    std::vector<float> div_, pressure_, work_, curl_;
    float peak_=0;
    bool wallsEnabled_=false,wallsContain_=false;
    Rect wall_{};
    static float Clamp(float v,float lo,float hi){return std::max(lo,std::min(hi,v));}
    int Idx(int x,int y) const{return y*n_+x;}
    // Edge-clamped sample. Velocities are in grid cells / second.
    Cell Sample(const std::vector<Cell>& v,float x,float y) const {
        x=Clamp(x,0.f,(float)n_-1.001f); y=Clamp(y,0.f,(float)n_-1.001f);
        int ix=(int)x,iy=(int)y;int ix1=std::min(n_-1,ix+1),iy1=std::min(n_-1,iy+1);
        float fx=x-ix,fy=y-iy;
        const Cell &a=v[Idx(ix,iy)],&b=v[Idx(ix1,iy)],
                   &c=v[Idx(ix,iy1)],&d=v[Idx(ix1,iy1)];
        float w0=(1-fx)*(1-fy),w1=fx*(1-fy),w2=(1-fx)*fy,w3=fx*fy;
        return {a.vx*w0+b.vx*w1+c.vx*w2+d.vx*w3,
                a.vy*w0+b.vy*w1+c.vy*w2+d.vy*w3,
                a.r*w0+b.r*w1+c.r*w2+d.r*w3,
                a.g*w0+b.g*w1+c.g*w2+d.g*w3,
                a.b*w0+b.b*w1+c.b*w2+d.b*w3,
                a.a*w0+b.a*w1+c.a*w2+d.a*w3};
    }
    float Scalar(const std::vector<float>& v,int x,int y) const{
        return v[Idx(std::max(0,std::min(n_-1,x)),std::max(0,std::min(n_-1,y)))];
    }
    bool Solid(int x,int y) const {
        if(!wallsEnabled_)return false;
        float wx=originX_+(x+0.5f)*cellSize_;
        float wy=originY_+(y+0.5f)*cellSize_;
        bool inside=wx>=wall_.left&&wx<wall_.right&&
                    wy>=wall_.top&&wy<wall_.bottom;
        return wallsContain_?!inside:inside;
    }
    void EnforceWalls(){
        if(!wallsEnabled_)return;
        // Solid cells lose dye and velocity, their immediate fluid-side
        // neighbours have zero velocity normal to the barrier.
        for(int y=0;y<n_;y++)for(int x=0;x<n_;x++){
            auto& c=grid_[Idx(x,y)];
            if(Solid(x,y)){c=Cell{};continue;}
            if((x>0&&Solid(x-1,y))||(x+1<n_&&Solid(x+1,y)))c.vx=0;
            if((y>0&&Solid(x,y-1))||(y+1<n_&&Solid(x,y+1)))c.vy=0;
        }
    }
public:
    bool ConfigureWall(bool enabled,bool contain,const Rect& box){
        if(wallsEnabled_==enabled && (!enabled ||
           (wallsContain_==contain && wall_.left==box.left &&
            wall_.top==box.top && wall_.right==box.right &&
            wall_.bottom==box.bottom)))return false;
        wallsEnabled_=enabled;wallsContain_=contain;wall_=box;
        if(Ready())EnforceWalls();
        return true;
    }
    void Reset(Vec2 center,int resolution,float radius){
        n_=std::max(32,std::min(192,resolution));
        radius=std::max(40.f,std::min(4096.f,radius));
        cellSize_=2*radius/n_;originX_=center.x-radius;originY_=center.y-radius;
        size_t count=(size_t)n_*n_;
        grid_.assign(count,{});tmp_.assign(count,{});
        div_.assign(count,0);pressure_.assign(count,0);
        work_.assign(count,0);curl_.assign(count,0);peak_=0;
        EnforceWalls();
    }
    bool Ready() const{return n_>0;}
    int Resolution() const{return n_;}
    float CellPixels() const{return cellSize_;}
    float OriginX() const{return originX_;}
    float OriginY() const{return originY_;}
    float Peak() const{return peak_;}
    // Translate the simulation window WITHOUT moving its contents in screen space.
    // Whole-cell steps avoid a blurry resampling on every mouse poll.
    bool Recenter(Vec2 mouse){
        if(!Ready())return false;
        float size=cellSize_*n_;
        int sx=(int)std::round((mouse.x-size*0.5f-originX_)/cellSize_);
        int sy=(int)std::round((mouse.y-size*0.5f-originY_)/cellSize_);
        if(!sx&&!sy)return false;
        if(std::abs(sx)>=n_/2||std::abs(sy)>=n_/2){
            std::fill(grid_.begin(),grid_.end(),Cell{});peak_=0;
        }else{
            for(int y=0;y<n_;y++)for(int x=0;x<n_;x++){
                int ox=x+sx,oy=y+sy;
                tmp_[Idx(x,y)]=(unsigned)ox<(unsigned)n_&&(unsigned)oy<(unsigned)n_?
                    grid_[Idx(ox,oy)]:Cell{};
            }
            grid_.swap(tmp_);
        }
        originX_+=sx*cellSize_;originY_+=sy*cellSize_;
        return true;
    }
    // Deposit coloured ink and cursor momentum along the actual movement path.
    void Splat(Vec2 point,Vec2 pixelsPerSecond,Color color,float radiusPx,float force){
        if(!Ready())return;
        float gx=(point.x-originX_)/cellSize_,gy=(point.y-originY_)/cellSize_;
        float rad=std::max(0.8f,radiusPx/cellSize_);
        int x0=std::max(1,(int)std::floor(gx-2.6f*rad));
        int x1=std::min(n_-2,(int)std::ceil(gx+2.6f*rad));
        int y0=std::max(1,(int)std::floor(gy-2.6f*rad));
        int y1=std::min(n_-2,(int)std::ceil(gy+2.6f*rad));
        float inv=1.f/(rad*rad);float px=Clamp(pixelsPerSecond.x/cellSize_*force,-65.f,65.f);
        float py=Clamp(pixelsPerSecond.y/cellSize_*force,-65.f,65.f);
        for(int y=y0;y<=y1;y++)for(int x=x0;x<=x1;x++){
            if(Solid(x,y))continue;
            float dx=x+.5f-gx,dy=y+.5f-gy;
            float fall=std::exp(-1.65f*(dx*dx+dy*dy)*inv);
            Cell& c=grid_[Idx(x,y)];
            // Perpendicular circulation adds visible ink vortices around the
            // pointer splat; projection spreads them into the flow field.
            const float spin=std::sqrt(px*px+py*py)*0.30f;
            const float tx=-dy/(rad+1.f),ty=dx/(rad+1.f);
            c.vx=Clamp(c.vx+fall*(px+spin*tx),-80.f,80.f);
            c.vy=Clamp(c.vy+fall*(py+spin*ty),-80.f,80.f);
            // Alpha-composited PREMULTIPLIED RGB dye: overlapping colours
            // remain saturated rather than accumulating into a white blob.
            const float ink=Clamp(fall*0.35f,0.f,.9f);
            c.r=c.r*(1.f-ink)+color.r*ink;
            c.g=c.g*(1.f-ink)+color.g*ink;
            c.b=c.b*(1.f-ink)+color.b*ink;
            c.a=c.a*(1.f-ink)+ink;
        }
        peak_=std::max(peak_,0.2f);
    }
    // One bounded step of semi-Lagrangian stable fluids. Pressure is projected
    // by Jacobi relaxation; vorticity confinement restores visible swirls.
    void Step(float dt,float velocityRetention,float dyeRetention,
              float vorticity,int pressureIterations){
        if(!Ready())return;
        // A 15 FPS adaptive frame is 66.7 ms. Avoid silently truncating
        // its elapsed time to 50 ms; the solver is semi-Lagrangian and
        // velocity/pressure work remains bounded.
        dt=Clamp(dt,0.001f,0.075f);
        EnforceWalls();
        float velDamp=std::pow(Clamp(velocityRetention,0.75f,1.f),dt*60.f);
        float dyeDamp=std::pow(Clamp(dyeRetention,0.80f,0.9998f),dt*60.f);
        // Curl of 2D velocity: dVy/dX - dVx/dY.
        for(int y=1;y<n_-1;y++)for(int x=1;x<n_-1;x++){
            curl_[Idx(x,y)]=.5f*(grid_[Idx(x+1,y)].vy-grid_[Idx(x-1,y)].vy
                                      -grid_[Idx(x,y+1)].vx+grid_[Idx(x,y-1)].vx);
        }
        // Advect velocities, with bounded vorticity forces.
        for(int y=0;y<n_;y++)for(int x=0;x<n_;x++){
            int i=Idx(x,y);const Cell& c=grid_[i];
            Cell a=Sample(grid_,x-c.vx*dt,y-c.vy*dt);
            if(x==0||y==0||x==n_-1||y==n_-1){a.vx=a.vy=0;}
            else if(vorticity>0.f){
                float gx=std::fabs(curl_[Idx(x+1,y)])-std::fabs(curl_[Idx(x-1,y)]);
                float gy=std::fabs(curl_[Idx(x,y+1)])-std::fabs(curl_[Idx(x,y-1)]);
                float norm=std::sqrt(gx*gx+gy*gy)+1.e-5f;gx/=norm;gy/=norm;
                a.vx=Clamp(a.vx+gy*curl_[i]*vorticity*dt,-80.f,80.f);
                a.vy=Clamp(a.vy-gx*curl_[i]*vorticity*dt,-80.f,80.f);
            }
            tmp_[i]=a;tmp_[i].vx*=velDamp;tmp_[i].vy*=velDamp;
        }
        // Divergence and pressure solve: non-compressible flow.
        std::fill(pressure_.begin(),pressure_.end(),0.f);
        for(int y=1;y<n_-1;y++)for(int x=1;x<n_-1;x++){
            div_[Idx(x,y)]=.5f*(tmp_[Idx(x+1,y)].vx-tmp_[Idx(x-1,y)].vx+
                                 tmp_[Idx(x,y+1)].vy-tmp_[Idx(x,y-1)].vy);
        }
        pressureIterations=std::max(4,std::min(28,pressureIterations));
        for(int j=0;j<pressureIterations;j++){
            for(int y=1;y<n_-1;y++)for(int x=1;x<n_-1;x++){
                int i=Idx(x,y);
                work_[i]=(pressure_[Idx(x+1,y)]+pressure_[Idx(x-1,y)]+
                          pressure_[Idx(x,y+1)]+pressure_[Idx(x,y-1)]-div_[i])*0.25f;
            }
            pressure_.swap(work_);
        }
        for(int y=1;y<n_-1;y++)for(int x=1;x<n_-1;x++){
            int i=Idx(x,y);
            tmp_[i].vx-=.5f*(pressure_[Idx(x+1,y)]-pressure_[Idx(x-1,y)]);
            tmp_[i].vy-=.5f*(pressure_[Idx(x,y+1)]-pressure_[Idx(x,y-1)]);
        }
        // Transport RGB dye using projected velocity, not an expanding blob.
        peak_=0;
        for(int y=0;y<n_;y++)for(int x=0;x<n_;x++){
            int i=Idx(x,y);Cell out=tmp_[i];
            Cell dye=Sample(grid_,x-out.vx*dt,y-out.vy*dt);
            out.r=dye.r*dyeDamp;out.g=dye.g*dyeDamp;
            out.b=dye.b*dyeDamp;out.a=dye.a*dyeDamp;
            // Prevent permanent dye stripes on the edge of the moving domain.
            float edge=(float)std::min(std::min(x,y),std::min(n_-1-x,n_-1-y));
            if(edge<5.f){float fade=Clamp(edge/5.f,0.f,1.f);out.r*=fade;out.g*=fade;out.b*=fade;out.a*=fade;}
            peak_=std::max(peak_,out.a);
            grid_[i]=out;
        }
        EnforceWalls();
    }
    Color SampleColor(float screenX,float screenY) const {
        if(!Ready())return {};
        float gx=(screenX-originX_)/cellSize_-0.5f,gy=(screenY-originY_)/cellSize_-0.5f;
        if(gx<0||gy<0||gx>=n_-1||gy>=n_-1)return {};
        Cell c=Sample(grid_,gx,gy);
        return {c.r,c.g,c.b,c.a};
    }
    Rect DyeBounds(float minValue=0.015f) const {
        Rect out;if(!Ready()||peak_<minValue)return out;
        int x0=n_,y0=n_,x1=0,y1=0;
        for(int y=0;y<n_;y++)for(int x=0;x<n_;x++){
            const Cell& c=grid_[Idx(x,y)];
            if(c.a<minValue)continue;
            x0=std::min(x0,x);x1=std::max(x1,x);
            y0=std::min(y0,y);y1=std::max(y1,y);
        }
        if(x0>x1)return out;
        out.left=(int)std::floor(originX_+(x0-2)*cellSize_);
        out.top=(int)std::floor(originY_+(y0-2)*cellSize_);
        out.right=(int)std::ceil(originX_+(x1+3)*cellSize_);
        out.bottom=(int)std::ceil(originY_+(y1+3)*cellSize_);
        out.empty=false;return out;
    }
};
} // namespace DodiFluid


namespace {
constexpr float kPi = 3.14159265358979323846f;
constexpr int kSpriteLimit = 256;
constexpr int kMaxParticlesHard = 300;
constexpr int kMaxTrailPoints = 450;
constexpr int kMaxWindowDimension = 4096;
constexpr size_t kMaxActiveFramePixels = 12u * 1024u * 1024u;

struct RGBColor { unsigned char r=160, g=90, b=255; };
struct Settings {
    bool enabled=false, fullscreen=true, capture=false, windowCollisions=false;
    int renderMode=2, fixedFps=120, idleFps=20, fastThreshold=1000;
    int maxFps=360, overlayRadius=440, simFps=60, sampleFps=120, glowFps=60;
    int cursorScale=100;
    bool cursorAutoSize=false, haloEnabled=false;
    bool adaptiveQuality=true, smoothing=false;
    bool allEffectsRGB=false;
    bool glow=false, trail=false, particles=false;
    bool fluidEnabled=false, fluidRainbow=false;
    int glowMode=1, glowStrength=75, glowRadius=10, glowThickness=2, glowSoftness=65;
    int effectSpeed=50, rainbowSpeed=45;
    int fluidResolution=96,fluidFPS=30,fluidFPSMode=0,fluidIdleFPS=15,fluidRadius=185,fluidSplat=15,fluidForce=65;
    int fluidVorticity=35,fluidDyeRetention=96,fluidVelocityRetention=98;
    int fluidPressureIterations=12,fluidOpacity=72;
    RGBColor fluidColor1={170,85,255},fluidColor2={0,238,255};
    int trailStyle=0, trailLengthMs=420, trailWidth=5, trailSpacing=3;
    bool trailRainbow=false;
    int physics=1, physicsStrength=100, drag=35, turbulence=20;
    bool usePhysics=false, bounce=false;
    int particleShape=0, imageScale=100;
    std::wstring particleImagePath;
    int particleMax=90, particlePer100=18, particleLifeMs=700;
    int particleSize=3, particleSpeed=115, gravity=220;
    bool particleRainbow=false;
    RGBColor glowColor, glowColor2={0,238,255}, trailColor, particleColor;
};

struct P2 { float x=0, y=0; };
struct TrailPoint { P2 p; double birth=0; };
struct Particle { P2 p, v; double age=0, life=1; float size=3, hue=0, seed=0; };
// Pure geometry state for visual-only window interaction. Each worker owns its
// own state. Never call ClipCursor or modify the application's window.
struct WindowWall {
    HWND hwnd=nullptr;
    RECT box{};
    bool active=false, contain=false;
};
namespace WindowWalls {
    bool Same(const WindowWall& a,const WindowWall& b) {
        return a.active==b.active && (!a.active || (a.hwnd==b.hwnd &&
            a.contain==b.contain && EqualRect(&a.box,&b.box)));
    }
    bool Allowed(float x,float y,const WindowWall& w) {
        if(!w.active)return true;
        bool within=x>=w.box.left && x<w.box.right &&
                    y>=w.box.top && y<w.box.bottom;
        return w.contain?within:!within;
    }
    float DistanceToRect(POINT p,const RECT& r) {
        float dx=p.x<r.left?(float)(r.left-p.x):
                 p.x>r.right?(float)(p.x-r.right):0.f;
        float dy=p.y<r.top?(float)(r.top-p.y):
                 p.y>r.bottom?(float)(p.y-r.bottom):0.f;
        return std::sqrt(dx*dx+dy*dy);
    }
    struct Search { HWND overlay=nullptr,foreground=nullptr; POINT cursor{};
        WindowWall best{};float score=100000.f; };
    BOOL CALLBACK Find(HWND hwnd, LPARAM param) {
        auto& s=*reinterpret_cast<Search*>(param);
        if(hwnd==s.overlay || !IsWindowVisible(hwnd) || IsIconic(hwnd) ||
           hwnd==GetShellWindow() || hwnd==GetDesktopWindow())return TRUE;
        LONG_PTR ex=GetWindowLongPtrW(hwnd,GWL_EXSTYLE);
        if(ex&(WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE))return TRUE;
        wchar_t cls[64]={};GetClassNameW(hwnd,cls,64);
        if(!lstrcmpW(cls,L"Progman")||!lstrcmpW(cls,L"WorkerW")||
           !lstrcmpW(cls,L"Shell_TrayWnd")||
           !lstrcmpW(cls,L"Shell_SecondaryTrayWnd"))return TRUE;
        DWORD cloaked=0;
        if(SUCCEEDED(DwmGetWindowAttribute(hwnd,DWMWA_CLOAKED,&cloaked,sizeof(cloaked)))&&cloaked)
            return TRUE;
        RECT r{};
        if(FAILED(DwmGetWindowAttribute(hwnd,DWMWA_EXTENDED_FRAME_BOUNDS,&r,sizeof(r))))
            if(!GetWindowRect(hwnd,&r))return TRUE;
        if(r.right-r.left<80 || r.bottom-r.top<60)return TRUE;
        float dist=DistanceToRect(s.cursor,r);
        if(dist>160.f)return TRUE;
        // A frame containing the pointer beats a merely nearby window;
        // the focused frame wins when both overlap it. This keeps an
        // adjacent foreground window from hiding effects over a different
        // background window. Ties retain EnumWindows' topmost first.
        bool focused=hwnd==s.foreground;
        float score=(dist<0.5f?(focused?-500.f:-400.f):
                     (focused?dist-100.f:dist));
        if(score<s.score) {
            s.score=score;s.best={hwnd,r,true,focused};
        }
        return TRUE;
    }
    WindowWall Detect(HWND overlay,POINT pos){
        Search search;search.overlay=overlay;search.cursor=pos;
        search.foreground=GetAncestor(GetForegroundWindow(),GA_ROOT);
        EnumWindows(Find,(LPARAM)&search);
        return search.best;
    }
    // Inside and outside use normal velocity reflection. This does NOT move
    // or constrain the real mouse; it only affects our temporary particles.
    void Reflect(P2 old,P2& p,P2& v,float radius,const WindowWall& w) {
        if(!w.active)return;
        float pad=std::max(1.f,std::min(28.f,radius));
        float l=w.box.left+(w.contain?pad:-pad),
              t=w.box.top+(w.contain?pad:-pad),
              r=w.box.right-(w.contain?pad:-pad),
              b=w.box.bottom-(w.contain?pad:-pad);
        if(w.contain){
            if(l>=r||t>=b)return;
            if(p.x<l){p.x=l;v.x=std::fabs(v.x)*0.7f;}
            if(p.x>r){p.x=r;v.x=-std::fabs(v.x)*0.7f;}
            if(p.y<t){p.y=t;v.y=std::fabs(v.y)*0.7f;}
            if(p.y>b){p.y=b;v.y=-std::fabs(v.y)*0.7f;}
            return;
        }
        // Expanded solid window. Use a swept segment test, so high velocity
        // particles don't tunnel through the wall in one physics update.
        if(old.x>=l&&old.x<=r&&old.y>=t&&old.y<=b){
            if(p.x<l||p.x>r||p.y<t||p.y>b)return;
            float d[4]={std::fabs(p.x-l),std::fabs(p.x-r),
                        std::fabs(p.y-t),std::fabs(p.y-b)};
            int edge=0;for(int i=1;i<4;i++)if(d[i]<d[edge])edge=i;
            if(edge==0){p.x=l;v.x=-std::fabs(v.x)*0.7f;}
            if(edge==1){p.x=r;v.x=std::fabs(v.x)*0.7f;}
            if(edge==2){p.y=t;v.y=-std::fabs(v.y)*0.7f;}
            if(edge==3){p.y=b;v.y=std::fabs(v.y)*0.7f;}
            return;
        }
        float dx=p.x-old.x,dy=p.y-old.y,entry=0.f,exit=1.f;
        int face=-1;
        auto axis=[&](float start,float motion,float lo,float hi,int lowFace,int highFace){
            if(std::fabs(motion)<0.00001f)return start<lo||start>hi?false:true;
            float a=(lo-start)/motion,b=(hi-start)/motion;
            int nearFace=lowFace;
            if(a>b){std::swap(a,b);nearFace=highFace;}
            if(a>entry){entry=a;face=nearFace;}
            exit=std::min(exit,b);
            return entry<=exit;
        };
        if(!axis(old.x,dx,l,r,0,1)||!axis(old.y,dy,t,b,2,3)||face<0||
           entry<0.f||entry>1.f)return;
        float x=old.x+entry*dx,y=old.y+entry*dy;
        float remain=1.f-entry;
        if(face==0||face==1){v.x=-v.x*0.7f;dx=-dx*0.7f;}
        else {v.y=-v.y*0.7f;dy=-dy*0.7f;}
        p.x=x+remain*dx;p.y=y+remain*dy;
    }
}
struct CursorSprite {
    HCURSOR handle=nullptr;
    int width=0, height=0, hotspotX=0, hotspotY=0, haloRadius=0;
    int sourceWidth=0,sourceHeight=0,sourceSystemSize=0;
    unsigned sourceHotX=0,sourceHotY=0;
    std::vector<unsigned int> sourcePixels;
    std::vector<unsigned char> alpha;
    std::vector<unsigned int> colors;
    std::vector<float> distance;
    std::vector<int> glowIndices;
};

Settings g_s;
static ExperimentalExtras::Config g_extra;
std::shared_ptr<IntegratedFormula::Compiled> g_custom[3];
void LoadCustom(){
    const wchar_t* keys[]={L"CursorEffects.Custom.Glow",L"CursorEffects.Custom.Trail",L"CursorEffects.Custom.Particles"};
    for(int i=0;i<3;i++){
        PCWSTR value=Wh_GetStringSetting(keys[i]);std::wstring code=value?value:L"";
        if(value)Wh_FreeStringSetting(value);
        g_custom[i]=code.empty()?nullptr:IntegratedFormula::Build(code,keys[i]);
    }
}
void CustomColor(int kind,RGBColor& color,float& opacity,float x,float y,float u,float v,float p,float d,double now){
    auto code=g_custom[kind];if(!code||!code->valid)return;
    float c1[]={color.r/255.f,color.g/255.f,color.b/255.f};
    float c2[]={g_s.glowColor2.r/255.f,g_s.glowColor2.g/255.f,g_s.glowColor2.b/255.f};
    // Alpha is a multiplier on native soft edges/fades, never an override of them.
    ExperimentalBorders::Color out{c1[0],c1[1],c1[2],1};
    IntegratedFormula::Run(*code,{x,y,u,v,p,d,(float)fmod(now,4096.),1,1,u,v,p},out,c1,c2);
    color={(unsigned char)(out.r*255),(unsigned char)(out.g*255),(unsigned char)(out.b*255)};
    opacity*=out.a;
}
HANDLE g_stopEvent=nullptr;
HANDLE g_worker=nullptr;
HANDLE g_primaryMutex=nullptr;
unsigned int g_rng=0x82bc7a51u;

unsigned int Random32() {
    g_rng ^= g_rng << 13; g_rng ^= g_rng >> 17; g_rng ^= g_rng << 5;
    return g_rng;
}
float Rand01() { return (Random32() & 0xffffff) / 16777215.0f; }
int ClampI(int x,int lo,int hi) { return std::max(lo,std::min(x,hi)); }
float ClampF(float x,float lo,float hi) { return std::max(lo,std::min(x,hi)); }

std::wstring ReadString(PCWSTR key) {
    PCWSTR str = CursorString(key);
    std::wstring result = str ? str : L"";
    if (str) Wh_FreeStringSetting(str);
    return result;
}

// A legacy key which is no longer declared in the YAML reads as 0 / empty.
// It is NOT possible to distinguish that from a deliberately configured zero
// using the ordinary Wh_GetIntSetting API. Never infer a fallback from the new
// setting being equal to its default: doing so silently overrides Reset and
// makes setting controls appear broken. We intentionally read ONLY the new key.
// Preserve the helper signatures to avoid touching the working effect loader.
int GetSettingIntCompat(PCWSTR current,PCWSTR /*previous*/,int /*defaultValue*/) {
    return CursorInt(current);
}
std::wstring GetSettingStringCompat(PCWSTR current,PCWSTR /*previous*/,PCWSTR /*defaultValue*/) {
    return ReadString(current);
}

RGBColor ParseColor(const std::wstring& s, RGBColor fallback) {
    if (s.size()!=7 || s[0]!=L'#') return fallback;
    auto nibble=[](wchar_t c)->int {
        if (c>=L'0'&&c<=L'9') return c-L'0';
        if (c>=L'a'&&c<=L'f') return c-L'a'+10;
        if (c>=L'A'&&c<=L'F') return c-L'A'+10;
        return -1;
    };
    int n[6];
    for (int i=0;i<6;i++) { n[i]=nibble(s[i+1]); if (n[i]<0) return fallback; }
    return {(unsigned char)(n[0]*16+n[1]), (unsigned char)(n[2]*16+n[3]),
            (unsigned char)(n[4]*16+n[5])};
}

Settings LoadSettings() {
    g_extra=ExperimentalExtras::Load();
    Settings s;
    s.enabled=GetSettingIntCompat(L"General.Controls.Enabled",L"General.Enabled",0)!=0;
    s.windowCollisions=CursorInt(L"General.Controls.WindowCollisions")!=0;
    s.fullscreen=GetSettingIntCompat(L"General.Controls.HideFullscreen",L"General.HideFullscreen",1)!=0;
    s.capture=GetSettingIntCompat(L"General.Controls.HideFromCapture",L"General.HideFromCapture",0)!=0;
    s.allEffectsRGB=CursorInt(L"General.RGB.AllEffects")!=0;
    auto mode=GetSettingStringCompat(L"Rendering.FrameRate.Mode",L"Rendering.Mode",L"adaptive");
    s.renderMode=mode==L"fixed"?0:mode==L"auto"?1:2;
    s.fixedFps=ClampI(GetSettingIntCompat(L"Rendering.FrameRate.FixedFPS",L"Rendering.FixedFPS",120),15,360);
    s.idleFps=ClampI(GetSettingIntCompat(L"Rendering.FrameRate.IdleFPS",L"Rendering.IdleFPS",20),15,30);
    s.fastThreshold=ClampI(GetSettingIntCompat(L"Rendering.FrameRate.HalfSpeedThreshold",L"Rendering.HalfSpeedThreshold",1000),300,3000);
    s.maxFps=ClampI(GetSettingIntCompat(L"Rendering.FrameRate.MaximumFPS",L"Rendering.MaximumFPS",360),30,360);
    s.overlayRadius=ClampI(GetSettingIntCompat(L"Rendering.Limits.OverlayRadius",L"Rendering.OverlayRadius",440),160,480);
    s.smoothing=GetSettingIntCompat(L"Rendering.Interpolation.Smoothing",L"Rendering.Smoothing",0)!=0;
    s.adaptiveQuality=GetSettingIntCompat(L"Rendering.Limits.AdaptiveQuality",L"Rendering.AdaptiveQuality",1)!=0;
    s.simFps=ClampI(GetSettingIntCompat(L"Rendering.Rates.ParticleSimulationFPS",L"Rendering.ParticleSimulationFPS",60),15,240);
    s.sampleFps=ClampI(GetSettingIntCompat(L"Rendering.Rates.TrailSampleFPS",L"Rendering.TrailSampleFPS",120),15,240);
    s.glowFps=ClampI(GetSettingIntCompat(L"Rendering.Rates.GlowAnimationFPS",L"Rendering.GlowAnimationFPS",60),15,240);
    s.cursorAutoSize=CursorInt(L"Cursor.Shape.AutoSize")!=0;
    s.haloEnabled=CursorInt(L"Glow.Switches.HaloEnabled")!=0;
    s.cursorScale=ClampI(GetSettingIntCompat(L"Cursor.Shape.Scale",L"Cursor.Scale",100),50,250);
    s.glow=GetSettingIntCompat(L"Glow.Switches.Enabled",L"Glow.Enabled",0)!=0;
    auto gm=GetSettingStringCompat(L"Glow.OutlineStyle.Mode",L"Glow.Mode",L"neon");
    const wchar_t* glowModes[]={L"solid",L"neon",L"breathing",L"rainbow",L"spatial",L"aurora",L"plasma",L"shimmer",L"mercury-v3",L"mercury-v6",L"liquid",L"comet"};
    s.glowMode=1;
    for(int i=0;i<12;i++)if(gm==glowModes[i])s.glowMode=i;
    s.glowColor=ParseColor(GetSettingStringCompat(L"Glow.OutlineColors.Color",L"Glow.Color",L"#AA55FF"),{170,85,255});
    s.glowColor2=ParseColor(GetSettingStringCompat(L"Glow.OutlineColors.Color2",L"Glow.Color2",L"#00EEFF"),{0,238,255});
    s.glowStrength=ClampI(GetSettingIntCompat(L"Glow.OutlineGeometry.Strength",L"Glow.Strength",75),0,100);
    s.glowRadius=ClampI(GetSettingIntCompat(L"Glow.OutlineGeometry.Radius",L"Glow.Radius",10),1,30);
    s.glowThickness=ClampI(GetSettingIntCompat(L"Glow.OutlineGeometry.Thickness",L"Glow.Thickness",2),0,16);
    s.glowSoftness=ClampI(GetSettingIntCompat(L"Glow.OutlineGeometry.Softness",L"Glow.Softness",65),0,100);
    s.effectSpeed=ClampI(GetSettingIntCompat(L"Glow.OutlineStyle.Speed",L"Glow.Speed",50),0,100);
    s.rainbowSpeed=ClampI(GetSettingIntCompat(L"Glow.OutlineStyle.RainbowSpeed",L"Glow.RainbowSpeed",45),0,360);
    s.fluidEnabled=CursorInt(L"Fluid.Switches.Enabled")!=0;
    s.fluidRainbow=CursorInt(L"Fluid.Switches.Rainbow")!=0;
    s.fluidColor1=ParseColor(ReadString(L"Fluid.Colors.Color1"),{170,85,255});
    s.fluidColor2=ParseColor(ReadString(L"Fluid.Colors.Color2"),{0,238,255});
    s.fluidOpacity=ClampI(CursorInt(L"Fluid.Colors.Opacity"),1,100);
    auto fr=ReadString(L"Fluid.Simulation.Resolution");
    s.fluidResolution=fr==L"r64"?64:fr==L"r128"?128:fr==L"r192"?192:96;
    s.fluidFPS=ClampI(CursorInt(L"Fluid.Simulation.FPS"),15,60);
    auto fluidMode=ReadString(L"Fluid.Simulation.FPSMode");
    s.fluidFPSMode=fluidMode==L"follow"?1:fluidMode==L"adaptive"?2:0;
    s.fluidIdleFPS=ClampI(CursorInt(L"Fluid.Simulation.IdleFPS"),15,30);
    s.fluidRadius=ClampI(CursorInt(L"Fluid.Simulation.Radius"),0,1600);
    s.fluidSplat=ClampI(CursorInt(L"Fluid.Simulation.SplatSize"),4,46);
    s.fluidForce=ClampI(CursorInt(L"Fluid.Simulation.Force"),0,200);
    s.fluidVorticity=ClampI(CursorInt(L"Fluid.Simulation.Vorticity"),0,100);
    s.fluidDyeRetention=ClampI(CursorInt(L"Fluid.Simulation.DyeRetention"),85,99);
    s.fluidVelocityRetention=ClampI(CursorInt(L"Fluid.Simulation.VelocityRetention"),85,100);
    s.fluidPressureIterations=ClampI(CursorInt(L"Fluid.Simulation.PressureIterations"),4,24);
    s.trail=GetSettingIntCompat(L"Trails.TrailOptions.Enabled",L"Trails.Enabled",0)!=0;
    auto st=GetSettingStringCompat(L"Trails.TrailOptions.Style",L"Trails.Style",L"ribbon");
    s.trailStyle=st==L"dots"?1:st==L"afterimage"?2:st==L"soft"?3:st==L"fluid"?4:0;
    s.trailColor=ParseColor(GetSettingStringCompat(L"Trails.Colors.Color",L"Trails.Color",L"#8B6DFF"),{139,109,255});
    s.trailRainbow=GetSettingIntCompat(L"Trails.TrailOptions.Rainbow",L"Trails.Rainbow",0)!=0;
    s.trailLengthMs=ClampI(GetSettingIntCompat(L"Trails.Dimensions.LengthMS",L"Trails.LengthMS",420),100,1500);
    s.trailWidth=ClampI(GetSettingIntCompat(L"Trails.Dimensions.Width",L"Trails.Width",5),1,22);
    s.trailSpacing=ClampI(GetSettingIntCompat(L"Trails.Dimensions.MinimumSpacing",L"Trails.MinimumSpacing",3),1,16);
    s.particles=GetSettingIntCompat(L"Particles.Appearance.Enabled",L"Particles.Enabled",0)!=0;
    auto pm=GetSettingStringCompat(L"Particles.MotionOptions.Physics",L"Particles.Physics",L"zero");
    s.physics=pm==L"gravity"?0:pm==L"upward"?2:pm==L"orbit"?3:pm==L"swirl"?4:pm==L"inertia"?5:1;
    s.usePhysics=GetSettingIntCompat(L"Particles.MotionOptions.UsePhysics",L"Particles.UsePhysics",0)!=0;
    s.physicsStrength=ClampI(GetSettingIntCompat(L"Particles.MotionOptions.PhysicsStrength",L"Particles.PhysicsStrength",100),0,200);
    s.drag=ClampI(GetSettingIntCompat(L"Particles.MotionOptions.Drag",L"Particles.Drag",35),0,100);
    s.turbulence=ClampI(GetSettingIntCompat(L"Particles.MotionOptions.Turbulence",L"Particles.Turbulence",20),0,100);
    s.bounce=GetSettingIntCompat(L"Particles.MotionOptions.Bounce",L"Particles.Bounce",0)!=0;
    auto ps=GetSettingStringCompat(L"Particles.Appearance.Shape",L"Particles.Shape",L"spark");
    const wchar_t* shapes[]={L"spark",L"soft",L"dot",L"star",L"diamond",L"square",L"ring",L"bubble",L"snow",L"heart",L"image"};
    for(int i=0;i<11;i++)if(ps==shapes[i])s.particleShape=i;
    s.particleImagePath=GetSettingStringCompat(L"Particles.Appearance.ImagePath",L"Particles.ImagePath",L"");
    s.imageScale=ClampI(GetSettingIntCompat(L"Particles.Appearance.ImageScale",L"Particles.ImageScale",100),25,200);
    s.particleColor=ParseColor(GetSettingStringCompat(L"Particles.Colors.Color",L"Particles.Color",L"#C3A0FF"),{195,160,255});
    s.particleRainbow=GetSettingIntCompat(L"Particles.Colors.Rainbow",L"Particles.Rainbow",0)!=0;
    s.particleMax=ClampI(GetSettingIntCompat(L"Particles.Emission.Maximum",L"Particles.Maximum",90),0,kMaxParticlesHard);
    s.particlePer100=ClampI(GetSettingIntCompat(L"Particles.Emission.Per100Pixels",L"Particles.Per100Pixels",18),0,100);
    s.particleLifeMs=ClampI(GetSettingIntCompat(L"Particles.Emission.LifetimeMS",L"Particles.LifetimeMS",700),150,2500);
    s.particleSize=ClampI(GetSettingIntCompat(L"Particles.Emission.Size",L"Particles.Size",3),1,16);
    s.particleSpeed=ClampI(GetSettingIntCompat(L"Particles.MotionOptions.Speed",L"Particles.Speed",115),0,500);
    s.gravity=ClampI(GetSettingIntCompat(L"Particles.MotionOptions.Gravity",L"Particles.Gravity",220),0,800);
    if(g_extra.comet)s.trail=true;
    return s;
}

RGBColor HSV(float degrees) {
    float h=std::fmod(degrees,360.0f); if(h<0) h+=360;
    float c=1, x=c*(1-std::fabs(std::fmod(h/60.0f,2.0f)-1));
    float r=0,g=0,b=0;
    if(h<60){r=c;g=x;} else if(h<120){r=x;g=c;}
    else if(h<180){g=c;b=x;} else if(h<240){g=x;b=c;}
    else if(h<300){r=x;b=c;} else {r=c;b=x;}
    return {(unsigned char)(r*255),(unsigned char)(g*255),(unsigned char)(b*255)};
}

double Seconds() {
    static LARGE_INTEGER frequency=[] { LARGE_INTEGER f; QueryPerformanceFrequency(&f); return f; }();
    LARGE_INTEGER now; QueryPerformanceCounter(&now);
    return (double)now.QuadPart / (double)frequency.QuadPart;
}

// Source cursor handles can be undersized relative to the effective Windows
// cursor buffer, especially at >=150% DPI. Dynamically resolve APIs to avoid
// an extra mandatory dependency on shcore at runtime.
int EffectiveCursorBufferSize(POINT point) {
    UINT dpiX=96,dpiY=96;
    HMODULE shcore=GetModuleHandleW(L"shcore.dll");
    if(!shcore)shcore=LoadLibraryW(L"shcore.dll");
    if(shcore){
        using GetMonitorDpiFn=HRESULT (WINAPI*)(HMONITOR,int,UINT*,UINT*);
        auto fn=(GetMonitorDpiFn)GetProcAddress(shcore,"GetDpiForMonitor");
        if(fn){
            const HMONITOR monitor=MonitorFromPoint(point,MONITOR_DEFAULTTONEAREST);
            if(FAILED(fn(monitor,0,&dpiX,&dpiY)))dpiX=96;
        }
    }
    using MetricsDpiFn=int (WINAPI*)(int,UINT);
    HMODULE user=GetModuleHandleW(L"user32.dll");
    auto getDpiMetric=user?(MetricsDpiFn)GetProcAddress(user,"GetSystemMetricsForDpi"):nullptr;
    int size=getDpiMetric?getDpiMetric(SM_CXCURSOR,dpiX):GetSystemMetrics(SM_CXCURSOR);
    return ClampI(size,16,256);
}

// Read cursor alpha from the original color bitmap. Classic AND/XOR-mask
// cursors fall back to the AND mask. Inverted pixels are approximate.
bool ReadBitmap32(HBITMAP bmp,int width,int height,std::vector<unsigned int>& out) {
    if (!bmp || width<1 || height<1 || width>kSpriteLimit || height>kSpriteLimit*2) return false;
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth=width;
    bmi.bmiHeader.biHeight=-height;
    bmi.bmiHeader.biPlanes=1;
    bmi.bmiHeader.biBitCount=32;
    bmi.bmiHeader.biCompression=BI_RGB;
    out.resize((size_t)width*height);
    HDC dc=GetDC(nullptr);
    if(!dc) return false;
    int lines=GetDIBits(dc,bmp,0,height,out.data(),&bmi,DIB_RGB_COLORS);
    ReleaseDC(nullptr,dc);
    return lines==height;
}

// Extract the actual native cursor bitmap and its hot spot. Build a cached
// 8-neighbour 2-pass distance field: unlike rectangular dilation, each contour
// follows the nearest visible source pixel, including holes and diagonal tips.
bool AcquireCursor(HCURSOR cursor,CursorSprite& sprite,int glowRadius,int systemSize,
                   const CursorSprite* cached=nullptr,bool* unchanged=nullptr) {
    if(unchanged)*unchanged=false;
    ICONINFO info{};
    if(!cursor || !GetIconInfo(cursor,&info))return false;
    BITMAP bm{};int w=0,h=0;
    if(info.hbmColor && GetObjectW(info.hbmColor,sizeof(bm),&bm)) {w=bm.bmWidth;h=bm.bmHeight;}
    else if(info.hbmMask && GetObjectW(info.hbmMask,sizeof(bm),&bm)) {w=bm.bmWidth;h=bm.bmHeight/2;}
    bool valid=w>0&&h>0&&w<=kSpriteLimit&&h<=kSpriteLimit;
    std::vector<unsigned int> colors, mask;
    if(valid) {
        if(info.hbmColor)ReadBitmap32(info.hbmColor,w,h,colors);
        if(info.hbmMask)ReadBitmap32(info.hbmMask,w,info.hbmColor?h:2*h,mask);
    }
    if(info.hbmColor)DeleteObject(info.hbmColor);
    if(info.hbmMask)DeleteObject(info.hbmMask);
    if(!valid || (colors.empty()&&mask.empty()))return false;
    std::vector<unsigned int> raw((size_t)w*h,0);
    bool hasAlpha=false;
    for(unsigned c:colors)if(c>>24){hasAlpha=true;break;}
    if(!hasAlpha&&mask.empty())return false; // Never approximate an unknown mask as a solid rectangle.

    for(int y=0;y<h;y++)for(int x=0;x<w;x++) {
        size_t i=(size_t)y*w+x;
        unsigned rgba=colors.empty()?0xffffffffu:colors[i];
        unsigned a=hasAlpha?(rgba>>24):(!mask.empty()?(mask[i]&255)?0:255:255);
        if(colors.empty() && mask.size()>=(size_t)2*w*h) {
            // Monochrome AND/XOR combination. Inverted pixels cannot be
            // accurately represented by a solid alpha sprite.
            bool andBlack=(mask[i]&255)==0;
            bool xorWhite=(mask[i+(size_t)w*h]&255)!=0;
            a=(andBlack||xorWhite)?255:0;
            rgba=xorWhite?0xffffffffu:0xff000000u;
        }
        raw[i]=(rgba&0x00ffffffu)|(a<<24);
    }
    if(cached && unchanged && cached->width>0 && cached->haloRadius==glowRadius &&
       cached->sourceWidth==w && cached->sourceHeight==h &&
       cached->sourceSystemSize==systemSize && cached->sourceHotX==info.xHotspot &&
       cached->sourceHotY==info.yHotspot && cached->sourcePixels==raw){
        *unchanged=true;return true;
    }
    // Auto sizing uses the effective cursor metrics, but does not downscale a
    // custom cursor pack that already has larger-than-standard artwork.
    // Only apply the automatic ratio to standard 32/48/64/96/128 buffers.
    const bool standard=(w==32||w==48||w==64||w==96||w==128) && h>0;
    const float autoRatio=g_s.cursorAutoSize&&standard&&systemSize>w
        ? ClampF((float)systemSize/w,1.f,2.5f):1.f;
    const float scale=g_s.cursorAutoSize?autoRatio:g_s.cursorScale/100.f;
    int sw=ClampI((int)std::round(w*scale),1,400);
    int sh=ClampI((int)std::round(h*scale),1,400);
    // World-space fluid owns its canvas; cursor mask only needs outline padding.
    int pad=glowRadius+g_s.glowThickness+6;
    int W=sw+2*pad,H=sh+2*pad;
    if(W>660||H>660)return false;
    CursorSprite next;
    next.handle=cursor;next.haloRadius=glowRadius;
    next.width=W;next.height=H;
    next.hotspotX=pad+ClampI((int)std::round(info.xHotspot*(float)sw/w),0,sw-1);
    next.hotspotY=pad+ClampI((int)std::round(info.yHotspot*(float)sh/h),0,sh-1);
    next.alpha.assign((size_t)W*H,0);
    next.colors.assign((size_t)W*H,0);
    for(int y=0;y<sh;y++)for(int x=0;x<sw;x++) {
        // Bilinear resampling reduces stepping on upscaled custom cursor packs.
        float sx=((float)x+0.5f)*w/sw-0.5f, sy=((float)y+0.5f)*h/sh-0.5f;
        int x0=ClampI((int)std::floor(sx),0,w-1),y0=ClampI((int)std::floor(sy),0,h-1);
        int x1=std::min(x0+1,w-1),y1=std::min(y0+1,h-1);
        float tx=ClampF(sx-x0,0,1),ty=ClampF(sy-y0,0,1);
        unsigned q[4]={raw[(size_t)y0*w+x0],raw[(size_t)y0*w+x1],
                       raw[(size_t)y1*w+x0],raw[(size_t)y1*w+x1]};
        unsigned out=0;
        for(int k=0;k<4;k++) {
            int shift=k*8;
            float a=(q[0]>>shift)&255,b=(q[1]>>shift)&255;
            float c=(q[2]>>shift)&255,d=(q[3]>>shift)&255;
            unsigned channel=(unsigned)(a*(1-tx)*(1-ty)+b*tx*(1-ty)+c*(1-tx)*ty+d*tx*ty+0.5f);
            out|=(channel<<shift);
        }
        size_t index=(size_t)(y+pad)*W+x+pad;
        next.alpha[index]=(unsigned char)(out>>24);
        next.colors[index]=out;
    }
    const float farDistance=10000.f, diag=1.41421356237f;
    next.distance.resize((size_t)W*H);
    for(size_t i=0;i<next.alpha.size();i++)next.distance[i]=next.alpha[i]>36?0.f:farDistance;
    for(int y=0;y<H;y++)for(int x=0;x<W;x++) {
        size_t i=(size_t)y*W+x;float d=next.distance[i];
        if(x>0)d=std::min(d,next.distance[i-1]+1);
        if(y>0) {
            d=std::min(d,next.distance[i-W]+1);
            if(x>0)d=std::min(d,next.distance[i-W-1]+diag);
            if(x+1<W)d=std::min(d,next.distance[i-W+1]+diag);
        }
        next.distance[i]=d;
    }
    for(int y=H-1;y>=0;y--)for(int x=W-1;x>=0;x--) {
        size_t i=(size_t)y*W+x;float d=next.distance[i];
        if(x+1<W)d=std::min(d,next.distance[i+1]+1);
        if(y+1<H) {
            d=std::min(d,next.distance[i+W]+1);
            if(x>0)d=std::min(d,next.distance[i+W-1]+diag);
            if(x+1<W)d=std::min(d,next.distance[i+W+1]+diag);
        }
        next.distance[i]=d;
    }
    next.glowIndices.reserve((size_t)std::min(40000,W*H));
    for(int i=0;i<W*H;i++)
        if(next.alpha[(size_t)i]<=26 && next.distance[(size_t)i]>0 &&
           next.distance[(size_t)i]<=glowRadius+g_s.glowThickness+3.f)
            next.glowIndices.push_back(i);
    next.sourceWidth=w;next.sourceHeight=h;next.sourceSystemSize=systemSize;
    next.sourceHotX=info.xHotspot;next.sourceHotY=info.yHotspot;
    next.sourcePixels=std::move(raw);
    sprite=std::move(next);return true;
}

struct Canvas {
    HDC dc=nullptr;
    HBITMAP bitmap=nullptr, old=nullptr;
    unsigned int* pixels=nullptr;
    int w=0,h=0;
    int activeW=0,activeH=0;
    int originX=0,originY=0;
    ~Canvas() {Reset();}
    void Reset() {
        if(dc && old) SelectObject(dc,old);
        if(bitmap) DeleteObject(bitmap);
        if(dc) DeleteDC(dc);
        dc=nullptr;bitmap=nullptr;old=nullptr;pixels=nullptr;w=h=activeW=activeH=0;
    }
    bool Resize(int width,int height) {
        if(width<1||height<1||width>kMaxWindowDimension||height>kMaxWindowDimension||
           (size_t)width*(size_t)height>kMaxActiveFramePixels)return false;
        // Reuse allocated DIB, but PRESENT only the required active region.
        // Avoid wasting compositor bandwidth on yesterday's larger bounds.
        if(pixels && width<=w && height<=h) {activeW=width;activeH=height;return true;}
        auto bucket=[](int size) {
            for(int edge : {64,128,256,384,512,640,768,896,1024,1280,
                            1536,1792,2048,2304,2560,3072,3584,4096})
                if(size<=edge)return edge;
            return kMaxWindowDimension;
        };
        const int requestedW=width,requestedH=height;
        const int allocW=bucket(width),allocH=bucket(height);
        // Oversized bucket rounding can exceed the allocation limit even
        // when the requested active rectangle is safely within the budget.
        width=(size_t)allocW*allocH>kMaxActiveFramePixels?requestedW:allocW;
        height=(size_t)allocW*allocH>kMaxActiveFramePixels?requestedH:allocH;
        Reset();
        dc=CreateCompatibleDC(nullptr); if(!dc)return false;
        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth=width;
        bmi.bmiHeader.biHeight=-height;
        bmi.bmiHeader.biPlanes=1;
        bmi.bmiHeader.biBitCount=32;
        bmi.bmiHeader.biCompression=BI_RGB;
        bitmap=CreateDIBSection(dc,&bmi,DIB_RGB_COLORS,(void**)&pixels,nullptr,0);
        if(!bitmap||!pixels){Reset();return false;}
        old=(HBITMAP)SelectObject(dc,bitmap);
        w=width;h=height;activeW=requestedW;activeH=requestedH;
        return true;
    }
    void Clear() { if(pixels)for(int y=0;y<activeH;y++)
        std::memset(pixels+(size_t)y*w,0,(size_t)activeW*4); }
    void Blend(int x,int y,RGBColor color,int a) {
        x-=originX;y-=originY;
        if((unsigned)x>=(unsigned)activeW||(unsigned)y>=(unsigned)activeH||a<=0)return;
        a=ClampI(a,0,255);
        unsigned int& px=pixels[(size_t)y*w+x];
        int inv=255-a;
        int da=(int)(px>>24),db=(int)(px&255),dg=(int)((px>>8)&255),dr=(int)((px>>16)&255);
        int r=(color.r*a+127)/255 + (dr*inv+127)/255;
        int g=(color.g*a+127)/255 + (dg*inv+127)/255;
        int b=(color.b*a+127)/255 + (db*inv+127)/255;
        int oa=a+(da*inv+127)/255;
        px=(unsigned int)((ClampI(oa,0,255)<<24)|(ClampI(r,0,255)<<16)|
            (ClampI(g,0,255)<<8)|ClampI(b,0,255));
    }
    void Erase(int x,int y) {
        x-=originX;y-=originY;
        if((unsigned)x<(unsigned)activeW&&(unsigned)y<(unsigned)activeH)pixels[(size_t)y*w+x]=0;
    }
    void Dot(float fx,float fy,float size,RGBColor color,float alpha) {
        int radius=std::max(1,(int)std::ceil(size+1.5f));
        int cx=(int)std::round(fx),cy=(int)std::round(fy);
        for(int yy=cy-radius;yy<=cy+radius;yy++) for(int xx=cx-radius;xx<=cx+radius;xx++) {
            float dx=xx-fx,dy=yy-fy,dist=std::sqrt(dx*dx+dy*dy);
            float k=ClampF((size+0.6f-dist)/1.5f,0,1);
            if(k>0) Blend(xx,yy,color,(int)(255*alpha*k));
        }
    }
};

void ClipVisualLayerToWall(Canvas& c,const WindowWall& wall){
    if(!wall.active||!c.pixels)return;
    int x0=std::max(0,(int)wall.box.left-c.originX);
    int x1=std::min(c.activeW,(int)wall.box.right-c.originX);
    int y0=std::max(0,(int)wall.box.top-c.originY);
    int y1=std::min(c.activeH,(int)wall.box.bottom-c.originY);
    if(wall.contain){
        for(int y=0;y<c.activeH;y++){
            unsigned int* p=c.pixels+(size_t)y*c.w;
            if(y<y0||y>=y1||x0>=x1){std::memset(p,0,(size_t)c.activeW*4);continue;}
            if(x0>0)std::memset(p,0,(size_t)x0*4);
            if(x1<c.activeW)std::memset(p+x1,0,(size_t)(c.activeW-x1)*4);
        }
    }else if(x0<x1&&y0<y1){
        for(int y=y0;y<y1;y++)
            std::memset(c.pixels+(size_t)y*c.w+x0,0,(size_t)(x1-x0)*4);
    }
}

float Smooth(float a,float b,float x) {
    if(a>=b)return x>=b?1.f:0.f;
    float t=ClampF((x-a)/(b-a),0,1);return t*t*(3-2*t);
}
RGBColor MixColor(RGBColor a,RGBColor b,float t) {
    t=ClampF(t,0,1);
    return {(unsigned char)(a.r*(1-t)+b.r*t),
            (unsigned char)(a.g*(1-t)+b.g*t),
            (unsigned char)(a.b*(1-t)+b.b*t)};
}

void DrawSpriteGlow(Canvas& c,const CursorSprite& spr,P2 hotspot,double now,
                    float phase,float globalAlpha=1.f) {
    if(spr.width<1||spr.height<1||!g_s.glow)return;
    const int startX=(int)std::round(hotspot.x)-spr.hotspotX;
    const int startY=(int)std::round(hotspot.y)-spr.hotspotY;
    const float t=(float)now*(0.05f+g_s.effectSpeed/15.f);
    const float cx=(float)spr.width*0.5f,cy=(float)spr.height*0.5f;
    float strength=g_s.glowStrength/100.f*globalAlpha;
    if(g_s.glowMode==2)strength*=0.68f+0.32f*std::sin((float)now*(1+g_s.effectSpeed/40.f)*3.f);
    float feather=std::max(1.2f,g_s.glowRadius*(0.28f+g_s.glowSoftness/130.f));
    float thickness=(float)g_s.glowThickness;
    for(int index:spr.glowIndices) {
        int x=index%spr.width,y=index/spr.width;
        size_t i=(size_t)index;
        float d=spr.distance[i];
        if(d>g_s.glowRadius+thickness+3.f)continue;
        float angle=std::atan2((float)y-cy,(float)x-cx);
        float distance=d;
        float core=thickness>0?1.f-Smooth(std::max(0.f,thickness-0.8f),thickness+0.8f,distance):0.f;
        float halo=g_s.haloEnabled?
            std::exp(-0.5f*std::pow(std::max(0.f,distance-thickness)/feather,2.f)):0.f;
        // One-pixel soft onset avoids an opaque rectangular/dilated block.
        float onset=Smooth(0.10f,1.20f,d);
        float falloff=1.f-Smooth(std::max(0.f,(float)g_s.glowRadius+thickness-4.f),
                                    (float)g_s.glowRadius+thickness+3.f,d);
        float opacity=onset*falloff*strength*(0.72f*core+0.26f*halo*(1.f-core));
        RGBColor color=g_s.glowColor;
        float u=(float)x/std::max(1,spr.width),v=(float)y/std::max(1,spr.height);
        switch(g_s.glowMode) {
        case 1: color=MixColor(color,{255,255,255},core*0.55f);break; // neon
        case 3: color=HSV(phase);break;
        case 4: color=HSV(phase+360*u*0.65f+360*v*0.35f);break;
        case 5: { // Aurora oscillates between chosen colors instead of hard-coded RGB.
            float k=0.5f+0.5f*std::sin(t*0.4f+u*8.f+v*5.f);
            color=MixColor(g_s.glowColor,g_s.glowColor2,k);break;
        }
        case 6: color=MixColor(color,g_s.glowColor2,0.5f+0.25f*std::sin(u*15+t)+0.25f*std::sin(v*12-t));break;
        case 7: {float k=std::pow(0.5f+0.5f*std::sin(angle*5.f-t*5.f),12.f);
                 color=MixColor(color,{255,255,255},k);opacity*=0.8f+k*0.5f;break;}
        case 8: case 9: {
            float waves=std::pow(0.5f+0.5f*std::sin(angle*(g_s.glowMode==8?5.f:11.f)-t*3.f+std::sin(angle*3+t)),
                                  g_s.glowMode==8?4.f:10.f);
            color=MixColor(g_s.glowColor,g_s.glowColor2,waves);
            opacity*=0.22f+waves*0.9f;break;
        }
        case 10:color=MixColor(color,g_s.glowColor2,0.5f+0.5f*std::sin(angle*4+t));break;
        case 11: {float k=std::pow(0.5f+0.5f*std::cos(angle*3.f-t*4.f),18.f);
                 color=MixColor(color,g_s.glowColor2,k);opacity*=0.18f+1.3f*k;break;}
        default:break;
        }
        // RGB override is independent from the shape/motion preset and never
        // applies to the native cursor body. Explicit Rainbow presets stay RGB.
        if(g_s.allEffectsRGB)
            color=HSV(phase+180.f*u+75.f*v);
        CustomColor(0,color,opacity,(float)x,(float)y,u,v,angle/(2*kPi)+.5f,distance/std::max(1.f,(float)g_s.glowRadius),now);
        int a=(int)(255*ClampF(opacity,0.f,1.f));
        if(a)c.Blend(startX+x,startY+y,color,a);
    }
}


// A true Eulerian fluid grid, separate from the cursor silhouette/outline.
// The grid is in screen coordinates: dye stays behind as the cursor travels.
// Numerical code is also released as dodi-fluid-simulation.h for later reuse.
struct FluidRasterRows {
    const DodiFluid::FluidGrid* fluid=nullptr;int x0=0,y0=0,step=2,row=-100,columns=0;
    std::vector<DodiFluid::Color> first,second;
    void Fill(std::vector<DodiFluid::Color>& out,int r){
        for(int i=0;i<columns;i++)out[i]=fluid->SampleColor(x0+i*step+.5f,y0+r*step+.5f);
    }
    bool Begin(const DodiFluid::FluidGrid& source,int left,int top,int right,int spacing)try{
        fluid=&source;x0=left;y0=top;step=spacing;row=-100;
        columns=(std::max(0,right-left)+step-1)/step+1;
        first.resize(columns);second.resize(columns);return true;
    }catch(const std::bad_alloc&){return false;}
    DodiFluid::Color At(int x,int y){
        int current=(y-y0)/step;
        if(current!=row){
            if(current==row+1){first.swap(second);Fill(second,current+1);}
            else {Fill(first,current);Fill(second,current+1);}
            row=current;
        }
        int col=(x-x0)/step;
        float u=(x-x0-col*step)/(float)step,v=(y-y0-row*step)/(float)step;
        const auto& a=first[col];const auto& b=first[col+1];
        const auto& c=second[col];const auto& d=second[col+1];
        auto blend=[&](float A,float B,float C,float D){return (A+(B-A)*u)*(1-v)+(C+(D-C)*u)*v;};
        return {blend(a.r,b.r,c.r,d.r),blend(a.g,b.g,c.g,d.g),blend(a.b,b.b,c.b,d.b),blend(a.a,b.a,c.a,d.a)};
    }
};

void DrawFluidDye(Canvas& c,const DodiFluid::FluidGrid& fluid,float opacity){
    auto r=fluid.DyeBounds(0.007f);
    if(r.empty)return;
    int x0=std::max(r.left,c.originX),x1=std::min(r.right,c.originX+c.activeW);
    int y0=std::max(r.top,c.originY),y1=std::min(r.bottom,c.originY+c.activeH);
    int sampleStep=PerformanceOptions::enabled.load()?PerformanceOptions::fluidStep.load():1;
    thread_local FluidRasterRows sampled;
    if(sampleStep>1&&!sampled.Begin(fluid,x0,y0,x1,sampleStep))sampleStep=1;
    for(int y=y0;y<y1;y++){
      if(RenderShutdown::requested.load())return;
      for(int x=x0;x<x1;x++){
        auto dye=sampleStep>1?sampled.At(x,y):fluid.SampleColor(x+0.5f,y+0.5f);
        if(dye.a<0.007f)continue;
        float a=ClampF(dye.a,0.f,1.f)*opacity;
        if(a<0.01f)continue;
        float inv=1.f/std::max(0.01f,dye.a);
        RGBColor color={ (unsigned char)(255*ClampF(dye.r*inv,0.f,1.f)),
                         (unsigned char)(255*ClampF(dye.g*inv,0.f,1.f)),
                         (unsigned char)(255*ClampF(dye.b*inv,0.f,1.f))};
        c.Blend(x,y,color,(int)(255*a));
      }
    }
}

void DrawSpriteAfterimage(Canvas& c,const CursorSprite& spr,P2 hotspot,RGBColor tint,float fade) {
    if(spr.width<1||fade<=0)return;
    int sx=(int)std::round(hotspot.x)-spr.hotspotX;
    int sy=(int)std::round(hotspot.y)-spr.hotspotY;
    for(int y=0;y<spr.height;y++)for(int x=0;x<spr.width;x++) {
        int a=(int)(spr.alpha[(size_t)y*spr.width+x]*fade*0.40f);
        if(a>0)c.Blend(sx+x,sy+y,tint,a);
    }
}

// Remove all overlay pixels over visible native pointer pixels. This is the
// principal protection against hardware cursor "flicker"/double-image when
// particles, trails or outer glow intersect the pointer's original shape.
void ClearNativeCursor(Canvas& c,const CursorSprite& spr,P2 hotspot) {
    if(spr.width<1)return;
    int sx=(int)std::round(hotspot.x)-spr.hotspotX;
    int sy=(int)std::round(hotspot.y)-spr.hotspotY;
    for(int y=0;y<spr.height;y++)for(int x=0;x<spr.width;x++)
        if(spr.alpha[(size_t)y*spr.width+x]>18)c.Erase(sx+x,sy+y);
}

int GetMonitorHz(POINT p) {
    HMONITOR monitor=MonitorFromPoint(p,MONITOR_DEFAULTTONEAREST);
    MONITORINFOEXW info{};info.cbSize=sizeof(info);
    if(!GetMonitorInfoW(monitor,reinterpret_cast<MONITORINFO*>(&info)))return 60;
    DEVMODEW mode{};mode.dmSize=sizeof(mode);
    if(!EnumDisplaySettingsW(info.szDevice,ENUM_CURRENT_SETTINGS,&mode))return 60;
    int hz=(int)mode.dmDisplayFrequency;
    return (hz>=30 && hz<=1000)?hz:60;
}

bool FullscreenForeground(POINT p) {
    HWND wnd=GetForegroundWindow();
    if(!wnd || IsIconic(wnd)||!IsWindowVisible(wnd))return false;
    if(wnd==GetShellWindow())return false;
    LONG_PTR style=GetWindowLongPtrW(wnd,GWL_STYLE);
    if(style&WS_CAPTION)return false; // Don't hide on normal maximized windows.
    RECT rect{};
    if(!GetWindowRect(wnd,&rect))return false;
    MONITORINFO mi{};mi.cbSize=sizeof(mi);
    if(!GetMonitorInfoW(MonitorFromPoint(p,MONITOR_DEFAULTTONEAREST),&mi))return false;
    return rect.left<=mi.rcMonitor.left+2 && rect.top<=mi.rcMonitor.top+2 &&
           rect.right>=mi.rcMonitor.right-2 && rect.bottom>=mi.rcMonitor.bottom-2;
}

LRESULT CALLBACK OverlayProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    if(msg==WM_NCHITTEST)return HTTRANSPARENT;
    if(msg==WM_MOUSEACTIVATE)return MA_NOACTIVATE;
    return DefWindowProcW(hwnd,msg,wp,lp);
}

struct ParticleImage {
    unsigned width=0,height=0;
    std::vector<unsigned char> bgra;
    bool Valid() const {return width&&height&&bgra.size()==(size_t)width*height*4;}
};
ParticleImage g_image; // worker-thread only, loaded once from an explicitly selected file

void LoadParticleImage(const std::wstring& path) {
    g_image={};
    if(path.size()<5||path.size()>512||path.rfind(L"\\\\",0)==0)return;
    // Only explicit local absolute paths; never download an arbitrary URL or UNC.
    if(path.size()<3||path[1]!=L':'||path[2]!=L'\\')return;
    std::wstring ext=path.substr(path.find_last_of(L'.')==std::wstring::npos?path.size():path.find_last_of(L'.'));
    for(auto& ch:ext)if(ch>=L'A'&&ch<=L'Z')ch=ch-L'A'+L'a';
    if(ext!=L".png"&&ext!=L".bmp")return;
    WIN32_FILE_ATTRIBUTE_DATA data{};
    if(!GetFileAttributesExW(path.c_str(),GetFileExInfoStandard,&data)||
       data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY || data.nFileSizeHigh ||
       data.nFileSizeLow>12*1024*1024u)return;
    IWICImagingFactory* factory=nullptr;
    IWICBitmapDecoder* decoder=nullptr;
    IWICBitmapFrameDecode* frame=nullptr;
    IWICFormatConverter* converter=nullptr;
    HRESULT hr=CoCreateInstance(CLSID_WICImagingFactory,nullptr,CLSCTX_INPROC_SERVER,
                                IID_IWICImagingFactory,(void**)&factory);
    if(SUCCEEDED(hr)&&factory)hr=factory->CreateDecoderFromFilename(
        path.c_str(),nullptr,GENERIC_READ,WICDecodeMetadataCacheOnLoad,&decoder);
    if(SUCCEEDED(hr)&&decoder)hr=decoder->GetFrame(0,&frame);
    UINT w=0,h=0;
    if(SUCCEEDED(hr)&&frame)hr=frame->GetSize(&w,&h);
    if(SUCCEEDED(hr)&&(w<1||h<1||w>128||h>128))hr=E_INVALIDARG;
    if(SUCCEEDED(hr))hr=factory->CreateFormatConverter(&converter);
    if(SUCCEEDED(hr)&&converter)hr=converter->Initialize(frame,
        GUID_WICPixelFormat32bppBGRA,WICBitmapDitherTypeNone,nullptr,0,
        WICBitmapPaletteTypeCustom);
    if(SUCCEEDED(hr)) {
        std::vector<unsigned char> bytes((size_t)w*h*4);
        if(SUCCEEDED(converter->CopyPixels(nullptr,w*4,(UINT)bytes.size(),bytes.data()))) {
            g_image.width=w;g_image.height=h;g_image.bgra=std::move(bytes);
        }
    }
    if(converter)converter->Release();
    if(frame)frame->Release();
    if(decoder)decoder->Release();
    if(factory)factory->Release();
}

void SpawnParticle(std::vector<Particle>& particles,P2 pos,P2 mouseVelocity) {
    if((int)particles.size()>=g_s.particleMax)return;
    float theta=Rand01()*2.f*kPi;
    float speed=(0.35f+Rand01()*0.85f)*g_s.particleSpeed;
    Particle p;
    p.p=pos;
    p.v={std::cos(theta)*speed+mouseVelocity.x*0.04f,
         std::sin(theta)*speed+mouseVelocity.y*0.04f};
    if(g_s.physics==0)p.v.y=-std::fabs(p.v.y)*0.4f;
    if(g_s.physics==2)p.v.y=-std::fabs(p.v.y);
    p.life=g_s.particleLifeMs/1000.0*(0.7+Rand01()*0.6);
    p.size=ClampF(g_s.particleSize*(0.65f+Rand01()*0.7f),1,24);
    p.hue=Rand01()*360;
    p.seed=Rand01()*1000;
    particles.push_back(p);
}

void AdvanceParticles(std::vector<Particle>& particles,double dt,const RECT& bounceBounds,
                      const WindowWall& wall) {
    for(auto& p:particles) {
        P2 before=p.p;
        p.age+=dt;
        if(g_s.usePhysics&&g_s.physicsStrength>0) {
            float k=g_s.physicsStrength/100.f,seconds=(float)dt;
            float swirl=std::sin((float)p.age*7.f+p.seed);
            if(g_s.physics==0)p.v.y+=g_s.gravity*k*seconds;
            if(g_s.physics==2)p.v.y-=g_s.gravity*k*seconds*0.75f;
            if(g_s.physics==3) {float vx=p.v.x; p.v.x-=p.v.y*k*seconds*2.5f;
                                 p.v.y+=vx*k*seconds*2.5f;}
            if(g_s.physics==4) {p.v.x+=swirl*k*seconds*110.f;
                                 p.v.y+=std::cos((float)p.age*6.f+p.seed)*k*seconds*110.f;}
            if(g_s.physics==1 || g_s.physics==4 || g_s.physics==3) {
                p.v.x+=swirl*g_s.turbulence*k*seconds*0.9f;
                p.v.y+=std::cos((float)p.age*5.f+p.seed)*g_s.turbulence*k*seconds*0.9f;
            }
            float drag=g_s.physics==5?0.05f:g_s.drag/100.f*1.9f;
            float damping=std::exp(-drag*seconds);
            p.v.x*=damping;p.v.y*=damping;
            p.p.x+=p.v.x*seconds*k;p.p.y+=p.v.y*seconds*k;
            if(g_s.bounce) {
                if(p.p.x<bounceBounds.left){p.p.x=(float)bounceBounds.left;p.v.x=std::fabs(p.v.x)*0.65f;}
                if(p.p.x>bounceBounds.right){p.p.x=(float)bounceBounds.right;p.v.x=-std::fabs(p.v.x)*0.65f;}
                if(p.p.y<bounceBounds.top){p.p.y=(float)bounceBounds.top;p.v.y=std::fabs(p.v.y)*0.65f;}
                if(p.p.y>bounceBounds.bottom){p.p.y=(float)bounceBounds.bottom;p.v.y=-std::fabs(p.v.y)*0.65f;}
            }
        }
        if(wall.active)WindowWalls::Reflect(before,p.p,p.v,p.size+1.f,wall);
    }
    particles.erase(std::remove_if(particles.begin(),particles.end(),
        [](const Particle& p){return p.age>=p.life;}),particles.end());
}

void DrawParticle(Canvas& c,const Particle& p,double now,double extrapolate,float distanceFade) {
    if(p.life<=0)return;
    float fade=ClampF((float)(1-p.age/p.life),0,1)*ClampF(distanceFade,0,1);
    // Extrapolate only a fraction of one physics step for smooth visual motion.
    float x=p.p.x+(g_s.usePhysics&&g_s.physicsStrength>0?p.v.x*(float)extrapolate:0),
          y=p.p.y+(g_s.usePhysics&&g_s.physicsStrength>0?p.v.y*(float)extrapolate:0);
    RGBColor color=(g_s.allEffectsRGB||g_s.particleRainbow)?
        HSV(p.hue+(float)now*g_s.rainbowSpeed):g_s.particleColor;
    CustomColor(2,color,fade,x,y,0,0,(float)(p.age/p.life),p.seed,now);
    float size=p.size;
    if(g_s.particleShape==10&&g_image.Valid()) {
        size*=g_s.imageScale/100.f;
        int radX=ClampI((int)std::ceil(size*1.7f),1,30);
        int radY=radX;
        if(g_image.width>g_image.height)
            radY=std::max(1,(int)std::round(radX*(float)g_image.height/g_image.width));
        else if(g_image.height>g_image.width)
            radX=std::max(1,(int)std::round(radY*(float)g_image.width/g_image.height));
        for(int dy=-radY;dy<=radY;dy++)for(int dx=-radX;dx<=radX;dx++) {
            float u=(dx+radX+0.5f)/(2.f*radX),v=(dy+radY+0.5f)/(2.f*radY);
            if(u<0||u>=1||v<0||v>=1)continue;
            unsigned ix=(unsigned)std::min((float)g_image.width-1.f,u*g_image.width);
            unsigned iy=(unsigned)std::min((float)g_image.height-1.f,v*g_image.height);
            size_t i=((size_t)iy*g_image.width+ix)*4;
            int a=(int)(g_image.bgra[i+3]*fade*0.85f);
            RGBColor pixel={g_image.bgra[i+2],g_image.bgra[i+1],g_image.bgra[i]};
            if(g_s.allEffectsRGB||g_s.particleRainbow||(g_custom[2]&&g_custom[2]->valid))
                pixel=MixColor(pixel,color,0.55f);
            if(a>0)c.Blend((int)std::round(x)+dx,(int)std::round(y)+dy,pixel,a);
        }
        return;
    }
    int radius=ClampI((int)std::ceil(size*2.5f),2,48);
    for(int dy=-radius;dy<=radius;dy++)for(int dx=-radius;dx<=radius;dx++) {
        float nx=dx/std::max(0.5f,size),ny=dy/std::max(0.5f,size);
        float r=std::sqrt(nx*nx+ny*ny), angle=std::atan2(ny,nx);
        float a=0;
        switch(g_s.particleShape) {
        case 0: a=std::exp(-r*r*2.5f)*1.5f;break; // spark
        case 1: a=std::exp(-r*r*1.5f)*0.55f;break; // smoke
        case 2: a=1.f-Smooth(0.75f,1.10f,r);break;
        case 3: {float edge=0.55f+0.38f*std::cos(angle*5.f);a=1.f-Smooth(edge-0.15f,edge+0.12f,r);break;}
        case 4: a=1.f-Smooth(0.78f,1.06f,std::fabs(nx)+std::fabs(ny));break;
        case 5: a=1.f-Smooth(0.80f,1.12f,std::max(std::fabs(nx),std::fabs(ny)));break;
        case 6: a=Smooth(0.55f,0.72f,r)*(1.f-Smooth(0.88f,1.06f,r));break;
        case 7: a=Smooth(0.60f,0.82f,r)*(1.f-Smooth(0.96f,1.08f,r));break;
        case 8: {
            float spokes=std::fabs(std::sin(angle*3.f));
            a=(1.f-Smooth(0.92f,1.1f,r))*
              (1.f-Smooth(0.16f,0.36f,spokes*r));break;
        }
        case 9: {float hx=nx,hy=-ny-0.15f;
            float f=std::pow(hx*hx+hy*hy-0.6f,3.f)-hx*hx*std::pow(hy,3.f);
            a=1.f-Smooth(-0.10f,0.14f,f);break;}
        default: a=std::exp(-r*r*2.5f);break;
        }
        if(a>0.01f)c.Blend((int)std::round(x)+dx,(int)std::round(y)+dy,
                           color,(int)(255*ClampF(a*fade*0.83f,0.f,1.f)));
    }
}


// Experimental cursor additions use existing layers and worker lifetime.
static RGBColor ExtraColor(const float (&rgb)[3]){return {(BYTE)(rgb[0]*255),(BYTE)(rgb[1]*255),(BYTE)(rgb[2]*255)};}
static void DrawCometCopy(Canvas& c,const CursorSprite& spr,P2 hotspot,RGBColor tint,float fade,float scale){
    if(spr.width<1||spr.height<1||fade<=0)return;
    int w=std::max(1,(int)std::round(spr.width*scale)),h=std::max(1,(int)std::round(spr.height*scale));
    int left=(int)std::round(hotspot.x-spr.hotspotX*scale),top=(int)std::round(hotspot.y-spr.hotspotY*scale);
    for(int y=0;y<h;y++)for(int x=0;x<w;x++){
        int sx=std::min(spr.width-1,(int)(x/scale)),sy=std::min(spr.height-1,(int)(y/scale));
        int a=(int)(spr.alpha[(size_t)sy*spr.width+sx]*fade);
        if(a)c.Blend(left+x,top+y,tint,a);
    }
}
static void DrawClickRings(Canvas& canvas,const ExperimentalExtras::ClickState& clicks,double now){
    for(const auto& ring:clicks.rings){
        float u=ExperimentalExtras::Unit((float)((now-ring.born)/(g_extra.shockMS/1000.)));
        float radius=4+(g_extra.shockRadius-4)*(1-(1-u)*(1-u));
        float alpha=(1-u)*(1-u)*g_extra.shockOpacity;
        RGBColor color=ExtraColor(g_extra.clickColors[ring.button]);
        int count=std::clamp((int)(radius*6.2831853f/2),24,512);
        for(int i=0;i<count;i++){float angle=i*6.2831853f/count;
            canvas.Dot(ring.x+cosf(angle)*radius,ring.y+sinf(angle)*radius,(float)g_extra.shockWidth,color,alpha*.5f);}
    }
}

// Continuous ribbon: path spacing is based on physical travel distance, NOT FPS.
// The live cursor position closes the most recent segment even if the sampler
// did not yet add another persistent history point.
void DrawTrail(Canvas& c,const std::deque<TrailPoint>& trail,const CursorSprite& sprite,
               double now,P2 current) {
    if(!g_s.trail || trail.empty())return;
    double lifetime=g_s.trailLengthMs/1000.0;
    if(g_extra.comet){
        size_t stride=std::max<size_t>(1,(trail.size()+g_extra.cometCopies-1)/g_extra.cometCopies);
        for(size_t i=0;i<trail.size();i+=stride){
            float fresh=ClampF(1.f-(float)((now-trail[i].birth)/lifetime),0,1);
            float dx=trail[i].p.x-current.x,dy=trail[i].p.y-current.y;
            float fade=fresh*fresh*g_extra.cometOpacity*(1-Smooth((float)g_s.overlayRadius-65.f,(float)g_s.overlayRadius,sqrtf(dx*dx+dy*dy)));
            RGBColor color=(g_s.allEffectsRGB||g_s.trailRainbow)?HSV((float)now*g_s.rainbowSpeed+i*6):g_s.trailColor;
            CustomColor(1,color,fade,trail[i].p.x,trail[i].p.y,(float)i/std::max(1.f,(float)trail.size()-1),0,1-fresh,0,now);
            DrawCometCopy(c,sprite,trail[i].p,color,fade,g_extra.cometScale+(1-g_extra.cometScale)*fresh);
        }
        return;
    }
    auto paintSegment=[&](P2 a,P2 b,float f0,float f1,int idx) {
        float dx=b.x-a.x,dy=b.y-a.y;
        float dist=std::sqrt(dx*dx+dy*dy);
        float left=(float)c.originX-30.f,right=(float)(c.originX+c.activeW)+30.f;
        float top=(float)c.originY-30.f,bottom=(float)(c.originY+c.activeH)+30.f;
        if(std::max(a.x,b.x)<left||std::min(a.x,b.x)>right||
           std::max(a.y,b.y)<top||std::min(a.y,b.y)>bottom)return;
        int steps=ClampI((int)std::ceil(dist/1.6f),1,160);
        for(int k=0;k<=steps;k++) {
            float u=(float)k/steps;
            float fade=f0+(f1-f0)*u;
            float xx=a.x+dx*u,yy=a.y+dy*u;
            float dMouse=std::sqrt((xx-current.x)*(xx-current.x)+
                                   (yy-current.y)*(yy-current.y));
            fade*=1.f-Smooth((float)g_s.overlayRadius-65.f,(float)g_s.overlayRadius,dMouse);
            if(fade<=0.002f)continue;
            RGBColor color=(g_s.allEffectsRGB||g_s.trailRainbow)?
                HSV((float)now*g_s.rainbowSpeed+idx*5+u*8):g_s.trailColor;
            CustomColor(1,color,fade,xx,yy,u,0,1-fade,0,now);
            float width=g_s.trailWidth*(0.28f+0.72f*fade);
            if(g_s.trailStyle==3)width*=1.45f;
            if(g_s.trailStyle==4) {
                float bend=std::sin((float)now*3.f+idx*0.5f+u*3.14f)*
                           width*0.32f;
                float inv=dist>0.01f?1.f/dist:0.f;
                c.Dot(a.x+dx*u-dy*inv*bend,a.y+dy*u+dx*inv*bend,
                      width,color,fade*0.28f);
            }else c.Dot(a.x+dx*u,a.y+dy*u,width,color,
                         fade*(g_s.trailStyle==3?0.15f:0.23f));
        }
    };
    for(size_t i=0;i<trail.size();i++) {
        float fade=ClampF(1.f-(float)((now-trail[i].birth)/lifetime),0,1);
        float dx=trail[i].p.x-current.x,dy=trail[i].p.y-current.y;
        fade*=1.f-Smooth((float)g_s.overlayRadius-65.f,
                         (float)g_s.overlayRadius,std::sqrt(dx*dx+dy*dy));
        if(fade<=0)continue;
        RGBColor color=(g_s.allEffectsRGB||g_s.trailRainbow)?
            HSV((float)now*g_s.rainbowSpeed+i*6):g_s.trailColor;
        if(g_s.trailStyle==1||g_s.trailStyle==2)CustomColor(1,color,fade,trail[i].p.x,trail[i].p.y,(float)i/std::max(1.f,(float)trail.size()-1),0,1-fade,0,now);
        if(g_s.trailStyle==2) {
            if(i%5==0)DrawSpriteAfterimage(c,sprite,trail[i].p,color,fade);
        }else if(g_s.trailStyle==1) {
            c.Dot(trail[i].p.x,trail[i].p.y,g_s.trailWidth*0.75f,color,fade*0.6f);
        }else {
            if(i+1<trail.size()){
                float next=ClampF(1.f-(float)((now-trail[i+1].birth)/lifetime),0,1);
                paintSegment(trail[i].p,trail[i+1].p,fade,next,(int)i);
            }else paintSegment(trail[i].p,current,fade,1.f,(int)i);
        }
    }
}

bool PresentScene(HWND wnd,Canvas& canvas,POINT where) {
    RenderTiming::Scope timing(RenderTiming::CursorUpload);
    if(!canvas.pixels)return false;
    SIZE size={canvas.activeW,canvas.activeH};
    POINT src={0,0};
    POINT dst={canvas.originX,canvas.originY};
    BLENDFUNCTION blend={AC_SRC_OVER,0,255,AC_SRC_ALPHA};
    HDC screen=GetDC(nullptr);
    if(!screen)return false;
    BOOL ok=UpdateLayeredWindow(wnd,screen,&dst,&size,canvas.dc,&src,0,&blend,ULW_ALPHA);
    ReleaseDC(nullptr,screen);
    (void)where;
    return ok!=FALSE;
}

void HideOverlay(HWND wnd) { if(IsWindowVisible(wnd))ShowWindow(wnd,SW_HIDE); }

void WaitUntil(HANDLE timer,double seconds) {
    if(seconds<=0)return;
    if(timer) {
        LARGE_INTEGER due{};
        due.QuadPart=-(LONGLONG)std::max(1.0,seconds*10000000.0);
        if(SetWaitableTimer(timer,&due,0,nullptr,nullptr,FALSE)) {
            HANDLE handles[2]={g_stopEvent,timer};
            WaitForMultipleObjects(2,handles,FALSE,INFINITE);
            return;
        }
    }
    DWORD ms=(DWORD)std::max(1.0,seconds*1000.0);
    WaitForSingleObject(g_stopEvent,ms);
}


DWORD WINAPI WorkerThread(LPVOID) {
    PerformanceOptions::WorkerPriority();
    HRESULT comResult=CoInitializeEx(nullptr,COINIT_MULTITHREADED);
    bool ownCom=SUCCEEDED(comResult);
    if(g_s.particleShape==10 && (ownCom||comResult==RPC_E_CHANGED_MODE)) {
        LoadParticleImage(g_s.particleImagePath);
        if(!g_image.Valid()) Wh_Log(L"Cursor: custom image particle unavailable; using spark fallback.");
    }
    const HINSTANCE inst=(HINSTANCE)GetModuleHandleW(nullptr);
    const wchar_t* name=L"WindhawkDodiCursorOverlayWindow";
    WNDCLASSEXW wc{};wc.cbSize=sizeof(wc);wc.hInstance=inst;
    wc.lpfnWndProc=OverlayProc;wc.lpszClassName=name;
    if(!RegisterClassExW(&wc) && GetLastError()!=ERROR_CLASS_ALREADY_EXISTS){
        if(ownCom)CoUninitialize();return 0;
    }
    HWND wnd=CreateWindowExW(WS_EX_LAYERED|WS_EX_TRANSPARENT|WS_EX_TOPMOST|
        WS_EX_NOACTIVATE|WS_EX_TOOLWINDOW,name,L"",WS_POPUP,0,0,1,1,
        nullptr,nullptr,inst,nullptr);
    if(!wnd){UnregisterClassW(name,inst);if(ownCom)CoUninitialize();return 0;}
    if(g_s.capture)SetWindowDisplayAffinity(wnd,WDA_EXCLUDEFROMCAPTURE);
    HWND glowWnd=nullptr;
    if(PerformanceOptions::follow.load()&&g_s.glow)glowWnd=CreateWindowExW(WS_EX_LAYERED|WS_EX_TRANSPARENT|WS_EX_TOPMOST|
        WS_EX_NOACTIVATE|WS_EX_TOOLWINDOW,name,L"",WS_POPUP,0,0,1,1,
        nullptr,nullptr,inst,nullptr);
    if(glowWnd&&g_s.capture)SetWindowDisplayAffinity(glowWnd,WDA_EXCLUDEFROMCAPTURE);
    bool separateGlow=glowWnd!=nullptr;
    Canvas canvas, renderCanvas,glowCanvas;
    bool glowShown=false;double nextGlowPresent=0;
    std::vector<Particle> particles;particles.reserve(kMaxParticlesHard);
    std::deque<TrailPoint> trail;
    ExperimentalExtras::ClickState clicks;
    CursorSprite sprite;
    DodiFluid::FluidGrid fluid;
    double lastFluidTick=0, lastFluidInputAt=0, fluidAccumulator=0;
    bool fluidVisualPending=false;
    P2 lastFluidMouse{};
    bool fluidStarted=false;
    HMONITOR fluidMonitor=nullptr;
    DodiFluid::Rect fluidBounds{};
    HCURSOR lastTriedCursor=nullptr;double lastCursorAttempt=0;
    HANDLE timer=CreateWaitableTimerExW(nullptr,nullptr,0x00000002,TIMER_ALL_ACCESS);
    if(!timer)timer=CreateWaitableTimerW(nullptr,FALSE,nullptr);
    POINT last{};
    P2 lastSampled{};
    bool hasLast=false,hasSample=false;
    double now=Seconds(),lastTick=now,nextSample=now,nextGlow=now;
    double lastMotion=now,lastRefresh=0,simAccumulator=0,emission=0,trailCarry=0;
    // Each layer has independent previous/current frames, cadence and cleanup.
    // 0: trails, 1: particles, 2: outline, 3: fluid simulation.
    DodiFrameSmoothing::FrameSmoother layers[4];
    double nextLayer[4]={now,now,now,now};
    bool previousActive[4]={false,false,false,false};
    double nextPresentation=now;
    bool cursorPositionDirty=false;
    std::vector<uint32_t> lastPresented;
    int lastLeft=0,lastTop=0,lastWidth=0,lastHeight=0;
    bool haveLastPresentation=false;
    auto ResetSmoothing=[&]() {
        for(int i=0;i<4;i++){
            layers[i].Reset();previousActive[i]=false;nextLayer[i]=now;
        }
        lastPresented.clear();haveLastPresentation=false;
        nextPresentation=now;cursorPositionDirty=false;
    };
    HMONITOR lastMonitor=nullptr;
    WindowWall wall{};double nextWallCheck=0;
    float estimatedSpeed=0,glowPhase=0;
    int monitorHz=60,speedTier=1;
    bool wasVisible=false;
    int particleDrawCap=g_s.particleMax;
    int busyFrames=0;
    while(WaitForSingleObject(g_stopEvent,0)==WAIT_TIMEOUT) {
        MSG msg;
        while(PeekMessageW(&msg,nullptr,0,0,PM_REMOVE)) {
            if(msg.message==WM_QUIT)break;
            TranslateMessage(&msg);DispatchMessageW(&msg);
        }
        now=Seconds();
        double delta=std::min(0.08,std::max(0.0,now-lastTick));
        lastTick=now;
        CURSORINFO ci{};ci.cbSize=sizeof(ci);
        POINT pos{};
        if(!g_s.enabled || !GetCursorInfo(&ci) || !(ci.flags&CURSOR_SHOWING) ||
           !GetCursorPos(&pos) || (g_s.fullscreen && FullscreenForeground(pos))) {
            HideOverlay(wnd);wasVisible=false;
            if(glowWnd){HideOverlay(glowWnd);glowShown=false;}
            hasLast=hasSample=false;trail.clear();particles.clear();clicks.Clear();
            simAccumulator=emission=trailCarry=0;
            fluidStarted=false;fluidMonitor=nullptr;fluidBounds={};
            fluidAccumulator=0;fluidVisualPending=false;
            ResetSmoothing();
            wall={};nextWallCheck=0;
            WaitUntil(timer,0.10);continue;
        }
        if(g_extra.shock){
            bool swapped=GetSystemMetrics(SM_SWAPBUTTON)!=0;
            clicks.Update((GetAsyncKeyState(swapped?VK_RBUTTON:VK_LBUTTON)&0x8000)!=0,(GetAsyncKeyState(swapped?VK_LBUTTON:VK_RBUTTON)&0x8000)!=0,
                (float)pos.x,(float)pos.y,now,g_extra.shockMS);
            if(!clicks.rings.empty())lastMotion=now;
        }
        if(g_s.windowCollisions && now>=nextWallCheck){
            WindowWall detected=WindowWalls::Detect(wnd,pos);
            int policy=PerformanceOptions::enabled.load()?PerformanceOptions::wallPolicy.load():0;
            nextWallCheck=now+(policy==1?(speedTier==0?.5:.15):policy==2?(speedTier==0?.1:.033):(speedTier==0?.25:.075));
            if(!WindowWalls::Same(detected,wall)){
                wall=detected;
                // A changed wall invalidates interpolated effect surfaces,
                // but does not mutate the pointer or erase existing dye.
                for(int layer:{0,1,3}){layers[layer].Reset();nextLayer[layer]=now;}
                haveLastPresentation=false;
            }
        }
        HMONITOR currentMonitor=MonitorFromPoint(pos,MONITOR_DEFAULTTONEAREST);
        if(currentMonitor!=lastMonitor || now-lastRefresh>=1.0 || !hasLast) {
            monitorHz=GetMonitorHz(pos);
            lastMonitor=currentMonitor;lastRefresh=now;
        }
        const bool handleChanged=ci.hCursor!=lastTriedCursor;
        int sourcePolicy=PerformanceOptions::enabled.load()?PerformanceOptions::cursorPoll.load():0;
        double sourceDelay=sourcePolicy==1?(now-lastMotion<.4?.5:1.5):
            sourcePolicy==2?(now-lastMotion<.4?.1:.5):(now-lastMotion<.4?.33:.85);
        const bool pollSource=now-lastCursorAttempt>=sourceDelay;
        if(handleChanged || pollSource || (sprite.width==0 && now-lastCursorAttempt>2.0)) {
            lastTriedCursor=ci.hCursor;lastCursorAttempt=now;
            CursorSprite refreshed;
            const int systemSize=EffectiveCursorBufferSize(pos);
            bool sourceUnchanged=false;
            if(AcquireCursor(ci.hCursor,refreshed,g_s.glowRadius,systemSize,(!PerformanceOptions::enabled.load()||PerformanceOptions::artworkCache.load())?&sprite:nullptr,&sourceUnchanged)){
                if(sourceUnchanged){sprite.handle=ci.hCursor;}
                else {
                    const bool changed=refreshed.width!=sprite.width ||
                        refreshed.height!=sprite.height ||
                        refreshed.hotspotX!=sprite.hotspotX ||
                        refreshed.hotspotY!=sprite.hotspotY ||
                        refreshed.alpha!=sprite.alpha ||
                        refreshed.colors!=sprite.colors;
                    // Retain the latest source cache even if its scaled output matches.
                    sprite=std::move(refreshed);
                    if(changed){
                        // New silhouette: do not blend the old cursor shape into it.
                        // World-space fluid ink remains independent.
                        ResetSmoothing();
                    }
                }
            }else if(handleChanged && sprite.width){
                sprite=CursorSprite{};
                ResetSmoothing();
            }
        }
        if(!hasLast) {
            last=pos;lastSampled={(float)pos.x,(float)pos.y};
            hasLast=hasSample=true;lastMotion=now;
            estimatedSpeed=0;trailCarry=0;
        }
        float dx=(float)(pos.x-last.x),dy=(float)(pos.y-last.y);
        float distance=std::sqrt(dx*dx+dy*dy);
        bool moved=distance>=0.5f;
        // Wake the stationary outline immediately ONCE when motion begins,
        // not on every mouse poll. Previously nextLayer was forced to 'now'
        // every poll (about 110-170Hz), bypassing Fixed/Auto/Adaptive and the
        // GlowAnimationFPS limit for static and Neon outlines while moving.
        bool motionBegan=moved && now-lastMotion>=0.18;
        if(distance>900.f) {
            motionBegan=false;
            trail.clear();particles.clear();emission=trailCarry=0;
            lastSampled={(float)pos.x,(float)pos.y};
            dx=dy=distance=0;moved=false;
            fluidStarted=false;fluidMonitor=nullptr;fluidBounds={};
            fluidAccumulator=0;fluidVisualPending=false;
            ResetSmoothing();
        }
        if(moved) {
            float instant=(float)(distance/std::max(delta,0.002));
            estimatedSpeed=estimatedSpeed*0.67f+instant*0.33f;
            lastMotion=now;
            if(g_s.particles && g_s.particleMax>0 && g_s.particlePer100>0) {
                float perPixel=(float)g_s.particlePer100/100.f;
                float pending=emission+distance*perPixel;
                int count=ClampI((int)pending,0,std::min(80,g_s.particleMax-(int)particles.size()));
                float velX=dx/std::max((float)delta,0.002f);
                float velY=dy/std::max((float)delta,0.002f);
                for(int j=0;j<count;j++) {
                    float f=ClampF((((float)j+1.f)-emission)/(distance*perPixel),0,1);
                    SpawnParticle(particles,{last.x+dx*f,last.y+dy*f},{velX,velY});
                }
                emission=std::fmod(std::max(0.f,pending),1.f);
            }
        }else estimatedSpeed*=0.89f;
        if(g_s.trail && now>=nextSample && hasSample) {
            float sx=(float)pos.x-lastSampled.x,sy=(float)pos.y-lastSampled.y;
            float len=std::sqrt(sx*sx+sy*sy);
            if(trail.empty()&&len>0.1f) {
                trail.push_back({lastSampled,now-delta});trailCarry=0;
            }
            const float spacing=(float)g_s.trailSpacing;
            if(len>=0.1f) {
                float travelled=spacing-trailCarry;
                int count=0;
                while(travelled<=len && count++<kMaxTrailPoints) {
                    float t=travelled/len;
                    trail.push_back({{lastSampled.x+sx*t,lastSampled.y+sy*t},
                                     now-delta*(1.0-t)});
                    if((int)trail.size()>kMaxTrailPoints)trail.pop_front();
                    travelled+=spacing;
                }
                trailCarry=std::fmod(trailCarry+len,spacing);
            }
            lastSampled={(float)pos.x,(float)pos.y};
            nextSample=now+1.0/g_s.sampleFps;
        }
        if(now-lastMotion>0.18)speedTier=0;
        else if(estimatedSpeed>g_s.fastThreshold*1.12f)speedTier=2;
        else if(estimatedSpeed<g_s.fastThreshold*0.78f)speedTier=1;
        int fps=g_s.renderMode==0?g_s.fixedFps:
               g_s.renderMode==1?monitorHz:
               speedTier==0?g_s.idleFps:(speedTier==1?monitorHz/2:monitorHz);
        fps=PerformanceOptions::Cap(ClampI(fps,15,g_s.maxFps));
        // Select fluid solver cadence separately from global draw cadence.
        // Independent is deliberately the default to preserve v0.4.3 visuals.
        int fluidTargetFPS=std::max(15,std::min(g_s.fluidFPS,
            g_s.fluidFPSMode==0 ? g_s.fluidFPS :
            g_s.fluidFPSMode==1 ? fps :
            speedTier==0 ? g_s.fluidIdleFPS : (speedTier==1 ? 30 : 60)));
        if(PerformanceOptions::enabled.load()&&PerformanceOptions::solverCap.load()>0)
            fluidTargetFPS=std::min(fluidTargetFPS,PerformanceOptions::solverCap.load());
        // The renderer should only re-rasterize ink after the fluid grid or
        // its visible geometry changes; cached pixels are reused otherwise.
        bool fluidGridChanged=false;
        // Fluid simulation runs at a source rate that may now be adapted;
        // simulation time is independent of the compositing/presentation FPS.
        const bool liquidActive=(g_s.fluidEnabled||(g_s.glow&&g_s.glowMode==10));
        if(liquidActive){
            P2 mouse={(float)pos.x,(float)pos.y};
            if(!fluidStarted || (g_s.fluidRadius==0 && fluidMonitor!=currentMonitor)){
                if(g_s.fluidRadius==0){
                    MONITORINFO fm{};fm.cbSize=sizeof(fm);
                    RECT screen={pos.x-320,pos.y-320,pos.x+320,pos.y+320};
                    if(GetMonitorInfoW(currentMonitor,&fm))screen=fm.rcMonitor;
                    float mx=(screen.left+screen.right)*0.5f;
                    float my=(screen.top+screen.bottom)*0.5f;
                    float r=std::max(screen.right-screen.left,
                                     screen.bottom-screen.top)*0.57f;
                    fluid.Reset({mx,my},g_s.fluidResolution,r);
                }else fluid.Reset({mouse.x,mouse.y},g_s.fluidResolution,
                                 (float)g_s.fluidRadius);
                lastFluidTick=lastFluidInputAt=now;
                fluidAccumulator=0;
                lastFluidMouse=mouse;fluidStarted=true;
                fluidMonitor=currentMonitor;
                fluidGridChanged=true;
            }
            // Cursor-local coverage follows the cursor; monitor coverage
            // stays anchored. Recenter reports whether grid cells shifted.
            if(g_s.fluidRadius>0)
                fluidGridChanged|=fluid.Recenter({mouse.x,mouse.y});
            DodiFluid::Rect fluidWall{};
            if(wall.active){fluidWall={ (int)wall.box.left,(int)wall.box.top,
                                    (int)wall.box.right,(int)wall.box.bottom,false };}
            fluidGridChanged|=fluid.ConfigureWall(wall.active,wall.contain,fluidWall);
            const float fx=mouse.x-lastFluidMouse.x,fy=mouse.y-lastFluidMouse.y;
            const float length=std::sqrt(fx*fx+fy*fy);
            const bool inputPending=length>0.25f && length<900.f;
            const double realElapsed=std::max(0.0,now-lastFluidTick);
            lastFluidTick=now;
            // Once no rendered ink or new input remains, perform ZERO solver
            // passes. Keeping lastFluidTick current prevents a huge catch-up
            // jump on the next movement.
            if(fluid.Peak()<0.006f && !inputPending){
                fluidAccumulator=0;
            }else{
                fluidAccumulator=std::min(0.20,fluidAccumulator+
                                          std::min(realElapsed,0.20));
                const double sourceStep=1.0/fluidTargetFPS;
                int steps=(int)((fluidAccumulator+1.e-8)/sourceStep);
                // A stall must not trigger an unbounded number of pressure
                // solves; cap catch-up and discard excess backlog.
                steps=std::max(0,std::min(3,steps));
                if(steps>0){
                    if(inputPending){
                        int count=ClampI((int)std::ceil(length/
                            std::max(5.f,g_s.fluidSplat*0.8f)),1,35);
                        const double inputElapsed=std::max(sourceStep,
                            std::min(0.25,now-lastFluidInputAt));
                        DodiFluid::Vec2 velocity={fx/(float)inputElapsed,
                                                  fy/(float)inputElapsed};
                        for(int j=0;j<count;j++){
                            float u=(j+0.5f)/count;
                            auto color1=g_s.fluidColor1,color2=g_s.fluidColor2;
                            RGBColor col=MixColor(color1,color2,
                                .5f+.5f*std::sin((float)now*2.f+j*0.24f));
                            if(g_s.fluidRainbow||g_s.allEffectsRGB)
                                col=HSV((float)now*g_s.rainbowSpeed+u*45.f+j*3.f);
                            float countScale=std::max(.18f,1.f/std::sqrt((float)count));
                            fluid.Splat({lastFluidMouse.x+fx*u,lastFluidMouse.y+fy*u},
                                    velocity,{col.r/255.f,col.g/255.f,col.b/255.f},
                                    (float)g_s.fluidSplat*1.2f,
                                    g_s.fluidForce/100.f*0.22f*countScale);
                        }
                    }
                    // Fixed-duration steps preserve damping and movement at
                    // different solver rates, including slow/fast transitions.
                    for(int j=0;j<steps;j++)
                        fluid.Step((float)sourceStep,
                            g_s.fluidVelocityRetention/100.f,
                            g_s.fluidDyeRetention/100.f,
                            g_s.fluidVorticity/3.f,
                            g_s.fluidPressureIterations);
                    fluidAccumulator-=steps*sourceStep;
                    if(fluidAccumulator>=sourceStep)
                        fluidAccumulator=0; // bounded catch-up after stalls
                    lastFluidMouse=mouse;lastFluidInputAt=now;
                    fluidGridChanged=true;
                }
            }
            if(fluidGridChanged){
                fluidBounds=fluid.DyeBounds(0.006f);
                fluidVisualPending=true;
            }
        }else {
            fluidStarted=false;fluidMonitor=nullptr;fluidBounds={};
            fluidAccumulator=0;fluidVisualPending=false;
        }
        last=pos;
        while(!trail.empty()&&now-trail.front().birth>g_s.trailLengthMs/1000.0)
            trail.pop_front();
        simAccumulator=std::min(0.16,simAccumulator+delta);
        double simStep=1.0/g_s.simFps;
        MONITORINFO mInfo{};mInfo.cbSize=sizeof(mInfo);
        RECT bounds{};
        if(GetMonitorInfoW(currentMonitor,&mInfo))bounds=mInfo.rcMonitor;
        else bounds={-32768,-32768,32767,32767};
        int steps=0;
        while(simAccumulator>=simStep && steps++<8) {
            AdvanceParticles(particles,simStep,bounds,wall);simAccumulator-=simStep;
        }
        if(simAccumulator>=simStep)simAccumulator=0;
        if(now>=nextGlow) {
            glowPhase=(float)std::fmod(now*g_s.rainbowSpeed,360.0);
            nextGlow=now+1.0/g_s.glowFps;
        }
        bool dynamicGlow=g_s.glow&&((g_custom[0]&&g_custom[0]->valid&&g_custom[0]->program.usesTime)||g_s.glowMode>1||
            (g_s.allEffectsRGB && g_s.rainbowSpeed>0));
            bool active[4]={g_s.trail&&!trail.empty(),(g_s.particles&&!particles.empty())||!clicks.rings.empty(),
                        g_s.glow&&sprite.width>0,liquidActive&&!fluidBounds.empty};
        int rates[4]={std::max(1,std::min(fps,g_s.sampleFps)),
                      std::max(1,std::min(fps,g_s.simFps)),
                      std::max(1,std::min(fps,g_s.glowFps)),
                      std::max(1,std::min(fps,fluidTargetFPS))};
        bool bypass[4];
        for(int i=0;i<4;i++)bypass[i]=!g_s.smoothing||rates[i]>=60;
        // Cursor-local outline position must update independently of the
        // expensive source FPS clock, even after movement STOPS. Otherwise a
        // static outline can be stranded at its previous source-frame spot.
        // Keep both smoother frames on the same hotspot to avoid ghost images.
        for(int layer:{2})if(active[layer])
            cursorPositionDirty=layers[layer].AnchorAt(
                (int)std::round(pos.x)-sprite.hotspotX,
                (int)std::round(pos.y)-sprite.hotspotY)||cursorPositionDirty;
        // Calculate *each effect* only at its own source clock, even if a
        // smoother presentation tick is due. A source layer is cleared once
        // after it deactivates, allowing the last content to fade to zero.
        for(int i=0;i<4;i++){
            if(!active[i]&&!previousActive[i])continue;
            bool animated=i==0?true:i==1?true:i==2?dynamicGlow:liquidActive;
            if(i==3 && active[i] && !fluidVisualPending &&
               layers[i].Ready() && previousActive[i]){
                nextLayer[i]=now+1.0/rates[i];
                continue;
            }
            if(active[i]&&!animated&&!moved&&layers[i].Ready()&&previousActive[i]){
                // Avoid busy-spinning on an expired source deadline when a
                // static cursor layer is unchanged. The cursor is still polled
                // normally and moving wakes calculation immediately.
                nextLayer[i]=now+0.05;
                continue;
            }
            if(motionBegan&&!animated)nextLayer[i]=now;
            if(now+0.00001<nextLayer[i])continue;
            int left=(int)pos.x-8,top=(int)pos.y-8,width=16,height=16;
            if(!active[i] && layers[i].Ready()){
                layers[i].CurrentGeometry(left,top,width,height);
            }else if(i==3 && !fluidBounds.empty){
                // Fluid occupies screen coordinates and must not inherit the
                // general 440px radius used by trails/particles.
                left=std::max(fluidBounds.left,static_cast<int>(bounds.left));
                top=std::max(fluidBounds.top,static_cast<int>(bounds.top));
                int right=std::min(fluidBounds.right,static_cast<int>(bounds.right));
                int bottom=std::min(fluidBounds.bottom,static_cast<int>(bounds.bottom));
                width=std::max(1,right-left);
                height=std::max(1,bottom-top);
            }else if(i==2){
                left=(int)std::round(pos.x)-sprite.hotspotX;
                top=(int)std::round(pos.y)-sprite.hotspotY;
                width=sprite.width;height=sprite.height;
            }else{
                float x0=(float)pos.x,x1=x0,y0=(float)pos.y,y1=y0;
                const float pad=i==0?std::max(g_extra.comet?(float)std::max(sprite.width,sprite.height):28.f,g_s.trailWidth*3.f):
                    std::max(28.f,g_s.particleSize*4.f*g_s.imageScale/100.f);
                if(i==0)for(const auto& point:trail){
                    x0=std::min(x0,point.p.x);x1=std::max(x1,point.p.x);
                    y0=std::min(y0,point.p.y);y1=std::max(y1,point.p.y);
                }
                if(i==1)for(const auto& ring:clicks.rings){
                    float extent=(float)g_extra.shockRadius+g_extra.shockWidth*3;
                    x0=std::min(x0,ring.x-extent);x1=std::max(x1,ring.x+extent);
                    y0=std::min(y0,ring.y-extent);y1=std::max(y1,ring.y+extent);
                }
                if(i==1)for(const auto& particle:particles){
                    x0=std::min(x0,particle.p.x);x1=std::max(x1,particle.p.x);
                    y0=std::min(y0,particle.p.y);y1=std::max(y1,particle.p.y);
                }
                const float r=(float)g_s.overlayRadius;
                x0=std::max(x0-pad,(float)pos.x-r);x1=std::min(x1+pad,(float)pos.x+r);
                y0=std::max(y0-pad,(float)pos.y-r);y1=std::min(y1+pad,(float)pos.y+r);
                left=(int)std::floor(x0)-2;top=(int)std::floor(y0)-2;
                width=ClampI(((int)std::ceil(x1)-left+17)/16*16,16,kMaxWindowDimension);
                height=ClampI(((int)std::ceil(y1)-top+17)/16*16,16,kMaxWindowDimension);
            }
            if(renderCanvas.Resize(width,height)){
                RenderTiming::Scope timing(RenderTiming::CursorSource);
                double calcStart=Seconds();
                renderCanvas.originX=left;renderCanvas.originY=top;
                renderCanvas.Clear();
                if(active[i]){
                    if(i==0)DrawTrail(renderCanvas,trail,sprite,now,{(float)pos.x,(float)pos.y});
                    if(i==1){
                        if(g_extra.shock)DrawClickRings(renderCanvas,clicks,now);
                        int first=std::max(0,(int)particles.size()-particleDrawCap);
                        for(int p=first;p<(int)particles.size();p++){
                            const auto& part=particles[(size_t)p];
                            float dx=part.p.x-pos.x,dy=part.p.y-pos.y;
                            float dist=std::sqrt(dx*dx+dy*dy);
                            float fade=1.f-Smooth((float)g_s.overlayRadius-65.f,
                                                  (float)g_s.overlayRadius,dist);
                            if(fade>0.002f)DrawParticle(renderCanvas,part,now,
                                std::min(simAccumulator,simStep),fade);
                        }
                    }
                    if(i==2)DrawSpriteGlow(renderCanvas,sprite,{(float)pos.x,(float)pos.y},now,glowPhase);
                    if(i==3)DrawFluidDye(renderCanvas,fluid,g_s.fluidOpacity/100.f);
                }
                if(g_extra.palette&&g_extra.paletteCursor){
                    float phases[kMaxWindowDimension*2];
                    int phaseCount=renderCanvas.activeW+renderCanvas.activeH-1;
                    for(int diagonal=0;diagonal<phaseCount;diagonal++)phases[diagonal]=.5f+.5f*sinf(diagonal*.004f);
                    for(int y=0;y<renderCanvas.activeH;y++)for(int x=0;x<renderCanvas.activeW;x++){
                        size_t index=(size_t)y*renderCanvas.w+x;
                        renderCanvas.pixels[index]=ExperimentalExtras::Palette(renderCanvas.pixels[index],g_extra,phases[x+y]);
                    }
                }
                // Window walls never affect the actual pointer/outline. The
                // 3 independent effect layers are visually confined to the
                // chosen window or cut away from inactive windows.
                if(i!=2)ClipVisualLayerToWall(renderCanvas,wall);
                // Frame dimensions include only active pixels. The DIB memory
                // can be larger; copy only the valid rows into this layer.
                layers[i].PushFrame(renderCanvas.pixels,renderCanvas.w,
                    renderCanvas.activeW,renderCanvas.activeH,left,top,now,1.0/rates[i],
                    !PerformanceOptions::enabled.load()||!PerformanceOptions::leanHistory.load()||!bypass[i]);
                if(i==3)fluidVisualPending=false;
                if(i==1 && g_s.adaptiveQuality && g_s.particleMax>25){
                    double cost=Seconds()-calcStart;
                    if(cost>0.86/rates[i])busyFrames=std::min(12,busyFrames+1);
                    else busyFrames=std::max(-12,busyFrames-1);
                    if(busyFrames>=4){particleDrawCap=std::max(25,particleDrawCap-12);busyFrames=0;}
                    else if(busyFrames<=-10){particleDrawCap=std::min(g_s.particleMax,particleDrawCap+4);busyFrames=0;}
                }
            }
            previousActive[i]=active[i];
            nextLayer[i]=now+1.0/rates[i];
        }
        if(separateGlow){
            const bool visible=layers[2].Ready()&&layers[2].ContentVisible(now,g_s.smoothing,bypass[2]);
            bool changed=cursorPositionDirty||layers[2].NeedsPresentation(now,g_s.smoothing,bypass[2]);
            if(!visible){HideOverlay(glowWnd);glowShown=false;}
            else if((changed||!glowShown)&&now>=nextGlowPresent){
                int left,top,width,height;layers[2].CurrentGeometry(left,top,width,height);
                if(glowCanvas.Resize(width,height)){
                    glowCanvas.originX=left;glowCanvas.originY=top;glowCanvas.Clear();
                    layers[2].Present(glowCanvas.pixels,glowCanvas.w,glowCanvas.activeW,glowCanvas.activeH,
                        left,top,now,g_s.smoothing,bypass[2],GpuStages::CursorGlow);
                    ClearNativeCursor(glowCanvas,sprite,{(float)pos.x,(float)pos.y});
                    if(PresentScene(glowWnd,glowCanvas,pos)){
                        if(!glowShown)ShowWindow(glowWnd,SW_SHOWNOACTIVATE);
                        glowShown=true;layers[2].MarkPresented(now,g_s.smoothing,bypass[2]);
                        cursorPositionDirty=false;
                    }else{
                        HideOverlay(glowWnd);DestroyWindow(glowWnd);glowWnd=nullptr;
                        separateGlow=false;glowShown=false;cursorPositionDirty=true;
                        haveLastPresentation=false;nextPresentation=now;
                    }
                }
                nextGlowPresent=now+1.0/std::max(60,fps);
            }
        }
        bool highDirty=false,lowDirty=false,anyVisible=false;
        DodiFrameSmoothing::Bounds total{};
        for(int i=0;i<4;i++){
            if(separateGlow&&i==2)continue;
            if(!layers[i].Ready())continue;
            const bool visible=layers[i].ContentVisible(now,g_s.smoothing,bypass[i]);
            if(visible){
                auto b=layers[i].VisibleBounds(now,g_s.smoothing,bypass[i]);
                if(!anyVisible)total=b;
                else{
                    total.left=std::min(total.left,b.left);
                    total.top=std::min(total.top,b.top);
                    total.right=std::max(total.right,b.right);
                    total.bottom=std::max(total.bottom,b.bottom);
                }
                anyVisible=true;
            }
            if(layers[i].NeedsPresentation(now,g_s.smoothing,bypass[i])){
                if(bypass[i])highDirty=true;
                else lowDirty=true;
            }
        }
        bool blending=false;
        for(int i=0;i<4;i++)if(layers[i].IsTransitioning(now,g_s.smoothing,bypass[i]))blending=true;
        const int presentationHz=blending?std::max(60,fps):fps;
        if(!anyVisible){
            cursorPositionDirty=false;
            HideOverlay(wnd);wasVisible=false;haveLastPresentation=false;
        }else if((cursorPositionDirty || highDirty || lowDirty) && now>=nextPresentation){
            // Fluid uses the active monitor's bounds; unrelated effects keep
            // their own cursor-local extent. Both obey the DIB safety budget.
            // Clip fluid to the active monitor, not the smaller cursor-local
            // trail/particle limit. The compositor is otherwise why the ink
            // used to disappear abruptly outside the 440px cursor radius.
            int left=active[3]?std::max(total.left,static_cast<int>(bounds.left)):
                std::max(total.left,(int)pos.x-g_s.overlayRadius);
            int top=active[3]?std::max(total.top,static_cast<int>(bounds.top)):
                std::max(total.top,(int)pos.y-g_s.overlayRadius);
            int right=active[3]?std::min(total.right,static_cast<int>(bounds.right)):
                std::min(total.right,(int)pos.x+g_s.overlayRadius);
            int bottom=active[3]?std::min(total.bottom,static_cast<int>(bounds.bottom)):
                std::min(total.bottom,(int)pos.y+g_s.overlayRadius);
            int width=right-left;
            int height=bottom-top;
            if(width<=0||height<=0){
                HideOverlay(wnd);wasVisible=false;haveLastPresentation=false;
            }else if(canvas.Resize(width,height)){
                RenderTiming::Scope timing(RenderTiming::CursorCompose);
                canvas.originX=left;canvas.originY=top;canvas.Clear();
                // Premultiplied BGRA source-over layering with independent
                // smoothing history for active trail, particle, outline layers.
                for(int i=0;i<4;i++)if(!separateGlow||i!=2)layers[i].Present(canvas.pixels,canvas.w,
                    canvas.activeW,canvas.activeH,left,top,now,g_s.smoothing,bypass[i],
                    static_cast<GpuStages::Category>(GpuStages::CursorTrail+i));
                if(sprite.width>0)ClearNativeCursor(canvas,sprite,{(float)pos.x,(float)pos.y});
                bool compareScene=!PerformanceOptions::enabled.load()||PerformanceOptions::skipDuplicate.load();
                bool unchanged=compareScene && haveLastPresentation && left==lastLeft && top==lastTop &&
                               width==lastWidth && height==lastHeight &&
                               lastPresented.size()==(size_t)width*height;
                if(unchanged){
                    for(int y=0;y<height;y++){
                        if(std::memcmp(lastPresented.data()+(size_t)y*width,
                            canvas.pixels+(size_t)y*canvas.w,(size_t)width*4)){
                            unchanged=false;break;
                        }
                    }
                }
                if(unchanged || PresentScene(wnd,canvas,pos)){
                    if(!wasVisible){ShowWindow(wnd,SW_SHOWNOACTIVATE);wasVisible=true;}
                    if(!unchanged&&compareScene){
                        lastPresented.resize((size_t)width*height);
                        for(int y=0;y<height;y++)std::memcpy(
                            lastPresented.data()+(size_t)y*width,
                            canvas.pixels+(size_t)y*canvas.w,(size_t)width*4);
                        lastLeft=left;lastTop=top;lastWidth=width;lastHeight=height;
                        haveLastPresentation=true;
                    }
                    if(!compareScene){std::vector<uint32_t>().swap(lastPresented);haveLastPresentation=false;}
                    for(int i=0;i<4;i++)if(!separateGlow||i!=2)layers[i].MarkPresented(now,g_s.smoothing,bypass[i]);
                    cursorPositionDirty=false;
                }
            }
            nextPresentation=now+1.0/presentationHz;
        }
        // Separate source clocks and bounded presentation clock. Once every smoother
        // is settled and no scene layer changed, no extra presentation runs.
        int idleHz=PerformanceOptions::enabled.load()?PerformanceOptions::idlePoll.load():30;
        double pollStep=speedTier==0&&!PerformanceOptions::follow.load()?1.0/idleHz:1.0/std::max(60,fps);
        double next=now+pollStep;
        for(int i=0;i<4;i++)if(active[i]||previousActive[i])
            next=std::min(next,nextLayer[i]);
        if(lowDirty||highDirty||cursorPositionDirty)next=std::min(next,nextPresentation);
        if(g_s.trail&&speedTier!=0)next=std::min(next,nextSample);
        if(dynamicGlow)next=std::min(next,nextGlow);
        // Idle polling is capped at 30 Hz; stop events remain interruptible.
        WaitUntil(timer,ClampF((float)(next-now),0.001f,1.f/idleHz));
    }
    HideOverlay(wnd);
    if(glowWnd){HideOverlay(glowWnd);DestroyWindow(glowWnd);}
    if(timer)CloseHandle(timer);
    DestroyWindow(wnd);
    UnregisterClassW(name,inst);
    g_image={};
    if(ownCom)CoUninitialize();
    return 0;
}
} // namespace

BOOL Initialize() {
    g_s=LoadSettings();
    LoadCustom();
    Wh_Log(L"Cursor settings: enabled=%d outline=%d trails=%d particles=%d fpsMode=%d maxFps=%d simFps=%d trailFps=%d outlineFps=%d radius=%d",
           g_s.enabled, g_s.glow, g_s.trail, g_s.particles,
           g_s.renderMode, g_s.maxFps, g_s.simFps, g_s.sampleFps,
           g_s.glowFps, g_s.overlayRadius);
    g_stopEvent=CreateEventW(nullptr,TRUE,FALSE,nullptr);
    return g_stopEvent!=nullptr;
}
void AfterInitialize() {
    if(!g_s.enabled||!(g_s.glow||g_s.trail||g_s.particles||g_s.fluidEnabled||g_extra.shock)||!g_stopEvent)return;
    // Only one overlay when Explorer launches extra folder processes.
    g_primaryMutex=CreateMutexW(nullptr,FALSE,L"Local\\DodiCursorOverlaySingleton_v01");
    if(!g_primaryMutex)return;
    if(GetLastError()==ERROR_ALREADY_EXISTS) {
        CloseHandle(g_primaryMutex);g_primaryMutex=nullptr;
        return;
    }
    g_worker=CreateThread(nullptr,0,WorkerThread,nullptr,0,nullptr);
    if(!g_worker) Wh_Log(L"Cursor overlay thread couldn't start");
}
void BeforeUninitialize() {
    if(g_stopEvent)SetEvent(g_stopEvent);
    if(g_worker) {WaitForSingleObject(g_worker,INFINITE);CloseHandle(g_worker);g_worker=nullptr;}
}
void Uninitialize() {
    if(g_stopEvent) {CloseHandle(g_stopEvent);g_stopEvent=nullptr;}
    if(g_primaryMutex) {CloseHandle(g_primaryMutex);g_primaryMutex=nullptr;}
}

void SettingsChanged(){
    BeforeUninitialize();Uninitialize();
    if(Initialize())AfterInitialize();
    else Wh_Log(L"Cursor effects could not restart; other features remain available");
}
} // namespace CursorEffectsEngine

namespace ScreenOverlayEngine {
// ====================================================================== shared helpers
static inline int Imin(int a, int b) { return a < b ? a : b; }
static inline int Imax(int a, int b) { return a > b ? a : b; }
static inline float Clamp01(float x) { return x < 0.f ? 0.f : (x > 1.f ? 1.f : x); }
static inline float Fract(float x) { return x - floorf(x); }

static std::wstring Lower(std::wstring s) {
    for (auto& ch : s)
        if (ch >= L'A' && ch <= L'Z') ch = (wchar_t)(ch - L'A' + L'a');
    return s;
}
static std::wstring Trim(const std::wstring& s) {
    size_t a = s.find_first_not_of(L" \t\r\n");
    if (a == std::wstring::npos) return L"";
    size_t b = s.find_last_not_of(L" \t\r\n");
    return s.substr(a, b - a + 1);
}
static void ParseRgb(PCWSTR c, const wchar_t* def, int* r, int* g, int* b) {
    const wchar_t* p = (c && *c) ? c : def;
    if (*p == L'#') p++;
    unsigned long v = wcstoul(p, nullptr, 16) & 0xFFFFFF;
    *r = (int)((v >> 16) & 0xFF);
    *g = (int)((v >> 8) & 0xFF);
    *b = (int)(v & 0xFF);
}

// ====================================================================== effect language
enum Var {
    V_X, V_Y, V_U, V_V, V_P, V_D, V_ANG, V_T, V_W, V_H, V_MON, V_SPEED, V_POWER, V_N, V_BW, V_RAD, V_OP,
    V_C1R, V_C1G, V_C1B, V_C2R, V_C2G, V_C2B, V_PI, V_TAU, V_AR, V_AG, V_AB, V_KR, V_KG, V_KB,
    V_R, V_G, V_B, V_A, V_FIRST_TEMP
};
static const int kMaxVars = 64;
static const wchar_t* kVarNames[V_FIRST_TEMP] = {
    L"x", L"y", L"u", L"v", L"p", L"d", L"ang", L"t", L"w", L"h", L"mon", L"speed", L"power", L"n", L"bw", L"rad", L"op",
    L"c1r", L"c1g", L"c1b", L"c2r", L"c2g", L"c2b", L"pi", L"tau", L"ar", L"ag", L"ab", L"kr", L"kg", L"kb",
    L"r", L"g", L"b", L"a"
};

enum NodeOp {
    N_NUM, N_VAR, N_NEG, N_NOT, N_ADD, N_SUB, N_MUL, N_DIV, N_MOD, N_POW,
    N_LT, N_GT, N_LE, N_GE, N_EQ, N_NE, N_AND, N_OR, N_CALL
};
enum Fn {
    F_SIN, F_COS, F_TAN, F_ATAN, F_ATAN2, F_ABS, F_SQRT, F_FLOOR, F_CEIL, F_FRACT, F_ROUND, F_SIGN,
    F_EXP, F_LOG, F_POW, F_MIN, F_MAX, F_CLAMP, F_MIX, F_STEP, F_SMOOTHSTEP, F_MOD, F_SAW, F_TRI, F_SQ,
    F_HR, F_HG, F_HB, F_IF, F_NOISE, F_HASH, F_HASH2, F_NOISE2, F_BAYER
};
struct FnDef { const wchar_t* name; int id; int argc; };
static const FnDef kFns[] = {
    {L"sin", F_SIN, 1}, {L"cos", F_COS, 1}, {L"tan", F_TAN, 1}, {L"atan", F_ATAN, 1}, {L"atan2", F_ATAN2, 2},
    {L"abs", F_ABS, 1}, {L"sqrt", F_SQRT, 1}, {L"floor", F_FLOOR, 1}, {L"ceil", F_CEIL, 1},
    {L"fract", F_FRACT, 1}, {L"round", F_ROUND, 1}, {L"sign", F_SIGN, 1}, {L"exp", F_EXP, 1},
    {L"log", F_LOG, 1}, {L"pow", F_POW, 2}, {L"min", F_MIN, 2}, {L"max", F_MAX, 2},
    {L"clamp", F_CLAMP, 3}, {L"mix", F_MIX, 3}, {L"step", F_STEP, 2}, {L"smoothstep", F_SMOOTHSTEP, 3},
    {L"mod", F_MOD, 2}, {L"saw", F_SAW, 1}, {L"tri", F_TRI, 1}, {L"sq", F_SQ, 1}, {L"hr", F_HR, 1},
    {L"hg", F_HG, 1}, {L"hb", F_HB, 1}, {L"if", F_IF, 3}, {L"noise", F_NOISE, 1},
    {L"hash", F_HASH, 1}, {L"hash2", F_HASH2, 2}, {L"noise2", F_NOISE2, 2}, {L"bayer", F_BAYER, 2},
};

struct Node {
    int op = 0;
    float num = 0;
    int var = 0;
    int fn = 0;
    int a = -1, b = -1, c = -1;
};
struct Stmt { int var; int root; };
struct Program {
    std::vector<Node> nodes;
    std::vector<Stmt> stmts;
    bool usesTime = false, usesAmb = false, usesCin = false;
    int tempCount = 0;
    mutable std::shared_ptr<void> classicPlan;
    mutable std::shared_ptr<void> gpuPlan;
    void Clear() { nodes.clear(); stmts.clear(); usesTime = usesAmb = usesCin = false; tempCount = 0; classicPlan.reset(); gpuPlan.reset(); }
};
static_assert(int(N_CALL)==int(GpuFormula::N_CALL)&&int(N_VAR)==int(GpuFormula::N_VAR)&&int(F_BAYER)==int(GpuFormula::F_BAYER)&&V_R==31&&V_A==34&&V_FIRST_TEMP==35&&kMaxVars==64,"GPU/classic formula layouts must match");
static const void* PrepareClassicBorder(const Program& program,const float* values);
static void RunClassicBorder(const void* prepared,float* values);

struct Parser {
    const wchar_t* s;
    size_t i = 0;
    Program* P;
    std::wstring err;
    std::vector<std::wstring> temps;
    std::vector<int> nodeDepth;
    int recursion = 0;
    struct RecursionScope {
        int& depth;
        explicit RecursionScope(int& value) : depth(value) { ++depth; }
        ~RecursionScope() { --depth; }
    };

    bool Fail(const std::wstring& m) {
        if (err.empty()) err = m + L" (at character " + std::to_wstring(i + 1) + L")";
        return false;
    }
    void Ws() { while (s[i] == L' ' || s[i] == L'\t' || s[i] == L'\r' || s[i] == L'\n') i++; }
    static bool IdStart(wchar_t c) { return (c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z') || c == L'_'; }
    static bool IdChar(wchar_t c) { return IdStart(c) || (c >= L'0' && c <= L'9'); }
    std::wstring Ident() {
        size_t st = i;
        while (IdChar(s[i])) i++;
        return Lower(std::wstring(s + st, i - st));
    }
    int Add(const Node& n) {
        if (!err.empty()) return -1;
        if (P->nodes.size() >= 4096) { Fail(L"Effect exceeds 4096 nodes"); return -1; }
        int depth = 1;
        for (int child : {n.a, n.b, n.c})
            if (child >= 0) depth = std::max(depth, nodeDepth[(size_t)child] + 1);
        if (depth > 128) { Fail(L"Expression exceeds 128 levels"); return -1; }
        P->nodes.push_back(n); nodeDepth.push_back(depth);
        return (int)P->nodes.size() - 1;
    }
    int Bin(int op, int a, int b) { Node n; n.op = op; n.a = a; n.b = b; return Add(n); }
    int Un(int op, int a) { Node n; n.op = op; n.a = a; return Add(n); }

    bool VarIndex(const std::wstring& id, bool assign, int* out) {
        for (int k = 0; k < V_FIRST_TEMP; k++) {
            if (id == kVarNames[k]) {
                if (assign && !(k == V_R || k == V_G || k == V_B || k == V_A))
                    return Fail(L"'" + id + L"' is read-only");
                *out = k;
                return true;
            }
        }
        for (size_t k = 0; k < temps.size(); k++)
            if (temps[k] == id) { *out = V_FIRST_TEMP + (int)k; return true; }
        if (!assign) return Fail(L"Unknown name '" + id + L"'");
        if (V_FIRST_TEMP + (int)temps.size() >= kMaxVars) return Fail(L"Too many variables");
        temps.push_back(id);
        *out = V_FIRST_TEMP + (int)temps.size() - 1;
        return true;
    }

    int Primary() {
        Ws();
        wchar_t c = s[i];
        if (c == L'(') {
            i++;
            int e = Or();
            if (e < 0) return -1;
            Ws();
            if (s[i] != L')') { Fail(L"Missing )"); return -1; }
            i++;
            return e;
        }
        if ((c >= L'0' && c <= L'9') || c == L'.') {
            wchar_t* end = nullptr;
            double v = wcstod(s + i, &end);
            if (end == s + i) { Fail(L"Bad number"); return -1; }
            i = (size_t)(end - s);
            Node n; n.op = N_NUM; n.num = (float)v;
            return Add(n);
        }
        if (IdStart(c)) {
            std::wstring id = Ident();
            Ws();
            if (s[i] == L'(') {
                const FnDef* fd = nullptr;
                for (const FnDef& f : kFns)
                    if (id == f.name) { fd = &f; break; }
                if (!fd) { Fail(L"Unknown function '" + id + L"'"); return -1; }
                i++;
                int args[3] = {-1, -1, -1};
                int argc = 0;
                Ws();
                if (s[i] != L')') {
                    for (;;) {
                        if (argc >= 3) { Fail(L"Too many arguments"); return -1; }
                        int e = Or();
                        if (e < 0) return -1;
                        args[argc++] = e;
                        Ws();
                        if (s[i] == L',') { i++; continue; }
                        break;
                    }
                }
                Ws();
                if (s[i] != L')') { Fail(L"Missing )"); return -1; }
                i++;
                if (argc != fd->argc) { Fail(L"Wrong number of arguments for '" + id + L"'"); return -1; }
                Node n; n.op = N_CALL; n.fn = fd->id; n.a = args[0]; n.b = args[1]; n.c = args[2];
                return Add(n);
            }
            int var;
            if (!VarIndex(id, false, &var)) return -1;
            if (var == V_T) P->usesTime = true;
            if (var >= V_AR && var <= V_AB) P->usesAmb = true;
            if (var >= V_KR && var <= V_KB) P->usesCin = true;
            Node n; n.op = N_VAR; n.var = var;
            return Add(n);
        }
        Fail(L"Unexpected character");
        return -1;
    }
    int Pow() {
        int l = Primary();
        if (l < 0) return -1;
        Ws();
        if (s[i] == L'^') {
            i++;
            int r = Unary();
            if (r < 0) return -1;
            return Bin(N_POW, l, r);
        }
        return l;
    }
    int Unary() {
        if (recursion >= 64) { Fail(L"Expression nesting exceeds 64 levels"); return -1; }
        RecursionScope scope(recursion);
        Ws();
        if (s[i] == L'-') { i++; int e = Unary(); if (e < 0) return -1; return Un(N_NEG, e); }
        if (s[i] == L'!' && s[i + 1] != L'=') { i++; int e = Unary(); if (e < 0) return -1; return Un(N_NOT, e); }
        return Pow();
    }
    int Mul() {
        int l = Unary();
        if (l < 0) return -1;
        for (;;) {
            Ws();
            int op = 0;
            if (s[i] == L'*') op = N_MUL;
            else if (s[i] == L'/') op = N_DIV;
            else if (s[i] == L'%') op = N_MOD;
            else break;
            i++;
            int r = Unary();
            if (r < 0) return -1;
            l = Bin(op, l, r);
        }
        return l;
    }
    int AddE() {
        int l = Mul();
        if (l < 0) return -1;
        for (;;) {
            Ws();
            int op = 0;
            if (s[i] == L'+') op = N_ADD;
            else if (s[i] == L'-') op = N_SUB;
            else break;
            i++;
            int r = Mul();
            if (r < 0) return -1;
            l = Bin(op, l, r);
        }
        return l;
    }
    int Cmp() {
        int l = AddE();
        if (l < 0) return -1;
        Ws();
        int op = 0, len = 0;
        if (s[i] == L'<' && s[i + 1] == L'=') { op = N_LE; len = 2; }
        else if (s[i] == L'>' && s[i + 1] == L'=') { op = N_GE; len = 2; }
        else if (s[i] == L'=' && s[i + 1] == L'=') { op = N_EQ; len = 2; }
        else if (s[i] == L'!' && s[i + 1] == L'=') { op = N_NE; len = 2; }
        else if (s[i] == L'<') { op = N_LT; len = 1; }
        else if (s[i] == L'>') { op = N_GT; len = 1; }
        if (!op) return l;
        i += len;
        int r = AddE();
        if (r < 0) return -1;
        return Bin(op, l, r);
    }
    int And() {
        int l = Cmp();
        if (l < 0) return -1;
        for (;;) {
            Ws();
            if (s[i] == L'&' && s[i + 1] == L'&') {
                i += 2;
                int r = Cmp();
                if (r < 0) return -1;
                l = Bin(N_AND, l, r);
            } else break;
        }
        return l;
    }
    int Or() {
        int l = And();
        if (l < 0) return -1;
        for (;;) {
            Ws();
            if (s[i] == L'|' && s[i + 1] == L'|') {
                i += 2;
                int r = And();
                if (r < 0) return -1;
                l = Bin(N_OR, l, r);
            } else break;
        }
        return l;
    }
    bool Statement() {
        Ws();
        if (!s[i] || s[i] == L';') return true;
        size_t save = i;
        if (IdStart(s[i])) {
            std::wstring id = Ident();
            Ws();
            if (s[i] == L'=' && s[i + 1] != L'=') {
                i++;
                int var;
                if (!VarIndex(id, true, &var)) return false;
                int e = Or();
                if (e < 0) return false;
                P->stmts.push_back({var, e});
                return true;
            }
            i = save;
        }
        return Or() >= 0;
    }
    bool ParseProgram() {
        for (;;) {
            if (!Statement()) return false;
            Ws();
            if (s[i] == L';') { i++; continue; }
            if (!s[i]) break;
            return Fail(L"Unexpected character");
        }
        return true;
    }
};

static bool CompileProgram(const std::wstring& src, Program* out, std::wstring* err) {
    out->Clear();
    if (src.size() > 16384) { *err = L"Effect exceeds 16384 characters"; return false; }
    Parser ps;
    ps.s = src.c_str();
    ps.P = out;
    bool ok = ps.ParseProgram();
    if (!ok) { *err = ps.err; out->Clear(); return false; }
    out->tempCount = (int)ps.temps.size();
    return true;
}

static float Hash01(int i) {
    unsigned n = (unsigned)i;
    n = (n << 13) ^ n;
    n = n * (n * n * 15731u + 789221u) + 1376312589u;
    return (float)(n & 0x7fffffffu) / 2147483648.0f;
}
static float Noise1(float x) {
    int i = (int)floorf(x);
    float f = x - (float)i;
    float u = f * f * (3.f - 2.f * f);
    float a = Hash01(i), b = Hash01(i + 1);
    return a + (b - a) * u;
}
static float Hash2(float x, float y) {
    unsigned a = (unsigned)(int)floorf(x), b = (unsigned)(int)floorf(y);
    unsigned n = a * 374761393u + b * 668265263u;
    n = (n ^ (n >> 13)) * 1274126177u;
    n ^= n >> 16;
    return (float)(n & 0xFFFFFFu) / 16777216.0f;
}
static float Noise2(float x, float y) {
    float fx = floorf(x), fy = floorf(y);
    float tx = x - fx, ty = y - fy;
    float u = tx * tx * (3.f - 2.f * tx), v = ty * ty * (3.f - 2.f * ty);
    float a = Hash2(fx, fy), b = Hash2(fx + 1, fy), c = Hash2(fx, fy + 1), d = Hash2(fx + 1, fy + 1);
    float top = a + (b - a) * u, bot = c + (d - c) * u;
    return top + (bot - top) * v;
}
static float Bayer(float x, float y) {
    static const int m[16] = {0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5};
    int ix = ((int)floorf(x)) & 3, iy = ((int)floorf(y)) & 3;
    return ((float)m[iy * 4 + ix] + 0.5f) / 16.0f;
}

static float Eval(const Program& P, int idx, const float* V);

static float CallFn(const Program& P, const Node& n, const float* V) {
    if (n.fn == F_IF) return Eval(P, n.a, V) != 0.f ? Eval(P, n.b, V) : Eval(P, n.c, V);
    float x = Eval(P, n.a, V);
    float y = n.b >= 0 ? Eval(P, n.b, V) : 0.f;
    float z = n.c >= 0 ? Eval(P, n.c, V) : 0.f;
    switch (n.fn) {
        case F_SIN: return sinf(x);
        case F_COS: return cosf(x);
        case F_TAN: return tanf(x);
        case F_ATAN: return atanf(x);
        case F_ATAN2: return atan2f(x, y);
        case F_ABS: return fabsf(x);
        case F_SQRT: return x > 0 ? sqrtf(x) : 0.f;
        case F_FLOOR: return floorf(x);
        case F_CEIL: return ceilf(x);
        case F_FRACT: return Fract(x);
        case F_ROUND: return roundf(x);
        case F_SIGN: return x > 0 ? 1.f : (x < 0 ? -1.f : 0.f);
        case F_EXP: return expf(x < -60.f ? -60.f : (x > 60.f ? 60.f : x));
        case F_LOG: return x > 0 ? logf(x) : 0.f;
        case F_POW: { float r = powf(x, y); return std::isfinite(r) ? r : 0.f; }
        case F_MIN: return x < y ? x : y;
        case F_MAX: return x > y ? x : y;
        case F_CLAMP: return x < y ? y : (x > z ? z : x);
        case F_MIX: return x + (y - x) * z;
        case F_STEP: return y < x ? 0.f : 1.f;   // step(edge, value)
        case F_SMOOTHSTEP: {
            if (y == x) return z < x ? 0.f : 1.f;
            float k = Clamp01((z - x) / (y - x));
            return k * k * (3.f - 2.f * k);
        }
        case F_MOD: return y == 0.f ? 0.f : x - y * floorf(x / y);
        case F_SAW: return Fract(x);
        case F_TRI: return 1.f - fabsf(2.f * Fract(x) - 1.f);
        case F_SQ: return Fract(x) < 0.5f ? 1.f : 0.f;
        case F_HR: return Clamp01(fabsf(6.f * Fract(x) - 3.f) - 1.f);
        case F_HG: return Clamp01(2.f - fabsf(6.f * Fract(x) - 2.f));
        case F_HB: return Clamp01(2.f - fabsf(6.f * Fract(x) - 4.f));
        case F_NOISE: return Noise1(x);
        case F_HASH: return Hash01((int)floorf(x));
        case F_HASH2: return Hash2(x, y);
        case F_NOISE2: return Noise2(x, y);
        case F_BAYER: return Bayer(x, y);
    }
    return 0.f;
}

static float Eval(const Program& P, int idx, const float* V) {
    const Node& n = P.nodes[idx];
    switch (n.op) {
        case N_NUM: return n.num;
        case N_VAR: return V[n.var];
        case N_NEG: return -Eval(P, n.a, V);
        case N_NOT: return Eval(P, n.a, V) == 0.f ? 1.f : 0.f;
        case N_ADD: return Eval(P, n.a, V) + Eval(P, n.b, V);
        case N_SUB: return Eval(P, n.a, V) - Eval(P, n.b, V);
        case N_MUL: return Eval(P, n.a, V) * Eval(P, n.b, V);
        case N_DIV: { float a = Eval(P, n.a, V), b = Eval(P, n.b, V); return b == 0.f ? 0.f : a / b; }
        case N_MOD: { float a = Eval(P, n.a, V), b = Eval(P, n.b, V); return b == 0.f ? 0.f : a - b * floorf(a / b); }
        case N_POW: { float r = powf(Eval(P, n.a, V), Eval(P, n.b, V)); return std::isfinite(r) ? r : 0.f; }
        case N_LT: return Eval(P, n.a, V) < Eval(P, n.b, V) ? 1.f : 0.f;
        case N_GT: return Eval(P, n.a, V) > Eval(P, n.b, V) ? 1.f : 0.f;
        case N_LE: return Eval(P, n.a, V) <= Eval(P, n.b, V) ? 1.f : 0.f;
        case N_GE: return Eval(P, n.a, V) >= Eval(P, n.b, V) ? 1.f : 0.f;
        case N_EQ: return Eval(P, n.a, V) == Eval(P, n.b, V) ? 1.f : 0.f;
        case N_NE: return Eval(P, n.a, V) != Eval(P, n.b, V) ? 1.f : 0.f;
        case N_AND: return (Eval(P, n.a, V) != 0.f && Eval(P, n.b, V) != 0.f) ? 1.f : 0.f;
        case N_OR: return (Eval(P, n.a, V) != 0.f || Eval(P, n.b, V) != 0.f) ? 1.f : 0.f;
        case N_CALL: return CallFn(P, n, V);
    }
    return 0.f;
}

// ====================================================================== built-in effects (written in the same language as Custom)
struct Preset { const wchar_t* key; const wchar_t* code; };
static const Preset kPresets[] = {
    {L"static", L"r=c1r; g=c1g; b=c1b; a=1"},
    {L"breathing", L"k=0.5+0.5*sin(tau*t*speed*0.5); r=c1r; g=c1g; b=c1b; a=mix(1-power,1,k)"},
    {L"glow", L"k=pow(1-d,1+3*(1-power)); pul=0.85+0.15*sin(tau*t*speed*0.5); r=c1r; g=c1g; b=c1b; a=k*pul"},
    {L"neon", L"core=1-smoothstep(0,2.5/bw,d); halo=pow(1-d,2.5)*power; fl=0.93+0.07*sin(tau*t*speed*2.5)+0.03*noise(t*speed*6); "
              L"r=mix(c1r,1,core*0.85); g=mix(c1g,1,core*0.85); b=mix(c1b,1,core*0.85); a=clamp(core+halo,0,1)*fl"},
    {L"wave", L"k=0.5+0.5*sin(tau*(p*n-t*speed*0.25)); r=mix(c1r,c2r,k); g=mix(c1g,c2g,k); b=mix(c1b,c2b,k); a=mix(1-power*0.8,1,k)"},
    {L"rainbow", L"hu=p*n-t*speed*0.1; r=hr(hu); g=hg(hu); b=hb(hu); a=1"},
    {L"comet", L"q=fract(t*speed*0.25-p*n); e=1+7*(1-power); "
               L"r=mix(c2r,c1r,q); g=mix(c2g,c1g,q); b=mix(c2b,c1b,q); a=pow(1-q,e)*smoothstep(0,0.08,q)*(1-0.55*d*d)"},
    {L"gradient", L"k=0.5+0.5*cos(tau*(p*n-t*speed*0.05)); r=mix(c1r,c2r,k); g=mix(c1g,c2g,k); b=mix(c1b,c2b,k); a=1"},
    {L"aurora", L"hu=0.5+0.18*sin(tau*(p*n*0.5-t*speed*0.05)); wob=0.5+0.5*sin(tau*(p*n-t*speed*0.1)); "
                L"r=hr(hu); g=hg(hu); b=hb(hu); a=mix(1-power*0.7,1,wob)*(1-0.5*d)"},
    {L"heartbeat", L"ph=fract(t*speed*0.4); b1=exp(-pow((ph-0.1)*14,2)); b2=exp(-pow((ph-0.3)*14,2)); k=clamp(b1+0.8*b2,0,1); "
                   L"r=c1r; g=c1g; b=c1b; a=mix(1-power,1,k)"},
    {L"fire", L"ht=1-v; n1=noise2(u*n*6,v*4+t*speed*1.2); n2=noise2(u*n*13+9,v*9+t*speed*2.1); fl=n1*0.62+n2*0.38; "
              L"k=clamp(fl*1.5-ht*(1.15-power*0.6)+0.1,0,1); lv=5; q=clamp(floor(k*lv+bayer(x,y))/lv,0,1); "
              L"r=mix(c1r,c2r,q); g=mix(c1g,c2g,q); b=mix(c1b,c2b,q); a=if(q>0,clamp(0.35+q,0,1),0)"},
    {L"stars", L"cs=max(8,bw*1.6); cx=floor(x/cs); cy=floor(y/cs); h1=hash2(cx,cy); h2=hash2(cx+11,cy+5); h3=hash2(cx+23,cy+41); "
               L"sx=(cx+0.25+0.5*h2)*cs; sy=(cy+0.25+0.5*h3)*cs; rr=bw*(0.1+0.4*h1)+0.6; dd=sqrt((x-sx)^2+(y-sy)^2); "
               L"on=if(h1<n*0.07,1,0); tw=0.45+0.55*sin(tau*(t*speed*0.3*(0.5+h2)+h3*7)); k=(1-smoothstep(0,rr,dd))*tw*on; "
               L"r=mix(c1r,c2r,h2); g=mix(c1g,c2g,h2); b=mix(c1b,c2b,h2); a=clamp(k*1.6,0,1)"},
    {L"wheel", L"hu=ang/tau-t*speed*0.1; r=hr(hu); g=hg(hu); b=hb(hu); a=1"},
    {L"silk", L"k=0.5+0.5*sin(tau*(p*n-t*speed*0.08)+2*sin(tau*(p-t*speed*0.03))); r=mix(c1r,c2r,k); g=mix(c1g,c2g,k); b=mix(c1b,c2b,k); a=0.65+0.35*k"},
    {L"plasma", L"k=0.5+0.25*sin(u*n*12+t*speed)+0.25*sin(v*n*12-t*speed*0.7); r=mix(c1r,c2r,k); g=mix(c1g,c2g,k); b=mix(c1b,c2b,k); a=1-d*0.35"},
    {L"pearls", L"q=fract(p*n-t*speed*0.1); k=exp(-pow((q-0.5)*9,2)); r=mix(c1r,c2r,k); g=mix(c1g,c2g,k); b=mix(c1b,c2b,k); a=(0.15+0.85*k)*(1-d*d*0.6)"},
    {L"mercury-v3", L"s=t*speed; q1=sin(tau*(p*3-s*0.06)+sin(tau*p*5+s*0.4)); q2=sin(tau*(p*4+s*0.045)); q3=sin(tau*(p*7-s*0.025)); k1=0.5+0.5*q1; k2=0.5+0.5*q2; k3=0.5+0.5*q3; k1=k1*k1*k1*k1; k2=k2*k2*k2*k2; k3=k3*k3*k3*k3; f1=0.5+0.5*sin(s*0.22+tau*p*2); f2=0.5+0.5*sin(s*0.17-tau*p*3); f3=0.5+0.5*sin(s*0.13+tau*p*5); k=clamp(k1*f1+k2*f2+k3*f3*0.7,0,1); r=(0.22+0.78*k)*k; g=(0.28+0.72*k)*k; b=(0.36+0.64*k)*k; a=k*(1-0.45*d)"},
    {L"mercury-v6", L"warp=0.055*sin(tau*p*8+t*speed*0.08)+0.025*sin(tau*p*13-t*speed*0.12); q1=sin(tau*((p+warp)*5-t*speed*0.085)); q2=sin(tau*((p-warp)*7+t*speed*0.065)); q3=sin(tau*(p*11-t*speed*0.11)); k1=pow(0.5+0.5*q1,10); k2=pow(0.5+0.5*q2,11); k3=pow(0.5+0.5*q3,16); pulse=pow(0.5+0.5*sin(t*speed*0.28+sin(tau*p*2)),12); k=clamp(k1*0.8+k2*0.8+k3*0.45+pulse*0.65,0,1); flicker=0.7+0.3*noise2(p*30,t*speed*0.5); k=k*flicker; r=(0.25+0.75*k)*k; g=(0.3+0.7*k)*k; b=(0.4+0.6*k)*k; a=k*(1-0.5*d)"},
    {L"shimmer", L"k=pow(0.5+0.5*sin(tau*(p*n-t*speed*0.15)),8); r=mix(c1r,1,k*power); g=mix(c1g,1,k*power); b=mix(c1b,1,k*power); a=0.55+0.45*k"},
    {L"wallpaper", L"r=ar; g=ag; b=ab; a=mix(1,1-d*0.85,power)"},
    {L"ambient", L"r=ar; g=ag; b=ab; a=mix(1,1-d*0.85,power)"},
    {L"cinema", L"r=kr; g=kg; b=kb; a=1-d*(0.2+0.65*power)"},
};

static const wchar_t* PresetCode(const std::wstring& key) {
    for (const Preset& p : kPresets)
        if (key == p.key) return p.code;
    return nullptr;
}

static bool CompileEffectCode(const std::wstring& effect, const std::wstring& custom, Program* out) {
    std::wstring code, err;
    if (effect == L"custom") code = custom;
    else {
        std::wstring selected = effect;
        if (effect == L"music-reactive" || effect == L"timecycle" || effect == L"weather") {
            PCWSTR style = Wh_GetStringSetting(L"MusicReactive.Style");
            selected = (effect == L"timecycle" || effect == L"weather") ? L"mercury-v3" : (style ? style : L"mercury-v3");
            Wh_FreeStringSetting(style);
            if (selected != L"mercury-v3" && selected != L"mercury-v6" &&
                selected != L"wave" && selected != L"neon" &&
                selected != L"rainbow" && selected != L"glow" &&
                selected != L"breathing") selected = L"mercury-v3";
        }
        const wchar_t* pc = PresetCode(selected);
        code = pc ? pc : kPresets[0].code;
    }
    if (!CompileProgram(code, out, &err)) {
        Wh_Log(L"Effect formula error: %s -- using the default effect instead", err.c_str());
        CompileProgram(kPresets[0].code, out, &err);
        return false;
    }
    return true;
}

// ====================================================================== color tables (ambient / cinema) and band renderer
static const int kSeg = 96;
struct ColorTable {
    float c[kSeg * 3];
    bool valid;
    ColorTable() : valid(false) { memset(c, 0, sizeof(c)); }
};

static void LookupTable(const ColorTable& t, float p, float* r, float* g, float* b) {
    p = p - floorf(p);
    float f = p * kSeg - 0.5f;
    int i0 = (int)floorf(f);
    float k = f - (float)i0;
    int i1 = i0 + 1;
    i0 = ((i0 % kSeg) + kSeg) % kSeg;
    i1 = ((i1 % kSeg) + kSeg) % kSeg;
    *r = t.c[i0 * 3] * (1.f - k) + t.c[i1 * 3] * k;
    *g = t.c[i0 * 3 + 1] * (1.f - k) + t.c[i1 * 3 + 1] * k;
    *b = t.c[i0 * 3 + 2] * (1.f - k) + t.c[i1 * 3 + 2] * k;
}

struct BandPx { int idx; float x, y, u, v, p, d, ang, cov; };
struct EvalParams {
    bool timecycle=false;
    bool weather=false;
    float W, H, mon, bw, rad, op, speed, power;
    int count;
    float c1[3], c2[3];
};

// Evaluates the effect for every border pixel and adds it (premultiplied) into bits.
static void RenderBand(DWORD* bits, const std::vector<BandPx>& band, const Program& P, const EvalParams& ep,
                       double t, const ColorTable* amb, const ColorTable* cin) {
    RenderTiming::Scope timing(RenderTiming::Classic);
    float V[kMaxVars];
    memset(V, 0, sizeof(V));
    V[V_T] = (float)t; V[V_W] = ep.W; V[V_H] = ep.H; V[V_MON] = ep.mon; V[V_SPEED] = ep.speed;
    V[V_POWER] = ep.power; V[V_N] = (float)ep.count; V[V_BW] = ep.bw; V[V_RAD] = ep.rad; V[V_OP] = ep.op;
    V[V_C1R] = ep.c1[0]; V[V_C1G] = ep.c1[1]; V[V_C1B] = ep.c1[2];
    V[V_C2R] = ep.c2[0]; V[V_C2G] = ep.c2[1]; V[V_C2B] = ep.c2[2];
    V[V_PI] = 3.14159265f; V[V_TAU] = 6.2831853f;
    bool useAmb = P.usesAmb && amb && amb->valid, useCin = P.usesCin && cin && cin->valid;
    if (P.usesAmb && !useAmb) { V[V_AR] = ep.c1[0]; V[V_AG] = ep.c1[1]; V[V_AB] = ep.c1[2]; }
    if (P.usesCin && !useCin) { V[V_KR] = ep.c1[0]; V[V_KG] = ep.c1[1]; V[V_KB] = ep.c1[2]; }

    const bool prepared=PerformanceOptions::enabled.load()&&PerformanceOptions::framePreparation.load();
    const MusicReactive::Frame musicFrame(false,prepared);
    const LiveWeather::Frame weatherFrame((float)t,ep.weather&&prepared);
    const float musicMultiplier = MusicReactive::Multiplier(false);
    const bool timeCycleActive = ep.timecycle;
    const TimeCycle::State solar = timeCycleActive ? TimeCycle::Get() : TimeCycle::State{1.f,0.f,0.f};
    const float musicSpeed = MusicReactive::Speed(false);
    V[V_T] = (float)(t * musicSpeed);
    const float* gpu=GpuFormula::Evaluate(P,band,V,useAmb?amb->c:nullptr,useCin?cin->c:nullptr,true);
    const void* compiled=gpu?nullptr:PrepareClassicBorder(P,V);
    size_t gpuIndex=0;
    for (const BandPx& b : band) {
        if(gpu){memcpy(&V[V_R],gpu+gpuIndex,4*sizeof(float));gpuIndex+=4;}
        else {
        V[V_X] = b.x; V[V_Y] = b.y; V[V_U] = b.u; V[V_V] = b.v;
        V[V_P] = b.p; V[V_D] = b.d; V[V_ANG] = b.ang;
        V[V_R] = V[V_C1R]; V[V_G] = V[V_C1G]; V[V_B] = V[V_C1B]; V[V_A] = 1.f;
        if (useAmb) LookupTable(*amb, b.p, &V[V_AR], &V[V_AG], &V[V_AB]);
        if (useCin) LookupTable(*cin, b.p, &V[V_KR], &V[V_KG], &V[V_KB]);
        for (int k = 0; k < P.tempCount; k++) V[V_FIRST_TEMP + k] = 0.f;
        if(compiled)RunClassicBorder(compiled,V);
        else for (const Stmt& st : P.stmts) {
            float v = Eval(P, st.root, V);
            if (!std::isfinite(v)) v = 0.f;
            V[st.var] = v;
        }
        }
        float a = Clamp01(V[V_A]) * ep.op * b.cov * musicMultiplier * (prepared?musicFrame.Mask(b.x,b.y,ep.W,ep.H):MusicReactive::Mask(false,b.x,b.y,ep.W,ep.H)) * solar.brightness;
        float r = Clamp01(V[V_R]), g = Clamp01(V[V_G]), bl = Clamp01(V[V_B]);
        if(prepared)musicFrame.Tint(r,g,bl);else MusicReactive::Tint(false,r,g,bl);
        if(timeCycleActive)TimeCycle::Tint(r,g,bl,solar);
        if(ep.weather){if(prepared)weatherFrame.Apply(b.x/std::max(1.f,ep.W),b.y/std::max(1.f,ep.H),r,g,bl,a);
            else LiveWeather::Apply(b.x/std::max(1.f,ep.W),b.y/std::max(1.f,ep.H),(float)t,r,g,bl,a);}
        if(a<=0.f)continue;
        DWORD d = bits[b.idx];
        int da = (int)(d >> 24), dr = (int)((d >> 16) & 255), dg = (int)((d >> 8) & 255), db = (int)(d & 255);
        int na = Imin(255, da + (int)(a * 255.f + 0.5f));
        int nr = Imin(na, dr + (int)(r * a * 255.f + 0.5f));
        int ng = Imin(na, dg + (int)(g * a * 255.f + 0.5f));
        int nb = Imin(na, db + (int)(bl * a * 255.f + 0.5f));
        bits[b.idx] = ((DWORD)na << 24) | ((DWORD)nr << 16) | ((DWORD)ng << 8) | (DWORD)nb;
    }
}
// ====================================================================== ambient (wallpaper) and cinema (screen) color sampling
static HBITMAP MakeDibSimple(HDC ref, int w, int h, void** bits) {
    if (!bits) return nullptr;
    *bits = nullptr;
    // Reject invalid geometry and oversized surfaces before calculating strides.
    if (w <= 0 || h <= 0 || w > 32768 || h > 32768 ||
        (size_t)w * (size_t)h > (256u * 1024u * 1024u) / 4u) return nullptr;
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    return CreateDIBSection(ref, &bi, DIB_RGB_COLORS, bits, nullptr, 0);
}

// Averages the pixels in strips along the edges of region (rx,ry,rw,rh) into kSeg colors around the perimeter
// (clockwise, starting at the top left).
static void BuildTable(ColorTable& out, const BYTE* src, int stride, int rx, int ry, int rw, int rh, float depthFrac) {
    float W = (float)rw, H = (float)rh;
    float perim = 2.f * (W + H);
    float depth = std::max(2.f, std::min(W, H) * depthFrac);
    float segLen = perim / (float)kSeg;
    for (int k = 0; k < kSeg; k++) {
        float s0 = ((float)k + 0.5f) * segLen;
        float cx, cy, hw, hh;
        if (s0 < W) { cx = s0; cy = depth * 0.5f; hw = segLen * 0.5f; hh = depth * 0.5f; }
        else if (s0 < W + H) { cx = W - depth * 0.5f; cy = s0 - W; hw = depth * 0.5f; hh = segLen * 0.5f; }
        else if (s0 < 2.f * W + H) { cx = W - (s0 - W - H); cy = H - depth * 0.5f; hw = segLen * 0.5f; hh = depth * 0.5f; }
        else { cx = depth * 0.5f; cy = H - (s0 - 2.f * W - H); hw = depth * 0.5f; hh = segLen * 0.5f; }
        double sr = 0, sg = 0, sb = 0;
        int cnt = 0;
        for (int iy = 0; iy < 5; iy++) {
            for (int ix = 0; ix < 5; ix++) {
                float fx = cx - hw + ((float)ix + 0.5f) * (2.f * hw) / 5.f;
                float fy = cy - hh + ((float)iy + 0.5f) * (2.f * hh) / 5.f;
                fx = std::max(0.f, std::min(fx, W - 1.f));
                fy = std::max(0.f, std::min(fy, H - 1.f));
                const BYTE* p = src + ((size_t)(ry + (int)fy) * (size_t)stride + (size_t)(rx + (int)fx)) * 4;
                sb += p[0]; sg += p[1]; sr += p[2];
                cnt++;
            }
        }
        out.c[k * 3] = (float)(sr / cnt / 255.0);
        out.c[k * 3 + 1] = (float)(sg / cnt / 255.0);
        out.c[k * 3 + 2] = (float)(sb / cnt / 255.0);
    }
    out.valid = true;
}

static void SmoothInto(ColorTable& dst, const ColorTable& src, float k) {
    if (!dst.valid) { dst = src; return; }
    for (int i = 0; i < kSeg * 3; i++) dst.c[i] += (src.c[i] - dst.c[i]) * k;
}

// ---- wallpaper source ------------------------------------------------------------------
struct WallpaperSrc {
    std::vector<BYTE> px;
    int w = 0, h = 0;
    bool captured = false, ok = false;
    RECT origin = {};
};

static BOOL CALLBACK EnumWallProc(HWND h, LPARAM lp) {
    wchar_t cls[32] = {};
    GetClassNameW(h, cls, 32);
    if (!wcscmp(cls, L"WorkerW") && !FindWindowExW(h, nullptr, L"SHELLDLL_DefView", nullptr)) {
        RECT r;
        if (GetWindowRect(h, &r) && r.right - r.left >= 640 && r.bottom - r.top >= 360) {
            *(HWND*)lp = h;
            return FALSE;
        }
    }
    return TRUE;
}

// PrintWindow capture. GPU-only renderers may not provide printable pixels.
static bool PrintToBuffer(HWND h, std::vector<BYTE>& px, int* w, int* hh, RECT* origin) {
    RECT r;
    if (!GetWindowRect(h, &r)) return false;
    int W = r.right - r.left, H = r.bottom - r.top;
    if (W <= 0 || H <= 0 || (long long)W * H > 40000000LL) return false;
    HDC scr = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(scr);
    void* bits = nullptr;
    HBITMAP bmp = MakeDibSimple(scr, W, H, &bits);
    bool ok = false;
    if (dc && bmp && bits) {
        HGDIOBJ old = SelectObject(dc, bmp);
        memset(bits, 0, (size_t)W * H * 4);
        ok = PrintWindow(h, dc, 2 /* PW_RENDERFULLCONTENT */) != 0;
        GdiFlush();
        if (ok) {
            px.assign((BYTE*)bits, (BYTE*)bits + (size_t)W * H * 4);
            *w = W; *hh = H; *origin = r;
        }
        SelectObject(dc, old);
    }
    if (bmp) DeleteObject(bmp);
    if (dc) DeleteDC(dc);
    ReleaseDC(nullptr, scr);
    return ok;
}

static bool MostlyBlack(const std::vector<BYTE>& px) {
    size_t n = px.size() / 4;
    if (!n) return true;
    unsigned long long sum = 0;
    size_t step = std::max<size_t>(1, n / 4000), cnt = 0;
    for (size_t i = 0; i < n; i += step) { sum += px[i * 4] + px[i * 4 + 1] + px[i * 4 + 2]; cnt++; }
    return (sum / (cnt * 3)) < 3;
}

// GDI+ (loaded dynamically so no extra libraries are needed)
struct GpStartupInput { UINT32 Version; void* DebugCb; BOOL NoThread; BOOL NoCodecs; };
struct GpRectI { INT X, Y, W, H; };
struct GpBmpData { UINT Width; UINT Height; INT Stride; INT Format; void* Scan0; UINT_PTR Reserved; };
typedef int (WINAPI* PGdiplusStartup)(ULONG_PTR*, const GpStartupInput*, void*);
typedef void (WINAPI* PGdiplusShutdown)(ULONG_PTR);
typedef int (WINAPI* PGdipCreateBitmapFromFile)(const WCHAR*, void**);
typedef int (WINAPI* PGdipGetImageDim)(void*, UINT*);
typedef int (WINAPI* PGdipBitmapLockBits)(void*, const GpRectI*, UINT, INT, GpBmpData*);
typedef int (WINAPI* PGdipBitmapUnlockBits)(void*, GpBmpData*);
typedef int (WINAPI* PGdipDisposeImage)(void*);

static HMODULE g_gdip = nullptr;
static ULONG_PTR g_gdipTok = 0;
static bool g_gdipTried = false;
static PGdiplusShutdown pGdipShutdown = nullptr;
static PGdipCreateBitmapFromFile pGdipCreate = nullptr;
static PGdipGetImageDim pGdipW = nullptr, pGdipH = nullptr;
static PGdipBitmapLockBits pGdipLock = nullptr;
static PGdipBitmapUnlockBits pGdipUnlock = nullptr;
static PGdipDisposeImage pGdipDispose = nullptr;

static bool GdipInit() {
    if (g_gdipTried) return g_gdip != nullptr;
    g_gdipTried = true;
    HMODULE m = LoadLibraryW(L"gdiplus.dll");
    if (!m) return false;
    PGdiplusStartup st = (PGdiplusStartup)GetProcAddress(m, "GdiplusStartup");
    pGdipShutdown = (PGdiplusShutdown)GetProcAddress(m, "GdiplusShutdown");
    pGdipCreate = (PGdipCreateBitmapFromFile)GetProcAddress(m, "GdipCreateBitmapFromFile");
    pGdipW = (PGdipGetImageDim)GetProcAddress(m, "GdipGetImageWidth");
    pGdipH = (PGdipGetImageDim)GetProcAddress(m, "GdipGetImageHeight");
    pGdipLock = (PGdipBitmapLockBits)GetProcAddress(m, "GdipBitmapLockBits");
    pGdipUnlock = (PGdipBitmapUnlockBits)GetProcAddress(m, "GdipBitmapUnlockBits");
    pGdipDispose = (PGdipDisposeImage)GetProcAddress(m, "GdipDisposeImage");
    GpStartupInput in = {1, nullptr, FALSE, FALSE};
    if (!st || !pGdipShutdown || !pGdipCreate || !pGdipW || !pGdipH || !pGdipLock || !pGdipUnlock || !pGdipDispose ||
        st(&g_gdipTok, &in, nullptr) != 0) {
        FreeLibrary(m);
        return false;
    }
    g_gdip = m;
    return true;
}

static void GdipShutdownAll() {
    if (g_gdip) {
        if (pGdipShutdown && g_gdipTok) pGdipShutdown(g_gdipTok);
        FreeLibrary(g_gdip);
        g_gdip = nullptr;
    }
    g_gdipTok = 0;
    g_gdipTried = false;
}

// Static wallpaper: read the image file (small thumbnail is enough for color averages).
static bool LoadWallpaperThumb(WallpaperSrc& wp) {
    wchar_t path[MAX_PATH] = {};
    SystemParametersInfoW(SPI_GETDESKWALLPAPER, MAX_PATH, path, 0);
    if (!path[0]) {   // solid color background
        COLORREF c = GetSysColor(COLOR_BACKGROUND);
        wp.w = wp.h = 1;
        wp.px.assign(4, 255);
        wp.px[0] = GetBValue(c); wp.px[1] = GetGValue(c); wp.px[2] = GetRValue(c);
        wp.captured = false;
        return true;
    }
    if (!GdipInit()) return false;
    void* bmp = nullptr;
    if (pGdipCreate(path, &bmp) != 0 || !bmp) return false;
    UINT w = 0, h = 0;
    pGdipW(bmp, &w);
    pGdipH(bmp, &h);
    bool ok = false;
    if (w > 0 && h > 0) {
        GpRectI r = {0, 0, (INT)w, (INT)h};
        GpBmpData bd = {};
        if (pGdipLock(bmp, &r, 1 /* read */, 0x26200A /* 32bppARGB */, &bd) == 0 && bd.Scan0) {
            int tw = Imin((int)w, 320);
            int th = Imax(1, (int)((long long)h * tw / (long long)w));
            wp.px.assign((size_t)tw * th * 4, 0);
            for (int ty = 0; ty < th; ty++) {
                int sy = (int)((long long)ty * h / th);
                for (int tx = 0; tx < tw; tx++) {
                    int sx = (int)((long long)tx * w / tw);
                    const BYTE* s = (const BYTE*)bd.Scan0 + (ptrdiff_t)sy * bd.Stride + (ptrdiff_t)sx * 4;
                    BYTE* d = &wp.px[((size_t)ty * tw + tx) * 4];
                    d[0] = s[0]; d[1] = s[1]; d[2] = s[2]; d[3] = 255;
                }
            }
            pGdipUnlock(bmp, &bd);
            wp.w = tw; wp.h = th; wp.captured = false;
            ok = true;
        }
    }
    pGdipDispose(bmp);
    return ok;
}

// Search only wallpaper renderer windows hosted under the desktop shell.
static BOOL CALLBACK EnumWallpaperRenderer(HWND h, LPARAM lp) {
    DWORD pid = 0; GetWindowThreadProcessId(h, &pid);
    HANDLE ph = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!ph) return TRUE;
    wchar_t path[32768] = {}; DWORD n = 32768;
    bool got = QueryFullProcessImageNameW(ph, 0, path, &n) != 0; CloseHandle(ph);
    if (!got) return TRUE;
    const wchar_t* base = wcsrchr(path, L'\\');
    std::wstring exe = Lower(base ? base+1 : path);
    if (exe == L"wallpaper32.exe" || exe == L"wallpaper64.exe" ||
        exe == L"lively.player.exe" || exe == L"lively.player.mpv.exe" ||
        exe == L"lively.player.cefsharp.exe" || exe == L"mpv.exe") {
        RECT r = {}; if (IsWindowVisible(h) && GetWindowRect(h, &r) &&
            r.right-r.left >= 160 && r.bottom-r.top >= 100)
            ((std::vector<HWND>*)lp)->push_back(h);
    }
    return TRUE;
}
static BOOL CALLBACK EnumWallpaperRoots(HWND h, LPARAM lp) {
    wchar_t cls[64] = {}; GetClassNameW(h, cls, 64);
    if (!wcscmp(cls,L"WorkerW") || !wcscmp(cls,L"Progman"))
        EnumChildWindows(h, EnumWallpaperRenderer, lp);
    return TRUE;
}
static bool GrabWallpaper(WallpaperSrc& wp) {
    RenderTiming::Scope timing(RenderTiming::Wallpaper);
    wp = WallpaperSrc();
    std::vector<HWND> renderers;
    EnumWindows(EnumWallpaperRoots, (LPARAM)&renderers);
    int x = GetSystemMetrics(SM_XVIRTUALSCREEN), y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int W = GetSystemMetrics(SM_CXVIRTUALSCREEN), H = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (!renderers.empty() && W > 0 && H > 0 && (long long)W*H <= 40000000LL) {
        // Initialize uncovered monitors from Windows wallpaper rather than sampling apps.
        WallpaperSrc fallback; LoadWallpaperThumb(fallback);
        wp.px.assign((size_t)W*H*4, 0);
        if (!fallback.px.empty()) for (int yy=0; yy<H; ++yy) for(int xx=0; xx<W; ++xx) {
            size_t a=((size_t)yy*W+xx)*4;
            size_t b=((size_t)(yy*fallback.h/H)*fallback.w+xx*fallback.w/W)*4;
            memcpy(&wp.px[a], &fallback.px[b], 4);
        }
        bool captured = false;
        for (HWND renderer : renderers) {
            std::vector<BYTE> px; int w=0,h=0; RECT r={};
            if (!PrintToBuffer(renderer,px,&w,&h,&r) || MostlyBlack(px)) continue;
            for (int yy=0; yy<h; ++yy) {
                int dy=r.top-y+yy; if (dy<0 || dy>=H) continue;
                int left=Imax(0,x-r.left), right=Imin(w,x+W-r.left);
                if(right>left) memcpy(&wp.px[((size_t)dy*W+r.left-x+left)*4],
                    &px[((size_t)yy*w+left)*4], (size_t)(right-left)*4);
            }
            captured = true;
        }
        if (captured) { wp.w=W; wp.h=H; wp.origin={x,y,x+W,y+H}; wp.captured=wp.ok=true; return true; }
    }
    wp = WallpaperSrc(); wp.ok = LoadWallpaperThumb(wp); return wp.ok;
}

static void MakeAmbientTable(const WallpaperSrc& wp, const RECT& mon, ColorTable& out) {
    if (!wp.ok || wp.px.empty() || wp.w < 2 || wp.h < 2) return;
    ColorTable tmp;
    if (wp.captured) {
        int rx = Imax(0, Imin((int)(mon.left - wp.origin.left), wp.w - 1));
        int ry = Imax(0, Imin((int)(mon.top - wp.origin.top), wp.h - 1));
        int rw = Imin((int)(mon.right - mon.left), wp.w - rx);
        int rh = Imin((int)(mon.bottom - mon.top), wp.h - ry);
        if (rw < 8 || rh < 8) return;
        BuildTable(tmp, wp.px.data(), wp.w, rx, ry, rw, rh, 0.14f);
    } else {
        BuildTable(tmp, wp.px.data(), wp.w, 0, 0, wp.w, wp.h, 0.14f);
    }
    out = tmp;
}

// Cinema: the colors on screen right next to the edge of region r.
struct ScreenSampleBuffer {
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ original = nullptr;
    void* bits = nullptr;
    int width = 0, height = 0;
    void Reset() {
        if (dc && original) SelectObject(dc, original);
        if (bitmap) DeleteObject(bitmap);
        if (dc) DeleteDC(dc);
        dc = nullptr; bitmap = nullptr; original = nullptr; bits = nullptr;
        width = height = 0;
    }
    bool Ensure(HDC screen, int w, int h) {
        if (dc && width >= w && height >= h) return true;
        w = Imax(w, width); h = Imax(h, height);
        Reset();
        dc = CreateCompatibleDC(screen);
        if (!dc) return false;
        bitmap = MakeDibSimple(screen, w, h, &bits);
        if (!bitmap || !bits) { Reset(); return false; }
        original = SelectObject(dc, bitmap);
        if (!original || original == HGDI_ERROR) {
            original = nullptr; Reset(); return false;
        }
        width = w; height = h;
        SetStretchBltMode(dc, HALFTONE);
        SetBrushOrgEx(dc, 0, 0, nullptr);
        return true;
    }
};
static thread_local ScreenSampleBuffer g_screenSample;

static bool SameColors(const ColorTable& a, const ColorTable& b) {
    return a.valid == b.valid && (!a.valid || memcmp(a.c, b.c, sizeof(a.c)) == 0);
}

// Return capture success separately from change detection: unchanged pixels
// must not trigger a fallback, invalidate a frame, or boost adaptive FPS.
static bool ScreenTable(const RECT& r, ColorTable& out, float depthFrac, bool* changed = nullptr) {
    RenderTiming::Scope timing(RenderTiming::Capture);
    if (changed) *changed = false;
    int w = (int)(r.right - r.left), h = (int)(r.bottom - r.top);
    if (w < 16 || h < 16) return false;
    int k = Imax(1, Imax(w, h) / 240);
    int sw = (w + k - 1) / k, sh = (h + k - 1) / k;
    HDC scr = GetDC(nullptr);
    if (!scr) return false;
    bool ok = false;
    if (g_screenSample.Ensure(scr, sw, sh) &&
        StretchBlt(g_screenSample.dc, 0, 0, sw, sh, scr, r.left, r.top, w, h, SRCCOPY)) {
        GdiFlush();
        ColorTable tmp;
        BuildTable(tmp, (const BYTE*)g_screenSample.bits, g_screenSample.width, 0, 0, sw, sh, depthFrac);
        if (changed) *changed = !SameColors(out, tmp);
        out = tmp;
        ok = true;
    }
    ReleaseDC(nullptr, scr);
    return ok;
}
static CinemaCapture::Worker<ColorTable> g_cinemaCapture;
static void CleanupCinemaCapture(){g_screenSample.Reset();}
static void InitializeCinemaCapture(){PerformanceOptions::WorkerPriority();SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);}
static void ConfigureCinemaCapture(){g_cinemaCapture.Configure(ScreenTable,CleanupCinemaCapture,InitializeCinemaCapture);}
static bool ReadCinemaSample(UINT_PTR key,const RECT& rect,float depth,ColorTable& out){
    ColorTable latest;if(!g_cinemaCapture.Read(key,rect,depth,latest))return false;
    bool changed=!SameColors(out,latest);out=latest;return changed;
}

// ====================================================================== settings
struct Settings {
    int cr = 0, cg = 0, cb = 0;
    float cornerOp = 1.f;
    int size = 30;
    int roundness = 100;
    bool hideFs = true;
    int bw = 0;
    int c1r = 0, c1g = 191, c1b = 255;
    int c2r = 255, c2g = 0, c2b = 255;
    float borderOp = 1.f;
    std::wstring effect = L"static";
    std::wstring custom;
    float speed = 1.f, power = 0.7f;
    int count = 2;
    int fps = 30;
    std::wstring fpsMode = L"f30";
    int sampleRate = 10, ambientSeconds = 5;
    std::wstring sampleMode = L"r10";
};

static Settings g_s;
static ExperimentalExtras::Config g_extra;
static Program g_prog;
static bool g_animated = false;
static const wchar_t* kClass = L"WH_DodiCombinedRoundedCornersOverlay";

static std::wstring GetStr(const wchar_t* name) {
    PCWSTR v = Wh_GetStringSetting(name);
    std::wstring s = v ? v : L"";
    Wh_FreeStringSetting(v);
    return s;
}

static int ParseFPSValue(const std::wstring& mode) {
    if (mode == L"f10") return 10; if (mode == L"f15") return 15;
    if (mode == L"f30") return 30; if (mode == L"f60") return 60;
    if (mode == L"f75") return 75; if (mode == L"f100") return 100;
    if (mode == L"f120") return 120; if (mode == L"f144") return 144;
    if (mode == L"f200") return 200; if (mode == L"f240") return 240;
    if (mode == L"f360") return 360; if (mode == L"f480") return 480;
    if (mode == L"f500") return 500; if (mode == L"f720") return 720;
    return 30;
}
static int MonitorRefreshHz(HWND hwnd) {
    HMONITOR mon = hwnd ? MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST) : MonitorFromPoint(POINT{0,0}, MONITOR_DEFAULTTOPRIMARY);
    struct RefreshEntry { HMONITOR monitor; DWORD checked; int hz; bool valid; };
    static thread_local RefreshEntry cache[8] = {};
    static thread_local unsigned next = 0;
    DWORD now = GetTickCount();
    RefreshEntry* entry = nullptr;
    for (auto& candidate : cache) if (candidate.valid && candidate.monitor == mon) {
        if (now - candidate.checked < 1000) return candidate.hz;
        entry = &candidate; break;
    }
    if (!entry) entry = &cache[next++ % 8];
    int hz = 60;
    MONITORINFOEXW mi = {}; mi.cbSize = sizeof(mi);
    if (mon && GetMonitorInfoW(mon, &mi)) {
        DEVMODEW dm = {}; dm.dmSize = sizeof(dm);
        if (EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm) && dm.dmDisplayFrequency >= 24 && dm.dmDisplayFrequency <= 1000)
            hz = (int)dm.dmDisplayFrequency;
    }
    *entry = {mon, now, hz, true};
    return hz;
}
static RECT g_adaptiveLastRect = {};
static HWND g_adaptiveLastWindow = nullptr;
static DWORD g_adaptiveActiveUntil = 0;
static void ObserveAdaptiveActivity() {
    HWND fg = GetForegroundWindow(); RECT r = {};
    if (fg && GetWindowRect(fg, &r)) {
        if (fg != g_adaptiveLastWindow || !EqualRect(&r, &g_adaptiveLastRect)) g_adaptiveActiveUntil = GetTickCount() + 350;
        g_adaptiveLastWindow = fg; g_adaptiveLastRect = r;
    } else g_adaptiveLastWindow = nullptr;
}
static ExperimentalBorders::Config g_experimental;
static int UncappedAnimationFPS() {

    if (g_s.fpsMode == L"auto") return MonitorRefreshHz(GetForegroundWindow());
    bool active = (int)(GetTickCount() - g_adaptiveActiveUntil) < 0;
    if (g_s.fpsMode == L"adaptive") return active ? MonitorRefreshHz(GetForegroundWindow()) : 30;
    if (g_s.fpsMode == L"adaptiveLite") return active ? std::min(60, MonitorRefreshHz(GetForegroundWindow())) : 30;
    return g_s.fps;
}


static int EffectiveAnimationFPS(){return PerformanceOptions::Cap(UncappedAnimationFPS());}
static void LoadSettings() {
    g_extra=ExperimentalExtras::Load();
    g_experimental=ExperimentalBorders::Load(true);
    Settings s;
    PCWSTR c = Wh_GetStringSetting(L"Corners.Color");
    ParseRgb(c, L"000000", &s.cr, &s.cg, &s.cb);
    Wh_FreeStringSetting(c);
    s.cornerOp = (float)std::max(1, std::min(100, Wh_GetIntSetting(L"Corners.Opacity"))) / 100.f;
    s.size = std::max(0, std::min(500, Wh_GetIntSetting(L"Corners.Size")));
    s.roundness = std::max(1, std::min(100, Wh_GetIntSetting(L"Corners.Roundness")));
    s.hideFs = Wh_GetIntSetting(L"Corners.HideOnFullscreen") != 0;

    s.bw = std::max(0, std::min(200, Wh_GetIntSetting(L"ScreenBorder.BorderWidth")));
    c = Wh_GetStringSetting(L"ScreenBorder.BorderColor");
    ParseRgb(c, L"00BFFF", &s.c1r, &s.c1g, &s.c1b);
    Wh_FreeStringSetting(c);
    c = Wh_GetStringSetting(L"ScreenBorder.BorderColor2");
    ParseRgb(c, L"FF00FF", &s.c2r, &s.c2g, &s.c2b);
    Wh_FreeStringSetting(c);
    s.borderOp = (float)std::max(1, std::min(100, Wh_GetIntSetting(L"ScreenBorder.BorderOpacity"))) / 100.f;

    s.effect = Lower(GetStr(L"ScreenBorder.Effect"));
    s.custom = GetStr(L"ScreenBorder.CustomEffect");
    s.speed = (float)std::max(1, std::min(100, Wh_GetIntSetting(L"ScreenBorder.EffectSpeed"))) / 20.f;
    s.power = (float)std::max(0, std::min(100, Wh_GetIntSetting(L"ScreenBorder.EffectIntensity"))) / 100.f;
    s.count = std::max(1, std::min(16, Wh_GetIntSetting(L"ScreenBorder.EffectCount")));

    std::wstring f = GetStr(L"ScreenBorder.Timing.AnimationFPS");
    s.fpsMode = f.empty() ? L"f30" : f;
    s.fps = ParseFPSValue(s.fpsMode);
    std::wstring sr = GetStr(L"ScreenBorder.Timing.SampleRate");
    s.sampleMode = sr;
    s.sampleRate = CinemaSampling::FixedRate(sr);
    { auto a = GetStr(L"ScreenBorder.Timing.AmbientInterval"); int n = a.size()>1 ? _wtoi(a.c_str()+1) : 5; s.ambientSeconds = (n==2||n==5||n==10||n==20||n==30||n==60||n==120||n==240) ? n : 5; }

    g_s = s;
    CompileEffectCode(g_s.effect, g_s.custom, &g_prog);
    g_animated = (g_prog.usesTime||g_experimental.enabled||(g_extra.glass&&g_extra.glassScreen)) && g_s.bw > 0;
}

// ====================================================================== overlay windows
struct Win {
    HWND hwnd = nullptr;
    int ox = 0, oy = 0, w = 0, h = 0;
    HDC dc = nullptr;
    HBITMAP bmp = nullptr;
    HGDIOBJ old = nullptr;
    DWORD* bits = nullptr;
    std::vector<DWORD> base;        // corner fill (premultiplied)
    std::vector<BandPx> band;       // pixels that belong to the border
    EffectSmoothing::Frame smooth;
    double lastPresent=0;
};

struct Overlay {
    HMONITOR mon = nullptr;
    RECT rc = {};
    int index = 0;
    int W = 0, H = 0, bw = 0;
    float R = 0;
    bool visible = false;
    ExperimentalBorders::Motion interaction;
    ColorTable amb, cin;
    std::vector<Win> wins;
};

static std::vector<Overlay> g_overlays;

static LRESULT CALLBACK WndProc(HWND h, UINT msg, WPARAM w, LPARAM l) {
    switch (msg) {
        case WM_NCHITTEST: return HTTRANSPARENT;
        case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    }
    return DefWindowProcW(h, msg, w, l);
}

static double Now() {
    static LARGE_INTEGER freq = {};
    static LARGE_INTEGER start = {};
    if (!freq.QuadPart) { QueryPerformanceFrequency(&freq); QueryPerformanceCounter(&start); }
    LARGE_INTEGER n;
    QueryPerformanceCounter(&n);
    return (double)(n.QuadPart - start.QuadPart) / (double)freq.QuadPart;
}

// Signed distance to the rounded screen shape: negative inside, positive outside (the corners).
static float ShapeDist(float px, float py, float W, float H, float R, float nexp) {
    px += 0.5f;
    py += 0.5f;
    if (R > 0) {
        float cx = -1, cy = -1;
        if (px < R) cx = R; else if (px > W - R) cx = W - R;
        if (py < R) cy = R; else if (py > H - R) cy = H - R;
        if (cx >= 0 && cy >= 0) {
            float dx = fabsf(px - cx) / R, dy = fabsf(py - cy) / R;
            float g = (nexp == 2.f) ? sqrtf(dx * dx + dy * dy)
                                    : powf(powf(dx, nexp) + powf(dy, nexp), 1.f / nexp);
            return (g - 1.f) * R;
        }
    }
    return -std::min(std::min(px, W - px), std::min(py, H - py));
}

static DWORD PremulF(int r, int g, int b, float a) {
    a = Clamp01(a);
    unsigned ai = (unsigned)(a * 255.f + 0.5f);
    return (ai << 24) | ((unsigned)((float)r * a + 0.5f) << 16) | ((unsigned)((float)g * a + 0.5f) << 8) | (unsigned)((float)b * a + 0.5f);
}

// Work out, once per window, the corner fill and which pixels belong to the border.
static void Precompute(const Overlay& o, Win& w) {
    float nexp = 2.f + (float)(100 - g_s.roundness) / 100.f * 6.f;
    float W = (float)o.W, H = (float)o.H;
    w.base.assign((size_t)w.w * w.h, 0);
    w.band.clear();

    for (int ly = 0; ly < w.h; ly++) {
        if(RenderShutdown::requested.load())return;
        for (int lx = 0; lx < w.w; lx++) {
            float px = (float)(w.ox + lx), py = (float)(w.oy + ly);
            float dist = ShapeDist(px, py, W, H, o.R, nexp);
            size_t idx = (size_t)ly * w.w + lx;

            float aOut = Clamp01(dist + 0.5f) * g_s.cornerOp;
            if (aOut > 0) w.base[idx] = PremulF(g_s.cr, g_s.cg, g_s.cb, aOut);

            if (o.bw > 0) {
                float ain = Clamp01(0.5f - dist);
                float depth = -dist;
                if (ain > 0 && depth < (g_experimental.enabled?(float)g_experimental.reach:(float)o.bw) + 0.5f) {
                    float cov = ain * Clamp01((g_experimental.enabled?(float)g_experimental.reach:(float)o.bw) - depth + 0.5f);
                    if (cov > 0) {
                        float cx = px + 0.5f, cy = py + 0.5f;
                        auto anchor=ExperimentalBorders::Project(cx,cy,W,H,o.R,nexp);
                        BandPx b;
                        b.idx = (int)idx;
                        b.x = px; b.y = py;
                        b.u = cx / W; b.v = cy / H;
                        b.p = anchor.p;
                        b.d = g_experimental.enabled?dist:Clamp01(std::max(depth, 0.f) / (float)o.bw);
                        float a = atan2f(cy - H * 0.5f, cx - W * 0.5f) / 6.2831853f;
                        b.ang = a - floorf(a);
                        b.cov = cov;
                        if(g_experimental.enabled){
                            b.p=anchor.p;b.u=anchor.x;b.v=anchor.y;
                        }
                        w.band.push_back(b);
                    }
                }
            }
        }
    }
}

static int CinemaRate(double now);
static void PresentWinPixels(const Overlay& o, Win& w, bool setPos) {
    RenderTiming::Scope timing(RenderTiming::ScreenUpload);
    HDC screen = GetDC(nullptr);
    POINT dst = {o.rc.left + w.ox, o.rc.top + w.oy};
    SIZE sz = {w.w, w.h};
    POINT src = {0, 0};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(w.hwnd, screen, setPos ? &dst : nullptr, &sz, w.dc, &src, 0, &bf, ULW_ALPHA);
    ReleaseDC(nullptr, screen);
}

static void RenderWin(const Overlay& o, Win& w, double t, bool setPos) {
    memcpy(w.bits, w.base.data(), w.base.size() * sizeof(DWORD));
    if (!w.band.empty()) {
        EvalParams ep;
        ep.timecycle = g_s.effect == L"timecycle";
        ep.weather = g_s.effect == L"weather";
        ep.W = (float)o.W; ep.H = (float)o.H; ep.mon = (float)o.index; ep.bw = (float)o.bw; ep.rad = o.R;
        ep.op = g_s.borderOp; ep.speed = g_s.speed; ep.power = g_s.power; ep.count = g_s.count;
        ep.c1[0] = g_s.c1r / 255.f; ep.c1[1] = g_s.c1g / 255.f; ep.c1[2] = g_s.c1b / 255.f;
        ep.c2[0] = g_s.c2r / 255.f; ep.c2[1] = g_s.c2g / 255.f; ep.c2[2] = g_s.c2b / 255.f;
        if(g_experimental.enabled)ExperimentalBorders::Draw(w.bits,w.band,g_experimental,o.interaction,t,(float)o.W,(float)o.H,g_s.borderOp,true,0,0,o.R,2.f+(100-g_s.roundness)/100.f*6.f);
        else RenderBand(w.bits, w.band, g_prog, ep, t, &o.amb, &o.cin);
    }
    if(g_extra.glass||g_extra.palette){
        float phase=0,mouseProximity=0;
        if(g_extra.glass&&g_extra.glassScreen){POINT mouse{};if(GetCursorPos(&mouse)){
            float mx=(float)(mouse.x-o.rc.left),my=(float)(mouse.y-o.rc.top);
            auto at=ExperimentalBorders::Project(mx,my,(float)o.W,(float)o.H,o.R);
            phase=at.p;float dx=at.x-mx,dy=at.y-my;mouseProximity=expf(-(dx*dx+dy*dy)/(120.f*120.f));}}
        ExperimentalExtras::BorderPass(w.bits,w.band,g_extra,t,true,phase,mouseProximity);
    }
    int rate=EffectiveAnimationFPS();
    if(g_prog.usesCin)rate=g_animated?std::max(rate,CinemaRate(t)):CinemaRate(t);
    if(!g_experimental.enabled&&EffectSmoothing::Use(g_animated||g_prog.usesCin,rate)&&
       w.smooth.PushBand(w.bits,w.band,t,1.0/std::max(1,rate)))w.smooth.BlendBand(w.bits,w.band,t);
    else w.smooth.Reset();
    if(RenderShutdown::requested.load())return;
    PresentWinPixels(o,w,setPos);w.lastPresent=t;
}

static bool MakeWin(Overlay& o, int ox, int oy, int w, int h) {
    if (w <= 0 || h <= 0) return false;
    Win win;
    win.ox = ox; win.oy = oy; win.w = w; win.h = h;
    win.hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kClass, L"", WS_POPUP, o.rc.left + ox, o.rc.top + oy, w, h, nullptr, nullptr,
        GetModuleHandleW(nullptr), nullptr);
    if (!win.hwnd) return false;
    CaptureVisibility::Apply(win.hwnd);

    HDC screen = GetDC(nullptr);
    win.dc = CreateCompatibleDC(screen);
    void* bits = nullptr;
    win.bmp = MakeDibSimple(screen, w, h, &bits);
    ReleaseDC(nullptr, screen);
    if (!win.dc || !win.bmp || !bits) {
        if (win.bmp) DeleteObject(win.bmp);
        if (win.dc) DeleteDC(win.dc);
        DestroyWindow(win.hwnd);
        return false;
    }
    win.old = SelectObject(win.dc, win.bmp);
    win.bits = (DWORD*)bits;
    Precompute(o, win);
    o.wins.push_back(std::move(win));
    return true;
}

static void DestroyOverlays() {
    for (auto& o : g_overlays) {
        for (auto& w : o.wins) {
            if (w.hwnd) DestroyWindow(w.hwnd);
            if (w.dc) { SelectObject(w.dc, w.old); DeleteDC(w.dc); }
            if (w.bmp) DeleteObject(w.bmp);
        }
        o.wins.clear();
    }
    g_overlays.clear();
}

static BOOL CALLBACK EnumMon(HMONITOR m, HDC, LPRECT, LPARAM lp) {
    MONITORINFO mi = {sizeof(mi)};
    if (GetMonitorInfoW(m, &mi)) {
        Overlay o;
        o.mon = m;
        o.rc = mi.rcMonitor;
        ((std::vector<Overlay>*)lp)->push_back(std::move(o));
    }
    return TRUE;
}

static std::vector<Overlay> QueryMonitors() {
    std::vector<Overlay> v;
    EnumDisplayMonitors(nullptr, nullptr, EnumMon, (LPARAM)&v);
    return v;
}

static DWORD g_nextAmb = 0;
static CinemaSampling::Clock g_cinema;
static bool CinemaVisible() {
    if (g_experimental.enabled || g_s.bw <= 0 || !g_prog.usesCin) return false;
    for (const auto& o : g_overlays) if (o.visible) return true;
    return false;
}
static int CinemaRate(double now) {
    bool windowActivity = (int)(GetTickCount() - g_adaptiveActiveUntil) < 0;
    bool active = g_cinema.Active(now, windowActivity);
    int refresh = g_s.sampleMode == L"auto" || g_s.sampleMode == L"adaptive" ||
        g_s.sampleMode == L"adaptiveLite" ? MonitorRefreshHz(GetForegroundWindow()) : 60;
    return CinemaSampling::Rate(g_s.sampleMode, g_s.sampleRate, refresh, active);
}
static DWORD CinemaWaitMs(double now) {
    if (!CinemaVisible()) return 1000;
    return CinemaSampling::WaitMs(g_cinema.Deadline(now, CinemaRate(now)) - now);
}

static void Rebuild() {
    DestroyOverlays();
    g_overlays = QueryMonitors();
    int idx = 0;
    double t = Now();
    for (auto& o : g_overlays) {
        o.index = idx++;
        o.W = (int)(o.rc.right - o.rc.left);
        o.H = (int)(o.rc.bottom - o.rc.top);
        int half = Imin(o.W, o.H) / 2;
        o.R = (float)Imin(g_s.size, half);
        o.bw = Imin(g_s.bw, Imin(o.W, o.H) / 4);
        int edge=g_experimental.enabled&&o.bw>0?Imin(g_experimental.reach,half):o.bw;
        int C = Imax((int)o.R, edge);

        if (C > 0) {
            MakeWin(o, 0, 0, C, C);
            MakeWin(o, o.W - C, 0, C, C);
            MakeWin(o, 0, o.H - C, C, C);
            MakeWin(o, o.W - C, o.H - C, C, C);
        }
        if (o.bw > 0) {
            MakeWin(o, C, 0, o.W - 2 * C, edge);
            MakeWin(o, C, o.H - edge, o.W - 2 * C, edge);
            MakeWin(o, 0, C, edge, o.H - 2 * C);
            MakeWin(o, o.W - edge, C, edge, o.H - 2 * C);
        }
        if(g_experimental.enabled)o.interaction.Update(t,o.rc.left,o.rc.top,o.W,o.H);
        ExperimentalBorders::FrameBatch batch;
        for (auto& w : o.wins) RenderWin(o, w, t, true);
    }
    g_nextAmb = GetTickCount();
    g_cinema = {};
}

static bool MonitorsChanged() {
    auto cur = QueryMonitors();
    if (cur.size() != g_overlays.size()) return true;
    for (size_t i = 0; i < cur.size(); i++) {
        if (cur[i].mon != g_overlays[i].mon || !EqualRect(&cur[i].rc, &g_overlays[i].rc))
            return true;
    }
    return false;
}

static void RenderAll() {
    double t = Now();
    for (auto& o : g_overlays) {
        if(RenderShutdown::requested.load())return;
        if (!o.visible) continue;
        if(g_experimental.enabled){
            o.interaction.Update(t,o.rc.left,o.rc.top,o.W,o.H);
            if(!ExperimentalBorders::NeedsFrame(g_experimental,o.interaction,t)&&!(g_extra.glass&&g_extra.glassScreen))continue;
        }
        ExperimentalBorders::FrameBatch batch;
        for (auto& w : o.wins){
            if(RenderShutdown::requested.load())return;
            RenderWin(o, w, t, false);
        }
    }
}

static DWORD SmoothingWaitMs(double now) {
    double delay=1;
    if(EffectSmoothing::enabled.load())for(const auto& o:g_overlays)if(o.visible)
        for(const auto& w:o.wins)if(w.smooth.pending)delay=std::min(delay,w.lastPresent+1.0/60.0-now);
    return CinemaSampling::WaitMs(delay);
}
static void PresentSmoothFrames(double now) {
    if(!EffectSmoothing::enabled.load())return;
    for(auto& o:g_overlays)if(o.visible)for(auto& w:o.wins)
        if(w.smooth.pending&&now-w.lastPresent>=1.0/60.0){
            w.smooth.BlendBand(w.bits,w.band,now);PresentWinPixels(o,w,false);w.lastPresent=now;
        }
}

// Reads the wallpaper (Ambient) or the screen edges (Cinema). Returns true when colors changed.
static bool UpdateColors() {
    if (g_experimental.enabled || g_s.bw <= 0) return false;
    bool changed = false;
    DWORD now = GetTickCount();
    if (g_prog.usesAmb && (int)(now - g_nextAmb) >= 0) {
        g_nextAmb = now + (DWORD)g_s.ambientSeconds * 1000;
        WallpaperSrc wp;
        if (g_s.effect == L"wallpaper") {
            if (GrabWallpaper(wp)) for (auto& o : g_overlays) {
                ColorTable previous = o.amb;
                MakeAmbientTable(wp, o.rc, o.amb);
                changed = !SameColors(previous, o.amb) || changed;
            }
        } else {
            for (auto& o : g_overlays) {
                bool sampleChanged = false;
                ScreenTable(o.rc, o.amb, 0.08f, &sampleChanged);
                changed = sampleChanged || changed;
            }
        }
    }
    double cinemaNow = Now();
    int cinemaRate = CinemaRate(cinemaNow);
    if(CinemaVisible()){
        bool due=g_cinema.Due(cinemaNow,cinemaRate),cinemaChanged=false;
        for(auto& o:g_overlays)if(o.visible){
            bool sampled=false;
            if(due&&!g_cinemaCapture.Request((UINT_PTR)o.mon,o.rc,.08f))ScreenTable(o.rc,o.cin,.08f,&sampled);
            sampled=ReadCinemaSample((UINT_PTR)o.mon,o.rc,.08f,o.cin)||sampled;cinemaChanged=sampled||cinemaChanged;
        }
        if(due)g_cinema.Sampled(cinemaNow,cinemaRate,false);
        if(cinemaChanged)g_cinema.activeUntil=cinemaNow+.5;
        changed=cinemaChanged||changed;
    }
    return changed;
}

static void Tick() {
    if (MonitorsChanged()) Rebuild();

    HMONITOR fsMon = nullptr;
    bool allFs = false;

    if (g_s.hideFs) {
        HWND fg = GetForegroundWindow();
        if (fg && !IsIconic(fg) && IsWindowVisible(fg)) {
            wchar_t cls[64] = {};
            GetClassNameW(fg, cls, 64);
            bool ignore = !wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW") ||
                          !wcscmp(cls, L"Shell_TrayWnd") || !wcscmp(cls, L"Shell_SecondaryTrayWnd") ||
                          !wcscmp(cls, kClass);
            if (!ignore) {
                HMONITOR m = MonitorFromWindow(fg, MONITOR_DEFAULTTONULL);
                if (m) {
                    MONITORINFO mi = {sizeof(mi)};
                    RECT wr;
                    if (GetMonitorInfoW(m, &mi) && GetWindowRect(fg, &wr)) {
                        bool covers = wr.left <= mi.rcMonitor.left && wr.top <= mi.rcMonitor.top &&
                                      wr.right >= mi.rcMonitor.right && wr.bottom >= mi.rcMonitor.bottom;
                        bool hasCaption = (GetWindowLongW(fg, GWL_STYLE) & WS_CAPTION) == WS_CAPTION;
                        if (covers && !hasCaption) fsMon = m;
                    }
                }
            }
        }
        QUERY_USER_NOTIFICATION_STATE st;
        if (SUCCEEDED(SHQueryUserNotificationState(&st)) && st == QUNS_RUNNING_D3D_FULL_SCREEN) {
            HMONITOR m = fg ? MonitorFromWindow(fg, MONITOR_DEFAULTTONULL) : nullptr;
            if (m) fsMon = m; else allFs = true;
        }
    }

    for (auto& o : g_overlays) {
        bool show = !(allFs || (fsMon && o.mon == fsMon));
        o.visible = show;
        for (auto& w : o.wins) {
            if (!w.hwnd) continue;
            if (show)
                SetWindowPos(w.hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
            else if (IsWindowVisible(w.hwnd))
                ShowWindow(w.hwnd, SW_HIDE);
        }
    }
}

// ====================================================================== thread and lifecycle
static HANDLE g_thread = nullptr;
static HANDLE g_stopEvt = nullptr;
static HANDLE g_reloadEvt = nullptr;

static DWORD WINAPI OverlayThread(LPVOID) {
    PerformanceOptions::WorkerPriority();
    // Request 1 ms timer granularity so high-FPS modes are not needlessly limited by coarse waits.
    timeBeginPeriod(1);
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kClass;
    RegisterClassW(&wc);

    LoadSettings();
    Rebuild();
    Tick();

    ConfigureCinemaCapture();
    HANDLE evts[3]={g_stopEvt,g_reloadEvt,nullptr};
    DWORD lastTick = GetTickCount();
    double lastFrame = Now();
    for (;;) {
        ObserveAdaptiveActivity();
        int effectiveFps = EffectiveAnimationFPS();
        bool tableFx = (g_prog.usesAmb || g_prog.usesCin) && g_s.bw > 0;
        if(g_experimental.enabled)effectiveFps=std::min(effectiveFps,g_experimental.fps);
        double framePeriod=g_animated?1.0/std::max(1,effectiveFps):(tableFx?.1:.25);
        DWORD frameMs=g_animated?CinemaSampling::WaitMs(std::max(0.,lastFrame+framePeriod-Now())):(tableFx?100u:250u);
        DWORD waitMs = std::min(frameMs, CinemaWaitMs(Now()));
        waitMs=std::min(waitMs,SmoothingWaitMs(Now()));
        evts[2]=g_cinemaCapture.ReadyEvent();
        DWORD r = MsgWaitForMultipleObjects(evts[2]?3:2, evts, FALSE, waitMs, QS_ALLINPUT);
        if (r == WAIT_FAILED) { Wh_Log(L"Overlay wait failed; stopping worker"); break; }
        if (r == WAIT_OBJECT_0) break;                 // unload
        if (r == WAIT_OBJECT_0 + 1) {                  // settings changed
            g_cinemaCapture.Stop();GpuFormula::Reset();
            PerformanceOptions::WorkerPriority();
            LoadSettings();
            Rebuild();
            Tick();
            lastTick=GetTickCount();lastFrame=Now();
            continue;
        }
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        DWORD now = GetTickCount();
        if (now - lastTick >= 250) {
            Tick();
            lastTick = now;
        }
        bool colorsChanged = UpdateColors();
        if (colorsChanged && (g_s.fpsMode == L"adaptive" || g_s.fpsMode == L"adaptiveLite"))
            g_adaptiveActiveUntil = now + 350;
        effectiveFps = EffectiveAnimationFPS();
        if(g_experimental.enabled)effectiveFps=std::min(effectiveFps,g_experimental.fps);
        framePeriod=g_animated?1.0/std::max(1,effectiveFps):(tableFx?.1:.25);
        double frameStart=Now();
        if ((g_animated && frameStart-lastFrame>=framePeriod) || colorsChanged) {
            RenderAll();lastFrame=frameStart;
        }
        PresentSmoothFrames(Now());
    }

    g_cinemaCapture.Stop();GpuFormula::Reset();
    DestroyOverlays();
    UnregisterClassW(kClass, GetModuleHandleW(nullptr));
    GdipShutdownAll();
    g_screenSample.Reset();
    timeEndPeriod(1);
    return 0;
}

BOOL Initialize() {
    g_stopEvt = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_reloadEvt = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    // the thread is started without waiting for it, so loading can never block
    if (g_stopEvt && g_reloadEvt)
        g_thread = CreateThread(nullptr, 0, OverlayThread, nullptr, 0, nullptr);
    if (!g_thread) {
        Wh_Log(L"Screen overlay worker could not start");
        if (g_stopEvt) { CloseHandle(g_stopEvt); g_stopEvt = nullptr; }
        if (g_reloadEvt) { CloseHandle(g_reloadEvt); g_reloadEvt = nullptr; }
        return FALSE;
    }
    return TRUE;
}

void RequestStop(){if(g_stopEvt)SetEvent(g_stopEvt);}
void SettingsChanged() {
    if (g_reloadEvt) SetEvent(g_reloadEvt);
}

void Uninitialize() {
    if (g_thread) {
        SetEvent(g_stopEvt);
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvt) { CloseHandle(g_stopEvt); g_stopEvt = nullptr; }
    if (g_reloadEvt) { CloseHandle(g_reloadEvt); g_reloadEvt = nullptr; }
}
} // namespace ScreenOverlayEngine

namespace ScreenOverlayEngine {
static_assert(kMaxVars==DodiEngine::kMaxVars&&int(V_KB)==int(DodiEngine::V_KB)&&int(N_CALL)==int(DodiEngine::N_CALL)&&int(F_BAYER)==int(DodiEngine::F_BAYER),"Classic formula layouts must match");
static const void* PrepareClassicBorder(const Program& p,const float* v){return IntegratedFormula::PrepareClassic(p,v);}
static void RunClassicBorder(const void* p,float* v){IntegratedFormula::RunClassic(p,v);}
}

// ====================================================================== combined lifecycle
// Keep each working engine in its original target process. No cross-engine calls.
enum class CombinedHost { None, Explorer, ScreenOverlay, StartMenu };
static CombinedHost g_combinedHost=CombinedHost::None;
static bool g_combinedInitialized=false;
static HANDLE g_screenOverlaySingleton=nullptr;

static CombinedHost DetectCombinedHost(){
    wchar_t path[32768];
    DWORD length=GetModuleFileNameW(nullptr,path,ARRAYSIZE(path));
    if(!length||length>=ARRAYSIZE(path))return CombinedHost::None;
    const wchar_t* slash=wcsrchr(path,L'\\');
    const wchar_t* name=slash?slash+1:path;
    if(!_wcsicmp(name,L"explorer.exe"))return CombinedHost::Explorer;
    if(!_wcsicmp(name,L"StartMenuExperienceHost.exe"))return CombinedHost::StartMenu;
    if(!_wcsicmp(name,L"windhawk.exe")){
        DWORD session=0;
        if(!ProcessIdToSessionId(GetCurrentProcessId(),&session)||session==0){
            Wh_Log(L"Monitor overlay skipped: non-interactive/service session");return CombinedHost::None;
        }
        USEROBJECTFLAGS flags{};DWORD bytes=0;
        HWINSTA station=GetProcessWindowStation();
        if(!station||!GetUserObjectInformationW(station,UOI_FLAGS,&flags,sizeof(flags),&bytes)||!(flags.dwFlags&WSF_VISIBLE)){
            Wh_Log(L"Monitor overlay skipped: process %lu has no visible window station",GetCurrentProcessId());
            return CombinedHost::None;
        }
        Wh_Log(L"Monitor overlay host candidate: process=%lu session=%lu",GetCurrentProcessId(),session);
        return CombinedHost::ScreenOverlay;
    }
    return CombinedHost::None;
}

BOOL Wh_ModInit(){
    RenderShutdown::requested.store(false);
    g_combinedHost=DetectCombinedHost();
    CaptureVisibility::Settings();
    EffectSmoothing::Settings();
    PerformanceOptions::Load();
    GpuFormula::Settings();
    GpuStages::Settings();
    if(g_combinedHost==CombinedHost::StartMenu){
        Wh_Log(L"Dodi customizations: Start menu bitmap icon host");
        g_combinedInitialized=DodiEngine::StartMenuFx::Initialize()!=FALSE;
    }else if(g_combinedHost==CombinedHost::Explorer){
        Wh_Log(L"Dodi customizations: Explorer effects host");
        g_combinedInitialized=DodiEngine::Initialize()!=FALSE;
        if(g_combinedInitialized&&!CursorEffectsEngine::Initialize())Wh_Log(L"Cursor engine initialization failed; other features remain available");
    }else if(g_combinedHost==CombinedHost::ScreenOverlay){
        SetLastError(ERROR_SUCCESS);
        g_screenOverlaySingleton=CreateMutexW(nullptr,FALSE,
            L"Local\\Windhawk_DodiScreenOverlay_" WH_MOD_ID);
        DWORD singletonError=GetLastError();
        if(!g_screenOverlaySingleton||singletonError==ERROR_ALREADY_EXISTS){
            if(g_screenOverlaySingleton)CloseHandle(g_screenOverlaySingleton);
            g_screenOverlaySingleton=nullptr;
            Wh_Log(L"Monitor overlay skipped: another instance owns it, or singleton creation failed");
            return FALSE;
        }
        Wh_Log(L"Dodi customizations: monitor effects host");
        g_combinedInitialized=ScreenOverlayEngine::Initialize()!=FALSE;
        // Original overlay init creates events before its thread. Release them
        // if thread creation fails and Windhawk won't call the unload callback.
        if(!g_combinedInitialized){
            ScreenOverlayEngine::Uninitialize();
            CloseHandle(g_screenOverlaySingleton);g_screenOverlaySingleton=nullptr;
        }
    }else{
        Wh_Log(L"Combined mod: unsupported host; skipping initialization");
        return FALSE;
    }
    if(g_combinedInitialized&&g_combinedHost!=CombinedHost::StartMenu){TimeCycle::Settings();MusicReactive::Start();LiveWeather::Start();}
    return g_combinedInitialized?TRUE:FALSE;
}

void Wh_ModAfterInit(){
    if(g_combinedInitialized&&g_combinedHost==CombinedHost::Explorer)
    {
        DodiEngine::AfterInitialize();
        CursorEffectsEngine::AfterInitialize();
    }
}

void Wh_ModSettingsChanged(){
    if(!g_combinedInitialized)return;
    CaptureVisibility::Settings();
    EffectSmoothing::Settings();
    PerformanceOptions::Load();
    GpuFormula::Settings();
    GpuStages::Settings();
    if(g_combinedHost==CombinedHost::StartMenu){DodiEngine::StartMenuFx::Reload();return;}
    MusicReactive::Settings();
    TimeCycle::Settings();
    LiveWeather::Settings();
    if(g_combinedHost==CombinedHost::Explorer){DodiEngine::SettingsChanged();CursorEffectsEngine::SettingsChanged();}
    else if(g_combinedHost==CombinedHost::ScreenOverlay)ScreenOverlayEngine::SettingsChanged();
}

void Wh_ModBeforeUninit(){
    RenderShutdown::requested.store(true);
    if(g_combinedHost==CombinedHost::StartMenu){DodiEngine::StartMenuFx::Stop();return;}
    LiveWeather::RequestStop();
    if(g_combinedHost==CombinedHost::Explorer)DodiEngine::RequestStop();
    else if(g_combinedHost==CombinedHost::ScreenOverlay)ScreenOverlayEngine::RequestStop();
    Wh_Log(L"Shutdown: render stop requested; restoring cursor and Explorer UI");
    if(g_combinedInitialized&&g_combinedHost==CombinedHost::Explorer)
    {
        CursorEffectsEngine::BeforeUninitialize();
        DodiEngine::BeforeUninitialize();
    }
}

void Wh_ModUninit(){
    if(!g_combinedInitialized)return;
    g_combinedInitialized=false;
    if(g_combinedHost==CombinedHost::StartMenu){DodiEngine::StartMenuFx::Uninitialize();GpuStages::Reset();return;}
    Wh_Log(L"Shutdown: joining weather worker");
    LiveWeather::Stop();
    Wh_Log(L"Shutdown: weather stopped; joining audio worker");
    MusicReactive::Stop();
    Wh_Log(L"Shutdown: joining render workers and releasing overlays");
    if(g_combinedHost==CombinedHost::Explorer){CursorEffectsEngine::Uninitialize();DodiEngine::Uninitialize();}
    else if(g_combinedHost==CombinedHost::ScreenOverlay){
        ScreenOverlayEngine::Uninitialize();
        if(g_screenOverlaySingleton)CloseHandle(g_screenOverlaySingleton);
        g_screenOverlaySingleton=nullptr;
    }
    GpuStages::Reset();
}
