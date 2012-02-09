package  
{
	import flash.display.InteractiveObject;
	import flash.display.Sprite;
	import flash.events.MouseEvent;
	import flash.filters.GlowFilter;
	
	/**
	 * ...
	 * @author bav
	 */
	public class Map extends Sprite 
	{
		public static const MAP_W:int = 24;
		public static const MAP_H:int = 25;
		public static const MAP:Array = 
			[	['#','#','#','#','#','#','#','#','#',':','#','#','#','#',':','#','#','#','#','#','#','#','#','#'],
				['#','#','#','#','#','#','#',':',':',':','#','#','#','#',':',':',':','#','#','#','#','#','#','#'],
				['#','#','#','#','#','#',':',':','#','#','#','#','#','#','#','#',':',':','#','#','#','#','#','#'],
				['#','#','#','#','#','#',':',':','#','#','#','#','#','#','#','#',':',':','#','#','#','#','#','#'],
				['#','#','#','#','#','#',':',':','#','#','#','#','#','#','#','#',':',':','#','#','#','#','#','#'],
				['#','#','#','#','#','#',':',':','B','#','#','#','#','#','#','B',':',':',':','C','#','#','#','#'],
				['#','#','#','#','k','#',':',':','#','#','#','#','#','#','#','#',':',':',':',':',':',':',':',':'],
				[':',':',':',':',':',':',':',':','#','B','#','#','#','#','B','#',':',':',':',':',':',':',':','#'],
				['#',':',':',':',':',':',':',':',':',':',':',':',':',':',':',':',':',':','#','#','#','#','#','#'],
				['#','#','#','#','#',':',':',':',':',':',':',':',':',':',':',':',':',':','b','#','#','#','#','#'],
				['#','#','#','#','#','#','#','#',':',':','#','#','#','#','#',':',':',':','#','#','#','#','#','#'],
				['#','#','#','#','#','#','#','#',':',':','#','#','#','#','#',':',':',':','#','#','#','#','#','#'],
				['#','#','#','#','#','#','#','d',':',':','#','#','#','#','#',':',':',':','#','#','#','#','b','#'],
				['#','#','#','#','#','#','#','#',':',':','#','#','#','#','#',':',':',':',':',':',':',':',':','#'],
				['#','#','#','#','#','#','#','#',':',':','#','#','#','#','#',':',':',':','#','#','L','#','#','#'],
				['#','#','#','#','#','#','d','#',':',':','#','#','#','#','#',':',':','#','#','#','#','#','#','#'],
				['#',':',':',':',':',':',':',':',':',':','#','#','#','#','#',':',':','L','#','#','#','#','#','#'],
				[':',':',':',':',':',':',':',':',':',':',':',':',':',':',':',':',':','#','#','#','#','#','#','#'],
				['#',':',':',':',':',':',':',':',':','#','#','h','h','#','#',':',':',':','#','#','#','#','#','#'],
				['#','#','#','#','#','#','l',':',':','#','#','#','#','#','#',':',':',':',':',':',':',':',':',':'],
				['#','#','#','#','#','#','#',':',':','#','#','#','#','#','h',':',':',':',':',':',':',':',':','#'],
				['#','#','#','#','#','#','#',':',':','#','#','#','#','#','#',':',':','s','#','#','#','#','#','#'],
				['#','#','#','#','#','#','#',':',':','#','#','#','#','#','#',':',':','#','#','#','#','#','#','#'],
				['#','#','#','#','#','#','#',':',':','#','#','#','#','#','#',':',':','#','#','#','#','#','#','#'],
				['#','#','#','#','#','#','#',':','#','#','#','#','#','#','#','#',':','#','#','#','#','#','#','#']
			];
			private var _cells:Array;
			private var _model:Model;
			private var _guests:Array;
			private var _guestsCon:Sprite;
			private var _glowed:Array;
		
		public function Map(model:Model) 
		{
			_model = model;
			_glowed = [];
			_cells = [];
			var yLen:int = MAP.length;
			var xLen:int = MAP[0].length;
			for (var i:int = 0; i < yLen; i++)
			{
				_cells[i] = [];
				for (var j:int = 0; j < xLen; j++)
				{
					var cell:Cell = new Cell(MAP[i][j]);
					cell.x = j * Cell.SIZE;
					cell.y = i * Cell.SIZE;
					cell.xc = j;
					cell.yc = i;
					_cells[i].push(cell);
					addChild(cell);
				}
			}
			
			_guestsCon = new Sprite();
			addChild(_guestsCon);
			_guests = [];
			for (var k:int = 1; k < 7; k++)
			{
				var g:Guest = new Guest(k);
				_guests.push(g);
				_guestsCon.addChild(g);
			}
			initGuestsPositions();
			_model.addEventListener(CluedoEvent.NEXT_MOVE, nextMoveHandler);
		}
		
		private function nextMoveHandler(e:CluedoEvent):void 
		{
			var id:int = (e.data.gt > 10 ? e.data.gt - 10 : e.data.gt);
			if (id == _model.guest)
				glowCells(_model.x, _model.y, e.data.fd + e.data.sd);
			else
				unglowCells();
		}
		
		private function initGuestsPositions():void
		{
			for (var i:int = 0; i < 6; i++)
			{
				var g:Guest = _guests[i] as Guest;
				if (i == 0) {
					g.x = 0; g.y = 17 * Cell.SIZE;
				}
				else if (i == 1) {
					g.x = 9 * Cell.SIZE; g.y = 0;
				}
				else if (i == 2) {
					g.x = 14 * Cell.SIZE; g.y = 0;
				}
				else if (i== 3) {
					g.x = 23 * Cell.SIZE; g.y = 6 * Cell.SIZE;
				}
				else if (i == 4) {
					g.x = 23 * Cell.SIZE; g.y = 19 * Cell.SIZE;
				}
				else if (i == 5) {
					g.x = 7 * Cell.SIZE; g.y = 24 * Cell.SIZE;
				}
			}
		}
		
		private function glowCells(sx:int, sy:int, steps:int):void
		{
			unglowCells();
			var open:Array = [];
			var c:Cell = _cells[sy][sx] as Cell;
			c.steps = steps;
			open.push(c);
			var index:int = 0;
			while (index >= 0)
			{
				c = open[index] as Cell;
				// Current cell
				var cc:Cell;
				// To left
				if (c.xc - 1 >= 0 && MAP[c.yc][c.xc - 1] != '#' && open.indexOf(_cells[c.yc][c.xc - 1]) < 0)
				{
					cc = _cells[c.yc][c.xc - 1] as Cell;
					cc.steps = c.steps - 1;
					open.push(cc);
				}
				// To right
				if (c.xc + 1 < MAP_W && MAP[c.yc][c.xc + 1] != '#' && open.indexOf(_cells[c.yc][c.xc + 1]) < 0)
				{
					cc = _cells[c.yc][c.xc + 1] as Cell;
					cc.steps = c.steps - 1;
					open.push(cc);
				}
				// To up
				if (c.yc - 1 >= 0 && MAP[c.yc - 1][c.xc] != '#' && open.indexOf(_cells[c.yc - 1][c.xc]) < 0)
				{
					cc = _cells[c.yc - 1][c.xc] as Cell;
					cc.steps = c.steps - 1;
					open.push(cc);
				}
				// To down
				if (c.yc + 1 < MAP_H && MAP[c.yc + 1][c.xc] != '#' && open.indexOf(_cells[c.yc + 1][c.xc]) < 0)
				{
					cc = _cells[c.yc + 1][c.xc] as Cell;
					cc.steps = c.steps - 1;
					open.push(cc);
				}
				c.steps = 0;
				index = getCellWithSteps(open);
			}
			while (open.length)
			{
				c = open.pop();
				c.filters = [new GlowFilter(0x0000FF, 1, 6, 6, 2, 1, true)];
				c.addEventListener(MouseEvent.CLICK, cellClickHandler);
				c.addEventListener(MouseEvent.ROLL_OUT, cellRollOutHandler);
				c.addEventListener(MouseEvent.ROLL_OVER, cellRollOverHandler);
				_glowed.push(c);
			}
		}
		
		private function getCellWithSteps(list:Array):int
		{
			var len:int = list.length;
			for (var i:int = 0; i < len; i++)
				if ((list[i] as Cell).steps > 0)
					return i;
			return -1;
		}
		
		private function unglowCells():void
		{
			while (_glowed.length)
			{
				var c:Cell = _glowed.pop() as Cell;
				c.filters = [];
				c.removeEventListener(MouseEvent.CLICK, cellClickHandler);
				c.removeEventListener(MouseEvent.ROLL_OUT, cellRollOutHandler);
				c.removeEventListener(MouseEvent.ROLL_OVER, cellRollOverHandler);
			}
		}
		
		private function cellRollOverHandler(e:MouseEvent):void
		{
			
		}
		
		private function cellRollOutHandler(e:MouseEvent):void
		{
			
		}
		
		private function cellClickHandler(e:MouseEvent):void
		{
			
		}
		
	}

}