package view.game 
{
	import flash.display.Graphics;
	import flash.display.Sprite;
	import flash.events.TimerEvent;
	import flash.utils.Timer;
	import model.*;

	/**
	 * ...
	 * @author bav
	 */
	public class Guest extends Sprite 
	{
		public static const MOVE_DELAY:int = 150;
		private var _path:Array;
		private var _timer:Timer;
		private var _id:int;
		private var _model:MainModel;

		public function Guest(model:MainModel, id:int) 
		{
			_id = id;
			_model = model;
			_timer = new Timer(MOVE_DELAY);
			_timer.addEventListener(TimerEvent.TIMER, nextStep);
			_path = [];
			var g:Graphics = graphics;
			g.lineStyle(1);
			g.beginFill(MainModel.COLORS[id > 10 ? id - 11 : id - 1]);
			g.drawCircle(Cell.SIZE / 2, Cell.SIZE / 2, Cell.SIZE / 3);
			mouseChildren = mouseEnabled = false;
		}

		public function go(path:Array):void
		{
			while (path.length)
				_path.push(path.shift());
			if (!_timer.running)
				_timer.start();
		}

		public function teleport(x:int, y:int):void
		{
			_timer.reset();
			_path.length = 0;
			if (parent)
				parent.addChild(this);
			this.x = Cell.SIZE * x;
			this.y = Cell.SIZE * y;
		}

		private function nextStep(e:TimerEvent = null):void
		{
			if (_path.length >= 2)
			{
				x = Cell.SIZE * _path.shift();
				y = Cell.SIZE * _path.shift();
			}
			else
			{
				_timer.reset();
				if (_id == _model.guest)
					dispatchEvent(new CluedoEvent(CluedoEvent.CHECK_STEPS));
			}
		}

	}

}