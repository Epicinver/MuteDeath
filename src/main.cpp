#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
using namespace geode::prelude;

class ModSettingsLayer : public CCLayer
{
public:
	static bool s_open;

	CCSprite *m_panel = nullptr;

	static ModSettingsLayer *create()
	{
		auto ret = new ModSettingsLayer();
		if (ret && ret->init())
		{
			ret->autorelease();
			return ret;
		}
		CC_SAFE_DELETE(ret);
		return nullptr;
	}

	bool init()
	{
		if (!CCLayer::init())
			return false;

		s_open = true;

		this->setTouchEnabled(true);
		this->setKeypadEnabled(true);

		auto winSize = CCDirector::sharedDirector()->getWinSize();

		// === Outside overlay (transparent, clickable) ===
		auto overlay = CCLayerColor::create({0, 0, 0, 120});
		overlay->setContentSize(winSize);
		overlay->setPosition({0, 0});
		this->addChild(overlay);

		// === Swallow touches INSIDE panel ===
		auto touchBlocker = CCLayer::create();
		touchBlocker->setTouchEnabled(true);
		touchBlocker->setContentSize({400.f, 260.f});
		touchBlocker->setPosition({winSize.width / 2 - 200.f,
								   winSize.height / 2 - 130.f});
		this->addChild(touchBlocker, 1);

		// === Panel (NO transparency) ===
		m_panel = CCSprite::createWithSpriteFrameName("GJ_fxOnBtn_001.png");
		m_panel->setScale(6.0f);
		m_panel->setPosition(winSize / 2);
		this->addChild(m_panel, 2);

		// === Menu ===
		auto menu = CCMenu::create();
		menu->setPosition(winSize / 2);
		this->addChild(menu, 3);

		// === Close button ===
		auto closeSprite = CCSprite::createWithSpriteFrameName("GJ_fxOnBtn_001.png");
		closeSprite->setScale(0.8f);

		auto closeBtn = CCMenuItemSpriteExtra::create(
			closeSprite,
			this,
			menu_selector(ModSettingsLayer::onClose));

		closeBtn->setPosition({160.f, 100.f});
		menu->addChild(closeBtn);

		// === Toggles ===
		float startY = 40.f;
		float spacing = 40.f;

		addToggle(menu, "ENABLE HACK", startY, "enable_hack");
		addToggle(menu, "SHOW HITBOXES", startY - spacing, "show_hitboxes");
		addToggle(menu, "FPS BYPASS", startY - spacing * 2, "fps_bypass");

		return true;
	}

	// === Toggle row ===
	void addToggle(CCMenu *menu, const char *text, float y, const std::string &key)
	{
		auto label = CCLabelBMFont::create(text, "goldFont.fnt");
		label->setScale(0.45f);
		label->setAnchorPoint({0.f, 0.5f});
		label->setPosition({-130.f, y});
		menu->addChild(label);

		bool enabled = Mod::get()->getSavedValue<bool>(key, false);

		auto onSprite = CCSprite::createWithSpriteFrameName("GJ_fxOnBtn_001.png");
		auto offSprite = CCSprite::createWithSpriteFrameName("GJ_fxOnBtn_001.png");

		auto toggle = CCMenuItemToggler::create(
			onSprite,
			offSprite,
			this,
			menu_selector(ModSettingsLayer::onToggle));

		toggle->setScale(0.6f);
		toggle->setPosition({130.f, y});
		toggle->setTag(reinterpret_cast<intptr_t>(new std::string(key)));
		toggle->toggle(enabled);

		menu->addChild(toggle);
	}

	void onToggle(CCObject *sender)
	{
		auto toggle = static_cast<CCMenuItemToggler *>(sender);
		auto keyPtr = reinterpret_cast<std::string *>(toggle->getTag());

		bool enabled = toggle->isToggled();
		Mod::get()->setSavedValue(*keyPtr, enabled);
	}

	// === Close ===
	void onClose(CCObject *)
	{
		s_open = false;
		this->removeFromParentAndCleanup(true);
	}

	// === Click outside ===
	bool ccTouchBegan(CCTouch *, CCEvent *) override
	{
		onClose(nullptr);
		return true;
	}

	// === ESC ===
	void keyBackClicked() override
	{
		onClose(nullptr);
	}
};

bool ModSettingsLayer::s_open = false;

class $modify(MyMenuLayer, MenuLayer)
{
public:
	bool init()
	{
		if (!MenuLayer::init())
		{
			return false;
		}

		// button creation (only thing in the mod that we use mostly)
		auto modSettings = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("GJ_fxOnBtn_001.png"),
			this,
			menu_selector(MyMenuLayer::onModSettings));

		log::info("adding button");

		auto menu = this->getChildByID("bottom-menu");
		menu->addChild(modSettings);

		modSettings->setID("mod-settings"_spr);

		menu->updateLayout();

		return true;
	}

	void onModSettings(CCObject *)
	{
		log::info("mod settings clicked");

		// Menu
		CCDirector::sharedDirector()
			->getRunningScene()
			->addChild(ModSettingsLayer::create(), 999);
	}
};
