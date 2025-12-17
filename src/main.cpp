#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
using namespace geode::prelude;

class ModSettingsLayer : public CCLayer
{
public:
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

		this->setTouchEnabled(true);
		this->setKeypadEnabled(true);

		auto winSize = CCDirector::sharedDirector()->getWinSize();

		// === Dark overlay ===
		auto overlay = CCLayerColor::create({0, 0, 0, 160});
		this->addChild(overlay);

		// === Panel ===
		auto panel = CCScale9Sprite::create("GJ_square01.png");
		panel->setContentSize({360.f, 240.f});
		panel->setPosition(winSize / 2);
		panel->setOpacity(220);
		this->addChild(panel);

		// === Title ===
		auto title = CCLabelBMFont::create("MOD SETTINGS", "bigFont.fnt");
		title->setScale(0.6f);
		title->setPosition({panel->getPositionX(),
							panel->getPositionY() + panel->getContentSize().height / 2 - 30});
		this->addChild(title);

		// === Menu ===
		auto menu = CCMenu::create();
		menu->setPosition(panel->getPosition());
		this->addChild(menu);

		// === Close button (top-right) ===
		auto closeCircle = CCSprite::createWithSpriteFrameName("GJ_fxOnBtn_001.png");
		closeCircle->setScale(0.7f);

		auto closeBtn = CCMenuItemSpriteExtra::create(
			closeCircle,
			this,
			menu_selector(ModSettingsLayer::onClose));

		closeBtn->setPosition({panel->getContentSize().width / 2 - 18,
							   panel->getContentSize().height / 2 - 18});

		auto closeIcon = CCSprite::createWithSpriteFrameName("GJ_fxOnBtn_001.png");
		closeIcon->setPosition(closeCircle->getContentSize() / 2);
		closeCircle->addChild(closeIcon);

		menu->addChild(closeBtn);

		// === SETTINGS ===
		float startY = 40.f;
		float spacing = 40.f;

		addToggle(menu, "ENABLE HACK", startY, "enable_hack");
		addToggle(menu, "SHOW HITBOXES", startY - spacing, "show_hitboxes");
		addToggle(menu, "FPS BYPASS", startY - spacing * 2, "fps_bypass");

		// === Pop-in animation ===
		panel->setScale(0.8f);
		panel->runAction(CCEaseBackOut::create(
			CCScaleTo::create(0.25f, 1.f)));

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

		auto toggle = CCMenuItemToggler::create(
			CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"),
			CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
			this,
			menu_selector(ModSettingsLayer::onToggle));

		toggle->setPosition({130.f, y});
		toggle->setTag(reinterpret_cast<intptr_t>(new std::string(key)));
		toggle->toggle(enabled);

		menu->addChild(toggle);
	}

	// === Toggle handler ===
	void onToggle(CCObject *sender)
	{
		auto toggle = static_cast<CCMenuItemToggler *>(sender);
		auto keyPtr = reinterpret_cast<std::string *>(toggle->getTag());

		bool enabled = toggle->isToggled();
		Mod::get()->setSavedValue(*keyPtr, enabled);

		log::info("{} = {}", *keyPtr, enabled);
	}

	// === Close ===
	void onClose(CCObject *)
	{
		this->removeFromParentAndCleanup(true);
	}

	// === ESC key ===
	void keyBackClicked() override
	{
		onClose(nullptr);
	}
};

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
