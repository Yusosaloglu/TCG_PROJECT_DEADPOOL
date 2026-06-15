# Blind Box Battle — Project Summary
### TCG6223 Computer Graphics | Group Project | Deadline: June 19 2026

---

## Overview

A 2-player fixed-function OpenGL fighting game featuring **Wolverine** vs **Deadpool** as Funko Pop-style blind box figures in a post-apocalyptic desert arena.

- **Build target**: Code::Blocks + MinGW on Windows (graded machine)
- **Renderer**: Fixed-function OpenGL (no shaders — `glBegin`/`glEnd`, `GL_LIGHTING`, `glNormal3f`)
- **Windowing**: freeglut (required for `glutKeyboardUpFunc` held-key detection)

---

## Rubric

| Criterion | Marks |
|-----------|-------|
| Construction (character geometry) | /14 |
| Animation of Character Skills | /14 |
| Battle Environment | /8 |
| Document Management (report) | /4 |
| **Total** | **/40** |

---

## What Is Built

### Framework
- `main.cpp` — freeglut init, callback wiring, game loop
- `platform_gl.h` — cross-platform GLUT include, `M_PI` guard, `GL_CLAMP_TO_EDGE` fallback for MinGW
- `Constants.h` — all magic numbers (camera, movement, combat timings, HP, damage, environment)
- `Geometry.h` — all `inline`: `drawBox`, `drawTrapezoidBody`, `drawWedgePrism`, `drawFlatQuad`, `drawGroundPlane`, hand-built `drawSphere` (lat/long quad strips, NOT gluSphere — counts for Construction marks)
- `Character.h/.cpp` — abstract base: `CharEvent`, `EmitRequest`, phase state machine (`IDLE → SKILL1_WINDUP → ACTIVE → RECOVERY`), `inHitRange()` (cone), `inRange()` (360°), `applyDamage()`
- `Game.h/.cpp` — game loop, two-light setup (GL_LIGHT1 impact flash / GL_LIGHT2 rage rim), min-separation world-level push, environment intensity, restart (`R` key)
- `Particles.h/.cpp` — pool of 512 additive-blend point particles, swap-remove
- `HUD.h/.cpp` — 2D ortho overlay: HP bars, Wolverine heal gauge, character names, controls hint, GAME_OVER win text
- `CMakeLists.txt` — CMake 3.20, C++17, links `OpenGL::GL OpenGL::GLU GLUT::GLUT`, macOS deprecation suppression

### Wolverine (`Wolverine.h/.cpp`)
- Funko Pop proportions: big peach face sphere, black swept-back hair sphere, two pointed hair peaks (wedge prisms), bushy mutton-chop sideburns
- Face: large round black eyes proud of the face surface, thick angled eyebrows (the scowl), skin-tone nose
- Yellow X-Men suit with navy legs/boots and gold belt buckle
- Yellow shoulder caps (spheres), X-suspender straps on chest
- **Skill 1 — Claw Slash (Z key)**: claws hidden at idle → pop out during WINDUP → sweep arm from low (25°) up to high (155°) in ACTIVE → retract in RECOVERY. Cone hit check (`inHitRange`). +18 HP damage.
- **Skill 2 — Berserker Heal (X key)**: gauge fills (+15 per landed Claw Slash, max 60 HP banked). Press X to activate: green translucent aura sphere ramps in (WINDUP), banked HP pours back over 0.7 s (ACTIVE), aura fades (RECOVERY). Gated — does nothing if gauge is empty.

### Deadpool (`Deadpool.h/.cpp`)
- Funko Pop proportions: large rounded-cube red head, black eye patches, white teardrop lenses
- Red/black suit with tan utility belt + Deadpool logo buckle (black ring, red centre, crossed silver lines)
- X-harness straps on back, twin scabbards tilted so tips poke above shoulders
- Both arms hold katanas upright at idle
- **Skill 1 — Katana Slash (N key)**: horizontal left→right yaw sweep at chest height. Blade `extend=true` so it points along the arm, never up through the face. Cone hit check. +20 HP damage.
- **Skill 2 — Desert Eagle (M key)**: raises the pistol arm, fires an instant hitscan shot (range 14 units, 12° cone), draws an additive-blend tracer beam in world space. +22 HP damage.

