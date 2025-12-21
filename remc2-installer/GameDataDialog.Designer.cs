namespace WixSharpSetup
{
    partial class GameDataDialog
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.Windows.Forms.TreeNode treeNode10 = new System.Windows.Forms.TreeNode("CD_Files");
            System.Windows.Forms.TreeNode treeNode11 = new System.Windows.Forms.TreeNode("NETHERW");
            System.Windows.Forms.TreeNode treeNode12 = new System.Windows.Forms.TreeNode("C:\\Program Files x86\\Remc2\\Magic Carpet 2 HD", new System.Windows.Forms.TreeNode[] {
            treeNode10,
            treeNode11});
            this.banner = new System.Windows.Forms.PictureBox();
            this.panel2 = new System.Windows.Forms.Panel();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.btnNext = new System.Windows.Forms.Button();
            this.btnRun = new System.Windows.Forms.Button();
            this.cboInstallLocation = new System.Windows.Forms.ComboBox();
            this.lblInstallLocation = new System.Windows.Forms.Label();
            this.txtPath = new System.Windows.Forms.TextBox();
            this.btnBrowse = new System.Windows.Forms.Button();
            this.lblGameData = new System.Windows.Forms.Label();
            this.grpBox = new System.Windows.Forms.GroupBox();
            this.btnInfo = new System.Windows.Forms.Button();
            this.treeViewInstallProgress = new System.Windows.Forms.TreeView();
            ((System.ComponentModel.ISupportInitialize)(this.banner)).BeginInit();
            this.panel2.SuspendLayout();
            this.panel1.SuspendLayout();
            this.grpBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // banner
            // 
            this.banner.BackColor = System.Drawing.Color.White;
            this.banner.Dock = System.Windows.Forms.DockStyle.Fill;
            this.banner.Location = new System.Drawing.Point(0, 0);
            this.banner.Name = "banner";
            this.banner.Size = new System.Drawing.Size(501, 59);
            this.banner.TabIndex = 0;
            this.banner.TabStop = false;
            // 
            // panel2
            // 
            this.panel2.BackColor = System.Drawing.SystemColors.Control;
            this.panel2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel2.Controls.Add(this.label2);
            this.panel2.Controls.Add(this.label1);
            this.panel2.Controls.Add(this.banner);
            this.panel2.Location = new System.Drawing.Point(-5, -5);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(503, 61);
            this.panel2.TabIndex = 10;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.BackColor = System.Drawing.Color.White;
            this.label2.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(30, 31);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(475, 21);
            this.label2.TabIndex = 1;
            this.label2.Text = "Please select where to get the Magic Carpet 2 game data from";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.BackColor = System.Drawing.Color.White;
            this.label1.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(16, 8);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(192, 22);
            this.label1.TabIndex = 1;
            this.label1.Text = "Game Data Location";
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.SystemColors.Control;
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel1.Controls.Add(this.btnNext);
            this.panel1.Controls.Add(this.btnRun);
            this.panel1.Location = new System.Drawing.Point(-3, 308);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(503, 57);
            this.panel1.TabIndex = 9;
            // 
            // btnNext
            // 
            this.btnNext.AutoSize = true;
            this.btnNext.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnNext.Location = new System.Drawing.Point(409, 3);
            this.btnNext.Name = "btnNext";
            this.btnNext.Size = new System.Drawing.Size(75, 31);
            this.btnNext.TabIndex = 1;
            this.btnNext.Text = "Next";
            this.btnNext.UseVisualStyleBackColor = true;
            this.btnNext.Click += new System.EventHandler(this.btnNext_Click);
            // 
            // btnRun
            // 
            this.btnRun.AutoSize = true;
            this.btnRun.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnRun.Location = new System.Drawing.Point(241, 3);
            this.btnRun.Name = "btnRun";
            this.btnRun.Size = new System.Drawing.Size(162, 31);
            this.btnRun.TabIndex = 0;
            this.btnRun.Text = "Extract Game Data";
            this.btnRun.UseVisualStyleBackColor = true;
            this.btnRun.Click += new System.EventHandler(this.btnRun_Click);
            // 
            // cboInstallLocation
            // 
            this.cboInstallLocation.FormattingEnabled = true;
            this.cboInstallLocation.Items.AddRange(new object[] {
            "GOG Edition",
            "DOS Edition"});
            this.cboInstallLocation.Location = new System.Drawing.Point(100, 17);
            this.cboInstallLocation.Name = "cboInstallLocation";
            this.cboInstallLocation.Size = new System.Drawing.Size(205, 29);
            this.cboInstallLocation.TabIndex = 3;
            // 
            // lblInstallLocation
            // 
            this.lblInstallLocation.AutoSize = true;
            this.lblInstallLocation.Location = new System.Drawing.Point(11, 20);
            this.lblInstallLocation.Name = "lblInstallLocation";
            this.lblInstallLocation.Size = new System.Drawing.Size(126, 21);
            this.lblInstallLocation.TabIndex = 2;
            this.lblInstallLocation.Text = "Get Data From:";
            // 
            // txtPath
            // 
            this.txtPath.Location = new System.Drawing.Point(10, 85);
            this.txtPath.Margin = new System.Windows.Forms.Padding(3, 10, 3, 3);
            this.txtPath.Name = "txtPath";
            this.txtPath.Size = new System.Drawing.Size(381, 27);
            this.txtPath.TabIndex = 5;
            this.txtPath.TextChanged += new System.EventHandler(this.Path_TextChanged);
            // 
            // btnBrowse
            // 
            this.btnBrowse.AutoSize = true;
            this.btnBrowse.Location = new System.Drawing.Point(10, 49);
            this.btnBrowse.Name = "btnBrowse";
            this.btnBrowse.Size = new System.Drawing.Size(75, 31);
            this.btnBrowse.TabIndex = 4;
            this.btnBrowse.Text = "Browse";
            this.btnBrowse.UseVisualStyleBackColor = true;
            this.btnBrowse.Click += new System.EventHandler(this.btnBrowse_Click);
            // 
            // lblGameData
            // 
            this.lblGameData.AutoSize = true;
            this.lblGameData.Location = new System.Drawing.Point(84, 54);
            this.lblGameData.Name = "lblGameData";
            this.lblGameData.Size = new System.Drawing.Size(236, 21);
            this.lblGameData.TabIndex = 15;
            this.lblGameData.Text = "Magic Carpet 2 GOG directory:";
            // 
            // grpBox
            // 
            this.grpBox.Controls.Add(this.lblGameData);
            this.grpBox.Controls.Add(this.cboInstallLocation);
            this.grpBox.Controls.Add(this.lblInstallLocation);
            this.grpBox.Controls.Add(this.txtPath);
            this.grpBox.Controls.Add(this.btnBrowse);
            this.grpBox.Location = new System.Drawing.Point(12, 91);
            this.grpBox.Name = "grpBox";
            this.grpBox.Size = new System.Drawing.Size(470, 128);
            this.grpBox.TabIndex = 17;
            this.grpBox.TabStop = false;
            // 
            // btnInfo
            // 
            this.btnInfo.AutoSize = true;
            this.btnInfo.Location = new System.Drawing.Point(407, 62);
            this.btnInfo.Name = "btnInfo";
            this.btnInfo.Size = new System.Drawing.Size(75, 31);
            this.btnInfo.TabIndex = 8;
            this.btnInfo.Text = "Help";
            this.btnInfo.UseVisualStyleBackColor = true;
            this.btnInfo.Click += new System.EventHandler(this.btnInfo_Click);
            // 
            // treeViewInstallProgress
            // 
            this.treeViewInstallProgress.CheckBoxes = true;
            this.treeViewInstallProgress.Enabled = false;
            this.treeViewInstallProgress.Location = new System.Drawing.Point(12, 232);
            this.treeViewInstallProgress.Margin = new System.Windows.Forms.Padding(3, 10, 3, 3);
            this.treeViewInstallProgress.Name = "treeViewInstallProgress";
            treeNode10.Name = "CDFiles";
            treeNode10.Text = "CD_Files";
            treeNode11.Name = "NETHERW";
            treeNode11.Text = "NETHERW";
            treeNode12.Checked = true;
            treeNode12.Name = "InstallPath";
            treeNode12.Text = "C:\\Program Files x86\\Remc2\\Magic Carpet 2 HD";
            this.treeViewInstallProgress.Nodes.AddRange(new System.Windows.Forms.TreeNode[] {
            treeNode12});
            this.treeViewInstallProgress.ShowPlusMinus = false;
            this.treeViewInstallProgress.ShowRootLines = false;
            this.treeViewInstallProgress.Size = new System.Drawing.Size(391, 52);
            this.treeViewInstallProgress.TabIndex = 19;
            this.treeViewInstallProgress.TabStop = false;
            // 
            // GameDataDialog
            // 
            this.AcceptButton = this.btnRun;
            this.ClientSize = new System.Drawing.Size(488, 344);
            this.Controls.Add(this.treeViewInstallProgress);
            this.Controls.Add(this.btnInfo);
            this.Controls.Add(this.grpBox);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(510, 400);
            this.MinimumSize = new System.Drawing.Size(510, 400);
            this.Name = "GameDataDialog";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Game Data";
            this.Load += new System.EventHandler(this.dialog_Load);
            ((System.ComponentModel.ISupportInitialize)(this.banner)).EndInit();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.grpBox.ResumeLayout(false);
            this.grpBox.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox banner;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button btnRun;
        private System.Windows.Forms.ComboBox cboInstallLocation;
        private System.Windows.Forms.Label lblInstallLocation;
        private System.Windows.Forms.TextBox txtPath;
        private System.Windows.Forms.Button btnBrowse;
        private System.Windows.Forms.Button btnNext;
        private System.Windows.Forms.Label lblGameData;
		private System.Windows.Forms.GroupBox grpBox;
		private System.Windows.Forms.Button btnInfo;
		private System.Windows.Forms.TreeView treeViewInstallProgress;
	}
}