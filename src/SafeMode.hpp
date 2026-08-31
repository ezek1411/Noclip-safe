#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

// ---------------------------------------------------------------------------
// SafeMode: tracks whether the player used noclip, so that a level completed
// with noclip is NOT counted as a real completion.
//
//   isCheated()  -> true  if noclip is active now OR was used this attempt
//                   false if the level was played completely vanilla
// ---------------------------------------------------------------------------
class SafeMode {
public:
    // Noclip state
    static bool isNoclip();
    static void setNoclip(bool on);

    // Cheat detection
    static bool isCheated();

    // Called at the start of every attempt (resets the per-attempt flag)
    static void onNewAttempt();

private:
    static bool s_noclip;
    static bool s_cheatedThisAttempt;
};
