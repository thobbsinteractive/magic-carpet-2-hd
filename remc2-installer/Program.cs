using System;
using System.Windows.Forms;
using WixSharp;
using WixSharp.Forms;
using WixSharpSetup;

namespace remc2_installer
{
    internal class Program
    {
        static void Main()
        {
#if WIN64
            var project = new ManagedProject("Magic Carpet 2 HD x64",
#else
			var project = new ManagedProject("Magic Carpet 2 HD x86",
#endif
                             new Dir(new Id("INSTALLDIR"), @"%ProgramFiles%\ReMC\Magic Carpet 2 HD",
                                 new DirPermission("Everyone", GenericPermission.All),
#if WIN64
                                 new WixSharp.File(new Id("PRODUCT_EXE"), @"..\x64\Release\remc2.exe", new FileShortcut(new Id("PRODUCT_EXE_SHORTCUT"), "Magic Carpet 2 HD", @"%ProgramMenu%\ReMC\Magic Carpet 2 HD")
                                 {
                                     IconFile = "Resources/app.ico"
                                 })
#else
								 new WixSharp.File(new Id("PRODUCT_EXE"), @"..\Release\remc2.exe", new FileShortcut(new Id("PRODUCT_EXE_SHORTCUT"), "Magic Carpet 2 HD", @"%ProgramMenu%\ReMC\Magic Carpet 2 HD")
                                 {
                                     IconFile = "Resources/app.ico"
                                 })
#endif
                                 {
                                     Permissions = new[] {
                                        new FilePermission("Everyone", GenericPermission.All) { Execute = true },
                                        new FilePermission("Users", GenericPermission.All) { Execute = true },
                                        new FilePermission("AuthenticatedUser", GenericPermission.All) { Execute = true },
                                        new FilePermission("CREATOR OWNER", GenericPermission.All)  { Execute = true },
                                    }
                                 },
#if WIN64
								 new WixSharp.File(new Id("EDITOR_EXE"), @"..\x64\Release\remc2-editor.exe", new FileShortcut(new Id("EDITOR_EXE_SHORTCUT"), "Editor", @"%ProgramMenu%\ReMC\Magic Carpet 2 HD")
								 {
									 IconFile = "Resources/editor.ico"
								 })
#else
								 new WixSharp.File(new Id("EDITOR_EXE"), @"..\Release\remc2-editor.exe", new FileShortcut(new Id("EDITOR_EXE_SHORTCUT"), "Editor", @"%ProgramMenu%\ReMC\Magic Carpet 2 HD")
                                 {
                                     IconFile = "Resources/editor.ico"
                                 })
#endif
								 {
									 Permissions = new[] {
										new FilePermission("Everyone", GenericPermission.All) { Execute = true },
										new FilePermission("Users", GenericPermission.All) { Execute = true },
										new FilePermission("AuthenticatedUser", GenericPermission.All) { Execute = true },
										new FilePermission("CREATOR OWNER", GenericPermission.All)  { Execute = true },
									}
								 },
								 new File(new Id("CONFIG_INI"), @"..\Release\config.ini")
								 {
									 Permissions = new[] {
										new FilePermission("Everyone", GenericPermission.All) { ChangePermission = true },
										new FilePermission("Users", GenericPermission.All) { ChangePermission = true },
										new FilePermission("AuthenticatedUser", GenericPermission.All) { ChangePermission = true },
										new FilePermission("CREATOR OWNER", GenericPermission.All)  { ChangePermission = true },
									}
								 },
								 new File(new Id("EDITOR_CONFIG_INI"), @"..\Release\editor-config.ini")
								 {
									Permissions = new[] {
										new FilePermission("Everyone", GenericPermission.All) { ChangePermission = true },
										new FilePermission("Users", GenericPermission.All) { ChangePermission = true },
										new FilePermission("AuthenticatedUser", GenericPermission.All) { ChangePermission = true },
										new FilePermission("CREATOR OWNER", GenericPermission.All)  { ChangePermission = true },
									}
								 },
#if WIN64
								 new File(new Id("BROTLICOMMON_DLL"), @"..\x64\Release\brotlicommon.dll"),
								 new File(new Id("BROTLIDEC_DLL"), @"..\x64\Release\brotlidec.dll"),
								 new File(new Id("BZ2_DLL"), @"..\x64\Release\bz2.dll"),
								 new File(new Id("FMT_DLL"),@"..\x64\Release\fmt.dll"),
								 new File(new Id("FREETYPE_DLL"), @"..\x64\Release\freetype.dll"),
								 new File(new Id("LIBPNG16_DLL"), @"..\x64\Release\libpng16.dll"),
								 new File(new Id("OGG_DLL"), @"..\x64\Release\ogg.dll"),
								 new File(new Id("SDL2_DLL"), @"..\x64\Release\SDL2.dll"),
								 new File(new Id("SDL2_IMAGE_DLL"), @"..\x64\Release\SDL2_image.dll"),
								 new File(new Id("SDL2_MIXER_DLL"), @"..\x64\Release\SDL2_mixer.dll"),
								 new File(new Id("SDL2_TTF_DLL"), @"..\x64\Release\SDL2_ttf.dll"),
								 new File(new Id("VORBIS_DLL"), @"..\x64\Release\vorbis.dll"),
								 new File(new Id("VORBIXFILE_DLL"), @"..\x64\Release\vorbisfile.dll"),
								 new File(new Id("ZLIB1_DLL"), @"..\x64\Release\zlib1.dll"),
#else
								 new File(new Id("BROTLICOMMON_DLL"), @"..\Release\brotlicommon.dll"),
								 new File(new Id("BROTLIDEC_DLL"), @"..\Release\brotlidec.dll"),
								 new File(new Id("BZ2_DLL"), @"..\Release\bz2.dll"),
								 new File(new Id("FMT_DLL"), @"..\Release\fmt.dll"),
								 new File(new Id("FREETYPE_DLL"), @"..\Release\freetype.dll"),
								 new File(new Id("LIBPNG16_DLL"), @"..\Release\libpng16.dll"),
								 new File(new Id("OGG_DLL"), @"..\Release\ogg.dll"),
								 new File(new Id("SDL2_DLL"), @"..\Release\SDL2.dll"),
								 new File(new Id("SDL2_IMAGE_DLL"), @"..\Release\SDL2_image.dll"),
								 new File(new Id("SDL2_MIXER_DLL"), @"..\Release\SDL2_mixer.dll"),
								 new File(new Id("SDL2_TTF_DLL"), @"..\Release\SDL2_ttf.dll"),
								 new File(new Id("VORBIS_DLL"), @"..\Release\vorbis.dll"),
								 new File(new Id("VORBIXFILE_DLL"), @"..\Release\vorbisfile.dll"),
								 new File(new Id("ZLIB1_DLL"), @"..\Release\zlib1.dll"),
#endif
								 new File(new Id("EXTRACT_BAT"), @"Extract.bat"),
								 new Dir(new Id("KISS_INSTALLDIR"), @"kiss",
									new Files(@"..\Release\kiss\*.*")),
								 new Dir(new Id("FONT_INSTALLDIR"), @"font",
                                    new Files(@"..\Release\font\*.*")),
                                 new Dir(new Id("BIGGRAPHICS_INSTALLDIR"), @"biggraphics",
                                    new Files(@"..\enhancedassets\biggraphics\*.*")
                                    {
                                        ComponentCondition = "HIGHTEX=\"yes\""
                                    }),
                                 new Dir(new Id("MUSICOGG_INSTALLDIR"), @"music-ogg",
                                    new Files(@"..\enhancedassets\music-ogg\*.*")),
                                 new Dir(new Id("EXTRACT_INSTALLDIR"), @"Extract",
                                    new File(new Id("DOSBOXEXTRACT_CONF"), @"Extract\dosboxExtract-GOG-CD.conf"),
                                    new File(new Id("XXCOPY16_EXE"), @"Extract\XXCOPY16.EXE"),
									new File(new Id("MPXPLAY_EXE"), @"Extract\mpxplay.exe"))),
                            new Property(new Id("HIGHTEX_PROPERTY"), "HIGHTEX", "yes"),
                            new ManagedAction(new Id("MANAGED_ACTION"), CustomActions.SetEnhancedTextures, Return.check, When.After, Step.InstallFinalize, Condition.NOT_Installed));

#if WIN64
            project.Platform = Platform.x64;
#endif
            project.GUID = new Guid("d945f1c4-cbe4-445c-9674-07de64692857");
			project.Version = new Version(0, 9, 7, 0);

			//project.ManagedUI = ManagedUI.Empty;    //no standard UI dialogs
			//project.ManagedUI = ManagedUI.Default;  //all standard UI dialogs

			//custom set of standard UI dialogs
			project.ManagedUI = new ManagedUI();
            project.ManagedUI.InstallDialogs.Add(Dialogs.Welcome)
                                            .Add(Dialogs.Licence)
                                            .Add(Dialogs.InstallDir)
                                            .Add<EnhancedDataDialog>()
                                            .Add(Dialogs.Progress)
                                            .Add<GameDataDialog>()
                                            .Add(Dialogs.Exit);

            project.ManagedUI.ModifyDialogs.Add(Dialogs.MaintenanceType)
                                           .Add<EnhancedDataDialog>()
                                           .Add(Dialogs.Progress)
										   .Add<GameDataDialog>()
										   .Add(Dialogs.Exit);

            project.ControlPanelInfo.ProductIcon = @"Resources\app.ico";
            project.ControlPanelInfo.Comments = "Enhanced Edition of Magic Carpet 2";
            project.ControlPanelInfo.UrlInfoAbout = "https://github.com/thobbsinteractive/magic-carpet-2-hd";
            project.ControlPanelInfo.Contact = "Magic Carpet 2 HD";
            project.ControlPanelInfo.Manufacturer = "Magic Carpet 2 HD";

            project.LicenceFile = @"Resources\MagicCarpet2HD.licence.rtf";
            project.BackgroundImage = @"Resources\MagicCarpet2HD.dialog_bmp.png";
            project.BannerImage = @"Resources\MagicCarpet2HD.dialog_banner.png";
            project.InstallPrivileges = InstallPrivileges.elevated;
            ValidateAssemblyCompatibility();
            project.AfterInstall += OnAfterInstall;
            project.BuildMsi();
        }

        static void ValidateAssemblyCompatibility()
        {
            var assembly = System.Reflection.Assembly.GetExecutingAssembly();

            if (!assembly.ImageRuntimeVersion.StartsWith("v2."))
            {
                Console.WriteLine("Warning: assembly '{0}' is compiled for {1} runtime, which may not be compatible with the CLR version hosted by MSI. " +
                                  "The incompatibility is particularly possible for the EmbeddedUI scenarios. " +
                                   "The safest way to solve the problem is to compile the assembly for v3.5 Target Framework.",
                                   assembly.GetName().Name, assembly.ImageRuntimeVersion);
            }
        }

        public static void OnAfterInstall(SetupEventArgs e)
        {
            if (e.IsUninstalling)
            {
				if (MessageBox.Show("Would you like to keep your Save Game data?", "Keep Save files?", MessageBoxButtons.YesNo, MessageBoxIcon.Question, MessageBoxDefaultButton.Button1) == DialogResult.Yes)
				{
					var path = e.InstallDir;
					if (path.IsDirectory())
					{
						Utils.DeleteFiles(new System.IO.DirectoryInfo(path), null, new[] { "SAVE" });
					}
				}
				else
				{
					e.InstallDir.DeleteIfExists();
				}
            }
        }
    }
}