package view 
{
	import flash.display.Sprite;
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.text.TextFormatAlign;

	/**
	 * ...
	 * @author bav
	 */
	public class Card extends Sprite 
	{
		// Guest constants
		public static const GT_SCARLETT:int = 1;
		public static const GT_MUSTARD:int = 2;
		public static const GT_WHITE:int = 3;
		public static const GT_GREEN:int = 4;
		public static const GT_PEACOCK:int = 5;
		public static const GT_PLUM:int = 6;
		// Weapon constants
		public static const WP_CANDLESTICK:int = 7;
		public static const WP_KNIFE:int = 8;
		public static const WP_AXE:int = 9;
		public static const WP_PISTOL:int = 10;
		public static const WP_ROPE:int = 11;
		public static const WP_TROPHY:int = 12;
		public static const WP_BAT:int = 13;
		public static const WP_DUMBBELL:int = 14;
		public static const WP_POISON:int = 15;
		// Appartments constants
		public static const AP_KITCHEN:int = 16;
		public static const AP_BALLROOM:int = 17;
		public static const AP_CONSERVATORY:int = 18;
		public static const AP_DINING_ROOM:int = 19;
		public static const AP_BILLIARD_ROOM:int = 20;
		public static const AP_LIBRARY:int = 21;
		public static const AP_LOUNGE:int = 22;
		public static const AP_HALL:int = 23;
		public static const AP_STUDY:int = 24;

		private var _field:TextField;
		private var _id:int;

		public function Card(id:int) 
		{
			_field = new TextField();
			_field.defaultTextFormat = new TextFormat("_typewriter", 12);
			_field.selectable = false;
			_field.height = 20;
			_field.border = true;
			_field.background = true;
			this.id = id;
			addChild(_field);
		}

		public function get id():int 
		{
			return _id;
		}

		public function set id(value:int):void 
		{
			_id = value;
			switch (_id)
			{
				case GT_SCARLETT: _field.text = "Красная"; break;
				case GT_MUSTARD: _field.text = "Желтый"; break;
				case GT_WHITE: _field.text = "Белая"; break;
				case GT_GREEN: _field.text = "Зеленый"; break;
				case GT_PEACOCK: _field.text = "Синяя"; break;
				case GT_PLUM: _field.text = "Фиолетовый"; break;
				case WP_CANDLESTICK: _field.text = "Канделябр"; break;
				case WP_KNIFE: _field.text = "Кинжал"; break;
				case WP_AXE: _field.text = "Топор"; break;
				case WP_PISTOL: _field.text = "Пистолет"; break;
				case WP_ROPE: _field.text = "Веревка"; break;
				case WP_TROPHY: _field.text = "Трофей"; break;
				case WP_BAT: _field.text = "Бита"; break;
				case WP_DUMBBELL: _field.text = "Гантеля"; break;
				case WP_POISON: _field.text = "Яд"; break;
				case AP_KITCHEN: _field.text = "Кухня"; break;
				case AP_BALLROOM: _field.text = "Танцпол"; break;
				case AP_CONSERVATORY: _field.text = "Консерватория"; break;
				case AP_DINING_ROOM: _field.text = "Столовая"; break;
				case AP_BILLIARD_ROOM: _field.text = "Бильярдная"; break;
				case AP_LIBRARY: _field.text = "Библиотека"; break;
				case AP_LOUNGE: _field.text = "Гостиная"; break;
				case AP_HALL: _field.text = "Холл"; break;
				case AP_STUDY: _field.text = "Кабинет"; break;
			}
		}

		public static function cardToString(id:int):String
		{
			var r:String = "";
			switch (id)
			{
				case GT_GREEN: r = "Зеленый"; break;
				case GT_MUSTARD: r = "Желтый"; break;
				case GT_PEACOCK: r = "Синяя"; break;
				case GT_PLUM: r = "Фиолетовый"; break;
				case GT_SCARLETT: r = "Красная"; break;
				case GT_WHITE: r = "Белая"; break;
				case AP_BALLROOM: r = "Танцпол"; break;
				case AP_BILLIARD_ROOM: r = "Бильярдная"; break;
				case AP_CONSERVATORY: r = "Консерватория"; break;
				case AP_DINING_ROOM: r = "Столовая"; break;
				case AP_HALL: r = "Холл"; break;
				case AP_KITCHEN: r = "Кухня"; break;
				case AP_LIBRARY: r = "Библиотека"; break;
				case AP_LOUNGE: r = "Гостиная"; break;
				case AP_STUDY: r = "Кабинет"; break;
				case WP_AXE: r = "Топор"; break;
				case WP_BAT: r = "Бита"; break;
				case WP_CANDLESTICK: r = "Канделябр"; break;
				case WP_DUMBBELL: r = "Гантеля"; break;
				case WP_KNIFE: r = "Кинжал"; break;
				case WP_PISTOL: r = "Пистолет"; break;
				case WP_POISON: r = "Яд"; break;
				case WP_ROPE: r = "Веревка"; break;
				case WP_TROPHY: r = "Трофей"; break;
			}
			return r;
		}
	}

}