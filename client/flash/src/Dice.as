package  
{
	import flash.display.Sprite;
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.text.TextFormatAlign;
	
	/**
	 * ...
	 * @author bav
	 */
	public class Dice extends Sprite 
	{
		private var _field:TextField;
		private var _model:Model;
		
		public function Dice(model:Model) 
		{
			_model = model;
			_model.addEventListener(CluedoEvent.NEXT_MOVE, nextMoveHandler);
			
			_field = new TextField();
			_field.height = 20;
			_field.width = 20;
			_field.border = true;
			_field.defaultTextFormat = new TextFormat("_typewriter", 12, 0, null, null, null, null, null, TextFormatAlign.RIGHT);
			addChild(_field);
		}
		
		private function nextMoveHandler(e:CluedoEvent):void 
		{
			_field.text = String(e.data.fd + e.data.sd);
		}
		
	}

}