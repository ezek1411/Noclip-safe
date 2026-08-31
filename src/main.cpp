#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "SafeMode.hpp"
#include <cmath>

using namespace geode::prelude;

// ---------------------------------------------------------------------------
// On-screen noclip toggle button.
//   green -> noclip ON   |   red -> noclip OFF   (flip via the "green-means-on" setting)
// Tapping it toggles noclip and redraws the circle colour.
// ---------------------------------------------------------------------------

// Simple filled-circle node so we don't depend on any sprite asset.
class NoclipButtonSprite : public cocos2d::CCNode {
public:
    static NoclipButtonSprite* create() {
        auto ret = new NoclipButtonSprite();
        if (ret && ret->init()) { ret->autorelease(); return ret; }
        delete ret; return nullptr;
    }

    bool init() {
        if (!CCNode::init()) return false;
        // circle + label live inside a container node
        m_circle = cocos2d::CCDrawNode::create();
        m_label = cocos2d::CCLabelBMFont::create("NC", "bigFont.fnt");
        m_label->setScale(0.6f);
        m_circle->setContentSize({0, 0});
        this->setContentSize({ m_circle->getContentSize().width, m_circle->getContentSize().height });
        this->addChild(m_circle);
        this->addChild(m_label);
        refresh();
        return true;
    }

    void refresh() {
        float radius = 22.f;
        cocos2d::CCPoint center = ccp(0, 0);

        // Build a filled polygon of 32 points approximating a circle.
        std::vector<cocos2d::CCPoint> pts;
        for (int i = 0; i < 32; i++) {
            float a = i * (acos(-1.f) * 2.f / 32.f);
            pts.push_back(ccp(center.x + std::cos(a) * radius, center.y + std::sin(a) * radius));
        }
        cocos2d::ccColor4F fill;
        if (SafeMode::isNoclip() == Mod::get()->getSettingValue<bool>("green-means-on"))
            fill = {0.f, 1.f, 0.55f, 1.f};   // green = on (or inverted)
        else
            fill = {1.f, 0.2f, 0.2f, 1.f};   // red = off (or inverted)

        m_circle->clear();
        m_circle->drawPolygon(pts.data(), pts.size(), fill, 2.f, {0.f, 0.f, 0.f, 1.f});
        this->setContentSize(cocos2d::CCSize(radius * 2, radius * 2));
    }

    void setLabelText(const char* t) {
        m_label->setString(t);
        m_label->setPosition(ccp(0, 0));
    }

private:
    cocos2d::CCDrawNode* m_circle = nullptr;
    cocos2d::CCLabelBMFont* m_label = nullptr;
};

class $modify(NoclipPlayLayer, PlayLayer) {
    // Geode forbids adding plain members to a modify class:
    // they must live in a `Fields` struct, accessed via `m_fields->`.
    struct Fields {
        cocos2d::CCMenu* m_noclipMenu = nullptr;
        NoclipButtonSprite* m_noclipSprite = nullptr;
    };

    void onEnter() {
        PlayLayer::onEnter();
        SafeMode::setNoclip(Mod::get()->getSettingValue<bool>("noclip-enabled"));
        SafeMode::onNewAttempt();
        createNoclipButton();
    }

    void createNoclipButton() {
        if (!Mod::get()->getSettingValue<bool>("show-button"))
            return;

        if (m_fields->m_noclipMenu)
            return;

        m_fields->m_noclipSprite = NoclipButtonSprite::create();

        // CCMenuItemSpriteExtra lives in the geode namespace (no `cocos2d::` prefix).
        auto item = CCMenuItemSpriteExtra::create(
            m_fields->m_noclipSprite, this, menu_selector(NoclipPlayLayer::onNoclipClicked));
        item->setScale(1.f);

        m_fields->m_noclipMenu = cocos2d::CCMenu::createWithItem(item);

        auto winSize = cocos2d::CCDirector::get()->getWinSize();
        m_fields->m_noclipMenu->setPosition(ccp(winSize.width - 40.f, winSize.height - 45.f));
        m_fields->m_noclipMenu->setZOrder(1000);

        this->addChild(m_fields->m_noclipMenu);
        updateNoclipVisual();
    }

    void onNoclipClicked(cocos2d::CCObject* sender) {
        SafeMode::setNoclip(!SafeMode::isNoclip());
        Mod::get()->setSettingValue("noclip-enabled", SafeMode::isNoclip());
        updateNoclipVisual();
    }

    void updateNoclipVisual() {
        if (!m_fields->m_noclipSprite)
            return;
        m_fields->m_noclipSprite->refresh();
        m_fields->m_noclipSprite->setLabelText(SafeMode::isNoclip() ? "NC ON" : "NC OFF");
    }

    // No-clip: skip death when enabled.
    // Signature taken from the 2.2 bindings (mirrors the common Geode noclip mod).
    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        if (SafeMode::isNoclip())
            return;
        PlayLayer::destroyPlayer(player, obj);
    }
};

// Sync noclip state whenever the setting is flipped (e.g. from the mod menu).
$on_mod(Loaded) {
    listenForSettingChanges<bool>("noclip-enabled", +[](bool enabled) {
        SafeMode::setNoclip(enabled);
    });
}
