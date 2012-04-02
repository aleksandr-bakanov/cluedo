package view.game 
{
	import flash.display.Sprite;
	import flash.events.MouseEvent;
	import flash.text.TextField;
	import flash.text.TextFormat;

	/**
	 * ...
	 * @author bav
	 */
	public class Button extends Sprite 
	{
		private var _field:TextField;

		public function Button(value:String) 
		{
			_field = new TextField();
			_field.defaultTextFormat = new TextFormat("_typewriter", 12);
			_field.selectable = false;
			_field.height = 20;
			_field.border = true;
			_field.background = true;
			_field.text = value;
			addChild(_field);
		}

	}

}