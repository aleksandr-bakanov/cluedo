package view.social 
{
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.Sprite;
	
	/**
	 * ...
	 * @author bav
	 */
	public class UserPic extends Sprite 
	{
		private var _picture:Bitmap;
		
		public function UserPic(bitmapData:BitmapData) 
		{
			_picture = new Bitmap(bitmapData);
			addChild(_picture);
		}
		
	}

}