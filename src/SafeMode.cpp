#include "SafeMode.hpp"
#include <Geode/modify/PlayLayer.hpp>

bool SafeMode::s_noclip = false;
bool SafeMode::s_cheatedThisAttempt = false;

bool SafeMode::isNoclip() {
    return s_noclip;
}

void SafeMode::setNoclip(bool on) {
    s_noclip = on;
    // Turning noclip ON at any point "cheats" the current attempt.
    if (on)
        s_cheatedThisAttempt = true;
}

bool SafeMode::isCheated() {
    return s_noclip || s_cheatedThisAttempt;
}

void SafeMode::onNewAttempt() {
    // A brand-new attempt starts vanilla unless noclip is simply still on.
    s_cheatedThisAttempt = s_noclip;
}

// ---------------------------------------------------------------------------
// Hooks on PlayLayer to prevent a cheated completion from counting.
// ---------------------------------------------------------------------------
class $modify(SafeModePlayLayer, PlayLayer) {
    // Reset the per-attempt "cheated" flag as soon as a new attempt starts.
    void resetLevel() {
        PlayLayer::resetLevel();
        SafeMode::onNewAttempt();
    }

    // When the level is completed and noclip was used, mark the run as a
    // "test" run so the game does not count it as a real completion / record.
    void levelComplete() {
        if (Mod::get()->getSettingValue<bool>("safe-mode") && SafeMode::isCheated()) {
            m_isTestMode = true;
        }
        PlayLayer::levelComplete();
    }
};
