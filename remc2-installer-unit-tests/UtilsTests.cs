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
	}


}