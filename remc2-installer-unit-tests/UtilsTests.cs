using MagicCarpet2HDInstaller;
using NUnit.Framework;

namespace MagicCarpet2HDInstallerUnitTests
{
	public class Tests
	{
		[SetUp]
		public void Setup()
		{
		}

		[TestCase(@"C:\Temp")]
		public void DeleteFilesTest(string testDir)
		{
			Utils.DeleteFiles(new System.IO.DirectoryInfo(testDir), null, new[] { "SAVE" });

			Assert.Pass();
		}

		[TestCase(@"C:\Temp\config.json")]
		public void EditConfigFileTest(string testFilePath)
		{
			Utils.SetEnhancedTextures(testFilePath, true);

			Assert.Pass();
		}
	}


}