package  
{
	import flash.display.Graphics;
	import flash.display.Sprite;
	
	/**
	 * ...
	 * @author bav
	 */
	public class Guest extends Sprite 
	{
		private var _moves:Array;
		
		public function Guest(id:int) 
		{
			var g:Graphics = graphics;
			g.lineStyle(1);
			g.beginFill(Model.COLORS[id > 10 ? id - 11 : id - 1]);
			g.drawCircle(Cell.SIZE / 2, Cell.SIZE / 2, Cell.SIZE / 3);
		}
		
		public function go(moves:Array):void
		{
			
		}
		
	}

}