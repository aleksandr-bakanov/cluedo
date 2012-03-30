package 
{
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.events.MouseEvent;
	import flash.text.TextField;
	import flash.text.TextFormat;
	import control.*;
	import model.*;
	import view.*;

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
		private var _ansWaitClock:Clock;
		private var _dice:Dice;
		private var _cardsPanel:CardsPanel;
		private var _enquirePanel:EnquirePanel;
		private var _guessSecretPanel:GuessSecretPanel;
		private var _noCardsButton:Button;
		private var _endTurnButton:Button;

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
			_clock.addNormalClockListeners();
			_ansWaitClock = new Clock(_model);
			_ansWaitClock.addWaitAnswerClockListeners();
			_dice = new Dice(_model);
            _dice.addEventListener(MouseEvent.CLICK, showGuessSecretPanel);
			_cardsPanel = new CardsPanel(_model);
			_enquirePanel = new EnquirePanel(_model);
			_guessSecretPanel = new GuessSecretPanel(_model);
			_noCardsButton = new Button("No cards");
			_noCardsButton.addEventListener(MouseEvent.CLICK, noCardsHandler);
			_endTurnButton = new Button("End turn");
			_endTurnButton.addEventListener(MouseEvent.CLICK, endTurnHandler);
			_connector = new Connector(_model, stage.loaderInfo.parameters.host, stage.loaderInfo.parameters.port);
		}

		private function configureModelListeners():void
		{
			_model.addEventListener(CluedoEvent.CONNECT, connectHandler);
			_model.addEventListener(CluedoEvent.AVAILABLE_GUESTS, availableGuestHandler);
			_model.addEventListener(CluedoEvent.INIT_GAME, initGameHandler);
			_model.addEventListener(CluedoEvent.SHOW_ENQUIRE_PANEL, showEnquirePanel);
			_model.addEventListener(CluedoEvent.NEXT_MOVE, nextMoveHandler);
			_model.addEventListener(CluedoEvent.END_GAME, endGameHandler);
			_model.addEventListener(CluedoEvent.PLAYER_ANSWER, playerAnswerHandler);
			_model.addEventListener(CluedoEvent.START_WAIT_ANSWER, startWaitAnswerHandler);
			_model.addEventListener(CluedoEvent.S_NO_CARDS, playerAnswerHandler);
		}

		private function connectHandler(e:CluedoEvent):void 
		{
			_roomChooser.x = (stage.stageWidth - _roomChooser.width) / 2;
			_roomChooser.y = 150;
			if (!contains(_roomChooser))
				addChild(_roomChooser);
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
			if (!contains(_cardsPanel))
			{
				_cardsPanel.initCards(_model.cards);
				_cardsPanel.y = 20;
				addChild(_cardsPanel);
			}
		}

		private function showEnquirePanel(e:CluedoEvent):void 
		{
			if (!contains(_enquirePanel))
			{
				_enquirePanel.setApp(e.data as int);
				_enquirePanel.x = (stage.stageWidth - _enquirePanel.width) * .5;
				_enquirePanel.y = 50;
				addChild(_enquirePanel);
			}
		}

		private function nextMoveHandler(e:CluedoEvent):void 
		{
			if (contains(_enquirePanel))
				removeChild(_enquirePanel);
			if (contains(_guessSecretPanel))
				removeChild(_guessSecretPanel);
			var id:int = (e.data.gt > 10 ? e.data.gt - 10 : e.data.gt);
			if (id == _model.guest)
			{
				if (!contains(_endTurnButton))
				{
					_endTurnButton.x = stage.stageWidth - _endTurnButton.width;
					_endTurnButton.y = stage.stageHeight - _output.height - _endTurnButton.height;
					addChild(_endTurnButton);
				}
			}
			else if (contains(_endTurnButton))
				removeChild(_endTurnButton);
		}

		private function endGameHandler(e:CluedoEvent):void 
		{
			CluedoMain.ttrace("CluedoMain::endGameHandler");
			if (contains(_orderPanel))
				removeChild(_orderPanel);
			if (contains(_map))
				removeChild(_map);
			if (contains(_clock))
				removeChild(_clock);
			if (contains(_dice))
				removeChild(_dice);
			if (contains(_cardsPanel))
				removeChild(_cardsPanel);
			if (contains(_ansWaitClock))
				removeChild(_ansWaitClock);
			if (contains(_noCardsButton))
				removeChild(_noCardsButton);
			if (contains(_endTurnButton))
				removeChild(_endTurnButton);
			if (!contains(_roomChooser))
				addChild(_roomChooser);
		}

		private function startWaitAnswerHandler(e:CluedoEvent):void 
		{
			if (!contains(_ansWaitClock))
			{
				_ansWaitClock.x = stage.stageWidth - _ansWaitClock.width * 2;
				addChild(_ansWaitClock);
			}
			if (e.data.gt == _model.guest && !contains(_noCardsButton))
			{
				if (_model.cards.indexOf(_model.suspectAp) < 0 &&
					_model.cards.indexOf(_model.suspectGt) < 0 &&
					_model.cards.indexOf(_model.suspectWp) < 0)
				{
					_noCardsButton.y = stage.stageHeight - _output.height - _noCardsButton.height;
					addChild(_noCardsButton);
				}
			}
			if (contains(_endTurnButton))
				removeChild(_endTurnButton);
		}

		private function playerAnswerHandler(e:CluedoEvent):void 
		{
			if (contains(_ansWaitClock))
				removeChild(_ansWaitClock);
			if (contains(_noCardsButton))
				removeChild(_noCardsButton);
		}

		private function showGuessSecretPanel(e:MouseEvent):void 
		{
			if (!contains(_guessSecretPanel))
			{
				_guessSecretPanel.clear();
				_guessSecretPanel.x = (stage.stageWidth - _guessSecretPanel.width) * .5;
				_guessSecretPanel.y = 50;
				addChild(_guessSecretPanel);
			}
		}

		private function noCardsHandler(e:MouseEvent):void
		{
			_model.dispatchEvent(new CluedoEvent(CluedoEvent.C_NO_CARDS));
		}

		private function endTurnHandler(e:MouseEvent):void
		{
			if (contains(_enquirePanel))
				removeChild(_enquirePanel);
			if (contains(_guessSecretPanel))
				removeChild(_guessSecretPanel);
			_model.dispatchEvent(new CluedoEvent(CluedoEvent.C_END_TURN));
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
			_output.scrollV = _output.maxScrollV;
		}

	}

}