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
constexpr float HEAL_RATE      = 15.f;   // HP per second (Wolverine Skill 2)

// Skill phase durations (seconds)
constexpr float WINDUP_SLASH   = 0.30f;
constexpr float ACTIVE_SLASH   = 0.20f;
constexpr float RECOV_SLASH    = 0.40f;

constexpr float WINDUP_KATA    = 0.25f;
constexpr float ACTIVE_KATA    = 0.15f;
constexpr float RECOV_KATA     = 0.50f;

constexpr float WINDUP_HEAL    = 0.10f;
constexpr float ACTIVE_HEAL    = 3.00f;  // 3-second healing channel
constexpr float RECOV_HEAL     = 0.30f;

constexpr float BLINK_HIDE     = 0.12f;  // Deadpool: invisible during teleport
constexpr float RECOV_BLINK    = 0.45f;

// Damage
constexpr float DMG_SLASH      = 18.f;
constexpr float DMG_KATANA     = 22.f;
constexpr float DMG_BLINK      = 28.f;   // backstab bonus damage

// Environment
constexpr float ENV_IMPACT_FLASH_DUR = 0.12f;  // white flash seconds
constexpr float ENV_RAGE_THRESHOLD   = 0.35f;  // HP fraction (0–1) for rage light
