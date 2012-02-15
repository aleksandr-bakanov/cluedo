package  
{
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.events.SecurityErrorEvent;
	import flash.net.Socket;
	import flash.utils.Endian;
	
	/**
	 * ...
	 * @author bav
	 */
	public class Connector extends EventDispatcher 
	{
		// Server side command ids.
		public static const S_READY:int = 1;
		public static const S_NO_ROOM:int = 3;
		public static const S_AVAILABLE_GUESTS:int = 5;
		public static const S_GUEST_CHOOSE_RESULT:int = 7;
		public static const S_START_GAME_INFO:int = 9;
		public static const S_GUEST_MOVE:int = 11;
		public static const S_NEXT_MOVE:int = 13;
		public static const S_PLAYER_ASK:int = 15;
		public static const S_PLAYER_ANSWER:int = 17;
		public static const S_WAIT_ANSWER:int = 19;
		public static const S_NO_CARDS:int = 21;
		public static const S_GUESS_SECRET:int = 23;
		public static const S_TRANS_GUEST:int = 25;
		// Client side command ids.
		public static const C_ENTER_ROOM:int = 2;
		public static const C_LEAVE_ROOM:int = 4;
		public static const C_CHOOSE_GUEST:int = 6;
		public static const C_GUEST_MOVE:int = 8;
		public static const C_ASK:int = 10;
		public static const C_ANSWER:int = 12;
		public static const C_GUESS_SECRET:int = 14;
		
		private var _socket:Socket;
		private var _lastComSize:int;
		private var _model:Model;
		
		public function Connector(model:Model) 
		{
			_model = model;
			_lastComSize = 0;
			configureModelListeners();
			configureSocket();
		}
		
		private function configureModelListeners():void 
		{
			_model.addEventListener(CluedoEvent.ENTER_ROOM, sendEnterRoom);
			_model.addEventListener(CluedoEvent.CHOOSE_GUEST, sendChooseGuest);
			_model.addEventListener(CluedoEvent.C_GUEST_MOVE, sendGuestMove);
			_model.addEventListener(CluedoEvent.C_ASK, sendAsk);
			_model.addEventListener(CluedoEvent.C_ANSWER, sendAnswer);
			_model.addEventListener(CluedoEvent.C_GUESS_SECRET, sendGuessSecret);
		}
        
        private function configureSocket():void
		{
			_socket = new Socket();
			_socket.endian = Endian.LITTLE_ENDIAN;
			_socket.addEventListener(Event.CONNECT, connectHandler);
			_socket.addEventListener(Event.CLOSE, closeHandler);
			_socket.addEventListener(IOErrorEvent.IO_ERROR, ioErrorHandler);
			_socket.addEventListener(SecurityErrorEvent.SECURITY_ERROR, securityErrorHandler);
			_socket.addEventListener(ProgressEvent.SOCKET_DATA, socketDataHandler);
			_socket.connect("192.168.1.3", 7003);
		}
        
        private function socketDataHandler(e:ProgressEvent):void 
		{
			while (_socket.bytesAvailable)
			{
				if (!_lastComSize)
				{
					_lastComSize = _socket.readShort();
					if (_socket.bytesAvailable >= _lastComSize)
					{
						parse();
						_lastComSize = 0;
					}
				}
				else if (_socket.bytesAvailable >= _lastComSize)
				{
					parse();
					_lastComSize = 0;
				}
				else
					break;
			}
		}
        
        private function parse():void
		{
			var comId:int = _socket.readShort();
			switch(comId)
			{
				case S_READY: readyHandler(); break;
				case S_NO_ROOM: noRoomHandler(); break;
				case S_AVAILABLE_GUESTS: availableGuestsHandler(); break;
				case S_GUEST_CHOOSE_RESULT: guestChooseResultHandler(); break;
				case S_START_GAME_INFO: startGameInfoHandler(); break;
				case S_GUEST_MOVE: guestMoveHandler(); break;
				case S_NEXT_MOVE: nextMoveHandler(); break;
				case S_PLAYER_ASK: playerAskHandler(); break;
				case S_PLAYER_ANSWER: playerAnswerHandler(); break;
				case S_WAIT_ANSWER: waitAnswerHandler(); break;
				case S_NO_CARDS: noCardsHandler(); break;
				case S_GUESS_SECRET: guessSecretHandler(); break;
				case S_TRANS_GUEST: transGuestHandler(); break;
			}
			_lastComSize = 0;
		}
		
		private function sendGuessSecret(e:CluedoEvent):void 
		{
			CluedoMain.ttrace("Connector::sendGuessSecret");
			_socket.writeShort(5);
			_socket.writeShort(C_GUESS_SECRET);
			_socket.writeByte(e.data.ap);
			_socket.writeByte(e.data.gt);
			_socket.writeByte(e.data.wp);
			_socket.flush();
		}
		
		private function sendAnswer(e:CluedoEvent):void 
		{
			_socket.writeShort(3);
			_socket.writeShort(C_ANSWER);
			_socket.writeByte(e.data as int);
			_socket.flush();
		}
		
		private function sendAsk(e:CluedoEvent):void 
		{
			_socket.writeShort(4);
			_socket.writeShort(C_ASK);
			_socket.writeByte(e.data.gt);
			_socket.writeByte(e.data.wp);
			_socket.flush();
		}
		
		private function sendGuestMove(e:CluedoEvent):void 
		{
			_socket.writeShort(4);
			_socket.writeShort(C_GUEST_MOVE);
			_socket.writeByte(e.data.x);
			_socket.writeByte(e.data.y);
			_socket.flush();
		}
		
		private function sendChooseGuest(e:CluedoEvent):void 
		{
			var gt:int = e.data as int;
			if (gt >= 1 && gt <= 6)
			{
				_socket.writeShort(3);
				_socket.writeShort(C_CHOOSE_GUEST);
				_socket.writeByte(gt);
				_socket.flush();
			}
		}
		
		private function sendEnterRoom(e:CluedoEvent):void 
		{
			var type:int = e.data as int;
			if (type >= 3 && type <= 6)
			{
				_socket.writeShort(3);
				_socket.writeShort(C_ENTER_ROOM);
				_socket.writeByte(type);
				_socket.flush();
			}
		}
		
		private function transGuestHandler():void 
		{
			var gt:int = _socket.readByte();
			var x:int = _socket.readByte();
			var y:int = _socket.readByte();
			_model.dispatchEvent(new CluedoEvent(CluedoEvent.TRANS_GUEST, { gt:gt, x:x, y:y } ));
		}
		
		private function guessSecretHandler():void 
		{
			var enq:int = _socket.readByte();
			var ap:int = 0;
			var gt:int = 0;
			var wp:int = 0;
			if (_lastComSize - 3 > 0)
			{
				ap = _socket.readByte();
				gt = _socket.readByte();
				wp = _socket.readByte();
			}
			if (ap != 0)
			{
				CluedoMain.ttrace(Card.cardToString(enq) + " угадал: " + Card.cardToString(ap) + ", " + Card.cardToString(gt) + ", " + Card.cardToString(wp));
				_model.dispatchEvent(new CluedoEvent(CluedoEvent.END_GAME));
			}
			else
				CluedoMain.ttrace(Card.cardToString(enq) + " ошибся.");
		}
		
		private function noCardsHandler():void 
		{
			var ans:int = _socket.readByte();
			var ap:int = _socket.readByte();
			var gt:int = _socket.readByte();
			var wp:int = _socket.readByte();
			CluedoMain.ttrace(Card.cardToString(ans) + " не имеет: " + Card.cardToString(gt) +
				", " + Card.cardToString(wp) + ", " + Card.cardToString(ap));
		}
		
		private function waitAnswerHandler():void 
		{
			var gt:int = _socket.readByte();
			var sc:int = _socket.readByte();
			CluedoMain.ttrace("Ждем ответа от " + Card.cardToString(gt) + "...");
			_model.dispatchEvent(new CluedoEvent(CluedoEvent.START_WAIT_ANSWER, { gt:gt, sc:sc } ));
		}
		
		private function playerAnswerHandler():void 
		{
			var ans:int = _socket.readByte();
			var card:int = 0;
			if (_lastComSize - 3 > 0)
				card = _socket.readByte();
			if (ans != _model.guest)
            {
				CluedoMain.ttrace(Card.cardToString(ans) + " отвечает: " + (card ? Card.cardToString(card) : "???"));
                _model.dispatchEvent(new CluedoEvent(CluedoEvent.PLAYER_ANSWER));
            }
		}
		
		private function playerAskHandler():void 
		{
			var enq:int = _socket.readByte();
			var ap:int = _socket.readByte();
			var gt:int = _socket.readByte();
			var wp:int = _socket.readByte();
			_model.suspectAp = ap;
			_model.suspectGt = gt;
			_model.suspectWp = wp;
			CluedoMain.ttrace(Card.cardToString(enq) + " подозревает: убийца - " + Card.cardToString(gt) +
				", оружие - " + Card.cardToString(wp) + ", место - " + Card.cardToString(ap));
		}
		
		private function nextMoveHandler():void 
		{
			var gt:int = _socket.readByte();
			var fd:int = _socket.readByte();
			var sd:int = _socket.readByte();
			var sc:int = _socket.readByte();
            _model.curGuest = gt;
			if (gt == _model.guest)
				_model.steps = fd + sd;
			else
				_model.steps = 0;
			_model.dispatchEvent(new CluedoEvent(CluedoEvent.NEXT_MOVE, { gt:gt, fd:fd, sd:sd, sc:sc } ));
		}
		
		private function guestMoveHandler():void 
		{
			var gt:int = _socket.readByte();
			var len:int = _lastComSize - 3;
			var path:Array = [];
			while (len--)
				path.push(_socket.readByte());
			if (gt == _model.guest)
			{
				_model.steps -= path.length / 2;
				if (Map.MAP[path[path.length - 1]][[path.length - 2]] != ':')
					_model.steps = 0;
			}
			_model.dispatchEvent(new CluedoEvent(CluedoEvent.S_GUEST_MOVE, { guest:gt, path:path } ));
		}
		
		private function startGameInfoHandler():void 
		{
			_model.guest = _socket.readByte();
			_model.x = _socket.readByte();
			_model.y = _socket.readByte();
			_model.guestsOrder = [];
			_model.cards = [];
			var i:int;
			for (i = 0; i < 6; i++)
				_model.guestsOrder.push(_socket.readByte());
			for (i = 0; i < _lastComSize - 11; i++)
				_model.cards.push(_socket.readByte());
			_model.dispatchEvent(new CluedoEvent(CluedoEvent.INIT_GAME));
		}
		
		private function guestChooseResultHandler():void 
		{
			var r:int = _socket.readByte();
			var c:Boolean = Boolean(r & 64);
			var g:int;
			for (g = 1; g < 7; g++)
				if (r & Math.pow(2, g - 1))
					break;
            if (c)
                _model.guest = g;
		}
		
		private function availableGuestsHandler():void 
		{
			var ag:int = _socket.readByte();
			_model.dispatchEvent(new CluedoEvent(CluedoEvent.AVAILABLE_GUESTS, ag));
		}
		
		private function noRoomHandler():void 
		{
			CluedoMain.ttrace("Свободных комнат нет, попробуйте позже.");
		}
		
		private function readyHandler():void 
		{
			_model.dispatchEvent(new CluedoEvent(CluedoEvent.CONNECT));
		}
		
		private function securityErrorHandler(e:SecurityErrorEvent):void 
		{
			CluedoMain.ttrace(e.toString());
		}
		
		private function ioErrorHandler(e:IOErrorEvent):void 
		{
			CluedoMain.ttrace(e.toString());
		}
		
		private function closeHandler(e:Event):void 
		{
			CluedoMain.ttrace("Close handler");
		}
		
		private function connectHandler(e:Event):void 
		{
			CluedoMain.ttrace("Connection established.");
		}
		
	}

}