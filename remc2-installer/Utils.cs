using Newtonsoft.Json;
using System.IO;
using System.Linq;

namespace MagicCarpet2HDInstaller
{
	public static class Utils
	{
		public static void CopyDirectory(string sourceDir, string destinationDir, bool recursive)
		{
			// Get information about the source directory
			var dir = new DirectoryInfo(sourceDir);

			// Check if the source directory exists
			if (!dir.Exists)
				throw new DirectoryNotFoundException($"Source directory not found: {dir.FullName}");

			// Cache directories before we start copying
			DirectoryInfo[] dirs = dir.GetDirectories();

			// Create the destination directory
			if (!Directory.Exists(destinationDir))
				Directory.CreateDirectory(destinationDir);

			// Get the files in the source directory and copy to the destination directory
			foreach (FileInfo file in dir.GetFiles())
			{
				string targetFilePath = Path.Combine(destinationDir, file.Name);
				file.CopyTo(targetFilePath, true);
			}

			// If recursive and copying subdirectories, recursively call this method
			if (recursive)
			{
				foreach (DirectoryInfo subDir in dirs)
				{
					string newDestinationDir = Path.Combine(destinationDir, subDir.Name);
					CopyDirectory(subDir.FullName, newDestinationDir, true);
				}
			}
		}

		public static bool DeleteFiles(DirectoryInfo baseDir, string[] excludedFiles, string[] excludedDirs)
		{
			bool keepBaseDir = false;

			// Check if the source directory exists
			if (baseDir.Exists)
			{
				if (excludedDirs?.Length > 0 && excludedDirs.Select(x => x.ToLower()).Contains(baseDir.Name.ToLower()))
				{
					keepBaseDir = true;
				}
				else
				{
					foreach (DirectoryInfo subDir in baseDir.GetDirectories())
					{
						if (DeleteFiles(subDir, excludedFiles, excludedDirs))
						{
							keepBaseDir = true;
						}
						else
						{
							subDir.Delete(true);
						}
					}

					foreach (FileInfo file in baseDir.GetFiles())
					{
						if (excludedFiles?.Length > 0 && excludedFiles.Select(x => x.ToLower()).Contains(file.Name.ToLower()))
						{
							//Skip File
						}
						else
						{
							file.Delete();
						}
					}
				}
			}
			return keepBaseDir;
		}

		public static bool SetEnhancedTextures(string configFilePath, bool enhancedTextures)
		{
			var json = File.ReadAllText(configFilePath);
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
					json = JsonConvert.SerializeObject(config, Formatting.Indented);
					File.WriteAllText(configFilePath, json);
					return true;
				}
			}
			return false;
		}
	}
}
