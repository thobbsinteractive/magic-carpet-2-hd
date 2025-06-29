using System;
using System.IO;
using System.Reflection;
using WixToolset.Dtf.WindowsInstaller;

public class CustomActions
{
	[CustomAction]
    public static ActionResult SetEnhancedTextures(Session session)
    {
		bool enhancedTextures = false;

		try
		{
			if (!string.IsNullOrWhiteSpace(session["HIGHTEX"]) && session["HIGHTEX"].Equals("yes", System.StringComparison.InvariantCultureIgnoreCase))
			{
				enhancedTextures = true;
			}

			session.Log($"Setting Enhanced Textures to: {enhancedTextures}");
			string path = session["INSTALLDIR"];
			string configFilePath = Path.Combine(path, "config.json");
			session.Log($"Setting config.json file: {configFilePath}");

			if (File.Exists(configFilePath))
			{
				session.Log($"Updating config File: {configFilePath}");

				if (Utils.SetEnhancedTextures(configFilePath, configFilePath, enhancedTextures))
				{
					session.Log($"Success updating config File: {configFilePath}");
					return ActionResult.Success;
				}

				return ActionResult.Failure;
			}
			else
			{
				return ActionResult.Failure;
			}
		}
		catch (Exception ex)
		{
			session.Log($"Error Setting enhanced textures: {ex.Message}");
			return ActionResult.Failure;
		}
    }
}

