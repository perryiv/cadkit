namespace DT.Minerva.DB
{
  partial class ConnectToDatabase
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
      this._host = new System.Windows.Forms.TextBox();
      this._database = new System.Windows.Forms.TextBox();
      this.label1 = new System.Windows.Forms.Label();
      this.label2 = new System.Windows.Forms.Label();
      this._user = new System.Windows.Forms.TextBox();
      this._password = new System.Windows.Forms.TextBox();
      this.label3 = new System.Windows.Forms.Label();
      this.label4 = new System.Windows.Forms.Label();
      this._okButton = new System.Windows.Forms.Button();
      this._cancelButton = new System.Windows.Forms.Button();
      this._savedEntries = new System.Windows.Forms.ComboBox();
      this.SuspendLayout();
      // 
      // _host
      // 
      this._host.Location = new System.Drawing.Point(90, 40);
      this._host.Name = "_host";
      this._host.Size = new System.Drawing.Size(181, 20);
      this._host.TabIndex = 0;
      // 
      // _database
      // 
      this._database.Location = new System.Drawing.Point(90, 80);
      this._database.Name = "_database";
      this._database.Size = new System.Drawing.Size(181, 20);
      this._database.TabIndex = 1;
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point(13, 43);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(29, 13);
      this.label1.TabIndex = 2;
      this.label1.Text = "Host";
      // 
      // label2
      // 
      this.label2.AutoSize = true;
      this.label2.Location = new System.Drawing.Point(13, 87);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(53, 13);
      this.label2.TabIndex = 3;
      this.label2.Text = "Database";
      // 
      // _user
      // 
      this._user.Location = new System.Drawing.Point(90, 123);
      this._user.Name = "_user";
      this._user.Size = new System.Drawing.Size(181, 20);
      this._user.TabIndex = 4;
      // 
      // _password
      // 
      this._password.Location = new System.Drawing.Point(90, 167);
      this._password.Name = "_password";
      this._password.PasswordChar = '*';
      this._password.Size = new System.Drawing.Size(181, 20);
      this._password.TabIndex = 5;
      // 
      // label3
      // 
      this.label3.AutoSize = true;
      this.label3.Location = new System.Drawing.Point(13, 130);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(29, 13);
      this.label3.TabIndex = 6;
      this.label3.Text = "User";
      // 
      // label4
      // 
      this.label4.AutoSize = true;
      this.label4.Location = new System.Drawing.Point(13, 170);
      this.label4.Name = "label4";
      this.label4.Size = new System.Drawing.Size(53, 13);
      this.label4.TabIndex = 7;
      this.label4.Text = "Password";
      // 
      // _okButton
      // 
      this._okButton.Location = new System.Drawing.Point(90, 215);
      this._okButton.Name = "_okButton";
      this._okButton.Size = new System.Drawing.Size(75, 23);
      this._okButton.TabIndex = 8;
      this._okButton.Text = "OK";
      this._okButton.UseVisualStyleBackColor = true;
      this._okButton.Click += new System.EventHandler(this._okButton_Click);
      // 
      // _cancelButton
      // 
      this._cancelButton.Location = new System.Drawing.Point(196, 215);
      this._cancelButton.Name = "_cancelButton";
      this._cancelButton.Size = new System.Drawing.Size(75, 23);
      this._cancelButton.TabIndex = 9;
      this._cancelButton.Text = "Cancel";
      this._cancelButton.UseVisualStyleBackColor = true;
      this._cancelButton.Click += new System.EventHandler(this._cancelButton_Click);
      // 
      // _savedEntries
      // 
      this._savedEntries.FormattingEnabled = true;
      this._savedEntries.Location = new System.Drawing.Point(90, 12);
      this._savedEntries.Name = "_savedEntries";
      this._savedEntries.Size = new System.Drawing.Size(181, 21);
      this._savedEntries.TabIndex = 10;
      this._savedEntries.SelectedIndexChanged += new System.EventHandler(this._savedEntries_SelectedIndexChanged);
      // 
      // ConnectToDatabase
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(318, 265);
      this.Controls.Add(this._savedEntries);
      this.Controls.Add(this._cancelButton);
      this.Controls.Add(this._okButton);
      this.Controls.Add(this.label4);
      this.Controls.Add(this.label3);
      this.Controls.Add(this._password);
      this.Controls.Add(this._user);
      this.Controls.Add(this.label2);
      this.Controls.Add(this.label1);
      this.Controls.Add(this._database);
      this.Controls.Add(this._host);
      this.Name = "ConnectToDatabase";
      this.Text = "ConnectToDatabase";
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.TextBox _host;
    private System.Windows.Forms.TextBox _database;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.TextBox _user;
    private System.Windows.Forms.TextBox _password;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.Label label4;
    private System.Windows.Forms.Button _okButton;
    private System.Windows.Forms.Button _cancelButton;
    private System.Windows.Forms.ComboBox _savedEntries;
  }
}