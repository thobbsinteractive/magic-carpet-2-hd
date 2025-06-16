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

			if (Utils.SetEnhancedTextures(configFilePath, enhancedTextures))
				return ActionResult.Success;

			return ActionResult.Failure;
		}
        else
        {
            return ActionResult.Failure;
        }
    }
}

