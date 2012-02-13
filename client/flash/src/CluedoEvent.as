package  
{
	import flash.events.Event;
	
	/**
	 * ...
	 * @author bav
	 */
	public class CluedoEvent extends Event 
	{
		public static const CONNECT:String = "connect";
		public static const ENTER_ROOM:String = "enter_room";
		public static const AVAILABLE_GUESTS:String = "available_guests";
		public static const CHOOSE_GUEST:String = "choose_guest";
		public static const INIT_GAME:String = "init_game";
		public static const NEXT_MOVE:String = "next_move";
		public static const C_GUEST_MOVE:String = "c_guest_move";
		public static const S_GUEST_MOVE:String = "s_guest_move";
		public static const CHECK_STEPS:String = "check_steps";
		public static const C_ASK:String = "c_ask";
		public static const SHOW_ENQUIRE_PANEL:String = "show_enquire_panel";
		public static const TRANS_GUEST:String = "trans_guest";
		public static const START_WAIT_ANSWER:String = "start_wait_answer";
		public static const C_ANSWER:String = "c_answer";
		public static const C_GUESS_SECRET:String = "c_guess_secret";
		public static const END_GAME:String = "end_game";
		
		public var data:Object;
		
		public function CluedoEvent(type:String, data:Object=null, bubbles:Boolean=false, cancelable:Boolean=false) 
		{ 
			super(type, bubbles, cancelable);
			this.data = data;
		} 
		
		public override function clone():Event 
		{ 
			return new CluedoEvent(type, data, bubbles, cancelable);
		} 
		
		public override function toString():String 
		{ 
			return formatToString("CluedoEvent", "type", "bubbles", "cancelable", "eventPhase"); 
		}
		
	}
	
}