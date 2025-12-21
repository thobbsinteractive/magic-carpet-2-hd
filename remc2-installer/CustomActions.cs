using Microsoft.Deployment.WindowsInstaller;
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
        string configFilePath = Path.Combine(path, "config.ini");

        session.Log($"Setting config.ini file: {configFilePath}");

        if (System.IO.File.Exists(configFilePath))
        {
            session.Log($"Updating Ini File: {configFilePath}");
            IniFile iniFile = new IniFile();
            iniFile.Load(configFilePath);
            iniFile["graphics"]["useEnhancedGraphics"] = $"{enhancedTextures} ; if set to true, bigGraphicsFolder must be set as well";
            iniFile.Save(configFilePath);
            return ActionResult.Success;
        }
        else
        {
            return ActionResult.Failure;
        }
    }
}

