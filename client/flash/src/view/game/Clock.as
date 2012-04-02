package view.game 
{
	import flash.display.Sprite;
	import flash.events.TimerEvent;
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.text.TextFormatAlign;
	import flash.utils.Timer;
	import model.*;

	/**
	 * ...
	 * @author bav
	 */
	public class Clock extends Sprite 
	{
		private var _field:TextField;
		private var _timer:Timer;
		private var _seconds:int;
		private var _model:MainModel;

		public function Clock(model:MainModel) 
		{
			_model = model;
			
			_field = new TextField();
			_field.height = 20;
			_field.width = 20;
			_field.border = true;
			_field.selectable = false;
			_field.defaultTextFormat = new TextFormat("_typewriter", 12, 0, null, null, null, null, null, TextFormatAlign.RIGHT);
			addChild(_field);

			_seconds = 0;
			_timer = new Timer(1000);
			_timer.addEventListener(TimerEvent.TIMER, timerHandler);
		}

		public function addNormalClockListeners():void
		{
			Dispatcher.instance.addEventListener(CluedoEvent.NEXT_MOVE, nextMoveHandler);
			Dispatcher.instance.addEventListener(CluedoEvent.END_GAME, endGameHandler);
		}

		public function addWaitAnswerClockListeners():void
		{
			Dispatcher.instance.addEventListener(CluedoEvent.START_WAIT_ANSWER, nextMoveHandler);
			Dispatcher.instance.addEventListener(CluedoEvent.END_GAME, endGameHandler);
		}

		private function endGameHandler(e:CluedoEvent):void 
		{
			_timer.reset();
		}

		private function timerHandler(e:TimerEvent):void 
		{
			if (_seconds > 0)
				_field.text = String(--_seconds);
			else
				_timer.stop();
		}

		private function nextMoveHandler(e:CluedoEvent):void 
		{
			_seconds = e.data.sc as int;
			if (!_timer.running)
				_timer.start();
		}

	}

}