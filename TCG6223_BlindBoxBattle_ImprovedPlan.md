# TCG6223 Computer Graphics — Group Project
# System Design Document (v2.1, Marks-Optimized): Blind Box Battle
### Deadpool vs Wolverine

> **This version is rewritten to maximise the 40-mark rubric.** Every major decision carries a
> **Rationale** note explaining *which marks it targets and why*. Read Section 2 first — it is the
> whole strategy in one table. Sections that say **CORE** are non-negotiable (that is where the marks
> live); **UPPER-BAND** pushes a category from "meet requirement" to "excellent"; **BONUS** targets
> the extra-credit clause. v2.1 folds in peer-review fixes — see Section 3.1.

---

## Table of Contents

1. [Project Overview](#1-project-overview)
2. [Marks Strategy (read this first)](#2-marks-strategy-read-this-first)
3. [Summary of Improvements + Review Responses](#3-summary-of-improvements-over-the-initial-plan)
4. [Characters](#4-characters)
5. [Character Construction Strategy](#5-character-construction-strategy)
6. [Arena](#6-arena)
7. [Rendering Pipeline](#7-rendering-pipeline)
8. [Animation & Skill System](#8-animation--skill-system)
9. [Battle Mechanics](#9-battle-mechanics)
10. [Environment Changes During Battle](#10-environment-changes-during-battle)
11. [Special Effects (Bonus Marks)](#11-special-effects-bonus-marks)
12. [Controls](#12-controls)
13. [File & Class Architecture](#13-file--class-architecture)
14. [Build Setup (Windows-first)](#14-build-setup-windows-first)
15. [Build Order & Timeline](#15-build-order--timeline)
16. [Marking Criteria Checklist](#16-marking-criteria-checklist)
17. [Pitfalls & Bug Fixes](#17-pitfalls--bug-fixes)
18. [Deliverables Checklist](#18-deliverables-checklist)
19. [Decision Log](#19-decision-log)
20. [Open Items to Confirm](#20-open-items-to-confirm)

---

## 1. Project Overview

**Course:** TCG6223 Computer Graphics, Multimedia University
**Submission Deadline:** 19 June 2026, 11:59 PM (target completion **18 June**; the 19th is buffer)
**Deliverables:** Written Report (PDF), Source Code, Team Declaration Form, Video Clip (<5 min)
**Demo:** Own laptop required at submission
**Graded environment:** MinGW C++ compiler + Code::Blocks IDE on Windows 10

**Concept:**
A two-player local fighting game in OpenGL featuring **Deadpool** and **Wolverine** as Funko-style
blind box collectible figures. They battle in a post-apocalyptic wasteland arena. Each character
moves freely on the ground plane, casts a signature offensive skill and a secondary skill, and the
**arena visibly transforms as the battle escalates**.

**Engine:** OpenGL + **freeglut**, fixed-function pipeline only (`glBegin/glEnd`, GLU quadrics,
`glTranslatef/glRotatef/glScalef`, `glPushMatrix/glPopMatrix`). No shaders. No external 3D model
libraries. The only third-party code is `stb_image.h` (image **decoding** for optional textures) —
all geometry, animation, and effects are hand-written.

> **Rationale (engine choice):** The rubric and BEWARE clause reward "your own code" and lab
> techniques. Fixed-function GLUT is exactly what the labs teach, so every technique we use maps to a
> lab and is defensible as our own work. Using shaders or a scene-graph library would be harder to
> defend and earns nothing extra.

---

## 2. Marks Strategy (read this first)

The entire design exists to convert each of the 40 marks into a concrete, demonstrable feature.

| # | Criterion | Max | Band we target | The concrete feature that earns it |
|---|---|---|---|---|
| 1 | Construction of Characters | 14 | **Excellent (12–14)** | Funko bodies built as **hand-coded vertex/face-list boxes & prisms**, not GLUT quadrics. Custom claws, cowl, katanas, eye-patches, pouches. A documented "custom geometry inventory" proves it. |
| 2 | Animation of Skills + rendering effects | 14 | **Excellent (12–14)** | **Two multi-phase skills per character**, plus lighting + smooth shading + **textures that actually ship** + specular metal + particle/trail effects on every skill. |
| 3 | Battle Environment changes during battle | 8 | **Excellent (7–8)** | **Both continuous (HP-driven dusk + sandstorm) and event-driven (impact flash, rage aura, camera shake)** changes that are impossible to miss on camera. |
| 4 | Document Management (report + code) | 4 | **Excellent (4)** | Class-per-file architecture, named constants (zero magic numbers), 4-space indent, header comments, plus a clean well-illustrated report. |
| — | **Bonus** | extra | — | Particle systems, motion-blur trails, projected shadows, procedural texture generation, camera shake — **techniques not taught in the labs**. |

> **Rationale (why this ordering):** Criteria 1 and 2 are worth **28 of 40 marks (70%)**. The single
> biggest mistake the reference Dimoo project made was importing character models, which scores ~0 on
> Criterion 1. So our #1 priority is **hand-modelled characters**, and our #2 is **rich, textured,
> animated skills**. Everything else is built only after those two are locked.

---

## 3. Summary of Improvements Over the Initial Plan

These are the deliberate changes from the first draft, each tied to a mark.

| Change | Why it raises marks |
|---|---|
| **Bodies/limbs hand-built as Funko-style boxes** instead of `gluCylinder`/`gluSphere` | GLUT quadrics explicitly **don't count** toward Construction marks. Funko proportions are blocky anyway, so this is *more accurate AND more marks* — pushes Criterion 1 from "meet" to "excellent". |
| **Textures are guaranteed via procedural generation** (generated in code), not "optional if assets exist" | Texturing is a named rubric item. The initial plan let it be cut to zero. Generating textures in code means it *always* ships and counts as a bonus technique too. |
| **Second skill per character** (Deadpool pistols, Wolverine regeneration) | Criterion 2 (14 marks) rewards richness/variety. Adds offensive **and** defensive, melee **and** ranged → "excellent" band. |
| **Event-driven environment changes added** (impact flash, rage aura, camera shake) on top of continuous HP-driven changes | Criterion 3 wants change that is *obvious to the examiner*. Two independent change systems make it unmissable. |
| **Windows/Code::Blocks declared the source of truth**; Mac/CMake demoted to optional dev convenience | The Mac build earns **zero marks** and adds risk. Reframing prevents wasted effort and demo-day surprises. |
| **Bug fixes folded in**: angle-wraparound in hit detection; auto-face only when idle | Prevents demo-day misfires and dead-code complexity in the graded source. |
| **Custom geometry inventory + lab-technique map** included as report artifacts | Gives the examiner direct evidence for marks instead of making them hunt for it. |
| **Particle system, motion trails, projected shadows** added as a dedicated effects layer | Directly targets the **bonus** clause ("special effects not taught in class"). |

### 3.1 Revision v2.1 — Responses to Peer Review

A peer review of v2 raised valid schedule/architecture risks. Changes applied in this revision:

- **Parallelised the timeline across the 4-member team** so two characters are built at once (Section 15). This is the real fix for the "Day 2 is overloaded" risk — better than simply slipping dates — and it doubles as the per-member milestone split the Team Declaration Form needs.
- **`Character` base is now thin (logic only); all geometry lives in the subclasses** (Section 13). Shared *drawing* across characters was a trap; reuse now happens at the **primitive** level via `Geometry.h`.
- **Particle-system foundation moved earlier (Day 2)** so Day-3/4 skills can emit sparks without depending on an unbuilt system.
- **Motion trails reclassified as BONUS, below secondary skills** (Section 11), with a cheaper implementation note (compute the claw-tip world position analytically — no matrix readback).
- **Secondary skills given a hard, time-boxed cut condition** (4 PM, Day 5) so they can never delay the video (Section 15).
- **Blind-box confirmation moved to Day 1** (Section 20), not report time.
- **Berserker Regeneration simplified** to heal + aura by default; the damage-reduction buff is optional (Section 8).
- **Blob shadows reaffirmed as the shipped default**; planar projection is last-day-only due to z-fighting (Section 11).

---

## 4. Characters

> **Blind-box eligibility note (confirm Day 1 + document):** Deadpool and Wolverine ship as genuine
> blind box collectibles (Funko **Mystery Minis** and blind-boxed **Pocket Pop!** lines), so the
> "blind box character" framing is defensible. **Action:** confirm acceptability with the lecturer on
> **Day 1** (before modelling effort goes in), and state this justification (with the Funko Mystery
> Minis reference) in the report's Introduction.
> **Rationale:** the brief's examples are Pop Mart toys; a one-line confirmation removes the only
> conceptual risk in the whole project for near-zero effort.

### 4.1 Wolverine — Player 1

| Property | Detail |
|---|---|
| Visual style | Funko proportions — oversized cuboid head, short blocky body |
| Costume | Yellow/blue suit, blue gloves & boots, brown belt with X buckle |
| Signature feature | Three adamantium claws per hand (custom tapered-prism geometry) |
| Primary skill | **Claw Lunge** (offensive melee) |
| Secondary skill | **Berserker Regeneration** (defensive heal + red aura) |

**Custom-built parts (Construction marks):** cowl with two pointed peaks (face-list), X belt buckle
(face-list), six claws via one reusable `drawClaw()` (tapered prism), blocky torso/upper-arm/forearm/
thigh/shin as **face-list boxes**, hands/feet as small boxes.

### 4.2 Deadpool — Player 2

| Property | Detail |
|---|---|
| Visual style | Funko proportions — large rounded-cuboid red head |
| Costume | Red/black suit with seam lines, tan utility belt with pouches, black boots |
| Signature feature | Two katanas crossed on back; white oval eye-lenses on black patches |
| Primary skill | **Katana Spin** (offensive 360° melee) |
| Secondary skill | **Bang Bang** (dual-pistol ranged projectiles + muzzle-flash particles) |

**Custom-built parts (Construction marks):** eye patches + lenses (flat polygon overlays), belt
pouches (face-list boxes), two katanas via one reusable `Katana` class (blade = thin face-list box
with correct normals, guard = small box, grip = short prism), blocky body as **face-list boxes**, two
pistols (small face-list models) used by the secondary skill.

> **Rationale (two skills each, with one offensive + one defensive/ranged):** Criterion 2 is 14 marks
> and rewards animation *range*. Four distinct skill animations (lunge, spin, ranged fire, buff/heal)
> demonstrate far more than the required minimum of one skill, and the offensive/defensive +
> melee/ranged spread reads as "excellent" rather than "met the requirement."

---

## 5. Character Construction Strategy

### Guiding rule (quoted from the marksheet)
> *"The more complex the objects you created on your own (not counting those generated by graphics
> software, not counting GLUT objects), the more marks you will get."*

**Decision: minimise GLUT quadrics; hand-build everything blocky.**

> **Rationale:** Quadrics (`gluSphere`, `gluCylinder`) are explicitly excluded from Construction
> marks. Funko Pop bodies are intrinsically **angular and boxy**, so modelling them as our own
> vertex/face-list boxes is simultaneously the most accurate style choice and the highest-scoring one.
> We reserve quadrics for only the few genuinely round details where a box would look wrong.

### What to hand-build vs. what may use a quadric

| Part | Method | Reason |
|---|---|---|
| Torso, upper/lower arms, thighs/shins, hands, feet | **Vertex/face-list boxes** | Funko style is blocky; counts for marks |
| Head (both) | **Rounded face-list box** (chamfered cube) | Custom, on-style; a bare `gluSphere` would not count |
| Wolverine cowl peaks, X buckle | **Vertex/face-list** | Signature, custom |
| Claws ×6 | **Tapered prism**, one `drawClaw()` reused | Skill centrepiece, custom |
| Deadpool eye patches/lenses, pouches | **Flat overlays / small boxes** | Custom |
| Katana blade/guard, pistols | **Face-list**, reusable `Katana` / pistol | Custom, reused |
| Joints (shoulder/knee), katana grip | `gluSphere`/short prism | Acceptable; small share of the model |
| Arena debris | **Face-list boxes/prisms** in a loop | "No GLUT objects in the scene" |

### Hierarchical modelling (Lab 07)

Every part is modelled at its own origin `(0,0,0)` and assembled with the matrix stack:

```
draw()
  glPushMatrix()
    translate to world position;  rotate to facing
    glPushMatrix() drawTorso()              glPopMatrix()
    glPushMatrix() translate-up;  drawHead() glPopMatrix()
    glPushMatrix() translate-shoulderL; drawArm(L) glPopMatrix()  // arm sub-assembles forearm+hand+claws
    glPushMatrix() translate-shoulderR; drawArm(R) glPopMatrix()
    glPushMatrix() translate-hipL; drawLeg(L) glPopMatrix()
    glPushMatrix() translate-hipR; drawLeg(R) glPopMatrix()
  glPopMatrix()
```

> **Rationale (rule h + Criterion 1 "object transformation"):** Modelling each part at the origin and
> placing it with transforms is the exact technique the brief asks for, and it makes the limbs
> independently animatable (a single fused model cannot animate its parts — see rule g).

### Custom Geometry Inventory (put this table in the report)

| Object | Technique | Reused? |
|---|---|---|
| Funko torso / arms / legs / hands / feet | Face-list box | shared `drawBox()` helper |
| Head (chamfered) | Face-list | per character |
| Wolverine cowl + peaks, X buckle | Face-list | — |
| Claw | Tapered prism | ×6 |
| Katana (blade+guard+grip) | Face-list + prism | ×2 (Deadpool) |
| Pistol | Face-list | ×2 |
| Eye patches + lenses, belt pouches | Flat overlay / box | looped |
| Debris (crates, beams, barrels) | Face-list box/prism | looped placement |

> **Rationale:** This inventory is direct evidence for the examiner. "Above requirement" vs
> "excellent" often comes down to whether the marker can *see* how much you built yourself.

---

## 6. Arena

**Theme:** post-apocalyptic wasteland.

| Element | How built | Notes |
|---|---|---|
| Sandy ground | Large quad grid (subdivided for per-vertex lighting) + **procedural sand texture** | ±40 units X/Z |
| Sky backdrop | Gradient skybox (own quads) + drifting `GL_POINTS` dust field | Dust seed shifts during battle |
| Debris | Loop-placed face-list crates/beams/barrels | Custom complex objects, no GLUT |
| Distant ruins | Vertex-list silhouettes on the horizon | Depth + scale reference |
| Blob shadows | Translucent ellipse under each character | Grounds characters; scales on jump |

**Camera:** fixed, elevated ~20° looking down the X axis at arena centre, framed so both fighters stay
visible across the full play area. **Camera shake** triggers on every hit; a slow zoom plays on KO.

> **Rationale (camera):** A fixed camera is robust and never loses the fighters — low risk. Shake and
> the KO zoom are ~15 lines each, read as polish on video, and qualify as effects "not taught in
> class" (bonus). A full follow-cam is deliberately *not* core because it risks framing bugs for
> little extra mark.

---

## 7. Rendering Pipeline

All fixed-function, as taught. No shaders.

### Lighting
```
GL_LIGHT0  Key light   — warm white, upper-left            (always on)
GL_LIGHT1  Fill light  — dim cool grey, opposite side      (always on)
GL_LIGHT2  Rage rim    — red, enabled only when a fighter is below 25% HP
```
- `glEnable(GL_LIGHTING)`, `GL_COLOR_MATERIAL` with `GL_AMBIENT_AND_DIFFUSE` so `glColor3f` keeps
  working as diffuse colour.
- **Specular** material on claws, katana blades, pistols (adamantium/steel highlight).
- `glShadeModel(GL_SMOOTH)` (Gouraud) everywhere.

> **Rationale:** Lighting + shading are named in Criterion 2. A 2–3 light rig with specular metal is
> clearly "applied appropriately" (not gratuitous), and the third light doubles as an
> environment-change cue (Criterion 3).

### Normals — critical
Every face-list polygon gets a `glNormal3f` **as it is built**. `glEnable(GL_NORMALIZE)` because
`glScalef` is used on animated parts (e.g. claw length).

> **Rationale:** Missing/!normalised normals → black or wrongly-lit models, the single most common way
> to lose lighting marks and look broken on video.

### Depth
`glEnable(GL_DEPTH_TEST)` in init — non-negotiable for overlapping limbs.

### Texturing — guaranteed, via procedural generation
- **Decision:** generate textures in code at startup into `GLuint` texture objects — a sand/noise
  ground, a Deadpool suit panel pattern, a metal sheen — and apply with `GL_MODULATE` so lighting
  still affects them. Image files via `stb_image.h` are an *optional* upgrade, not a dependency.
- Round parts that are textured use `gluQuadricTexture(q, GL_TRUE)`.
- **Graceful fallback:** if an optional image file is missing, log a warning and keep the procedural
  texture. The program must never crash on the examiner's laptop for a missing asset.

```cpp
// Procedural sand: an N×N RGB checkerboard+noise baked once into a texture.
GLuint makeSandTexture(int N) {
    std::vector<unsigned char> px(N*N*3);
    for (int y=0;y<N;y++) for (int x=0;x<N;x++){
        int i=(y*N+x)*3;
        unsigned char base = 150 + ((x/8 + y/8) % 2)*15;   // subtle tiling
        unsigned char n = (unsigned char)((x*73 + y*151) % 24); // cheap deterministic noise
        px[i]=base+n; px[i+1]=(base-20)+n; px[i+2]=90+n/2;   // tan
    }
    GLuint t; glGenTextures(1,&t); glBindTexture(GL_TEXTURE_2D,t);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,N,N,0,GL_RGB,GL_UNSIGNED_BYTE,px.data());
    return t;
}
```

> **Rationale (procedural textures):** This is the most important fix to the original plan. Texturing
> is explicitly graded, yet the first draft made it optional and cut-first. Generating textures in
> code removes the "no art assets" excuse entirely — texturing is *always present* — and procedural
> generation is itself a technique beyond the labs, so it also nudges the **bonus**. We still keep the
> `stb_image` path so real art can be dropped in later with no code change.

### Blending
- Additive (`GL_SRC_ALPHA, GL_ONE`) for glows, sparks, muzzle flash, rage aura.
- Alpha (`GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA`) for shadows, HUD, sky.

---

## 8. Animation & Skill System

### Movement (Lab 09 time-based pattern)
Velocity-based, frame-rate independent, using *held-key state* so both players move smoothly and
simultaneously:

```cpp
// keyState[] set in glutKeyboardFunc/Up + glutSpecialFunc/Up
float dt = elapsedMs / 1000.0f;
if (keyState['a']) p1.x -= SPEED*dt;
if (keyState['d']) p1.x += SPEED*dt;
if (keyState['w']) p1.z -= SPEED*dt;
if (keyState['s']) p1.z += SPEED*dt;
p1.x = clampf(p1.x, -ARENA_HALF, ARENA_HALF);
p1.z = clampf(p1.z, -ARENA_HALF, ARENA_HALF);
```

**Auto-facing — fixed:** a character faces its opponent **only while idle/moving**, *not* during a
skill (so the Katana Spin's rotation is visible and hit-detection facing checks remain meaningful).

> **Rationale:** Real two-player control with held keys is what "user controlled during battle"
> means. Locking facing during a skill fixes the dead-logic problem where auto-facing made the
> hit-detection facing test always-true.

### Skill state machine (per character)
```cpp
enum SkillPhase { SKILL_IDLE, SKILL_WINDUP, SKILL_ACTIVE, SKILL_RECOVERY };
```
A skill can only start from `SKILL_IDLE`, so holding the key cannot re-trigger mid-animation.

### Wolverine — Claw Lunge (primary, offensive)
| Phase | Time | What happens |
|---|---|---|
| WINDUP | 0.0–0.2s | `clawLength` lerps 0→1 (claws extend); spark particles at claw tips |
| ACTIVE | 0.2–0.5s | lunge toward opponent; **hit window open**; motion-trail on claws |
| RECOVERY | 0.5–0.8s | return; `clawLength` lerps 1→0 |

`clawLength` scales the claw geometry on its long axis — retracted = length 0, hidden in the fist. No
visibility flag needed.

### Deadpool — Katana Spin (primary, offensive)
| Phase | Time | What happens |
|---|---|---|
| WINDUP | 0.0–0.3s | katanas lerp from back to hands (`drawProgress` 0→1) |
| ACTIVE | 0.3–0.7s | upper body rotates 360°; **hit window open ~90°–270°**; translucent arc trail |
| RECOVERY | 0.7–1.0s | katanas sheathed; body resets |

### Secondary skills (UPPER-BAND)
- **Deadpool — Bang Bang (ranged):** fires 3 projectiles with muzzle-flash particles; reuses a tiny
  `Projectile` struct; each projectile does small damage on contact. Adds **ranged** variety.
- **Wolverine — Berserker Regeneration (defensive):** heals over 1.5s with a red aura (additive glow)
  and heal-mote particles. Adds **defensive** variety and a non-attack animation.
  *Baseline = heal + aura only. The optional brief damage-reduction buff needs a buffed-state flag and
  a modifier in hit detection — add it only if Day 5 has slack; the visual reads the same without it.*

> **Rationale:** One skill each already satisfies the brief; the second each is what separates
> "excellent" from "met requirement" on the 14-mark animation criterion, and it gives the demo video
> obvious variety. Both secondaries reuse systems we already build (particles, projectile), so the
> marginal cost is low — and they sit behind the cut-line so the core loop is never at risk.

---

## 9. Battle Mechanics

### HP & win condition
- 100 HP each; primary hit −25, projectile −8; first to 0 loses.
- `GameState`: `STATE_INTRO → STATE_FIGHTING → STATE_WIN`; press Enter to restart.
- HP bars + skill-ready indicators drawn as a 2D ortho overlay (own quads + bitmap text).

### Hit detection — corrected
```cpp
float dx = atk.x - def.x, dz = atk.z - def.z;
float dist = sqrtf(dx*dx + dz*dz);

float angleToTarget = atan2f(dx, dz) * 180.0f/ M_PI;
float diff = angleToTarget - atk.facingAngle;
while (diff > 180.f) diff -= 360.f;     // <-- wrap-around fix
while (diff < -180.f) diff += 360.f;

if (dist < HIT_RANGE && fabsf(diff) < HIT_ANGLE_TOL && !hitRegistered) {
    def.hp -= damage;
    hitRegistered = true;               // one hit per cast; reset on return to IDLE
    def.enterHitStun(0.5f);
}
```
- Brief 0.5s `HIT_STUN` (flinch, no input).
- Minimum-separation constraint each tick so characters cannot clip through each other.

> **Rationale:** The wrap-around normalisation prevents the facing test from failing near the 0/360°
> seam — without it, hits silently miss on camera, which looks like a broken game to the examiner.

---

## 10. Environment Changes During Battle

> **Worth 8 marks and must be unmistakable on camera. We use two independent systems so the change is
> impossible to miss.**

### Continuous — escalation tied to HP
`Arena::setEnvironmentIntensity(t)` where `t = 1 - combinedHP/200` (0 = full health, 1 = someone at 0):

| t | Lighting | Sky |
|---|---|---|
| 0.0 | bright warm daylight | calm, sparse dust |
| 0.5 | dimmer, orange-red dusk | dust begins drifting |
| 1.0 | near-dark, deep red ambient | heavy sandstorm |

Implemented via `glLightfv(GL_LIGHT0, GL_DIFFUSE,…)` + `glLightModelfv(GL_LIGHT_MODEL_AMBIENT,…)` each
frame; the dust field's drift speed scales with `t`.

### Event-driven — reactions to battle moments
| Event | Effect |
|---|---|
| Any hit lands | white screen flash (0.2s) + **camera shake** + impact particles |
| Fighter drops below 25% HP | red **rage rim light** (`GL_LIGHT2`) switches on around them |
| KO | slow camera zoom + environment settles to deep red |

> **Rationale:** Criterion 3 says "the environment **changes** during battle." A single slow fade is
> easy to overlook; pairing a *continuous* escalation with *discrete, punchy* events guarantees the
> marker sees change within seconds of the demo starting. The rage light reuses the lighting rig, and
> the flash/shake reuse the hit event — high visibility, low cost.

---

## 11. Special Effects (Bonus Marks)

The brief grants bonus marks for effects "not being taught in classes." We implement a small,
reusable effects layer:

| Effect | Implementation | Used for |
|---|---|---|
| **Particle system** | `struct Particle{vec3 pos,vel; float life,size; vec3 col;}` updated in `tickTime`, drawn as additive billboard quads | sparks, dust, muzzle flash, blood, heal motes |
| **Blob shadows (shipped default)** | translucent ellipse under each fighter, scales with jump | grounding characters |
| **Motion-blur trails (BONUS, below cut-line)** | triangle strip over the last N claw/katana **world** positions during ACTIVE | claw streak, katana arc |
| **Planar projected shadows (BONUS, last-day only)** | shadow projection matrix onto y=0 | sharper shadows |
| **Procedural textures** | generated in code (Section 7) | sand, suit, metal |
| **Camera shake / KO zoom** | random eye offset for 0.15s; lerp zoom on win | impact + finish feel |

> **Rationale:** Each item is genuinely beyond the standard lab syllabus and improves the *video* (a
> graded deliverable). The **particle system is the highest-value single addition** — sparks, dust,
> muzzle flash, blood, and heal motes all come from one piece of code (rule b: reuse) — so its
> *foundation* (struct + additive billboard + spawn/update) is built on **Day 2**, before any skill
> needs to emit particles.
>
> **Trails are not a "15-line" feature:** they need the claw/katana tip in *world* space across
> frames. Don't read it back with `glGetFloatv` mid-draw — instead compute it analytically from the
> character's known `pos`, `facingAngle`, and the fixed claw offset (we own that transform), then
> ring-buffer the last N points. Even so, trails sit **below secondary skills** on the cut-line.
>
> **Shadows:** ship the **blob ellipse first** — it achieves "grounded" cheaply. Planar projection
> z-fights with the floor (needs `glPolygonOffset` or drawing a hair above y=0) and is last-day-only.

---

## 12. Controls

### Player 1 — Wolverine
| Key | Action |
|---|---|
| `W`/`S` | move −Z / +Z |
| `A`/`D` | move −X / +X |
| `F` | Claw Lunge (primary) |
| `G` | Berserker Regeneration (secondary) |

### Player 2 — Deadpool
| Key | Action |
|---|---|
| `↑`/`↓` | move −Z / +Z |
| `←`/`→` | move −X / +X |
| `.` (or `M`) | Katana Spin (primary) |
| `,` (or `N`) | Bang Bang (secondary) |

### Global
| Key | Action |
|---|---|
| `Enter` | start / restart |
| `1` | toggle wireframe/shading (debug & shows construction) |
| `2` | toggle lighting |
| `Esc` | quit |

> **Rationale:** Held-key movement uses `keyState[]` + `glutKeyboardUpFunc` (needs **freeglut**). The
> wireframe toggle is worth keeping because it lets the examiner *see* the hand-built polygon mesh —
> direct evidence for Construction marks. Keys are named constants, not scattered literals.
>
> **Keep bindings in sync:** these keys must match the in-game HUD help line *and* the report's
> user-manual table exactly — examiners test the documented controls. Define them once in
> `Constants.h` and render the HUD help from the same source so they can't drift.

---

## 13. File & Class Architecture

```
BlindBoxBattle/
├── BlindBoxBattle.cbp          ← Code::Blocks / Windows (GRADED build)
├── CMakeLists.txt              ← optional: CLion/Mac dev only
├── src/
│   ├── platform_gl.h           ← GL include abstraction (Win/Mac)
│   ├── Constants.h             ← ALL tunables: speeds, HP, durations, sizes, colours, keys
│   ├── MathUtil.h              ← vec3, lerp, clampf, deg/rad, angle-wrap
│   ├── Geometry.h              ← drawBox()/drawPrism()/drawQuad() primitives (reused everywhere)
│   ├── CGLabmain.hpp/.cpp      ← framework (keyboard routing added)
│   ├── CGProject.hpp/.cpp      ← MyVirtualWorld: owns world, runs battle loop
│   ├── Character.hpp/.cpp      ← THIN base: transform, HP, skill state machine (NO geometry)
│   ├── Wolverine.hpp/.cpp      ← all Wolverine geometry: cowl, claws, body; two skills
│   ├── Deadpool.hpp/.cpp       ← all Deadpool geometry: head/patches, katanas, pistols; two skills
│   ├── Katana.hpp/.cpp         ← reusable katana model (used by Deadpool ×2)
│   ├── Arena.hpp/.cpp          ← ground, sky, debris, environment intensity
│   ├── Particles.hpp/.cpp      ← particle system (sparks/dust/flash/blood/heal)
│   ├── TextureGen.hpp/.cpp     ← procedural textures (+ optional stb_image load)
│   └── HUD.hpp/.cpp            ← HP bars, skill indicators, text, win screen
└── assets/   (optional)        ← drop-in .bmp/.png upgrades; absence is handled
```

```
MyVirtualWorld
 ├ owns Wolverine, Deadpool, Arena, ParticleSystem, HUD
 ├ GameState, combinedHP, flashTimer, shakeTimer
 ├ draw()     → arena.draw(); shadows; characters; particles; HUD
 ├ tickTime() → movement, skill phases, hit detection, env intensity, particles
 └ handleKey()→ keyState[], skill triggers, restart

Character (THIN base of Wolverine/Deadpool — logic only, NO drawing)
 ├ pos, vel, facingAngle, hp, hitStun, skillPhase, skillTimer, buffs
 ├ move(dt), faceOpponentIfIdle(), startSkill(id), tickSkill(dt), takeHit()
 └ virtual draw() = 0;          // each subclass owns ALL of its geometry

Wolverine / Deadpool : public Character
 └ draw(): assembles body from Geometry.h primitives (drawBox/drawPrism)
           + own parts (Wolverine: cowl, claws;  Deadpool: head, patches, katanas, pistols)
```

> **Rationale (Criterion 4 + rules a,b):** The base class shares *logic* (movement, skill state
> machine, hit handling) but **not drawing** — Wolverine's arms end in claws and Deadpool's in
> katanas/pistols, so a shared `drawArms()` would create an override hierarchy that costs more than it
> saves. Reuse instead happens at the **primitive level**: every body part calls `drawBox()` /
> `drawPrism()` from `Geometry.h`, so we satisfy rule (b) reuse without a fragile draw hierarchy.
> `Constants.h` makes "no hard-coding" literally true; one class per file reads cleanly for the marker.

---

## 14. Build Setup (Windows-first)

> **Decision: the graded build is Code::Blocks + MinGW on Windows 10. That build is the source of
> truth and is tested from Day 1.** The CMake/Mac path is kept only as an optional dev convenience and
> must never delay the Windows build.

> **Rationale:** The Mac build earns **zero marks** and the demo is on a laptop running the graded
> toolchain. The first draft treated both equally, which risks "works on Mac, breaks in Code::Blocks"
> the night before. Keeping `platform_gl.h` is nearly free; *gold-plating* the Mac path is not.

### `platform_gl.h`
```cpp
#ifndef PLATFORM_GL_H
#define PLATFORM_GL_H
#ifdef __APPLE__
  #define GL_SILENCE_DEPRECATION
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>          // freeglut under MinGW / Code::Blocks
#endif
#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif
#endif
```

### Code::Blocks / Windows
- Use **freeglut** (needed for `glutKeyboardUpFunc`). Linker order: `freeglut`, `opengl32`, `glu32`.
- Put freeglut headers/libs on the search paths; drop `freeglut.dll` beside the `.exe`.
- **Milestone 0:** the source compiles and shows a lit test cube/axis in Code::Blocks on Windows
  before any modelling begins.

---

## 15. Build Order & Timeline

> **Core rule: reach a playable battle loop (both move, primary skills work, someone wins) before any
> textures or bonus polish. The loop is where 28+ of the 40 marks live.**

### Team lanes (4 members — parallelise, don't serialise)
The biggest schedule risk is treating this as one developer's serial to-do list. It is a **4-person
team**, so the two characters (the heaviest work) are built **simultaneously**:

| Lane | Owner | Responsible for |
|---|---|---|
| **A — Wolverine** | Dev 1 | Wolverine model (body boxes, cowl, claws) + both his skills |
| **B — Deadpool** | Dev 2 | Deadpool model (body, head/patches, katanas, pistols) + both his skills |
| **C — World & FX** | Dev 3 | Arena, sky, debris, lighting rig, environment intensity, **particle system**, shadows, procedural textures |
| **D — Loop & Docs** | Dev 4 | Framework, movement/facing, hit detection, HP, game states, HUD, build setup, report assembly |

> **Rationale:** This resolves the "Day 2 is overloaded" risk by *parallelism* rather than just
> slipping dates — two people model two characters at once, so each gets ~1.5 days, not half a day.
> It also produces the **per-member milestone split** the Team Declaration Form requires for free.
> (Adjust owners to your team's actual strengths; the lanes, not the names, are the point.)

### Daily targets (dependency order preserved within each lane)

| Day | Shared / cross-lane gate | Per-lane targets |
|---|---|---|
| **Day 1 (Jun 14)** | Milestone 0 (Windows build shows a lit cube); `platform_gl.h`, `Constants.h`, `MathUtil.h`, **`Geometry.h`** done; empty classes compile. **Confirm blind-box characters with lecturer today.** | C: arena ground + camera + lighting rig. D: framework + `keyState[]` routing. |
| **Day 2 (Jun 15)** | — | A: Wolverine static model, lit. B: Deadpool static model, lit. C: debris + sky + sand texture + **particle foundation**. D: movement + facing + blob shadows + HUD skeleton. |
| **Day 3 (Jun 16)** | Integrate models into world; **normals/specular pass** (kill every black face). | A/B: start primary skills (lunge / spin), emit sparks. C: env intensity (dusk + sandstorm) + impact flash + camera shake. D: hit detection + HP + hit-stun + win/restart. |
| **Day 4 (Jun 17)** | **Lock the playable loop end-to-end** — both primaries hit, HP drops, someone wins, environment visibly changes, ≥1 texture applied. *Core 28+ marks are done today.* | A/B: polish primary-skill timing. C: textures on suit/ground. D: states + HUD complete. |
| **Day 5 (Jun 18)** | Secondary skills (pistols, regen) — **HARD CUT at 4 PM if not done, no debate.** Then **freeze features**: code cleanup (constants, comments, zero warnings), final compile **on the demo laptop**, **record the video**, assemble report. | Trails / planar shadows only if everything above is finished. |
| **Jun 19** | **Real buffer.** Submit by midday; fix only what breaks. Never submit at 11:58 PM. | — |

**If time is short, cut in this exact order (never touch the core):**
1. **Secondary skills** — hard cut 4 PM Day 5 (keep one polished skill each)
2. **Motion trails** (needs world-space tracking — see Section 11)
3. **Planar projected shadows** → blob shadows are already shipped
4. **Camera shake / KO zoom**

**Never cut:** hand-built characters, primary skills, hit detection, environment change, *at least one
shipped texture.*

> **Rationale:** Highest-mark, highest-risk work runs first and in parallel; everything optional sits
> behind an explicit, *time-boxed* cut-line. A crunch then degrades gracefully from "excellent" toward
> "above requirement" instead of threatening the video or a clean compile.

---

## 16. Marking Criteria Checklist

| Criterion | Max | Target | Evidence in this design |
|---|---|---|---|
| Construction of Characters | 14 | 12–14 | Hand-built face-list bodies + claws/cowl/katanas/patches/pouches/debris; custom-geometry inventory; wireframe toggle to prove it |
| Animation of Skills + effects | 14 | 12–14 | 2 skills × 2 characters, 3-phase state machines, lighting + smooth shading + specular + **procedural textures** + particles/trails |
| Battle Environment | 8 | 7–8 | Continuous HP-driven dusk + sandstorm **and** event-driven flash/rage/shake |
| Document Management | 4 | 4 | Class-per-file, `Constants.h` (no magic numbers), comments, 4-space indent, illustrated report |
| Bonus | + | — | Particles, trails, projected shadows, procedural textures, camera shake |

**Lab-technique map (cite in report):** vertex/face list (Lab 04), push/pop (Lab 05/07), hierarchical
modelling (Lab 07), time-based animation (Lab 09), quadrics (Lab 05/07), random point field (Lab 06),
Bézier surface for dunes (Lab 08, optional bonus).

---

## 17. Pitfalls & Bug Fixes

| Pitfall | Prevention |
|---|---|
| Missing normals → black models | `glNormal3f` on every face as it is built |
| `glScalef` breaks lighting | `glEnable(GL_NORMALIZE)` in init |
| **Facing test fails at 0/360° seam → hits miss** | Normalise angle difference to [−180,180] (Section 9) |
| Auto-facing makes facing test dead | Only auto-face while idle/moving, not during a skill |
| Cumulative transforms corrupt 2nd character | Every `draw()` wrapped in push/pop |
| Skill re-fires while key held | Skill starts only from `SKILL_IDLE` |
| Characters clip through each other | Minimum-separation clamp each tick |
| Missing texture file crashes demo | Procedural texture is the default; image load is optional + null-checked |
| Shared `drawArms()` in base class | Geometry lives in subclasses; reuse via `Geometry.h` primitives only |
| Trails depend on unbuilt particle system | Particle foundation built Day 2, before skills emit |
| Windows-only breakage found late | Build/test in Code::Blocks from Day 1; final compile on the demo laptop |
| freeglut.dll not found at runtime | Ship `freeglut.dll` next to the `.exe` |

---

## 18. Deliverables Checklist

- [ ] **Report (PDF):** cover (team, IDs, names, lab section), introduction + screenshot, virtual-model
      documentation (custom-geometry inventory), user manual (controls table), labelled screenshots,
      **acknowledgement of `stb_image` and the blind-box character justification**
- [ ] **Source code:** organised, commented, `Constants.h`, compiles clean in Code::Blocks/MinGW
- [ ] **Team Declaration Form:** per-member milestones + timeline (use the Section 15 lanes)
- [ ] **Video (<5 min):** both characters, all skills, a hit + flash/shake, environment dusk→sandstorm,
      a KO + restart
- [ ] `freeglut.dll` bundled; final compile verified on the demo laptop

---

## 19. Decision Log

| Decision | Chosen | Rejected | Why (marks) |
|---|---|---|---|
| Character geometry | Hand-built face-list boxes | GLUT quadrics for bodies | Quadrics don't count for Construction (14 marks) |
| Texturing | Procedural (code) + optional image | Image-only / optional | Texturing is graded; procedural guarantees it ships + bonus |
| Skills per character | 2 (offensive + defensive/ranged) | 1 each | Richness → "excellent" on the 14-mark animation criterion |
| Environment change | Continuous + event-driven | Single slow fade | Must be obvious to the marker (8 marks) |
| Build target | Windows/Code::Blocks first | Mac/CMake co-equal | Only Windows is graded; Mac earns 0 |
| GLUT flavour | freeglut | classic GLUT/glut32 | Needs `glutKeyboardUpFunc` for held-key movement |
| `Character` base | Thin (logic only) | Shared draw hierarchy | Avoids override-hierarchy trap; reuse via `Geometry.h` |
| Timeline | Parallel team lanes | Serial single-dev list | Resolves Day-2 crunch; feeds the declaration form |
| Camera | Fixed + shake/zoom | Follow-cam | Low risk; shake/zoom add bonus polish cheaply |
| Effects layer | One reusable particle system | Per-effect ad hoc code | Reuse (rule b) + bonus marks |
| Constants | `Constants.h` | Inline literals | "No hard-coding" (rule b) + Document marks |

---

## 20. Open Items to Confirm

| # | Item | Recommendation |
|---|---|---|
| 1 | Lecturer accepts Funko Deadpool/Wolverine as "blind box" | **Confirm on Day 1** (one-line email or ask before/after class) — not at report time. Then justify in the report (Funko Mystery Minis). Low risk, but resolve it before modelling effort goes in. |
| 2 | Jump (Y-axis)? | Defer — add only if Day 4 finishes early; X/Z + skills is sufficient |
| 3 | Real texture art vs procedural only | Procedural is the baseline; drop in `.bmp`/`.png` later if a teammate makes them |
| 4 | Second skills in scope? | Yes if core loop is done by end of Day 4; otherwise they are the first cut (hard cut 4 PM Day 5) |

---

*Document prepared for TCG6223 Group Project — Deadline 19 June 2026 (v2.1).*
*C++ | Build: Code::Blocks + MinGW (Windows, graded) — CMake/Mac optional | Pipeline: OpenGL fixed-function + freeglut.*
