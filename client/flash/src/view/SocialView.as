package view 
{
	import control.ResourceManager;
	import flash.display.Sprite;
	import model.CluedoEvent;
	import view.social.UserPic;
	import control.Dispatcher;
	
	/**
	 * ...
	 * @author bav
	 */
	public class SocialView extends Sprite 
	{
		private var _userPic:UserPic;

		public function SocialView() 
		{
			Dispatcher.instance.addEventListener(CluedoEvent.RESOURCES_READY, resourcesReadyHandler);
		}

		private function resourcesReadyHandler(e:CluedoEvent):void 
		{
			Dispatcher.instance.removeEventListener(CluedoEvent.RESOURCES_READY, resourcesReadyHandler);
			trace(CluedoEvent.RESOURCES_READY);
			_userPic = new UserPic(ResourceManager.bitmaps["userpic"]);
			addChild(_userPic);
		}

		public function resize():void
		{
			if (!stage)
				return;
			if (_userPic)
			{
				_userPic.y = 0;
				_userPic.x = stage.stageWidth - _userPic.width;
			}
		}
		
	}

}