package model 
{
	import flash.events.EventDispatcher;
	
	/**
	 * Main application model
	 * @author bav
	 */
	public class MainModel extends EventDispatcher 
	{
		public static const COLORS:Array = [0xFF0000, 0xFFFF00, 0xFFFFFF, 0x00FF00, 0x0000FF, 0xFF00FF];

		public var guestsOrder:Array;
		public var cards:Array;
		public var guest:int;
		public var x:int;
		public var y:int;
		public var steps:int;

		public var suspectGt:int;
		public var suspectWp:int;
		public var suspectAp:int;

		public var curGuest:int;

		public function MainModel() 
		{

		}

	}

}