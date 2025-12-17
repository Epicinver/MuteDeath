#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
using namespace geode::prelude;

static bool g_modSettingsOpen = false;

class ModSettingsLayer : public CCLayer
{
public:
	CCSprite *m_panel;

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

		// === Outside dim (ONLY outside transparency) ===
		auto overlay = CCLayerColor::create({0, 0, 0, 140});
		overlay->setContentSize(winSize);
		overlay->setPosition({0, 0});
		this->addChild(overlay, 0);

		// === Solid panel ===
		m_panel = CCSprite::createWithSpriteFrameName("GJ_fxOnBtn_001.png");
		m_panel->setScale(6.0f);
		m_panel->setPosition(winSize / 2);
		this->addChild(m_panel, 1);

		// === Menu container ===
		auto menu = CCMenu::create();
		menu->setPosition(winSize / 2);
		this->addChild(menu, 2);

		// === Close button ===
		auto closeSprite = CCSprite::createWithSpriteFrameName("GJ_fxOnBtn_001.png");
		closeSprite->setScale(0.7f);

		auto closeBtn = CCMenuItemSpriteExtra::create(
			closeSprite,
			this,
			menu_selector(ModSettingsLayer::onClose));

		closeBtn->setPosition({170.f, 110.f});
		menu->addChild(closeBtn);

		return true;
	}

	// === Outside click detection (SAFE + SIMPLE) ===
	bool ccTouchBegan(CCTouch *touch, CCEvent *) override
	{
		auto p = this->convertTouchToNodeSpace(touch);

		auto size = m_panel->getContentSize() * m_panel->getScale();
		auto pos = m_panel->getPosition();

		CCRect panelRect(
			pos.x - size.width / 2,
			pos.y - size.height / 2,
			size.width,
			size.height);

		// Outside → close
		if (!panelRect.containsPoint(p))
		{
			onClose(nullptr);
			return true;
		}

		// Inside → let menu handle it
		return false;
	}

	void onClose(CCObject *)
	{
		g_modSettingsOpen = false;
		this->removeFromParentAndCleanup(true);
	}

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
		if (g_modSettingsOpen)
			return;

		g_modSettingsOpen = true;

		CCDirector::sharedDirector()
			->getRunningScene()
			->addChild(ModSettingsLayer::create(), 999);
	}
};
