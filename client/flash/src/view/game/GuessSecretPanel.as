package view.game 
{
	import flash.display.DisplayObjectContainer;
	import flash.display.Graphics;
	import flash.display.Sprite;
	import flash.events.MouseEvent;
	import flash.text.TextField;
	import flash.filters.GlowFilter;
	import flash.filters.DropShadowFilter;
	import flash.filters.BitmapFilterQuality;
	import flash.text.TextFormatAlign;
	import flash.text.TextFormat;
	import model.*;

	/**
	 * ...
	 * @author bav
	 */
	public class GuessSecretPanel extends Sprite 
	{
		private var _model:MainModel;
		private var _apps:Sprite;
		private var _guests:Sprite;
		private var _weapons:Sprite;

		private var _curGuest:int;
		private var _curWeapon:int;
		private var _curAppartments:int;

		private var _ok:TextField;
		private var _cancel:TextField;

		public function GuessSecretPanel(model:MainModel) 
		{
			_model = model;
			var g:Graphics = graphics;
			g.beginFill(0xDEDEDE);
			g.drawRoundRect(0, 0, 360, 255, 20, 20);
			g.endFill();
			filters = [new DropShadowFilter(0, 0, 0, 0.6, 5, 5, 2, BitmapFilterQuality.HIGH)];
			initLists();

			_ok = new TextField();
			_ok.defaultTextFormat = new TextFormat("_typewriter", 12, 0, null, null, null, null, null, TextFormatAlign.CENTER);
			_ok.selectable = false;
			_ok.height = 20;
			_ok.border = _ok.background = true;
			_ok.text = "Ok";
			_ok.x = 50;
			_ok.y = 230;
			addChild(_ok);
			_ok.addEventListener(MouseEvent.CLICK, okClickHandler);

			_cancel = new TextField();
			_cancel.defaultTextFormat = new TextFormat("_typewriter", 12, 0, null, null, null, null, null, TextFormatAlign.CENTER);
			_cancel.selectable = false;
			_cancel.height = 20;
			_cancel.border = _cancel.background = true;
			_cancel.text = "Cancel";
			_cancel.x = 200;
			_cancel.y = 230;
			addChild(_cancel);
			_cancel.addEventListener(MouseEvent.CLICK, cancelClickHandler);
		}

		private function cancelClickHandler(e:MouseEvent):void 
		{
			_curWeapon = _curGuest = _curAppartments = 0;
			removeFilters(_guests);
			removeFilters(_weapons);
			removeFilters(_apps);
			if (parent)
				parent.removeChild(this);
		}

		private function okClickHandler(e:MouseEvent):void 
		{
			if (_curGuest && _curWeapon && _curAppartments)
			{
				Dispatcher.instance.dispatchEvent(new CluedoEvent(CluedoEvent.C_GUESS_SECRET, { gt:_curGuest, wp:_curWeapon, ap:_curAppartments } ));
				if (parent)
					parent.removeChild(this);
			}
		}

		private function initLists():void
		{
			_guests = new Sprite();
			var i:int;
			var c:Card;
			for (i = 1; i < 7; i++)
			{
				c = new Card(i);
				c.y = c.height * (i - 1);
				c.addEventListener(MouseEvent.CLICK, guestClickHandler);
				_guests.addChild(c);
			}
			_weapons = new Sprite();
			for (i = 7; i < 16; i++)
			{
				c = new Card(i);
				c.y = c.height * (i - 7);
				c.addEventListener(MouseEvent.CLICK, weaponClickHandler);
				_weapons.addChild(c);
			}
			_apps = new Sprite();
			for (i = 16; i < 25; i++)
			{
				c = new Card(i);
				c.y = c.height * (i - 16);
				c.addEventListener(MouseEvent.CLICK, appClickHandler);
				_apps.addChild(c);
			}
			_guests.x = 20;
			_weapons.x = _guests.x + _guests.width + 20;
			_apps.x = _weapons.x + _weapons.width + 20;
			_apps.y = _guests.y = _weapons.y = 40;
			addChild(_guests);
			addChild(_weapons);
			addChild(_apps);
		}

		private function guestClickHandler(e:MouseEvent):void 
		{
			removeFilters(_guests);
			var c:Card = e.currentTarget as Card;
			_curGuest = c.id;
			c.filters = [new GlowFilter(0x0000FF, 1, 6, 6, 2, 1, true)];
		}

		private function weaponClickHandler(e:MouseEvent):void 
		{
			removeFilters(_weapons);
			var c:Card = e.currentTarget as Card;
			_curWeapon = c.id;
			c.filters = [new GlowFilter(0x0000FF, 1, 6, 6, 2, 1, true)];
		}

		private function appClickHandler(e:MouseEvent):void 
		{
			removeFilters(_apps);
			var c:Card = e.currentTarget as Card;
			_curAppartments = c.id;
			c.filters = [new GlowFilter(0x0000FF, 1, 6, 6, 2, 1, true)];
		}

		private function removeFilters(p:DisplayObjectContainer):void
		{
			var len:int = p.numChildren;
			for (var i:int = 0; i < len; i++)
				p.getChildAt(i).filters = [];
		}

		public function clear():void 
		{
			_curWeapon = _curGuest = _curAppartments = 0;
			removeFilters(_guests);
			removeFilters(_weapons);
			removeFilters(_apps);
		}
	}

}