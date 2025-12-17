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
		// the stinky menu
		log::info("stinky mod settings clicked");
		// ill add this soon ig so placeholder
		FLAlertLayer::create(
			"settings",
			"This is where Mute Death settings would go, but i have to upload to github. This is a placeholder and is not permanent.",
			"Alright")
			->show();
	}
};