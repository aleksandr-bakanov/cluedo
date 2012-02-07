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