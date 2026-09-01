#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "SafeMode.hpp"
#include <cmath>

using namespace geode::prelude;

// ---------------------------------------------------------------------------
// Bouton noclip à l'écran (dessiné sans sprite + label texte) — vert = ON, rouge = OFF.
// ---------------------------------------------------------------------------
class NoclipButtonSprite : public cocos2d::CCNode {
public:
    static NoclipButtonSprite* create() {
        auto ret = new NoclipButtonSprite();
        if (ret && ret->init()) { ret->autorelease(); return ret; }
        delete ret; return nullptr;
    }

    bool init() {
        if (!CCNode::init()) return false;
        m_circle = cocos2d::CCDrawNode::create();
        m_label = cocos2d::CCLabelBMFont::create("NC", "bigFont.fnt");
        m_label->setScale(0.6f);
        this->addChild(m_circle);
        this->addChild(m_label);
        refresh();
        return true;
    }

    void refresh() {
        float radius = 22.f;
        std::vector<cocos2d::CCPoint> pts;
        for (int i = 0; i < 32; i++) {
            float a = i * (acos(-1.f) * 2.f / 32.f);
            pts.push_back(ccp(std::cos(a) * radius, std::sin(a) * radius));
        }
        cocos2d::ccColor4F fill;
        if (SafeMode::isNoclip() == Mod::get()->getSettingValue<bool>("green-means-on"))
            fill = {0.f, 1.f, 0.55f, 1.f};   // vert = ON (ou inversé)
        else
            fill = {1.f, 0.2f, 0.2f, 1.f};   // rouge = OFF (ou inversé)

        m_circle->clear();
        m_circle->drawPolygon(pts.data(), pts.size(), fill, 2.f, {0.f, 0.f, 0.f, 1.f});
        this->setContentSize(cocos2d::CCSize(radius * 2, radius * 2));
        m_label->setPosition(ccp(0, 0));
    }

    void setLabelText(const char* t) {
        m_label->setString(t);
    }

private:
    cocos2d::CCDrawNode* m_circle = nullptr;
    cocos2d::CCLabelBMFont* m_label = nullptr;
};

class $modify(NoclipPlayLayer, PlayLayer) {
    struct Fields {
        cocos2d::CCMenu* m_noclipMenu = nullptr;
        NoclipButtonSprite* m_noclipSprite = nullptr;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;

        SafeMode::setNoclip(Mod::get()->getSettingValue<bool>("noclip-enabled"));
        SafeMode::onNewAttempt();
        createNoclipButton();

        log::info("SafeNoclip: PlayLayer::init called, show-button = {}",
                  Mod::get()->getSettingValue<bool>("show-button"));
        return true;
    }

    // Fallback : si onEnter est appelé et que le bouton manque encore, on le crée.
    void onEnter() {
        PlayLayer::onEnter();
        if (!m_fields->m_noclipMenu)
            createNoclipButton();
    }

    void createNoclipButton() {
        if (!Mod::get()->getSettingValue<bool>("show-button")) {
            log::info("SafeNoclip: show-button is OFF, skipping button");
            return;
        }
        if (m_fields->m_noclipMenu)
            return;

        m_fields->m_noclipSprite = NoclipButtonSprite::create();

        auto item = CCMenuItemSpriteExtra::create(
            m_fields->m_noclipSprite, this, menu_selector(NoclipPlayLayer::onNoclipClicked));
        item->setScale(1.f);

        m_fields->m_noclipMenu = cocos2d::CCMenu::createWithItem(item);

        auto winSize = cocos2d::CCDirector::get()->getWinSize();
        m_fields->m_noclipMenu->setPosition(ccp(winSize.width - 45.f, winSize.height - 55.f));
        m_fields->m_noclipMenu->setZOrder(100000);

        this->addChild(m_fields->m_noclipMenu);
        updateNoclipVisual();

        log::info("SafeNoclip: button added at ({}, {})", winSize.width - 45.f, winSize.height - 55.f);
    }

    void onNoclipClicked(cocos2d::CCObject* sender) {
        SafeMode::setNoclip(!SafeMode::isNoclip());
        Mod::get()->setSettingValue("noclip-enabled", SafeMode::isNoclip());
        updateNoclipVisual();
        log::info("SafeNoclip: noclip toggled -> {}", SafeMode::isNoclip());
    }

    void updateNoclipVisual() {
        if (!m_fields->m_noclipSprite)
            return;
        m_fields->m_noclipSprite->refresh();
        m_fields->m_noclipSprite->setLabelText(SafeMode::isNoclip() ? "NC ON" : "NC OFF");
    }

    // No-clip : évite la mort quand activé.
    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        if (SafeMode::isNoclip()) {
            log::info("SafeNoclip: skipping death (noclip ON)");
            return;
        }
        PlayLayer::destroyPlayer(player, obj);
    }
};

$on_mod(Loaded) {
    listenForSettingChanges<bool>("noclip-enabled", +[](bool enabled) {
        SafeMode::setNoclip(enabled);
    });
    log::info("SafeNoclip mod loaded");
}