### Arena (`Arena.h/.cpp`)
- **Procedural cloud sky texture** (256×256): value-noise FBM (4 octaves, integer hash). Applied with `GL_MODULATE` so the per-vertex sky gradient colour multiplies through cloud brightness.
- **Sky gradient**: shifts from overcast grey → deep red/orange as combined HP drops (`setEnvironmentIntensity`).
- **Tiled sand ground texture** (128×128): checker + noise, one large 60×60 plane with 44 tiles. Covers entire visible field.
- **Background structures**: leaning CN-tower spire, steel lattice tower, collapsed sandstone building, two market stalls, wrecked vehicle, three signposts, 12 rubble chunks, 5 fallen girders, 5 distant hills.
- **150-grain blowing dust system**: tan stretched quads (3:1 X-stretch = wind streaks), fade-in/out alpha envelope, gusting wind speed `WIND_BASE + WIND_GUST·sin(windPhase·0.6) + envT·2.4`, intensity scales with environment state.
- **Impact flash** (GL_LIGHT1): white overhead point light, 0.12 s burst on any hit.
- **Rage rim light** (GL_LIGHT2): red underlit point light when either fighter below 35% HP.

---

## Controls

| Action | Wolverine (P1) | Deadpool (P2) |
|--------|---------------|--------------|
| Move forward | W | I |
| Move backward | S | K |
| Turn left | A | J |
| Turn right | D | L |
| Skill 1 | Z — Claw Slash | N — Katana Slash |
| Skill 2 | X — Berserker Heal | M — Desert Eagle |
| Restart | R (after game over) | — |
| Quit | ESC | — |

---

## Key Technical Decisions

| Decision | Reason |
|----------|--------|
| All geometry in `Geometry.h` marked `inline` | Prevents multiple-definition linker errors on MinGW when the header is included by multiple .cpp files |
| `drawSphere` hand-built (lat/long quad strips) | `gluSphere` would not count for Construction marks |
| CharEvent / EmitRequest pattern | Keeps characters independent of ParticleSystem and Arena |
| Min-separation resolved once in `Game::tick()` | Resolving inside each character's `tick()` would double-push and cause jitter |
| GL_LIGHT1 (flash) and GL_LIGHT2 (rage) are separate slots | Using the same slot caused state stomping |
| `tolower()` on all key input | Prevents Caps Lock breaking controls on demo day |
| `GL_MODULATE` on cloud texture | Lets the sky gradient vertex colour tint the cloud brightness automatically |

---

## Where We Stopped (end of last session)

The most recent completed work was:
- Procedural cloud sky texture
- Fully tiled textured desert ground
- 150-grain blowing sand-wind dust system
- Movement changed to full 3D (W/S/A/D and I/K/J/L) — no longer Mortal-Kombat X-axis only
- Wolverine's mutton-chop beard made wider and bushier

---

## What To Do Next

### Immediate / this session
- [ ] Rebuild and test on macOS, confirm zero warnings
- [ ] Verify Windows build in Code::Blocks (MinGW) — check all `inline` guards hold

### Before June 19 deadline
- [ ] **Report PDF** (4 marks, required): cover page, design description, features list, screenshots, group member contributions. Must be submitted alongside the source code.
- [ ] **Code::Blocks .cbp project file**: needed so the marker can open and build the project on their Windows machine without running cmake. Link freeglut.dll, opengl32.lib, glu32.lib in the project settings.
- [ ] **Windows build verification**: run the final build on a Windows machine or VM and confirm the exe launches, renders, and the skills work.
- [ ] (Optional) tune dust density / opacity if it feels too heavy or light on the Windows GPU
- [ ] (Optional) add more environment reactions (screen shake on hit, arena-floor cracks at low HP)
