package  
{
	import flash.display.Sprite;
	import flash.events.TimerEvent;
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.text.TextFormatAlign;
	import flash.utils.Timer;
	
	/**
	 * ...
	 * @author bav
	 */
	public class Clock extends Sprite 
	{
		private var _field:TextField;
		private var _timer:Timer;
		private var _seconds:int;
		private var _model:Model;
		
		public function Clock(model:Model) 
		{
			_model = model;
			_model.addEventListener(CluedoEvent.NEXT_MOVE, nextMoveHandler);
			
			_field = new TextField();
			_field.height = 20;
			_field.width = 20;
			_field.border = true;
			_field.defaultTextFormat = new TextFormat("_typewriter", 12, 0, null, null, null, null, null, TextFormatAlign.RIGHT);
			addChild(_field);
			
			_seconds = 0;
			_timer = new Timer(1000);
			_timer.addEventListener(TimerEvent.TIMER, timerHandler);
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