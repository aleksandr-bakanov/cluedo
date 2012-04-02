package view.game 
{
	import flash.display.Sprite;
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.text.TextFormatAlign;
	import model.*;

	/**
	 * ...
	 * @author bav
	 */
	public class Dice extends Sprite 
	{
		private var _field:TextField;
		private var _model:MainModel;

		public function Dice(model:MainModel) 
		{
			_model = model;
			Dispatcher.instance.addEventListener(CluedoEvent.NEXT_MOVE, nextMoveHandler);
			Dispatcher.instance.addEventListener(CluedoEvent.S_GUEST_MOVE, sGuestMoveHandler);
			
			_field = new TextField();
			_field.height = 20;
			_field.width = 20;
			_field.border = true;
			_field.selectable = false;
			_field.defaultTextFormat = new TextFormat("_typewriter", 12, 0, null, null, null, null, null, TextFormatAlign.RIGHT);
			addChild(_field);
		}

		private function sGuestMoveHandler(e:CluedoEvent):void 
		{
			_field.text = _model.steps.toString();
		}

		private function nextMoveHandler(e:CluedoEvent):void 
		{
			_field.text = _model.steps.toString();
		}

	}

}