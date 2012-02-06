package 
{
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.text.TextField;
	import flash.text.TextFormat;

	/**
	 * ...
	 * @author bav
	 */
	[Frame(factoryClass="Preloader")]
	public class CluedoMain extends Sprite 
	{
		private static var _output:TextField;
		private var _connector:Connector;

		public function CluedoMain():void 
		{
			if (stage) init();
			else addEventListener(Event.ADDED_TO_STAGE, init);
		}

		private function init(e:Event = null):void 
		{
			removeEventListener(Event.ADDED_TO_STAGE, init);
			configureOutput();
			_connector = new Connector();
		}
		
		private function configureOutput():void 
		{
			_output = new TextField();
			_output.width = stage.stageWidth;
			_output.height = stage.stageHeight / 3;
			_output.border = _output.multiline = _output.wordWrap = true;
			_output.y = stage.stageHeight - _output.height;
			_output.defaultTextFormat = new TextFormat("_typewriter");
			addChild(_output);
		}
		
		public static function ttrace(obj:Object):void
		{
			_output.appendText(String(obj) + '\n');
		}

	}

}