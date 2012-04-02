package view 
{
	import flash.display.DisplayObjectContainer;
	import flash.display.Sprite;
	import flash.events.Event;
	
	/**
	 * Main application view.
	 * @author bav
	 */
	public class MainView extends Sprite 
	{
		private var _socialView:SocialView;
		private var _gameView:GameView;

		public function MainView(host:DisplayObjectContainer) 
		{
			host.addChild(this);
			initViews();
			configureHandlers();
		}
		
		private function initViews():void
		{
			_socialView = new SocialView();
			_gameView = new GameView();
			addChild(_socialView);
		}

		private function configureHandlers():void
		{
			stage.addEventListener(Event.RESIZE, resizeHandler);
		}
		
		private function resizeHandler(e:Event):void
		{
			_socialView.resize();
			_gameView.resize();
		}
		
	}

}