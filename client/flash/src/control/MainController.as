package control 
{
	import flash.display.DisplayObjectContainer;
	import model.MainModel;
	import view.MainView;
	/**
	 * Main application controller.
	 * @author bav
	 */
	public class MainController 
	{
		private var _mainView:MainView;
		private var _mainModel:MainModel;
		private var _connector:Connector;
		private var _resourceManager:ResourceManager;

		public function MainController(host:DisplayObjectContainer) 
		{
			_mainModel = new MainModel();
			_mainView = new MainView(host);
			_connector = new Connector(host.stage.loaderInfo.parameters.host, host.stage.loaderInfo.parameters.port);
			_resourceManager = new ResourceManager();
		}
		
	}

}