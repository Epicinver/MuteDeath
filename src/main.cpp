#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
using namespace geode::prelude;
static bool g_modSettingsOpen = false;
class ModSettingsLayer : public CCLayer
{
public:
	CCScale9Sprite *m_panel = nullptr;
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
	bool init() override
	{
		if (!CCLayer::init())
			return false;

		g_modSettingsOpen = true;

		this->setTouchEnabled(true);
		this->setKeypadEnabled(true);

		auto winSize = CCDirector::sharedDirector()->getWinSize();
		auto overlay = CCLayerColor::create({0, 0, 0, 140});

		overlay->setContentSize(winSize);
		overlay->setPosition({0, 0});

		this->addChild(overlay, 0);

		m_panel = CCScale9Sprite::createWithSpriteFrameName("GJ_fxOnBtn_001.png");
		m_panel->setContentSize({380.f, 260.f});
		m_panel->setPosition(winSize / 2);
		m_panel->setOpacity(255);
		this->addChild(m_panel, 1);

		auto title = CCLabelBMFont::create("MOD SETTINGS", "bigFont.fnt");
		title->setScale(0.55f);
		title->setPosition({m_panel->getPositionX(),
							m_panel->getPositionY() + (m_panel->getContentSize().height / 2) - 28.f});
		this->addChild(title, 2);
		auto menu = CCMenu::create();
		menu->setPosition(m_panel->getPosition());
		this->addChild(menu, 3);
		auto closeSprite = CCSprite::createWithSpriteFrameName("GJ_fxOnBtn_001.png");
		closeSprite->setScale(0.6f);
		auto closeBtn = CCMenuItemSpriteExtra::create(
			closeSprite,
			this,
			menu_selector(ModSettingsLayer::onClose));
		closeBtn->setPosition({(m_panel->getContentSize().width / 2) - 24.f,
							   (m_panel->getContentSize().height / 2) - 24.f});
		menu->addChild(closeBtn);
		m_panel->setScale(0.85f);
		m_panel->runAction(CCEaseBackOut::create(CCScaleTo::create(0.20f, 1.0f)));
		return true;
	}
	void registerWithTouchDispatcher() override
	{
		CCDirector::sharedDirector()
			->getTouchDispatcher()
			->addTargetedDelegate(this, -500, true);
	}
	bool ccTouchBegan(CCTouch *touch, CCEvent *) override
	{
		auto p = this->convertTouchToNodeSpace(touch);
		auto rect = m_panel->boundingBox();
		if (!rect.containsPoint(p))
		{
			onClose(nullptr);
		}
		return true;
	}
	void onClose(CCObject *)
	{
		this->removeFromParentAndCleanup(true);
	}
	void keyBackClicked() override
	{
		onClose(nullptr);
	}
	void onExit() override
	{
		g_modSettingsOpen = false;
		CCLayer::onExit();
	}
};
class $modify(MyMenuLayer, MenuLayer)
{
public:
	bool init()
	{
		if (!MenuLayer::init())
			return false;
		auto menu = this->getChildByID("bottom-menu");
		if (!menu)
		{
			log::warn("bottom-menu not found");
			return true;
		}
		auto spr = CCSprite::createWithSpriteFrameName("GJ_fxOnBtn_001.png");
		spr->setScale(0.9f);
		auto modSettings = CCMenuItemSpriteExtra::create(
			spr,
			this,
			menu_selector(MyMenuLayer::onModSettings));
		modSettings->setID("mod-settings"_spr);
		menu->addChild(modSettings);
		menu->updateLayout();
		return true;
	}
	void onModSettings(CCObject *)
	{
		// e
		if (g_modSettingsOpen)
			return;

		log::info("ok open menu");

		CCDirector::sharedDirector()
			->getRunningScene()
			->addChild(ModSettingsLayer::create(), 999);
	}
};