package  
{
	import flash.display.Graphics;
	import flash.display.Sprite;
	
	/**
	 * ...
	 * @author bav
	 */
	public class OrderPanel extends Sprite 
	{
		private var _model:Model;
		private var _marker:Sprite;
		
		public function OrderPanel(model:Model) 
		{
			_model = model;
			_model.addEventListener(CluedoEvent.NEXT_MOVE, nextMoveHandler);
			_marker = new Sprite();
			_marker.graphics.lineStyle(1, 0, 1, true);
			_marker.graphics.drawRect(0, 0, 30, 30);
			_marker.y = 5;
		}
		
		private function nextMoveHandler(e:CluedoEvent):void 
		{
			var index:int = e.data.gt as int;
			if (index)
				_marker.x = 15 + _model.guestsOrder.indexOf(index) * 60;
			if (!contains(_marker))
				addChild(_marker);
		}
		
		public function init(order:Array):void
		{
			var g:Graphics = graphics;
			g.clear();
			g.beginFill(0xDEDEDE);
			g.drawRect(0, 0, 380, 40);
			g.endFill();
			for (var i:int = 0; i < 6; i++)
			{
				var index:int = order[i] > 10 ? order[i] - 10 : order[i];
				g.beginFill(Model.COLORS[index - 1], order[i] > 10 ? 1 : .3);
				g.drawRect(20 + 60 * i, 10, 20, 20);
				g.endFill();
			}
		}
		
	}

}