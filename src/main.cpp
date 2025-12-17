#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/ui/TextInput.hpp>
#include <filesystem>

using namespace geode::prelude;

static bool g_modSettingsOpen = false;
class ModSettingsLayer : public CCLayer
{
public:
	CCScale9Sprite *m_panel = nullptr;
	TextInput *m_pathInput = nullptr;

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

		auto overlay = CCLayerColor::create({0, 0, 0, 120});
		overlay->setContentSize(winSize);
		overlay->setPosition({0, 0});
		this->addChild(overlay, 0);

		m_panel = CCScale9Sprite::create("GJ_square01.png");
		m_panel->setContentSize({360.f, 240.f});
		m_panel->setPosition(winSize / 2);
		m_panel->setOpacity(255);
		this->addChild(m_panel, 1);

		auto title = CCLabelBMFont::create("Mute Death Options", "goldFont.fnt");
		title->setScale(0.55f);
		title->setPosition({m_panel->getPositionX(),
							m_panel->getPositionY() + (m_panel->getContentSize().height / 2) - 26.f});
		this->addChild(title, 2);

		auto menu = CCMenu::create();
		menu->setPosition(m_panel->getPosition());
		menu->setTouchPriority(-600);
		this->addChild(menu, 3);

		bool muted = Mod::get()->getSavedValue<bool>("mute_death_sfx", false);
		bool useCustom = Mod::get()->getSavedValue<bool>("use_custom_death", false);

		auto offSpr = CCSprite::create("GJ_checkOff_001.png");
		auto onSpr = CCSprite::create("GJ_checkOn_001.png");

		auto muteToggle = CCMenuItemToggler::create(
			offSpr,
			onSpr,
			this,
			menu_selector(ModSettingsLayer::onMuteToggle));
		muteToggle->toggle(muted);
		muteToggle->setPosition({-130.f, 35.f});
		menu->addChild(muteToggle);

		auto muteLabel = CCLabelBMFont::create("Mute Death Sound", "goldFont.fnt");
		muteLabel->setAnchorPoint({0.f, 0.5f});
		muteLabel->setScale(0.45f);
		muteLabel->setPosition({-100.f, 35.f});
		menu->addChild(muteLabel);

		auto cOff = CCSprite::create("GJ_checkOff_001.png");
		auto cOn = CCSprite::create("GJ_checkOn_001.png");

		auto customToggle = CCMenuItemToggler::create(
			cOff,
			cOn,
			this,
			menu_selector(ModSettingsLayer::onCustomToggle));
		customToggle->toggle(useCustom);
		customToggle->setPosition({-130.f, 0.f});
		menu->addChild(customToggle);

		auto customLabel = CCLabelBMFont::create("Use Custom Death Sound", "goldFont.fnt");
		customLabel->setAnchorPoint({0.f, 0.5f});
		customLabel->setScale(0.45f);
		customLabel->setPosition({-100.f, 0.f});
		menu->addChild(customLabel);

		m_pathInput = TextInput::create(250.f, "Insert path here");
		m_pathInput->setPosition({0.f, -60.f});
		m_pathInput->setScale(0.9f);

		std::string saved = Mod::get()->getSavedValue<std::string>("custom_death_path", "");
		if (!saved.empty())
			m_pathInput->setString(saved);

		m_pathInput->setCallback([this](std::string const &text)
								 { Mod::get()->setSavedValue("custom_death_path", text); });

		menu->addChild(m_pathInput);

		auto closeSprite = CCSprite::create("GJ_closeBtn_001.png");
		closeSprite->setScale(0.7f);

		auto closeBtn = CCMenuItemSpriteExtra::create(
			closeSprite,
			this,
			menu_selector(ModSettingsLayer::onClose));
		closeBtn->setPosition({(m_panel->getContentSize().width / 2) - 22.f,
							   (m_panel->getContentSize().height / 2) - 22.f});
		menu->addChild(closeBtn);

		m_panel->setScale(0.9f);
		m_panel->runAction(CCEaseBackOut::create(CCScaleTo::create(0.18f, 1.0f)));

		return true;
	}

	void onMuteToggle(CCObject *sender)
	{
		auto t = static_cast<CCMenuItemToggler *>(sender);
		Mod::get()->setSavedValue("mute_death_sfx", t->isToggled());
	}

	void onCustomToggle(CCObject *sender)
	{
		auto t = static_cast<CCMenuItemToggler *>(sender);
		Mod::get()->setSavedValue("use_custom_death", t->isToggled());
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
		if (!m_panel->boundingBox().containsPoint(p))
		{
			onClose(nullptr);
			return true;
		}
		return false;
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

class $modify(PlayerObject)
{
public:
	using Base = PlayerObject;

	void playDeathEffect()
	{
		bool muted = Mod::get()->getSavedValue<bool>("mute_death_sfx", false);
		bool useCustom = Mod::get()->getSavedValue<bool>("use_custom_death", false);
		std::string path = Mod::get()->getSavedValue<std::string>("custom_death_path", "");

		if (muted)
			return;

		if (useCustom && !path.empty())
		{
			FMODAudioEngine::sharedEngine()->playEffect(path);
			return;
		}

		Base::playDeathEffect();
	}
};

class $modify(MyMenuLayer, MenuLayer)
{
public:
	bool init()
	{
		if (!MenuLayer::init())
			return false;

		auto modSettings = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("GJ_fxOnBtn_001.png"),
			this,
			menu_selector(MyMenuLayer::onModSettings));

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

		CCDirector::sharedDirector()
			->getRunningScene()
			->addChild(ModSettingsLayer::create(), 999);
	}
};
