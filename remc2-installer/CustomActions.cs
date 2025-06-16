using Microsoft.Deployment.WindowsInstaller;
using Newtonsoft.Json;
using remc2_installer;
using System.IO;

public class CustomActions
{
    [CustomAction]
    public static ActionResult SetEnhancedTextures(Session session)
    {
        bool enhancedTextures = false;
            
        if (!string.IsNullOrWhiteSpace(session["HIGHTEX"]) && session["HIGHTEX"].Equals("yes", System.StringComparison.InvariantCultureIgnoreCase))
        {
            enhancedTextures = true;
        }


        session.Log($"Setting Enhanced Textures to: {enhancedTextures}");
        string path = session["INSTALLDIR"];
        string configFilePath = Path.Combine(path, "config.json");

        session.Log($"Setting config.json file: {configFilePath}");

        if (System.IO.File.Exists(configFilePath))
        {
            session.Log($"Updating config File: {configFilePath}");

			var json = File.ReadAllText(path);
			if (json != null) 
			{
				bool updated = false;
				var config = JsonConvert.DeserializeObject<Config>(json);

				foreach (var setting in config.settings)
				{
					if (setting.isActive)
					{
						setting.graphics.gameDetail.useEnhancedGraphics = enhancedTextures;
						updated = true;
						break;
					}
				}

				if (updated)
				{
					JsonConvert.SerializeObject(config);
					File.WriteAllText(json, configFilePath);
				}
			}
            return ActionResult.Success;
        }
        else
        {
            return ActionResult.Failure;
        }
    }
}

