namespace CodeGenWF
{
    partial class Form1
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
            this._creator = new System.Windows.Forms.TextBox();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.button1 = new System.Windows.Forms.Button();
            this._pluginCreate = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this._interfaces = new System.Windows.Forms.ListBox();
            this._pluginDirectory = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this._pluginName = new System.Windows.Forms.TextBox();
            this.Interface = new System.Windows.Forms.TabPage();
            this.button2 = new System.Windows.Forms.Button();
            this._createInterface = new System.Windows.Forms.Button();
            this._interfaceDirectory = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this._interfaceName = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this._closeButton = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this._log = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.Interface.SuspendLayout();
            this.SuspendLayout();
            // 
            // _creator
            // 
            this._creator.Location = new System.Drawing.Point( 143, 13 );
            this._creator.Name = "_creator";
            this._creator.Size = new System.Drawing.Size( 399, 20 );
            this._creator.TabIndex = 0;
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add( this.tabPage1 );
            this.tabControl1.Controls.Add( this.Interface );
            this.tabControl1.Location = new System.Drawing.Point( 33, 77 );
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size( 613, 284 );
            this.tabControl1.TabIndex = 1;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add( this.button1 );
            this.tabPage1.Controls.Add( this._pluginCreate );
            this.tabPage1.Controls.Add( this.label3 );
            this.tabPage1.Controls.Add( this._interfaces );
            this.tabPage1.Controls.Add( this._pluginDirectory );
            this.tabPage1.Controls.Add( this.label2 );
            this.tabPage1.Controls.Add( this.label1 );
            this.tabPage1.Controls.Add( this._pluginName );
            this.tabPage1.Location = new System.Drawing.Point( 4, 22 );
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding( 3 );
            this.tabPage1.Size = new System.Drawing.Size( 605, 258 );
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "Plugin";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point( 429, 52 );
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size( 75, 23 );
            this.button1.TabIndex = 7;
            this.button1.Text = "...";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler( this.button1_Click );
            // 
            // _pluginCreate
            // 
            this._pluginCreate.Location = new System.Drawing.Point( 515, 220 );
            this._pluginCreate.Name = "_pluginCreate";
            this._pluginCreate.Size = new System.Drawing.Size( 75, 23 );
            this._pluginCreate.TabIndex = 6;
            this._pluginCreate.Text = "Create";
            this._pluginCreate.UseVisualStyleBackColor = true;
            this._pluginCreate.Click += new System.EventHandler( this._pluginCreate_Click );
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point( 272, 91 );
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size( 54, 13 );
            this.label3.TabIndex = 5;
            this.label3.Text = "Interfaces";
            // 
            // _interfaces
            // 
            this._interfaces.FormattingEnabled = true;
            this._interfaces.Location = new System.Drawing.Point( 195, 107 );
            this._interfaces.Name = "_interfaces";
            this._interfaces.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
            this._interfaces.Size = new System.Drawing.Size( 220, 95 );
            this._interfaces.TabIndex = 4;
            // 
            // _pluginDirectory
            // 
            this._pluginDirectory.Location = new System.Drawing.Point( 235, 52 );
            this._pluginDirectory.Name = "_pluginDirectory";
            this._pluginDirectory.Size = new System.Drawing.Size( 159, 20 );
            this._pluginDirectory.TabIndex = 3;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point( 139, 52 );
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size( 49, 13 );
            this.label2.TabIndex = 2;
            this.label2.Text = "Directory";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point( 139, 7 );
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size( 67, 13 );
            this.label1.TabIndex = 1;
            this.label1.Text = "Plugin Name";
            // 
            // _pluginName
            // 
            this._pluginName.Location = new System.Drawing.Point( 235, 7 );
            this._pluginName.Name = "_pluginName";
            this._pluginName.Size = new System.Drawing.Size( 160, 20 );
            this._pluginName.TabIndex = 0;
            // 
            // Interface
            // 
            this.Interface.Controls.Add( this.button2 );
            this.Interface.Controls.Add( this._createInterface );
            this.Interface.Controls.Add( this._interfaceDirectory );
            this.Interface.Controls.Add( this.label6 );
            this.Interface.Controls.Add( this._interfaceName );
            this.Interface.Controls.Add( this.label5 );
            this.Interface.Location = new System.Drawing.Point( 4, 22 );
            this.Interface.Name = "Interface";
            this.Interface.Padding = new System.Windows.Forms.Padding( 3 );
            this.Interface.Size = new System.Drawing.Size( 605, 258 );
            this.Interface.TabIndex = 1;
            this.Interface.Text = "Interface";
            this.Interface.UseVisualStyleBackColor = true;
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point( 429, 56 );
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size( 75, 23 );
            this.button2.TabIndex = 5;
            this.button2.Text = "...";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler( this.button2_Click );
            // 
            // _createInterface
            // 
            this._createInterface.Location = new System.Drawing.Point( 527, 217 );
            this._createInterface.Name = "_createInterface";
            this._createInterface.Size = new System.Drawing.Size( 75, 23 );
            this._createInterface.TabIndex = 4;
            this._createInterface.Text = "Create";
            this._createInterface.UseVisualStyleBackColor = true;
            this._createInterface.Click += new System.EventHandler( this._createInterface_Click );
            // 
            // _interfaceDirectory
            // 
            this._interfaceDirectory.Location = new System.Drawing.Point( 186, 56 );
            this._interfaceDirectory.Name = "_interfaceDirectory";
            this._interfaceDirectory.Size = new System.Drawing.Size( 191, 20 );
            this._interfaceDirectory.TabIndex = 3;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point( 109, 64 );
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size( 49, 13 );
            this.label6.TabIndex = 2;
            this.label6.Text = "Directory";
            // 
            // _interfaceName
            // 
            this._interfaceName.Location = new System.Drawing.Point( 186, 16 );
            this._interfaceName.Name = "_interfaceName";
            this._interfaceName.Size = new System.Drawing.Size( 191, 20 );
            this._interfaceName.TabIndex = 1;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point( 106, 24 );
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size( 80, 13 );
            this.label5.TabIndex = 0;
            this.label5.Text = "Interface Name";
            // 
            // _closeButton
            // 
            this._closeButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this._closeButton.Location = new System.Drawing.Point( 571, 502 );
            this._closeButton.Name = "_closeButton";
            this._closeButton.Size = new System.Drawing.Size( 75, 23 );
            this._closeButton.TabIndex = 2;
            this._closeButton.Text = "Close";
            this._closeButton.UseVisualStyleBackColor = true;
            this._closeButton.Click += new System.EventHandler( this._closeButton_Click );
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point( 96, 19 );
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size( 35, 13 );
            this.label4.TabIndex = 3;
            this.label4.Text = "Name";
            // 
            // _log
            // 
            this._log.Location = new System.Drawing.Point( 33, 381 );
            this._log.Multiline = true;
            this._log.Name = "_log";
            this._log.ReadOnly = true;
            this._log.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this._log.Size = new System.Drawing.Size( 609, 115 );
            this._log.TabIndex = 4;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point( 33, 364 );
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size( 28, 13 );
            this.label7.TabIndex = 5;
            this.label7.Text = "Log:";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this._closeButton;
            this.ClientSize = new System.Drawing.Size( 675, 537 );
            this.Controls.Add( this.label7 );
            this.Controls.Add( this._log );
            this.Controls.Add( this.label4 );
            this.Controls.Add( this._closeButton );
            this.Controls.Add( this.tabControl1 );
            this.Controls.Add( this._creator );
            this.Name = "Form1";
            this.Text = "Code Generator";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler( this._formClosing );
            this.tabControl1.ResumeLayout( false );
            this.tabPage1.ResumeLayout( false );
            this.tabPage1.PerformLayout();
            this.Interface.ResumeLayout( false );
            this.Interface.PerformLayout();
            this.ResumeLayout( false );
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox _creator;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox _pluginName;
        private System.Windows.Forms.TabPage Interface;
        private System.Windows.Forms.Button _closeButton;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ListBox _interfaces;
        private System.Windows.Forms.TextBox _pluginDirectory;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button _pluginCreate;
        private System.Windows.Forms.Button _createInterface;
        private System.Windows.Forms.TextBox _interfaceDirectory;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox _interfaceName;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.TextBox _log;
        private System.Windows.Forms.Label label7;
    }
}

