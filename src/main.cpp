#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>
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

		// dark background
		auto bg = CCLayerColor::create({0, 0, 0, 160});
		this->addChild(bg);

		// menu container
		auto menu = CCMenu::create();
		menu->setPosition({winSize.width / 2, winSize.height / 2});
		this->addChild(menu);

		// close button (round style)
		auto circle = CCSprite::createWithSpriteFrameName("GJ_fxOnBtn_001.png");
		auto closeBtn = CCMenuItemSpriteExtra::create(
			circle,
			this,
			menu_selector(ModSettingsLayer::onClose));

		auto xIcon = CCSprite::createWithSpriteFrameName("GJ_fxOnBtn_001.png");
		xIcon->setPosition(circle->getContentSize() / 2);
		circle->addChild(xIcon);

		menu->addChild(closeBtn);

		return true;
	}

	void onClose(CCObject *)
	{
		this->removeFromParentAndCleanup(true);
	}
};
