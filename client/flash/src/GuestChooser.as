package  
{
	import flash.display.DisplayObject;
	import flash.display.Graphics;
	import flash.display.Sprite;
	import flash.events.MouseEvent;
	import flash.filters.DropShadowFilter;
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.text.TextFormatAlign;
	import flash.filters.BitmapFilterQuality;
	
	/**
	 * ...
	 * @author bav
	 */
	public class GuestChooser extends Sprite 
	{
		private var _model:Model;
		private var _colors:Array = [0xFF0000, 0xFFFF00, 0xFFFFFF, 0x00FF00, 0x0000FF, 0xFF00FF];
		private var _buttons:Array;
		
		public function GuestChooser(model:Model) 
		{
			_model = model;
			_model.addEventListener(CluedoEvent.AVAILABLE_GUESTS, availableGuestsHandler);
			init();
		}
		
		private function init():void
		{
			var g:Graphics = graphics;
			g.beginFill(0xEDEDED);
			g.drawRoundRect(0, 0, 450, 70, 20, 20);
			g.endFill();
			_buttons = [];
			for (var i:int = 0; i < 6; i++)
			{
				var s:Sprite = new Sprite();
				g = s.graphics;
				g.lineStyle(1, 0, 1, true);
				g.beginFill(0xAAAAAA);
				g.drawCircle(25, 25, 25);
				g.endFill();
				s.addEventListener(MouseEvent.CLICK, buttonClickHandler);
				s.y = 10;
				s.x = 20 + 70 * i;
				s.name = String(i + 1);
				addChild(s);
				_buttons.push(s);
			}
			filters = [new DropShadowFilter(0, 0, 0, 0.6, 5, 5, 2, BitmapFilterQuality.HIGH)];
		}
		
		private function buttonClickHandler(e:MouseEvent):void
		{
			if (e.target is Sprite)
			{
				var gt:int = parseInt((e.target as DisplayObject).name);
				if (gt)
					_model.dispatchEvent(new CluedoEvent(CluedoEvent.CHOOSE_GUEST, gt));
			}
		}
		
		private function availableGuestsHandler(e:CluedoEvent):void 
		{
			var ag:int = e.data as int;
			var s:Sprite;
			var g:Graphics;
			for (var i:int = 0; i < 6; i++)
			{
				s = _buttons[i] as Sprite;
				g = s.graphics;
				g.clear();
				g.lineStyle(1, 0, 1, true);
				if (ag & Math.pow(2, i))
					g.beginFill(_colors[i]);
				else
					g.beginFill(0xAAAAAA);
				g.drawCircle(25, 25, 25);
				g.endFill();
			}
		}
		
	}

}