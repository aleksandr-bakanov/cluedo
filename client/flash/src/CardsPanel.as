package  
{
	import flash.display.DisplayObject;
	import flash.display.InteractiveObject;
	import flash.display.Sprite;
	import flash.events.MouseEvent;
	import flash.filters.GlowFilter;
	
	/**
	 * ...
	 * @author bav
	 */
	public class CardsPanel extends Sprite 
	{
		private var _model:Model;
		private var _cards:Array /* of Card */;
		
		public function CardsPanel(model:Model) 
		{
			_model = model;
			configureMovelListeners();
		}
		
		private function configureMovelListeners():void 
		{
			_model.addEventListener(CluedoEvent.START_WAIT_ANSWER, startWaitAnswerHandler);
			_model.addEventListener(CluedoEvent.PLAYER_ANSWER, playerAnswerHandler);
		}
        
        private function playerAnswerHandler(e:CluedoEvent):void 
        {
            removeGlow();
        }
		
		private function startWaitAnswerHandler(e:CluedoEvent):void 
		{
			if (e.data.gt == _model.guest)
			{
				removeGlow();
				var len:int = _cards.length;
				for (var i:int = 0; i < len; i++)
				{
					var c:Card = _cards[i] as Card;
					if (c.id == _model.suspectAp || c.id == _model.suspectWp || c.id == _model.suspectGt)
					{
						c.filters = [new GlowFilter(0x0000FF, 1, 6, 6, 2, 1, true)];
						c.addEventListener(MouseEvent.CLICK, cardClickHandler);
					}
				}
			}
            else
                removeGlow();
		}
		
		private function removeGlow():void
		{
			var len:int = _cards.length;
			for (var i:int = 0; i < len; i++)
			{
				var c:Card = _cards[i] as Card;
                if (c)
                {
                    if (c.filters.length)
                        c.filters = [];
                    if (c.hasEventListener(MouseEvent.CLICK))
                        c.removeEventListener(MouseEvent.CLICK, cardClickHandler);
                }
			}
		}
		
		private function cardClickHandler(e:MouseEvent):void
		{
			var id:int = (e.currentTarget as Card).id;
			removeGlow();
			_model.dispatchEvent(new CluedoEvent(CluedoEvent.C_ANSWER, id));
		}
		
		public function initCards(cards:Array):void
		{
			_cards = [];
			var len:int = cards.length;
			for (var i:int = 0; i < len; i++)
			{
				var c:Card = new Card(cards[i]);
				_cards.push(c);
				c.y = c.height * i;
				addChild(c);
			}
		}
		
	}

}