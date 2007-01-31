namespace Minerva.Plugins.App.Distributed
{
    partial class SessionControl
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this._sessionName = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this._okButton = new System.Windows.Forms.Button();
            this._availableSessions = new System.Windows.Forms.ListBox();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(48, 156);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(78, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Session Name:";
            // 
            // _sessionName
            // 
            this._sessionName.Location = new System.Drawing.Point(146, 153);
            this._sessionName.Name = "_sessionName";
            this._sessionName.Size = new System.Drawing.Size(120, 20);
            this._sessionName.TabIndex = 1;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(108, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(79, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Enter a session";
            // 
            // _okButton
            // 
            this._okButton.Location = new System.Drawing.Point(112, 191);
            this._okButton.Name = "_okButton";
            this._okButton.Size = new System.Drawing.Size(75, 23);
            this._okButton.TabIndex = 3;
            this._okButton.Text = "OK";
            this._okButton.UseVisualStyleBackColor = true;
            this._okButton.Click += new System.EventHandler(this._okButton_Click);
            // 
            // _availableSessions
            // 
            this._availableSessions.FormattingEnabled = true;
            this._availableSessions.Location = new System.Drawing.Point(51, 39);
            this._availableSessions.Name = "_availableSessions";
            this._availableSessions.Size = new System.Drawing.Size(215, 108);
            this._availableSessions.TabIndex = 4;
            this._availableSessions.DoubleClick += new System.EventHandler(this._onListDoubleClick);
            this._availableSessions.SelectedIndexChanged += new System.EventHandler(this._availableSessions_SelectedIndexChanged);
            // 
            // SessionControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(329, 237);
            this.Controls.Add(this._availableSessions);
            this.Controls.Add(this._okButton);
            this.Controls.Add(this.label2);
            this.Controls.Add(this._sessionName);
            this.Controls.Add(this.label1);
            this.Name = "SessionControl";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox _sessionName;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button _okButton;
        private System.Windows.Forms.ListBox _availableSessions;
    }
}
