package control 
{
	import flash.display.Bitmap;
	import flash.display.Loader;
	import flash.events.Event;
	import flash.events.HTTPStatusEvent;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;
	import model.CluedoEvent;
	/**
	 * ...
	 * @author bav
	 */
	public class ResourceManager 
	{
		private static const LIB_PREFIX:String = "../lib/";
		// Map of BitmapData objects. Keys are strings.
		public static const bitmaps:Object = { };
		// Resource downloading queue
		private const queue:Vector.<String> = new Vector.<String>();
		// Picture loader
		private const loader:Loader = new Loader();

		public function ResourceManager() 
		{
			configureLoader();
			var xmlLoader:URLLoader = new URLLoader();
			xmlLoader.dataFormat = URLLoaderDataFormat.TEXT;
			var request:URLRequest = new URLRequest(LIB_PREFIX + "resources.xml");
			xmlLoader.addEventListener(Event.COMPLETE, xmlCompleteHandler);
			xmlLoader.load(request);
		}

		private function configureLoader():void
		{
			loader.contentLoaderInfo.addEventListener(Event.COMPLETE, pictureLoadComplete);
			loader.contentLoaderInfo.addEventListener(IOErrorEvent.IO_ERROR, ioErrorHandler);
			loader.contentLoaderInfo.addEventListener(HTTPStatusEvent.HTTP_STATUS, httpStatusHandler);
            loader.contentLoaderInfo.addEventListener(Event.INIT, initHandler);
            loader.contentLoaderInfo.addEventListener(Event.OPEN, openHandler);
            loader.contentLoaderInfo.addEventListener(ProgressEvent.PROGRESS, progressHandler);
            loader.contentLoaderInfo.addEventListener(Event.UNLOAD, unLoadHandler);
		}
		
		private function unLoadHandler(e:Event):void 
		{
			//trace(e);
		}
		
		private function progressHandler(e:ProgressEvent):void 
		{
			//trace(e);
		}
		
		private function openHandler(e:Event):void 
		{
			//trace(e);
		}
		
		private function initHandler(e:Event):void 
		{
			//trace(e);
		}
		
		private function httpStatusHandler(e:HTTPStatusEvent):void 
		{
			//trace(e);
		}
		
		private function ioErrorHandler(e:IOErrorEvent):void 
		{
			trace(e);
		}
		
		private function pictureLoadComplete(e:Event):void 
		{
			trace("pictureLoadComplete");
			var key:String = queue.shift();
			bitmaps[key] = (loader.content as Bitmap).bitmapData;
			loadNextPicture();
		}

		private function xmlCompleteHandler(e:Event):void 
		{
			var xmlLoader:URLLoader = e.target as URLLoader;
			xmlLoader.removeEventListener(Event.COMPLETE, xmlCompleteHandler);
			var xml:XML = XML(xmlLoader.data);
			for each (var node:XML in xml.images.image)
			{
				queue.push(node.@path);
				queue.push(node.@key);
			}
			loadNextPicture();
		}
		
		private function loadNextPicture():void
		{
			if (!queue.length)
			{
				Dispatcher.instance.dispatchEvent(new CluedoEvent(CluedoEvent.RESOURCES_READY));
				return;
			}
			var path:String = LIB_PREFIX + queue.shift();
			loader.load(new URLRequest(path));
		}

	}

}