package view.game 
{
	import flash.display.Graphics;
	import flash.display.Sprite;
	import flash.events.MouseEvent;
	import flash.filters.DropShadowFilter;
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.text.TextFormatAlign;
	import flash.filters.BitmapFilterQuality;
	import model.*;

	/**
	 * ...
	 * @author bav
	 */
	public class RoomChooser extends Sprite 
	{
		private var _model:MainModel;

		public function RoomChooser(model:MainModel) 
		{
			_model = model;
			init();
		}

		private function init():void
		{
			var g:Graphics = graphics;
			g.beginFill(0xEDEDED);
			g.drawRoundRect(0, 0, 300, 70, 20, 20);
			g.endFill();
			for (var i:int = 0; i < 4; i++)
			{
				var t:TextField = new TextField();
				t.width = t.height = 50;
				t.defaultTextFormat = new TextFormat("_typewriter", 36, null, true, null, null, null, null, TextFormatAlign.CENTER);
				t.text = String(i + 3);
				t.selectable = false;
				t.border = t.background = true;
				t.addEventListener(MouseEvent.CLICK, buttonClickHandler);
				t.y = 10;
				t.x = 20 + 70 * i;
				addChild(t);
			}
			filters = [new DropShadowFilter(0, 0, 0, 0.6, 5, 5, 2, BitmapFilterQuality.HIGH)];
		}

		private function buttonClickHandler(e:MouseEvent):void
		{
			if (e.target is TextField)
			{
				var type:int = parseInt((e.target as TextField).text);
				if (type)
				{
					Dispatcher.instance.dispatchEvent(new CluedoEvent(CluedoEvent.ENTER_ROOM, type));
				}
			}
		}

	}

}