#pragma once

// ──────────────────────────────────────────────────────────────────────────────
// Camera — tuned so a character at (±ARENA_HALF, 0, ±ARENA_HALF) stays framed.
// Verify on Day 1: place a test box at (6,0,6) and check it's fully on-screen.
// If not, increase CAMERA_DIST or decrease ARENA_HALF before proceeding.
// ──────────────────────────────────────────────────────────────────────────────
constexpr float ARENA_HALF    = 6.f;
constexpr float CAMERA_HEIGHT = 7.f;
constexpr float CAMERA_DIST   = 18.f;
constexpr float CAMERA_FOV    = 60.f;
constexpr float NEAR_PLANE    = 0.1f;
constexpr float FAR_PLANE     = 100.f;

// Movement
constexpr float PLAYER_SPEED   = 3.f;    // world units per second
constexpr float PLAYER_TURN    = 90.f;   // degrees per second
constexpr float MIN_SEPARATION = 1.2f;   // characters can't overlap closer than this

// Combat
constexpr float HIT_RANGE      = 2.2f;   // world units
constexpr float HIT_ANGLE_TOL  = 45.f;   // half-cone degrees from facing direction

// HP
constexpr float MAX_HP         = 100.f;

// Skill 1 — forward/upward slash phase durations (seconds)
constexpr float WINDUP_SLASH   = 0.25f;  // Wolverine claw slash (down -> up)
constexpr float ACTIVE_SLASH   = 0.22f;
constexpr float RECOV_SLASH    = 0.35f;

constexpr float WINDUP_KATA    = 0.22f;  // Deadpool katana slash (forward)
constexpr float ACTIVE_KATA    = 0.20f;
constexpr float RECOV_KATA     = 0.35f;

// Skill 2 — Wolverine: Berserker Heal (gauge fuelled by landed Claw Slashes)
constexpr float HEAL_GAUGE_MAX    = 60.f;   // max HP that can be banked in the gauge
constexpr float HEAL_GAIN_PER_HIT = 15.f;   // HP added to the gauge per Claw Slash hit
constexpr float WINDUP_HEAL       = 0.30f;  // aura ramps in
constexpr float ACTIVE_HEAL       = 0.70f;  // banked HP pours back over this window
constexpr float RECOV_HEAL        = 0.30f;  // aura ramps out
constexpr float HEAL_AURA_RADIUS  = 0.95f;  // translucent green glow sphere radius
constexpr float HEAL_AURA_ALPHA   = 0.30f;  // peak aura opacity

// Skill 2 — Deadpool: Desert Eagle (instant hitscan + tracer)
constexpr float WINDUP_GUN     = 0.22f;  // raise the gun into the aim pose
constexpr float ACTIVE_GUN     = 0.14f;  // the fire frame
constexpr float RECOV_GUN      = 0.30f;  // lower the gun
constexpr float GUN_RANGE      = 14.f;   // hitscan reach (spans the arena diagonal)
constexpr float GUN_ANGLE_TOL  = 12.f;   // half-cone degrees from facing for a hit
constexpr float TRACER_DUR     = 0.10f;  // seconds the tracer beam stays visible
constexpr float GUN_AIM_PITCH  = 90.f;   // arm pitch for the straight-forward aim
constexpr float GUN_RECOIL_DEG = 18.f;   // arm kick on the fire frame
constexpr float MUZZLE_FWD     = 0.55f;  // muzzle offset forward of body centre
constexpr float MUZZLE_SIDE    = 0.36f;  // muzzle offset to the gun-arm side (shoulder)
constexpr float MUZZLE_HEIGHT  = 1.15f;  // muzzle height above ground

// Damage
constexpr float DMG_SLASH      = 18.f;   // Wolverine claw slash
constexpr float DMG_KATANA     = 20.f;   // Deadpool katana slash
constexpr float DMG_GUN        = 22.f;   // Deadpool Desert Eagle shot (ranged)

// Skill animation poses (degrees)
constexpr float CLAW_SLASH_LOW   = 25.f;   // arm pitch at start of Wolverine up-slash
constexpr float CLAW_SLASH_HIGH  = 155.f;  // arm pitch at top of up-slash
constexpr float KATA_IDLE_PITCH  = 35.f;   // arm pitch when idle: hands forward, blades up
constexpr float KATA_SLASH_PITCH = 85.f;   // arm held near-horizontal during the slash
constexpr float KATA_SLASH_YAW   = 75.f;   // half-arc of the left->right horizontal sweep
constexpr float CLAW_LEN         = 0.42f;  // extended claw blade length (units)

// Environment
constexpr float ENV_IMPACT_FLASH_DUR = 0.12f;  // white flash seconds
constexpr float ENV_RAGE_THRESHOLD   = 0.35f;  // HP fraction (0–1) for rage light

// Sand-wind ambience
constexpr int   DUST_COUNT  = 150;   // drifting sand particles kept alive
constexpr float WIND_BASE   = 1.4f;  // base wind speed (units/s, blows +X)
constexpr float WIND_GUST   = 1.4f;  // extra speed swing from gusts
constexpr float DUST_ALPHA  = 0.22f; // peak opacity of a dust streak
