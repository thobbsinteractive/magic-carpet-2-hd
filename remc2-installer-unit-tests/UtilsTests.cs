using NUnit.Framework;

namespace remc2_installer_unit_tests
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