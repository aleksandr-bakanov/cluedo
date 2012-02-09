package  
{
	import flash.display.Graphics;
	import flash.display.Sprite;
	
	/**
	 * ...
	 * @author bav
	 */
	public class Cell extends Sprite 
	{
		public static const SIZE:int = 10;
		public var steps:int;
		public var xc:int;
		public var yc:int;
		
		public function Cell(type:String) 
		{
			var g:Graphics = graphics;
			g.lineStyle(1, 0, 1, true);
			if (type == ":")
				g.beginFill(0xEEEEEE);
			else if (type == "#")
				g.beginFill(0x666666);
			else if (type == "k")
				g.beginFill(0xABCDEF);
			else if (type == "B")
				g.beginFill(0xFEDCBA);
			else if (type == "C")
				g.beginFill(0xCDEFAB);
			else if (type == "d")
				g.beginFill(0xEFABDC);
			else if (type == "b")
				g.beginFill(0x9AD8C2);
			else if (type == "L")
				g.beginFill(0xCD8AA8);
			else if (type == "l")
				g.beginFill(0xC9A72F);
			else if (type == "h")
				g.beginFill(0x2FC9A7);
			else if (type == "s")
				g.beginFill(0x8BB8DC);
			g.drawRect(0, 0, SIZE, SIZE);
			g.endFill();
		}
		
	}

}