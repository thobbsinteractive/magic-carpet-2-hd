using System;
using WixSharp;
using WixSharp.UI.Forms;

namespace WixSharpSetup
{
    public partial class EnhancedDataDialog : ManagedForm, IManagedDialog
    {
        public EnhancedDataDialog()
        {
            //NOTE: If this assembly is compiled for v4.0.30319 runtime, it may not be compatible with the MSI hosted CLR.
            //The incompatibility is particularly possible for the Embedded UI scenarios.
            //The safest way to avoid the problem is to compile the assembly for v3.5 Target Framework.remc2_installer
            InitializeComponent();
        }

        void dialog_Load(object sender, EventArgs e)
        {
            banner.Image = Runtime.Session.GetResourceBitmap("WixUI_Bmp_Banner");
            Text = "[ProductName] Setup";

            this.chkInstallTextures.Checked = (!string.IsNullOrWhiteSpace(Runtime.Session["HIGHTEX"]) && Runtime.Session["HIGHTEX"].Equals("yes", StringComparison.InvariantCultureIgnoreCase));

            //resolve all Control.Text cases with embedded MSI properties (e.g. 'ProductName') and *.wxl file entries
            base.Localize();
        }

        void back_Click(object sender, EventArgs e)
        {
            Shell.GoPrev();
        }

        void next_Click(object sender, EventArgs e)
        {
            Runtime.Session["HIGHTEX"] = (chkInstallTextures.Checked ? "yes": "no");
            Shell.GoNext();
        }

        void cancel_Click(object sender, EventArgs e)
        {
            Shell.Cancel();
        }
    }
}