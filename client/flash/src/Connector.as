package  
{
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.events.SecurityErrorEvent;
	import flash.net.Socket;
	
	/**
	 * ...
	 * @author bav
	 */
	public class Connector extends EventDispatcher 
	{
		private var _socket:Socket;
		
		public function Connector() 
		{
			configureSocket();
		}
		
		private function configureSocket():void
		{
			_socket = new Socket();
			_socket.addEventListener(Event.CONNECT, connectHandler);
			_socket.addEventListener(Event.CLOSE, closeHandler);
			_socket.addEventListener(IOErrorEvent.IO_ERROR, ioErrorHandler);
			_socket.addEventListener(SecurityErrorEvent.SECURITY_ERROR, securityErrorHandler);
			_socket.addEventListener(ProgressEvent.SOCKET_DATA, socketDataHandler);
			_socket.connect("192.168.1.3", 7003);
		}
		
		private function socketDataHandler(e:ProgressEvent):void 
		{
			CluedoMain.ttrace("Data received.");
		}
		
		private function securityErrorHandler(e:SecurityErrorEvent):void 
		{
			CluedoMain.ttrace("Security error");
		}
		
		private function ioErrorHandler(e:IOErrorEvent):void 
		{
			CluedoMain.ttrace("IOError");
		}
		
		private function closeHandler(e:Event):void 
		{
			CluedoMain.ttrace("Close handler");
		}
		
		private function connectHandler(e:Event):void 
		{
			CluedoMain.ttrace("Connection established.");
		}
		
	}

}