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
		private var _orderPanel:OrderPanel;
		private var _map:Map;
		private var _clock:Clock;
		private var _dice:Dice;

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
			_orderPanel = new OrderPanel(_model);
			_map = new Map(_model);
			_clock = new Clock(_model);
			_dice = new Dice(_model);
			_connector = new Connector(_model);
		}
		
		private function configureModelListeners():void
		{
			_model.addEventListener(CluedoEvent.CONNECT, connectHandler);
			_model.addEventListener(CluedoEvent.AVAILABLE_GUESTS, availableGuestHandler);
			_model.addEventListener(CluedoEvent.INIT_GAME, initGameHandler);
		}
		
		private function initGameHandler(e:CluedoEvent):void 
		{
			if (contains(_guestChooser))
				removeChild(_guestChooser);
			if (!contains(_orderPanel))
			{
				_orderPanel.init(_model.guestsOrder);
				_orderPanel.x = (stage.stageWidth - _orderPanel.width) / 2;
				addChild(_orderPanel);
			}
			if (!contains(_map))
			{
				_map.x = (stage.stageWidth - _map.width) / 2;
				_map.y = _orderPanel.height + 20;
				addChild(_map);
			}
			if (!contains(_clock))
			{
				_clock.x = stage.stageWidth - _clock.width;
				addChild(_clock);
			}
			if (!contains(_dice))
			{
				addChild(_dice);
			}
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