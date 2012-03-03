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
			_guestsCon.mouseChildren = _guestsCon.mouseEnabled = false;
			addChild(_guestsCon);
			_guests = [];
			for (var k:int = 1; k < 7; k++)
			{
				var g:Guest = new Guest(_model, k);
				g.addEventListener(CluedoEvent.CHECK_STEPS, checkStepsHandler);
				_guests.push(g);
				_guestsCon.addChild(g);
			}
			initGuestsPositions();
			configureModelListeners();
		}
		
		private function checkStepsHandler(e:CluedoEvent):void 
		{
			if (MAP[_model.y][_model.x] != ':')
				_model.dispatchEvent(new CluedoEvent(CluedoEvent.SHOW_ENQUIRE_PANEL, getAppByCoordinates(_model.x, _model.y)));
			else if ((_guests.indexOf(e.currentTarget) + 1) == _model.guest && _model.steps && MAP[_model.y][_model.x] == ':')
				glowCells(_model.x, _model.y, _model.steps);
		}
		
		private function getAppByCoordinates(x:int, y:int):int
		{
			var r:int = 0;
			if (x == 4 && y == 6)
				r = Card.AP_KITCHEN;
			else if ((x == 8 && y == 5) || (x == 15 && y == 5) ||
					 (x == 9 && y == 7) || (x == 14 && y == 7))
				r = Card.AP_BALLROOM;
			else if (x == 19 && y == 5)
				r = Card.AP_CONSERVATORY;
			else if ((x == 7 && y == 12) || (x == 6 && y == 15))
				r = Card.AP_DINING_ROOM;
			else if ((x == 18 && y == 9) || (x == 22 && y == 12))
				r = Card.AP_BILLIARD_ROOM;
			else if ((x == 20 && y == 14) || (x == 17 && y == 16))
				r = Card.AP_LIBRARY;
			else if (x == 6 && y == 19)
				r = Card.AP_LOUNGE;
			else if (((x == 11 || x == 12) && y == 18) || (x == 14 && y == 20))
				r = Card.AP_HALL;
			else if (x == 17 && y == 21)
				r = Card.AP_STUDY;
			return r;
		}
		
		private function configureModelListeners():void
		{
			_model.addEventListener(CluedoEvent.NEXT_MOVE, nextMoveHandler);
			_model.addEventListener(CluedoEvent.S_GUEST_MOVE, sGuestMoveHandler);
			_model.addEventListener(CluedoEvent.TRANS_GUEST, transGuestHandler);
		}
		
		private function transGuestHandler(e:CluedoEvent):void 
		{
			var guest:Guest = _guests[e.data.gt - 1] as Guest;
			if (guest)
				guest.teleport(e.data.x, e.data.y);
			if (e.data.gt == _model.guest)
			{
				_model.x = e.data.x;
				_model.y = e.data.y;
				if (_model.curGuest == _model.guest && _model.steps)
					glowCells(_model.x, _model.y, _model.steps);
			}
		}
		
		private function sGuestMoveHandler(e:CluedoEvent):void 
		{
			var guest:Guest = _guests[e.data.guest - 1] as Guest;
			if (e.data.guest == _model.guest)
			{
				_model.x = e.data.path[e.data.path.length - 2];
				_model.y = e.data.path[e.data.path.length - 1];
			}
			if (guest)
				guest.go(e.data.path);
		}
		
		private function nextMoveHandler(e:CluedoEvent):void 
		{
			var id:int = (e.data.gt > 10 ? e.data.gt - 10 : e.data.gt);
			if (id == _model.guest)
				glowCells(_model.x, _model.y, _model.steps);
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
			
			if (steps)
			{
				if (MAP[sy][sx] == 'B')
					open.push(_cells[5][8], _cells[5][15], _cells[7][9], _cells[7][14]);
				else if (MAP[sy][sx] == 'd')
					open.push(_cells[12][7], _cells[15][6]);
				else if (MAP[sy][sx] == 'b')
					open.push(_cells[9][18], _cells[12][22]);
				else if (MAP[sy][sx] == 'L')
					open.push(_cells[14][20], _cells[16][17]);
				else if (MAP[sy][sx] == 'h')
					open.push(_cells[18][11], _cells[18][12], _cells[20][14]);
				else if (MAP[sy][sx] == 'l')
					open.push(_cells[5][19]);
				else if (MAP[sy][sx] == 'C')
					open.push(_cells[19][6]);
				else if (MAP[sy][sx] == 'k')
					open.push(_cells[21][17]);
				else if (MAP[sy][sx] == 's')
					open.push(_cells[6][4]);
			}
			var len:int = open.length;
			for (var i:int = 0; i < len; i++)
				(open[i] as Cell).steps = 0;
			
			c.steps = steps;
			if (open.indexOf(c) < 0)
				open.push(c);
			var index:int = getCellWithSteps(open);
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
					if (open.indexOf(cc) < 0)
						open.push(cc);
				}
				// To right
				if (c.xc + 1 < MAP_W && MAP[c.yc][c.xc + 1] != '#' && open.indexOf(_cells[c.yc][c.xc + 1]) < 0)
				{
					cc = _cells[c.yc][c.xc + 1] as Cell;
					cc.steps = c.steps - 1;
					if (open.indexOf(cc) < 0)
						open.push(cc);
				}
				// To up
				if (c.yc - 1 >= 0 && MAP[c.yc - 1][c.xc] != '#' && open.indexOf(_cells[c.yc - 1][c.xc]) < 0)
				{
					cc = _cells[c.yc - 1][c.xc] as Cell;
					cc.steps = c.steps - 1;
					if (open.indexOf(cc) < 0)
						open.push(cc);
				}
				// To down
				if (c.yc + 1 < MAP_H && MAP[c.yc + 1][c.xc] != '#' && open.indexOf(_cells[c.yc + 1][c.xc]) < 0)
				{
					cc = _cells[c.yc + 1][c.xc] as Cell;
					cc.steps = c.steps - 1;
					if (open.indexOf(cc) < 0)
						open.push(cc);
				}
				c.steps = 0;
				index = getCellWithSteps(open);
			}
			while (open.length)
			{
				c = open.pop() as Cell;
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
			var c:Cell = e.target as Cell;
			unglowCells();
			_model.dispatchEvent(new CluedoEvent(CluedoEvent.C_GUEST_MOVE, { x:c.xc, y:c.yc } ));
		}
		
	}

}