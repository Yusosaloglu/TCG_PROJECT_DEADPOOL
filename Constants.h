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

// Skill 2 — spin attacks (in place) shared by both characters
constexpr float WINDUP_SPIN    = 0.25f;  // raise weapon, claws pop out
constexpr float ACTIVE_SPIN    = 0.55f;  // the 360-degree whirlwind
constexpr float RECOV_SPIN     = 0.30f;
constexpr float SPIN_TURNS     = 2.0f;   // full rotations during ACTIVE (x360)

// Damage
constexpr float DMG_SLASH      = 18.f;   // Wolverine claw slash
constexpr float DMG_KATANA     = 20.f;   // Deadpool katana slash
constexpr float DMG_SPIN       = 26.f;   // either spin attack (omni-directional)

// Skill animation poses (degrees)
constexpr float CLAW_SLASH_LOW   = 25.f;   // arm pitch at start of Wolverine up-slash
constexpr float CLAW_SLASH_HIGH  = 155.f;  // arm pitch at top of up-slash
constexpr float KATA_IDLE_PITCH  = 35.f;   // arm pitch when idle: hands forward, blades up
constexpr float KATA_RAISE_PITCH = 150.f;  // cocked back before the forward chop
constexpr float KATA_CHOP_PITCH  = 25.f;   // follow-through of the forward chop
constexpr float SPIN_ARMS_OUT    = 90.f;   // arms spread to the sides during spin
constexpr float CLAW_LEN         = 0.42f;  // extended claw blade length (units)

// Environment
constexpr float ENV_IMPACT_FLASH_DUR = 0.12f;  // white flash seconds
constexpr float ENV_RAGE_THRESHOLD   = 0.35f;  // HP fraction (0–1) for rage light
