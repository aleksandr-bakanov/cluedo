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
		private var _model:Model;
		private var _roomChooser:RoomChooser;
		private var _guestChooser:GuestChooser;

		public function CluedoMain():void 
		{
			if (stage) init();
			else addEventListener(Event.ADDED_TO_STAGE, init);
		}

		private function init(e:Event = null):void 
		{
			removeEventListener(Event.ADDED_TO_STAGE, init);
			configureOutput();
			_model = new Model();
			configureModelListeners();
			_roomChooser = new RoomChooser(_model);
			_guestChooser = new GuestChooser(_model);
			_connector = new Connector(_model);
		}
		
		private function configureModelListeners():void
		{
			_model.addEventListener(CluedoEvent.CONNECT, connectHandler);
			_model.addEventListener(CluedoEvent.AVAILABLE_GUESTS, availableGuestHandler);
		}
		
		private function availableGuestHandler(e:CluedoEvent):void 
		{
			if (contains(_roomChooser))
				removeChild(_roomChooser);
			if (!contains(_guestChooser))
			{
				_guestChooser.x = (stage.stageWidth - _guestChooser.width) / 2;
				_guestChooser.y = 150;
				addChild(_guestChooser);
			}
		}
		
		private function connectHandler(e:CluedoEvent):void 
		{
			_roomChooser.x = (stage.stageWidth - _roomChooser.width) / 2;
			_roomChooser.y = 150;
			addChild(_roomChooser);
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